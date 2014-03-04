// Copyright 2011-2014 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
)

var (
	flagInputSrc   = flag.String("src", "", "input go source file")
	flagStdin      = flag.Bool("stdin", false, "input source data from stdin")
	flagInputFiles = flag.String("files", "", "input go files")
	flagExpr	   = flag.Bool("expr", true, "print expr string")
)

func main() {
	flag.Parse()
	if len(*flagInputSrc) == 0 && len(*flagInputFiles) == 0 {
		flag.Usage()
		os.Exit(1)
	}

	if len(*flagInputFiles) > 0 {
		var files []string = strings.Split(*flagInputFiles, " ")
		err := PrintFilesTree(files, os.Stdout, *flagExpr)
		if err != nil {
			fmt.Fprint(os.Stderr, err)
			os.Exit(1)
		}
		os.Exit(0)
	}
	if len(*flagInputSrc) > 0 {
		var f *os.File
		if *flagStdin {
			f = os.Stdin
		} else {
			var err error
			f, err = os.OpenFile(*flagInputSrc, os.O_RDONLY, 0)
			if err != nil {
				fmt.Fprintf(os.Stderr, "Error:%s", err)
				os.Exit(1)
			}
		}
		defer f.Close()

		view, err := NewFilePackageSource(*flagInputSrc, f)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error:%s", err)
			os.Exit(1)
		}
		view.PrintTree(os.Stdout)
	}
	os.Exit(0)
}
