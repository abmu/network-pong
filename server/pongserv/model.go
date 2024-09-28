package pongserv

type Vec2 struct {
	x, y float32
}

type Ball struct {
	startPos Vec2
	pos      Vec2
	size     int
	vel      Vec2
}

type Model struct {
}
