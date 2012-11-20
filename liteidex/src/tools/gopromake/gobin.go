// Copyright 2011 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"errors"
	"os"
	"path"
	"runtime"
)

type GoBin struct {
	gocmd    string
	compiler string
	link     string
	pack     string
	cgo      string
	objext   string
	exeext   string
	pakext   string
	rm       string
}

func defGoroot() string {
	var curos = runtime.GOOS
	if curos == "windows" {
		return "c:/go"
	}
	return os.Getenv("HOME") + "/go"
}

func NewGoBin(defgoroot string) (p *GoBin, err error) {
	goroot := os.Getenv("GOROOT")
	if goroot == "" {
		goroot = defgoroot
		os.Setenv("GOROOT", goroot)
	}

	gobin := os.Getenv("GOBIN")
	if gobin == "" {
		gobin = goroot + "/bin"
		os.Setenv("GOBIN", gobin)
	}

	goos := os.Getenv("GOOS")
	if goos == "" {
		goos = runtime.GOOS
		os.Setenv("GOOS", goos)
	}

	var exeext string
	var rm string = "rm"

	switch goos {
	case "windows":
		exeext = ".exe"
		rm = "del"
	}

	var goarch string
	goarch = os.Getenv("GOARCH")
	if goarch == "" {
		goarch = runtime.GOARCH
		os.Setenv("GOARCH", goarch)
	}
	var o string
	switch goarch {
	case "amd64":
		o = "6"
	case "386":
		o = "8"
	case "arm":
		o = "5"
	default:
		err = errors.New("Unsupported arch: " + goarch)
		return
	}

	p = new(GoBin)
	p.gocmd = path.Join(gobin, "go")
	p.compiler = o + "g"
	p.link = o + "l"
	p.pack = "pack"
	p.cgo = "cgo"
	p.objext = "." + o
	p.exeext = exeext
	p.pakext = ".a"
	p.rm = rm

	return
}
