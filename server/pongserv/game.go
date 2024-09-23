package main

import (
	"fmt"
	"net"
)

type Game struct {
	clients []*net.UDPAddr
}

func newGame() *Game {
	return &Game{
		clients: make([]*net.UDPAddr, 0, 2),
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
	return true
}

func (g *Game) processMsg(addr *net.UDPAddr, msg string) {
	fmt.Println(msg)
}
