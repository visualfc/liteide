// Copyright 2011-2012 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"flag"
	"fmt"
	"os"
	"path"
)

var (
	proFileName  *string = flag.String("gopro", "", "make go project")
	goFileName   *string = flag.String("gofiles", "", "make go sources")
	goTargetName *string = flag.String("o", "", "file specify output file")
	printDep     *bool   = flag.Bool("dep", false, "print packages depends ")
	showVer      *bool   = flag.Bool("ver", false, "print version ")
	buildLib     *bool   = flag.Bool("lib", false, "build packages as librarys outside main")
	goroot       *string = flag.String("goroot", defGoroot(), "default go root")
	clean        *string = flag.String("clean", "", "clean project [obj|all]")
	install      *bool   = flag.Bool("install", false, "install package")
)

var Usage = func() {
	_, name := path.Split(os.Args[0])
	fmt.Fprintf(os.Stderr, "usage: %s -gopro   example.e64\n", name)
	fmt.Fprintf(os.Stderr, "       %s -gofiles \"go1.go go2.go\"\n", name)
	flag.PrintDefaults()
}

func exitln(err error) {
	fmt.Println(err)
	os.Exit(1)
}

func main() {
	flag.Parse()
	fmt.Println("Golang Project Build Tools.")

	if *showVer == true {
		fmt.Println("Version 1.1, make by visualfc <visualfc@gmail.com>.")
	}

	gobin, err := NewGoBin(*goroot)
	if err != nil {
		exitln(err)
	}

	var pro *GoProject

	if len(*proFileName) > 0 {
		pro, err = NewGoProject(*proFileName)
		if err != nil {
			exitln(err)
		}
	} else if len(*goFileName) > 0 {
		var input []byte = []byte(*goFileName)
		all := bytes.SplitAfter(input, []byte(" "))
		pro, err = NewGoProjectWithFiles(all)
		if err != nil {
			exitln(err)
		}
	}
	if pro == nil {
		Usage()
		os.Exit(1)
	}

	if len(*goTargetName) > 0 {
		pro.Values["TARGET"] = []string{*goTargetName}
	}
	fmt.Println("Parser Files", pro.Gofiles())

	files := pro.Gofiles()
	pro.array = ParserFiles(files)

	if printDep != nil && *printDep == true {
		fmt.Printf("AllPackage:\n%s\n", pro.array)
	}

	if pro.array.HasMain == false {
		*buildLib = true
	}

	if len(*clean) > 0 {
		if *clean == "obj" || *clean == "all" {
			err := pro.Clean(*clean)
			if err != nil {
				exitln(err)
			}
		} else {
			Usage()
			os.Exit(1)
		}
		os.Exit(0)
	}

	err = pro.MakeTarget(gobin)
	if err != nil {
		exitln(err)
	}
	os.Exit(0)
}
