package main

import (
	"fmt"
	"net"
	"sync"
	"time"
)

type Game struct {
	s            *Server
	clients      []*net.UDPAddr
	lastActive   map[*net.UDPAddr]time.Time
	heartbeatSec int
	tickRate     int
	mutex        sync.Mutex
}

const (
	msgInit = iota + 1
	msgInitack
	msgHeartbeat
)

func newGame(s *Server) *Game {
	return &Game{
		s:            s,
		clients:      make([]*net.UDPAddr, 0, 2),
		lastActive:   make(map[*net.UDPAddr]time.Time),
		heartbeatSec: 5,
		tickRate:     64,
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
	g.mutex.Lock()
	defer g.mutex.Unlock()

	if !g.canJoin() || g.hasClient(addr) {
		return false
	}

	g.clients = append(g.clients, addr)
	if len(g.clients) == 2 {
		go g.start()
	}
	return true
}

func (g *Game) processMsg(addr *net.UDPAddr, msg []byte) {
	g.mutex.Lock()
	defer g.mutex.Unlock()

	msgType := int(msg[0])
	fmt.Printf("%v, %v, %v\n", addr, msgType, msg[0])
	g.lastActive[addr] = time.Now()
	if msgType == msgInit {
		g.s.conn.WriteToUDP([]byte{byte(msgInitack), byte(g.tickRate), byte(g.heartbeatSec)}, addr)
	} else if msgType == msgHeartbeat {
		fmt.Printf("Heartbeat\n")
	}
}

func (g *Game) start() {
	tickInterval := time.Duration(1000/g.tickRate) * time.Millisecond
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()
	timeoutInterval := time.Duration(3*g.heartbeatSec) * time.Second
	for range ticker.C {
		now := time.Now()
		g.mutex.Lock()
		for _, client := range g.clients {
			g.s.conn.WriteToUDP([]byte("Hello"), client)
			last, ok := g.lastActive[client]
			if ok && now.Sub(last) > timeoutInterval {
				fmt.Printf("Client %v has timed out\n", client)
				g.s.removeGame(g)
				g.mutex.Unlock()
				return
			}
		}
		g.mutex.Unlock()
	}
}
