package main

import (
	"fmt"
	"log"
	"net"
)

type Server struct {
	conn *net.UDPConn
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
		conn: conn,
	}, nil
}

func (s *Server) run() {
	s.handleIncomingMessages()
}

func (s *Server) handleIncomingMessages() {
	buffer := make([]byte, 1024)
	for {
		n, remoteAddr, err := s.conn.ReadFromUDP(buffer)
		if err != nil {
			log.Printf("Error reading from UDP: %v", err)
			continue
		}

		message := buffer[:n]
		s.processMessage(remoteAddr, string(message))
	}
}

func (s *Server) processMessage(addr *net.UDPAddr, message string) {
	fmt.Println(message)
}
