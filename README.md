# network-pong

A network pong game with a GO server and a C++ client, communicating over UDP.

## Server setup

Before you begin, ensure that you have GO installed.

1. To run the server, first get a copy of the `server/` directory. Within the `main.go` file you can set the port (default is 9999).

2. Within your `server/` directory run the following:

```bash
go run main.go
```

## Client setup

Before you begin, ensure that you have C++ and CMake installed, as well as the dependencies listed below.

1. To run the client first get a copy of the `client/` directory.

2. Within it create a `build/` directory and change into it.

```bash
mkdir build
cd build
```

3. Generate the build files and build the project.

```bash
cmake ..
make
```

4. Run the client.

```bash
./pong <ip address> <port>
```

Replace <ip address> and <port> with the ip address of the server and the port you are running it on. To connect to a server running on your local machine, for example:

```bash
./pong 127.0.0.1 9999
```

### Dependencies

The client depends on the following libraries:

- SDL2 (Simple DirectMedia Layer)
- SDL2 ttf

#### Installation on Linux (Ubuntu):

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

## Notes

Ensure that the server and client are running on machines that can communicate with each other over the specified IP address and port

The server should always be started before the clients. A game will only start when two clients join, and ends when either client leaves (or the server is closed).