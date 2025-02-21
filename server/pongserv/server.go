package pongserv

import (
	"log"
	"net"
	"sync"
)

// Server represents the UDP game server that manages multiple pong games
type Server struct {
	conn    *net.UDPConn     // UDP connection for network communication
	clients map[string]*game // Maps client addresses to their current game
	games   []*game          // List of all active games
	mutex   sync.Mutex       // Mutex for thread-safe access to server state
}

// NewServer creates and initializes a new UDP server on the specified address
func NewServer(address string) (*Server, error) {
	addr, err := net.ResolveUDPAddr("udp", address)
	if err != nil {
		return nil, err
	}

	conn, err := net.ListenUDP("udp", addr)
	if err != nil {
		return nil, err
	}

	return &Server{
		conn:    conn,
		clients: make(map[string]*game),
		games:   make([]*game, 0),
	}, nil
}

// Run starts the main server loop, handling incoming UDP packets
func (s *Server) Run() {
	defer s.conn.Close()
	buffer := make([]byte, 1024)
	for {
		n, remoteAddr, err := s.conn.ReadFromUDP(buffer)
		if err != nil {
			log.Printf("Error reading from UDP: %v", err)
			continue
		}
		if n < 1 {
			continue
		}

		s.mutex.Lock()

		msgBuff := buffer[:n]
		addrStr := remoteAddr.String()
		g, ok := s.clients[addrStr]

		// Handle new client connections
		if !ok {
			msgType := msg(msgBuff[0])
			if msgType == msgInit {
				g = s.assignGame(remoteAddr)
				s.clients[addrStr] = g
			}
		}

		if g != nil {
			g.parseMsg(remoteAddr, msgBuff)
		}

		s.mutex.Unlock()
	}
}

// assignGame places a client in an available game or creates a new one
func (s *Server) assignGame(addr *net.UDPAddr) *game {
	// Try to find an existing game with space
	for _, g := range s.games {
		if g.canJoin() {
			g.addClient(addr)
			return g
		}
	}

	// Create new game if no existing games have space
	g := newGame(s)
	g.addClient(addr)
	s.games = append(s.games, g)
	go g.run()
	return g
}

// removeGame cleans up a game and removes all associated client mappings
func (s *Server) removeGame(game *game) {
	s.mutex.Lock()
	defer s.mutex.Unlock()

	// Remove game from games list
	for i, g := range s.games {
		if g == game {
			s.games = append(s.games[:i], s.games[i+1:]...)
			break
		}
	}

	// Remove client mappings
	for _, client := range game.clients {
		addrStr := client.addr.String()
		delete(s.clients, addrStr)
	}
}
