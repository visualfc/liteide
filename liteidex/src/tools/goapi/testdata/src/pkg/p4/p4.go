package p4

var (
	V1 = make([]uint, 10)
	V2 = string("hello")
)

type T []string

var V3 = T([]string{})

var V4 = make(chan int)
var V5 = make(chan<- int)
var V6 = make(<-chan int)

type Err struct {
	s string
}

func (p *Err) Error() string {
	return p.s
}

func NewError(s string) *Err {
	return &Err{s}
}

var (
	V7     = "hello"
	V8, V9 = func(int) (<-chan int, error) {
		return make(chan int), NewError("error")
	}(10)
	V10 = V9.Error()
)

const intSize = 32 << uint(^uint(0)>>63)
const IntSize = intSize // number of bits in int, uint (32 or 64)

type ss struct {
	tag []byte
}

func (p *ss) test1() string {
	return "hello"
}

func (p *ss) test2() (b []byte) {
	return
}

func (p *ss) test3() (map[int]string, *ss) {
	return nil, newss()
}

func (p *ss) test4() func(x int) *int {
	return func(x int) *int {
		return &x
	}
}

func (p *ss) test5() func(x int) *ss {
	return func(x int) *ss {
		return p
	}
}

func (p *ss) test6() string {
	return "hello"
}

func newss() *ss {
	return &ss{}
}

var (
	s0     ss
	s1     = &ss{}
	s2     = newss()
	P1     = s0.test1()
	P2     = s1.test1()
	P3     = s2.test2()
	P4, P5 = s2.test3()
	P6     = P5.test1()
	P7     = s2.test5()(100).test5()
	P8     = append([]int{}, 10)
	P9     = append(P8, 10)
	P10    = cap(P9)
	P11    = new(ss).tag
	P12    = NewError("hello").Error()
	s13    = s2.test5()(100).test5()(200).test5()
	P13    = s13(100).test2()
	s14    = s1.test4()
	P14    = s14(100)
)

const (
	C1 = complex(100, 200)
	C2 = real(C1)
)
