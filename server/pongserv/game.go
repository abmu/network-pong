package main

import (
	"encoding/binary"
	"fmt"
	"net"
	"sync"
	"time"
)

type Game struct {
	s             *Server
	clients       []*net.UDPAddr
	lastActive    map[string]time.Time
	heartbeatMsec uint16
	timeoutMsec   uint16
	tickRate      uint8
	start         bool
	mutex         sync.Mutex
}

const (
	msgInit uint8 = iota + 1
	msgInitack
	msgHeartbeat
)

func newGame(s *Server) *Game {
	return &Game{
		s:             s,
		clients:       make([]*net.UDPAddr, 0, 2),
		lastActive:    make(map[string]time.Time),
		heartbeatMsec: 1000,
		timeoutMsec:   3000,
		tickRate:      64,
		start:         false,
	}
}

func (g *Game) canJoin() bool {
	return len(g.clients) < 2
}

func (g *Game) hasClient(addr *net.UDPAddr) bool {
	addrStr := addr.String()
	for _, client := range g.clients {
		if client.String() == addrStr {
			return true
		}
	}
	return false
}

func (g *Game) addClient(addr *net.UDPAddr) bool {
	if !g.canJoin() || g.hasClient(addr) {
		return false
	}

	g.clients = append(g.clients, addr)
	if len(g.clients) == 2 {
		g.start = true
	}
	return true
}

func (g *Game) sendInitack(addr *net.UDPAddr) {
	buffer := make([]byte, 5)
	buffer[0] = byte(msgInitack)
	binary.BigEndian.PutUint16(buffer[1:3], g.heartbeatMsec)
	binary.BigEndian.PutUint16(buffer[3:5], g.timeoutMsec)
	g.s.conn.WriteToUDP(buffer, addr)
}

func (g *Game) sendHeartbeat(addr *net.UDPAddr) {
	buffer := make([]byte, 1)
	buffer[0] = byte(msgHeartbeat)
	g.s.conn.WriteToUDP(buffer, addr)
}

func (g *Game) processMsg(addr *net.UDPAddr, msg []byte) {
	g.mutex.Lock()
	defer g.mutex.Unlock()

	g.lastActive[addr.String()] = time.Now()
	msgType := uint8(msg[0])
	fmt.Printf("%v, %v, %v\n", addr, msgType, msg[0])
	if msgType == msgInit {
		g.sendInitack(addr)
	} else if msgType == msgHeartbeat {
	}
}

func (g *Game) run() {
	tickInterval := time.Duration(1000/float32(g.tickRate)) * time.Millisecond
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()
	timeoutInterval := time.Duration(g.timeoutMsec) * time.Millisecond
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
			if ok && now.Sub(last) > timeoutInterval {
				fmt.Printf("Client %v has timed out\n", client)
				g.s.removeGame(g)
				g.mutex.Unlock()
				return
			}
			g.mutex.Unlock()
		}
	}
}
