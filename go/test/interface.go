package main

import "fmt"
import "time"
import (
	"io"
	"os"
	"strings"
)

type Writer interface {
	Write(b []byte) (n int, err error)
}

type MyError struct {
	When time.Time
	What string
}

func (err *MyError) Error() string {
	return fmt.Sprintf("at %v %s", err.When, err.What)
}

func run() error {
	return &MyError{time.Now(), "bad number"}
}

func main() {
	_, err := sqrt(-1)
	if err != nil {
		fmt.Println(err)
	}
	s := strings.NewReader(
		"Lbh penpxrq gur pbqr!")
	r := rot13Reader{s}
	io.Copy(os.Stdout, &r)

}

type rot13Reader struct {
	r io.Reader
}

func sqrt(x float64) (float64, error) {
	if x < 0 {
		return -1, &MyError{time.Now(), fmt.Sprintf("invalid param %f", x)}
	}

	result := 1.0
	for i := 0; i < 4; i++ {
		if i == 0 {
			result = 1
		} else {
			result = (result + float64(x)/result) / 2.0
		}
	}
	return result, nil
}
