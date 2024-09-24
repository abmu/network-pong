package main

import (
	"log"
	"net"
)

type Server struct {
	conn    *net.UDPConn
	clients map[string]bool
	games   []*Game
}

func newServer(address string) (*Server, error) {
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
		clients: make(map[string]bool),
		games:   make([]*Game, 0),
	}, nil
}

func (s *Server) run() {
	defer s.conn.Close()
	buffer := make([]byte, 1024)
	for {
		n, remoteAddr, err := s.conn.ReadFromUDP(buffer)
		if err != nil {
			log.Printf("Error reading from UDP: %v", err)
			continue
		}

		addrStr := remoteAddr.String()
		var g *Game
		_, ok := s.clients[addrStr]
		if !ok {
			s.clients[addrStr] = true
			g, _ = s.assignGame(remoteAddr)
		} else {
			g, _ = s.findGame(remoteAddr)
		}

		msg := buffer[:n]
		g.processMsg(remoteAddr, string(msg))
	}
}

func (s *Server) assignGame(addr *net.UDPAddr) (*Game, bool) {
	new := false
	for _, g := range s.games {
		if g.canJoin() {
			g.addClient(addr)
			return g, new
		}
	}

	new = true
	g := newGame(s.conn)
	g.addClient(addr)
	s.games = append(s.games, g)
	return g, new
}

func (s *Server) findGame(addr *net.UDPAddr) (*Game, bool) {
	for _, g := range s.games {
		if g.hasClient(addr) {
			return g, true
		}
	}
	return nil, false
}
