package pongserv

import (
	"encoding/binary"
	"fmt"
	"net"
	"sync"
	"time"
)

type game struct {
	s          *Server
	clients    []*net.UDPAddr
	lastActive map[string]time.Time
	timeoutMs  uint16
	tickRate   uint8
	clientRate uint8
	start      bool
	mutex      sync.Mutex
}

type msg byte

const (
	msgInit msg = iota + 1
	msgInitack
	msgHeartbeat
	msgPaddleDir
)

func newGame(s *Server) *game {
	return &game{
		s:          s,
		clients:    make([]*net.UDPAddr, 0, 2),
		lastActive: make(map[string]time.Time),
		timeoutMs:  3000,
		tickRate:   64,
		clientRate: 32,
		start:      false,
	}
}

func (g *game) canJoin() bool {
	return len(g.clients) < 2
}

func (g *game) hasClient(addr *net.UDPAddr) bool {
	addrStr := addr.String()
	for _, client := range g.clients {
		if client.String() == addrStr {
			return true
		}
	}
	return false
}

func (g *game) addClient(addr *net.UDPAddr) bool {
	if !g.canJoin() || g.hasClient(addr) {
		return false
	}

	g.clients = append(g.clients, addr)
	if len(g.clients) == 2 {
		g.start = true
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

func (g *game) processMsg(addr *net.UDPAddr, msgBuff []byte) {
	g.mutex.Lock()
	defer g.mutex.Unlock()

	g.lastActive[addr.String()] = time.Now()
	msgType := msg(msgBuff[0])
	fmt.Printf("%v, %v, %v\n", addr, msgType, msgBuff[0])
	if msgType == msgInit {
		g.sendInitack(addr)
	} else if msgType == msgPaddleDir {
		dir := uint8(msgBuff[1])
		fmt.Printf("%v\n", dir)
	}
}

func (g *game) run() {
	tickInterval := time.Duration(1000/float32(g.tickRate)) * time.Millisecond
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()
	timeout := time.Duration(g.timeoutMs) * time.Millisecond
	for range ticker.C {
		now := time.Now()
		for _, client := range g.clients {
			g.mutex.Lock()
			if g.start {
				g.s.conn.WriteToUDP([]byte("Hello"), client)
			} else {
				g.sendHeartbeat(client)
			}
			last, ok := g.lastActive[client.String()]
			if ok && now.Sub(last) > timeout {
				fmt.Printf("Client %v has timed out\n", client)
				g.s.removeGame(g)
				g.mutex.Unlock()
				return
			}
			g.mutex.Unlock()
		}
	}
}
