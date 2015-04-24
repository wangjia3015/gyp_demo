package main

import "fmt"

func sum(a []int, c chan int) {
	sum := 0
	for _, v := range a {
		sum += v
	}
	c <- sum
}

func fibonacci(n int, c chan int) {
	x, y := 0, 1
	for i := 0; i < n; i++ {
		c <- x
		x, y = y, x+y
	}
	close(c)
}

func main() {
	c := make(chan int, 10)
	go fibonacci(cap(c), c)
	for n := range c {
		fmt.Println(n)
	}
	//var array []int = []int{1, 2, 3, 4, 5, 6, 7, 8, 9}
	//c := make(chan int)
	//go sum(array[:len(array)/2], c)
	//go sum(array[len(array)/2:], c)
	//x, y := <-c, <-c
	//fmt.Println(x, y)
}
