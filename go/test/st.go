package main

import "fmt"

type Vertex struct {
	X int
	Y int
}

var (
	p = Vertex{1, 2}
	q = &Vertex{1, 2}
	r = Vertex{X: 2}
	s = Vertex{}
)

//var m map[int]Vertex

func sqrt(x float64) float64 {
	result := 1.0
	for i := 0; i < 4; i++ {
		if i == 0 {
			result = 1
		} else {
			result = (result + float64(x)/result) / 2.0
		}
	}
	return result
}

func (v *Vertex) change_value() {
	v.X = v.X * v.X
	v.Y = v.Y * 2
}

type MyInt int

func (x *MyInt) change_value() {
	*x = *x * 2
}

func fibonacci() func() []int {
	return func() []int {
		const n = 10
		array := make([]int, n)
		array[0] = 1
		array[1] = 1
		for i := 2; i < n; i++ {
			array[i] = array[i-1] + array[i-2]
		}
		return array
	}
}

func main() {
	//fmt.Println(math.Sqrt(2))

	//fmt.Println(sqrt(2))
	//fmt.Println(fibonacci()())
	//v := &Vertex{2, 4}
	//fmt.Println(v)
	//v.change_value()
	//fmt.Println(v)

	x := MyInt(10)
	y := &x
	fmt.Println(*y)
	y.change_value()
	fmt.Println(*y)

}
