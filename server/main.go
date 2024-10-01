package main

import (
	"fmt"
	"log"

	"github.com/abmu/network-pong/server/pongserv"
)

func main() {
	port := 9999
	s, err := pongserv.NewServer(fmt.Sprintf(":%v", port))
	if err != nil {
		log.Fatalf("Error creating server: %v", err)
	}

	fmt.Printf("Server is running on port %v...\n", port)
	s.Run()
}
