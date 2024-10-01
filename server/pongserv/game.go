package pongserv

import (
	"encoding/binary"
	"fmt"
	"math"
	"net"
	"sync"
	"time"
)

type client struct {
	addr       *net.UDPAddr
	lastActive time.Time
	lastSeqNum uint16
}

func newClient(addr *net.UDPAddr) *client {
	return &client{
		addr: addr,
	}
}

type game struct {
	s          *Server
	m          *model
	clients    []*client
	timeoutMs  uint16
	tickRate   uint8
	clientRate uint8
	seqNum     uint16
	start      bool
	mutex      sync.Mutex
}

type msg byte

const (
	msgInit msg = iota + 1
	msgInitack
	msgHeartbeat
	msgPaddleDir
	msgModelUpdate
)

func newGame(s *Server) *game {
	return &game{
		s:          s,
		m:          newModel(),
		clients:    make([]*client, 0, 2),
		timeoutMs:  3000,
		tickRate:   64,
		clientRate: 32,
	}
}

func (g *game) canJoin() bool {
	return len(g.clients) < 2
}

func (g *game) getClient(addr *net.UDPAddr) *client {
	addrStr := addr.String()
	for _, client := range g.clients {
		if client.addr.String() == addrStr {
			return client
		}
	}
	return nil
}

func (g *game) addClient(addr *net.UDPAddr) bool {
	if !g.canJoin() || g.getClient(addr) != nil {
		return false
	}

	g.clients = append(g.clients, newClient(addr))
	if len(g.clients) == 2 {
		g.start = true
		g.m.startPause(3000)
	}
	return true
}

func (g *game) sendInitack(addr *net.UDPAddr) {
	buffer := make([]byte, 4)
	buffer[0] = byte(msgInitack)
	binary.BigEndian.PutUint16(buffer[1:3], g.timeoutMs)
	buffer[3] = byte(g.clientRate)
	g.s.conn.WriteToUDP(buffer, addr)
}

func (g *game) sendHeartbeat(addr *net.UDPAddr) {
	buffer := make([]byte, 1)
	buffer[0] = byte(msgHeartbeat)
	g.s.conn.WriteToUDP(buffer, addr)
}

func (g *game) getModelUpdate() []byte {
	g.seqNum++
	m := g.m
	buffer := make([]byte, 55)
	buffer[0] = byte(msgModelUpdate)
	binary.BigEndian.PutUint16(buffer[1:3], g.seqNum)
	binary.BigEndian.PutUint32(buffer[3:7], math.Float32bits(m.b.pos.x))
	binary.BigEndian.PutUint32(buffer[7:11], math.Float32bits(m.b.pos.y))
	binary.BigEndian.PutUint32(buffer[11:15], math.Float32bits(m.b.vel.x))
	binary.BigEndian.PutUint32(buffer[15:19], math.Float32bits(m.b.vel.y))
	binary.BigEndian.PutUint32(buffer[19:23], math.Float32bits(m.p1.pos.x))
	binary.BigEndian.PutUint32(buffer[23:27], math.Float32bits(m.p1.pos.y))
	binary.BigEndian.PutUint32(buffer[27:31], math.Float32bits(m.p1.vel.x))
	binary.BigEndian.PutUint32(buffer[31:35], math.Float32bits(m.p1.vel.y))
	binary.BigEndian.PutUint32(buffer[35:39], math.Float32bits(m.p2.pos.x))
	binary.BigEndian.PutUint32(buffer[39:43], math.Float32bits(m.p2.pos.y))
	binary.BigEndian.PutUint32(buffer[43:47], math.Float32bits(m.p2.vel.x))
	binary.BigEndian.PutUint32(buffer[47:51], math.Float32bits(m.p2.vel.y))
	binary.BigEndian.PutUint16(buffer[51:53], m.s1)
	binary.BigEndian.PutUint16(buffer[53:55], m.s2)
	return buffer
}

func (g *game) sendModelUpdate(addr *net.UDPAddr, buffer []byte) {
	g.s.conn.WriteToUDP(buffer, addr)
}

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

func (g *game) handleInit(client *client) {
	g.sendInitack(client.addr)
}

func (g *game) handlePaddleDir(client *client, buffer []byte) {
	seqNum := binary.BigEndian.Uint16(buffer[1:3])
	if !g.ascSeqNum(client.lastSeqNum, seqNum) {
		return
	}
	client.lastSeqNum = seqNum
	dir := dir(buffer[3])
	if g.clients[0] == client {
		g.m.p1.moveDir(dir)
	} else if g.clients[1] == client {
		g.m.p2.moveDir(dir)
	}
}

func (g *game) ascSeqNum(seq1 uint16, seq2 uint16) bool {
	n := 65536
	a := ((int(seq1)-int(seq2))%n + n) % n
	b := ((int(seq2)-int(seq1))%n + n) % n
	return a > b
}

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
		if g.start {
			g.m.update(dt)
		}
		mUpdate := g.getModelUpdate()
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
			if g.start {
				g.sendModelUpdate(client.addr, mUpdate)
			} else {
				g.sendHeartbeat(client.addr)
			}
		}
	}
}
