package pongserv

import (
	"log"
	"net"
	"sync"
)

type Server struct {
	conn    *net.UDPConn
	clients map[string]*game
	games   []*game
	mutex   sync.Mutex
}

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

func (s *Server) Run() {
	defer s.conn.Close()
	buffer := make([]byte, 1024)
	for {
		n, remoteAddr, err := s.conn.ReadFromUDP(buffer)
		if err != nil {
			log.Printf("Error reading from UDP: %v", err)
			continue
		}

		s.mutex.Lock()

		msgBuff := buffer[:n]
		addrStr := remoteAddr.String()
		g, ok := s.clients[addrStr]
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

func (s *Server) assignGame(addr *net.UDPAddr) *game {
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

func (s *Server) removeGame(game *game) {
	s.mutex.Lock()
	defer s.mutex.Unlock()

	for i, g := range s.games {
		if g == game {
			s.games = append(s.games[:i], s.games[i+1:]...)
			break
		}
	}

	for _, client := range game.clients {
		addrStr := client.addr.String()
		delete(s.clients, addrStr)
	}
}
