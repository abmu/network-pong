package pongserv

import (
	"encoding/binary"
	"fmt"
	"math"
	"net"
	"sync"
	"time"
)

// client represents a connected player
type client struct {
	addr       *net.UDPAddr // Client's network address
	lastActive time.Time    // Last time client sent a message
	lastSeqNum uint16       // Last received sequence number for this client
}

// newClient creates a new client instance with the given network address
func newClient(addr *net.UDPAddr) *client {
	return &client{
		addr: addr,
	}
}

// game represents an active pong match between two players
type game struct {
	s          *Server    // Reference to main server
	m          *model     // Game state model
	clients    []*client  // Connected clients (max 2)
	timeoutMs  uint16     // Client timeout threshold in milliseconds
	tickRate   uint8      // Server update rate in Hz
	clientRate uint8      // Client update rate in Hz
	seqNum     uint16     // Current sequence number for game state updates
	start      bool       // Whether the game has started
	mutex      sync.Mutex // Mutex for thread-safe access to game state
}

// Message types for client-server communication
type msg byte

const (
	msgInit        msg = iota + 1 // Initial connection request from client
	msgInitack                    // Server acknowledgment of connection
	msgHeartbeat                  // Keep-alive message for inactive games
	msgPaddleDir                  // Paddle movement command from client
	msgModelUpdate                // Game state update from server
)

// newGame creates a new game instance with default settings
func newGame(s *Server) *game {
	return &game{
		s:          s,
		m:          newModel(),
		clients:    make([]*client, 0, 2),
		timeoutMs:  3000, // 3 second timeout
		tickRate:   64,   // 64 Hz server tick rate
		clientRate: 32,   // 32 Hz client update rate
	}
}

// canJoin checks if the game has room for another player
func (g *game) canJoin() bool {
	return len(g.clients) < 2
}

// getClient returns the client instance associated with the given address
// Returns nil if no matching client is found
func (g *game) getClient(addr *net.UDPAddr) *client {
	addrStr := addr.String()
	for _, client := range g.clients {
		if client.addr.String() == addrStr {
			return client
		}
	}
	return nil
}

// addClient adds a new client to the game if possible
// Returns true if client was successfully added, false otherwise
func (g *game) addClient(addr *net.UDPAddr) bool {
	if !g.canJoin() || g.getClient(addr) != nil {
		return false
	}

	g.clients = append(g.clients, newClient(addr))
	if len(g.clients) == 2 {
		g.start = true
		g.m.startPause(3000) // 3 second countdown before game starts
	}
	return true
}

// sendInitack sends initial acknowledgment to client with game settings
func (g *game) sendInitack(addr *net.UDPAddr) {
	buffer := make([]byte, 4)
	buffer[0] = byte(msgInitack)
	binary.BigEndian.PutUint16(buffer[1:3], g.timeoutMs)
	buffer[3] = byte(g.clientRate)
	g.s.conn.WriteToUDP(buffer, addr)
}

// sendHeartbeat sends a keep-alive message to client
func (g *game) sendHeartbeat(addr *net.UDPAddr) {
	buffer := make([]byte, 1)
	buffer[0] = byte(msgHeartbeat)
	g.s.conn.WriteToUDP(buffer, addr)
}

