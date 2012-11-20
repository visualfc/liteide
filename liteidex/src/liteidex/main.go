package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	fmt.Println("liteidex", os.Args)
	cdrv_main(os.Args)
}

var (
	Env = os.Environ()
)

func init() {
	RegCmd("version", func(args []byte) ([]byte, error) {
		return []byte("liteidex(go) version 15.0"), nil
	})
	RegCmd("setenv", func(args []byte) ([]byte, error) {
		Env = strings.Split(string(args), "\n")
		return nil, nil
	})
}
