package main

import (
	"fmt"
	"log"
)

func main() {
	s, err := newServer(":9999")
	if err != nil {
		log.Fatalf("Error creating server: %v", err)
	}

	fmt.Println("Server is running on port 9999...")
	s.run()
}