// getModelUpdate creates a byte buffer containing the current game state
// Includes ball position/velocity, paddle positions/velocities, and scores
func (g *game) getModelUpdate() []byte {
	g.seqNum++
	m := g.m
	buffer := make([]byte, 55)
	buffer[0] = byte(msgModelUpdate)
	binary.BigEndian.PutUint16(buffer[1:3], g.seqNum)
	// Pack ball state
	binary.BigEndian.PutUint32(buffer[3:7], math.Float32bits(m.b.pos.x))
	binary.BigEndian.PutUint32(buffer[7:11], math.Float32bits(m.b.pos.y))
	binary.BigEndian.PutUint32(buffer[11:15], math.Float32bits(m.b.vel.x))
	binary.BigEndian.PutUint32(buffer[15:19], math.Float32bits(m.b.vel.y))
	// Pack player 1 paddle state
	binary.BigEndian.PutUint32(buffer[19:23], math.Float32bits(m.p1.pos.x))
	binary.BigEndian.PutUint32(buffer[23:27], math.Float32bits(m.p1.pos.y))
	binary.BigEndian.PutUint32(buffer[27:31], math.Float32bits(m.p1.vel.x))
	binary.BigEndian.PutUint32(buffer[31:35], math.Float32bits(m.p1.vel.y))
	// Pack player 2 paddle state
	binary.BigEndian.PutUint32(buffer[35:39], math.Float32bits(m.p2.pos.x))
	binary.BigEndian.PutUint32(buffer[39:43], math.Float32bits(m.p2.pos.y))
	binary.BigEndian.PutUint32(buffer[43:47], math.Float32bits(m.p2.vel.x))
	binary.BigEndian.PutUint32(buffer[47:51], math.Float32bits(m.p2.vel.y))
	// Pack scores
	binary.BigEndian.PutUint16(buffer[51:53], m.s1)
	binary.BigEndian.PutUint16(buffer[53:55], m.s2)
	return buffer
}

// sendModelUpdate sends the current game state to the specified client
func (g *game) sendModelUpdate(addr *net.UDPAddr, buffer []byte) {
	g.s.conn.WriteToUDP(buffer, addr)
}

// parseMsg handles incoming messages from clients
func (g *game) parseMsg(addr *net.UDPAddr, buffer []byte) {
	g.mutex.Lock()
	defer g.mutex.Unlock()

	client := g.getClient(addr)
	if client == nil {
		return
	}
	client.lastActive = time.Now()
	msgType := msg(buffer[0])
	if msgType == msgInit {
		g.handleInit(client)
	} else if msgType == msgPaddleDir {
		g.handlePaddleDir(client, buffer)
	}
}

// handleInit processes an initial connection request from a client
func (g *game) handleInit(client *client) {
	g.sendInitack(client.addr)
}

// handlePaddleDir processes a paddle movement command from a client
func (g *game) handlePaddleDir(client *client, buffer []byte) {
	if len(buffer) < 4 {
		return
	}
	seqNum := binary.BigEndian.Uint16(buffer[1:3])
	// Verify sequence number is newer than last received
	if !g.ascSeqNum(client.lastSeqNum, seqNum) {
		return
	}
	client.lastSeqNum = seqNum
	dir := dir(buffer[3])
	addrStr := client.addr.String()
	// Update appropriate paddle direction based on client
	if g.clients[0].addr.String() == addrStr {
		g.m.p1.moveDir(dir)
	} else if g.clients[1].addr.String() == addrStr {
		g.m.p2.moveDir(dir)
	}
}

// ascSeqNum checks if seq2 is more recent than seq1, accounting for uint16 wraparound
func (g *game) ascSeqNum(seq1 uint16, seq2 uint16) bool {
	n := 65536 // 2^16, the range of uint16
	// Calculate distances between sequences in both directions
	a := ((int(seq1)-int(seq2))%n + n) % n
	b := ((int(seq2)-int(seq1))%n + n) % n
	return a > b // If a > b, seq2 is more recent
}

// run is the main game loop that updates game state and sends updates to clients
func (g *game) run() {
	tickInterval := time.Duration(1000/float32(g.tickRate)) * time.Millisecond
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()
	timeout := time.Duration(g.timeoutMs) * time.Millisecond
	lastTick := time.Now()

	for range ticker.C {
		now := time.Now()
		dt := float32(now.Sub(lastTick).Seconds())
		lastTick = now

		// Update game state if game has started
		if g.start {
			g.m.update(dt)
		}

		mUpdate := g.getModelUpdate()

		// Send updates to all clients and check for timeouts
		for _, client := range g.clients {
			g.mutex.Lock()
			last := client.lastActive
			if now.Sub(last) > timeout {
				fmt.Printf("Client %v has timed out\n", client.addr)
				g.s.removeGame(g)
				g.mutex.Unlock()
				return
			}
			g.mutex.Unlock()

			// Send game state if started, otherwise send heartbeat
			if g.start {
				g.sendModelUpdate(client.addr, mUpdate)
			} else {
				g.sendHeartbeat(client.addr)
			}
		}
	}
}
