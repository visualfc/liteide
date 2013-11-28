// gopresent project main.go
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
)

var (
	verify = flag.Bool("v", false, "verify present only")
	input  = flag.String("i", "", "input golang present file")
	stdout = flag.Bool("stdout", false, "output use std output")
	output = flag.String("o", "", "output html file")
)

func abort(err error) {
	fmt.Fprint(os.Stderr, err)
	os.Exit(1)
}

func main() {
	flag.Parse()
	if *input == "" {
		flag.Usage()
		return
	}
	if !isDoc(*input) {
		flag.Usage()
		return
	}

	if *verify {
		err := VerifyDoc(*input)
		if err != nil {
			abort(err)
		}
		return
	}

	w := os.Stdout
	if !*stdout {
		if *output == "" {
			*output = *input + ".html"
		}
		ext := filepath.Ext(*output)
		if ext != ".htm" && ext != ".html" {
			*output += ".html"
		}
		var err error
		w, err = os.Create(*output)
		if err != nil {
			abort(err)
		}
	}
	err := RenderDoc(w, *input)
	if err != nil {
		abort(err)
	}

}
