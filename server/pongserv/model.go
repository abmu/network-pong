package pongserv

import "time"

const (
	gWidth  int     = 400
	gHeight int     = 300
	margin  int     = 30
	bSpeed  float32 = 200
	bSize   int     = 10
	pSpeed  float32 = 250
	pWidth  int     = 10
	pHeight int     = 50
)

type dir byte

const (
	dirNone dir = iota + 1
	dirUp
	dirDown
	dirLeft
	dirRight
)

type vec2 struct {
	x, y float32
}

type ball struct {
	startPos vec2
	pos      vec2
	size     int
	vel      vec2
	dir
}

func (b *ball) move(dt float32) {
	b.pos.x += b.vel.x * dt
	b.pos.y += b.vel.y * dt
	ymax := float32(gHeight - b.size)
	if b.pos.y > ymax {
		b.pos.y = ymax - (b.pos.y - ymax)
		b.vel.y *= -1
	} else if b.pos.y < 0 {
		b.pos.y = -b.pos.y
		b.vel.y *= -1
	}
}

func (b *ball) checkPaddle(p *paddle) {
	if b.pos.y+float32(b.size) < p.pos.y {
		return
	}

	if b.pos.y > p.pos.y+float32(p.height) {
		return
	}

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

	b.handleCollide(p)
}

func (b *ball) handleCollide(p *paddle) {
	if b.pos.y+float32(b.size) < p.pos.y+float32(p.height)/3 {
		b.vel.y = -bSpeed
	} else if b.pos.y > p.pos.y+float32(p.height)*2/3 {
		b.vel.y = bSpeed
	} else {
		b.vel.y /= 1.5
	}
	b.reverseDir()
}

func (b *ball) reverseDir() {
	b.vel.x *= -1
	if b.vel.x > 0 {
		b.dir = dirRight
	} else {
		b.dir = dirLeft
	}
}

func (b *ball) scored() bool {
	if b.pos.x < float32(-b.size) || b.pos.x > float32(gWidth) {
		return true
	}
	return false
}

func (b *ball) resetPos() {
	b.pos = b.startPos
	b.reverseDir()
}

type paddle struct {
	pos    vec2
	width  int
	height int
	vel    vec2
}

func (p *paddle) move(dt float32) {
	p.pos.y += p.vel.y * dt
	p.pos.x += p.vel.x * dt
	if p.pos.y < 0 {
		p.pos.y = 0
	} else if p.pos.y > float32(gHeight-p.height) {
		p.pos.y = float32(gHeight - p.height)
	}
}

func (p *paddle) moveDir(dir dir) {
	if dir == dirNone {
		p.vel.y = 0
	} else if dir == dirUp {
		p.vel.y = -pSpeed
	} else if dir == dirDown {
		p.vel.y = pSpeed
	}
}

type model struct {
	b        *ball
	p1       *paddle
	p2       *paddle
	s1       uint16
	s2       uint16
	paused   bool
	pauseEnd time.Time
}

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

func (m *model) update(dt float32) {
	m.p1.move(dt)
	m.p2.move(dt)
	if m.paused {
		m.checkPause()
		return
	}
	m.checkBall()
	m.b.move(dt)
}

func (m *model) checkBall() {
	if m.b.scored() {
		if m.b.dir == dirRight {
			m.s1++
		} else if m.b.dir == dirLeft {
			m.s2++
		}
		m.b.resetPos()
		m.startPause(1000)
		return
	}

	if m.b.dir == dirRight {
		m.b.checkPaddle(m.p2)
	} else if m.b.dir == dirLeft {
		m.b.checkPaddle(m.p1)
	}
}

func (m *model) checkPause() {
	if time.Now().After(m.pauseEnd) {
		m.paused = false
	}
}

func (m *model) startPause(durationMs int) {
	duration := time.Duration(durationMs) * time.Millisecond
	m.paused = true
	m.pauseEnd = time.Now().Add(duration)
}
