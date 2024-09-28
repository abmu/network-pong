package main

import (
	"fmt"
	"log"

	"github.com/abmu/network-pong/server/pongserv"
)

func main() {
	s, err := pongserv.NewServer(":9999")
	if err != nil {
		log.Fatalf("Error creating server: %v", err)
	}

	fmt.Println("Server is running on port 9999...")
	s.Run()
}
