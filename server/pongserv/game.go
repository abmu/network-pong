package main

import (
	"fmt"
	"net"
	"time"
)

type Game struct {
	conn     *net.UDPConn
	clients  []*net.UDPAddr
	tickRate int
}

func newGame(conn *net.UDPConn) *Game {
	return &Game{
		conn:     conn,
		clients:  make([]*net.UDPAddr, 0, 2),
		tickRate: 64,
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
		go g.start()
	}
	return true
}

func (g *Game) processMsg(addr *net.UDPAddr, msg string) {
	fmt.Println(msg)
}

func (g *Game) start() {
	tickInterval := time.Duration(1000/g.tickRate) * time.Millisecond
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()

	for range ticker.C {
		for _, client := range g.clients {
			g.conn.WriteToUDP([]byte("Hello"), client)
		}
	}
}
