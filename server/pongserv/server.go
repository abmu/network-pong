package main

import (
	"log"
	"net"
	"sync"
)

type Server struct {
	conn    *net.UDPConn
	clients map[string]*Game
	games   []*Game
	mutex   sync.Mutex
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
		clients: make(map[string]*Game),
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

		s.mutex.Lock()

		msg := buffer[:n]
		addrStr := remoteAddr.String()
		g, ok := s.clients[addrStr]
		if !ok {
			msgType := uint8(msg[0])
			if msgType == msgInit {
				g = s.assignGame(remoteAddr)
				s.clients[addrStr] = g
			}
		}

		if g != nil {
			g.processMsg(remoteAddr, msg)
		}

		s.mutex.Unlock()
	}
}

func (s *Server) assignGame(addr *net.UDPAddr) *Game {
	for _, g := range s.games {
		if g.canJoin() {
			g.addClient(addr)
			return g
		}
	}

	g := newGame(s)
	g.addClient(addr)
	s.games = append(s.games, g)
	go g.run()
	return g
}

func (s *Server) removeGame(game *Game) {
	s.mutex.Lock()
	defer s.mutex.Unlock()

	for i, g := range s.games {
		if g == game {
			s.games = append(s.games[:i], s.games[i+1:]...)
			break
		}
	}

	for _, client := range game.clients {
		addrStr := client.String()
		delete(s.clients, addrStr)
	}
}
