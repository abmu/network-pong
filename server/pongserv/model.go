package pongserv

import "time"

// Constants defining the game dimensions, margins, speeds, and sizes
const (
	gWidth  int     = 400 // Game width
	gHeight int     = 300 // Game height
	margin  int     = 30  // Margin from the edges
	bSpeed  float32 = 200 // Ball speed
	bSize   int     = 10  // Ball size (diameter)
	pSpeed  float32 = 250 // Paddle speed
	pWidth  int     = 10  // Paddle width
	pHeight int     = 50  // Paddle height
)

// Direction type to represent movement directions
type dir byte

// Enumeration of possible directions
const (
	dirNone  dir = iota + 1 // No direction
	dirUp                   // Up direction
	dirDown                 // Down direction
	dirLeft                 // Left direction
	dirRight                // Right direction
)

// vec2 represents a 2D vector with x and y coordinates
type vec2 struct {
	x, y float32
}

// ball represents the ball in the game
type ball struct {
	startPos vec2 // Starting position of the ball
	pos      vec2 // Current position of the ball
	size     int  // Size of the ball
	vel      vec2 // Velocity of the ball
	dir           // Current direction of the ball
}

// move updates the ball's position based on its velocity and the elapsed time (dt)
func (b *ball) move(dt float32) {
	b.pos.x += b.vel.x * dt
	b.pos.y += b.vel.y * dt

	// Handle ball collision with the top and bottom walls
	ymax := float32(gHeight - b.size)
	if b.pos.y > ymax {
		b.pos.y = ymax - (b.pos.y - ymax)
		b.vel.y *= -1
	} else if b.pos.y < 0 {
		b.pos.y = -b.pos.y
		b.vel.y *= -1
	}
}

// checkPaddle checks if the ball collides with a paddle
func (b *ball) checkPaddle(p *paddle) {
	// Check if the ball is outside the paddle's vertical range
	if b.pos.y+float32(b.size) < p.pos.y {
		return
	}
	if b.pos.y > p.pos.y+float32(p.height) {
		return
	}

	// Check if the ball is outside the paddle's horizontal range based on its direction
	if b.dir == dirRight {
		bRight := b.pos.x + float32(b.size)
		if bRight < p.pos.x || b.pos.x > p.pos.x+float32(p.width) {
			return
		}
	} else if b.dir == dirLeft {
		if b.pos.x > p.pos.x+float32(p.width) || b.pos.x < p.pos.x {
			return
		}
	} else {
		return
	}

	// Handle the collision if the ball is within the paddle's range
	b.handleCollide(p)
}

// handleCollide handles the ball's collision with a paddle
func (b *ball) handleCollide(p *paddle) {
	// Adjust the ball's vertical velocity based on where it hits the paddle
	if b.pos.y+float32(b.size) < p.pos.y+float32(p.height)/3 {
		b.vel.y = -bSpeed
	} else if b.pos.y > p.pos.y+float32(p.height)*2/3 {
		b.vel.y = bSpeed
	} else {
		b.vel.y /= 1.5
	}
	b.reverseDir()
}

// reverseDir reverses the ball's horizontal direction
func (b *ball) reverseDir() {
	b.vel.x *= -1
	if b.vel.x > 0 {
		b.dir = dirRight
	} else {
		b.dir = dirLeft
	}
}

// scored checks if the ball has gone out of bounds (i.e., a player has scored)
func (b *ball) scored() bool {
	if b.pos.x < float32(-b.size) || b.pos.x > float32(gWidth) {
		return true
	}
	return false
}

// resetPos resets the ball to its starting position and reverses its direction
func (b *ball) resetPos() {
	b.pos = b.startPos
	b.reverseDir()
}

// paddle represents a paddle in the game
type paddle struct {
	pos    vec2 // Current position of the paddle
	width  int  // Width of the paddle
	height int  // Height of the paddle
	vel    vec2 // Velocity of the paddle
}

// move updates the paddle's position based on its velocity and the elapsed time (dt)
func (p *paddle) move(dt float32) {
	p.pos.y += p.vel.y * dt
	p.pos.x += p.vel.x * dt

	// Ensure the paddle stays within the game boundaries
	if p.pos.y < 0 {
		p.pos.y = 0
	} else if p.pos.y > float32(gHeight-p.height) {
		p.pos.y = float32(gHeight - p.height)
	}
}

// moveDir sets the paddle's vertical velocity based on the given direction
func (p *paddle) moveDir(dir dir) {
	if dir == dirNone {
		p.vel.y = 0
	} else if dir == dirUp {
		p.vel.y = -pSpeed
	} else if dir == dirDown {
		p.vel.y = pSpeed
	}
}

// model represents the game state, including the ball, paddles, scores, and pause state
type model struct {
	b        *ball     // The ball
	p1       *paddle   // Player 1's paddle
	p2       *paddle   // Player 2's paddle
	s1       uint16    // Player 1's score
	s2       uint16    // Player 2's score
	paused   bool      // Whether the game is paused
	pauseEnd time.Time // Time when the pause ends
}

// newModel initializes a new game model with the ball and paddles in their starting positions
func newModel() *model {
	bStartPos := vec2{
		x: float32(gWidth)/2 - float32(bSize)/2,
		y: float32(gHeight)/2 - float32(bSize)/2,
	}
	bStartVel := vec2{
		x: bSpeed,
	}

	p1StartPos := vec2{
		x: float32(margin) - float32(pWidth)/2,
		y: float32(gHeight)/2 - float32(pHeight)/2,
	}

	p2StartPos := vec2{
		x: float32(gWidth) - float32(margin) - float32(pWidth)/2,
		y: float32(gHeight)/2 - float32(pHeight)/2,
	}

	return &model{
		b: &ball{
			startPos: bStartPos,
			pos:      bStartPos,
			size:     bSize,
			vel:      bStartVel,
			dir:      dirRight,
		},
		p1: &paddle{
			pos:    p1StartPos,
			width:  pWidth,
			height: pHeight,
		},
		p2: &paddle{
			pos:    p2StartPos,
			width:  pWidth,
			height: pHeight,
		},
	}
}

// update updates the game state based on the elapsed time (dt)
func (m *model) update(dt float32) {
	m.p1.move(dt)
	m.p2.move(dt)

	// If the game is paused, check if the pause should end
	if m.paused {
		m.checkPause()
		return
	}

	// Check if the ball has scored and update the game state accordingly
	m.checkBall()
	m.b.move(dt)
}

// checkBall checks if the ball has scored and handles paddle collisions
func (m *model) checkBall() {
	if m.b.scored() {
		// Update the score based on the ball's direction
		if m.b.dir == dirRight {
			m.s1++
		} else if m.b.dir == dirLeft {
			m.s2++
		}
		// Reset the ball's position and start a pause
		m.b.resetPos()
		m.startPause(1000)
		return
	}

	// Check for collisions with the paddles based on the ball's direction
	if m.b.dir == dirRight {
		m.b.checkPaddle(m.p2)
	} else if m.b.dir == dirLeft {
		m.b.checkPaddle(m.p1)
	}
}

// checkPause checks if the pause duration has elapsed and resumes the game if so
func (m *model) checkPause() {
	if time.Now().After(m.pauseEnd) {
		m.paused = false
	}
}

// startPause starts a pause for the specified duration in milliseconds
func (m *model) startPause(durationMs int) {
	duration := time.Duration(durationMs) * time.Millisecond
	m.paused = true
	m.pauseEnd = time.Now().Add(duration)
}
