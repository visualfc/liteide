// Copyright 2013 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"fmt"
	"go/parser"
	"go/printer"
	"go/scanner"
	"go/token"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"sync"
)

var cmdGoimports = &Command{
	Run:       runGoimports,
	UsageLine: "goimports [flags] [path ...]",
	Short:     "updates go import lines",
	Long:      `goimports updates your Go import lines, adding missing ones and removing unreferenced ones. `,
}

var (
	goimportsList      bool
	goimportsWrite     bool
	goimportsDiff      bool
	goimportsAllErrors bool

	// layout control
	goimportsComments  bool
	goimportsTabWidth  int
	goimportsTabIndent bool
)

//func init
func init() {
	cmdGoimports.Flag.BoolVar(&goimportsList, "l", false, "list files whose formatting differs from goimport's")
	cmdGoimports.Flag.BoolVar(&goimportsWrite, "w", false, "write result to (source) file instead of stdout")
	cmdGoimports.Flag.BoolVar(&goimportsDiff, "d", false, "display diffs instead of rewriting files")
	cmdGoimports.Flag.BoolVar(&goimportsAllErrors, "e", false, "report all errors (not just the first 10 on different lines)")

	// layout control
	cmdGoimports.Flag.BoolVar(&goimportsComments, "comments", true, "print comments")
	cmdGoimports.Flag.IntVar(&goimportsTabWidth, "tabwidth", 8, "tab width")
	cmdGoimports.Flag.BoolVar(&goimportsTabIndent, "tabs", true, "indent with tabs")
}

var (
	fileSet  = token.NewFileSet() // per process FileSet
	exitCode = 0

	initModesOnce sync.Once // guards calling initModes
	parserMode    parser.Mode
	printerMode   printer.Mode
	options       *Options
)

func report(err error) {
	scanner.PrintError(os.Stderr, err)
	exitCode = 2
}

func runGoimports(cmd *Command, args []string) {
	runtime.GOMAXPROCS(runtime.NumCPU())

	if goimportsTabWidth < 0 {
		fmt.Fprintf(os.Stderr, "negative tabwidth %d\n", goimportsTabWidth)
		exitCode = 2
		return
	}

	options = &Options{
		TabWidth:  goimportsTabWidth,
		TabIndent: goimportsTabIndent,
		Comments:  goimportsComments,
		AllErrors: goimportsAllErrors,
		Fragment:  true,
	}

	if len(args) == 0 {
		if err := processFile("<standard input>", os.Stdin, os.Stdout, true); err != nil {
			report(err)
		}
		return
	}

	for _, path := range args {
		switch dir, err := os.Stat(path); {
		case err != nil:
			report(err)
		case dir.IsDir():
			walkDir(path)
		default:
			if err := processFile(path, nil, os.Stdout, false); err != nil {
				report(err)
			}
		}
	}
	os.Exit(exitCode)

}

func isGoFile(f os.FileInfo) bool {
	// ignore non-Go files
	name := f.Name()
	return !f.IsDir() && !strings.HasPrefix(name, ".") && strings.HasSuffix(name, ".go")
}

func processFile(filename string, in io.Reader, out io.Writer, stdin bool) error {
	if in == nil {
		f, err := os.Open(filename)
		if err != nil {
			return err
		}
		defer f.Close()
		in = f
	}

	src, err := ioutil.ReadAll(in)
	if err != nil {
		return err
	}

	res, err := Process(filename, src, options)
	if err != nil {
		return err
	}

	if !bytes.Equal(src, res) {
		// formatting has changed
		if goimportsList {
			fmt.Fprintln(out, filename)
		}
		if goimportsWrite {
			err = ioutil.WriteFile(filename, res, 0)
			if err != nil {
				return err
			}
		}
		if goimportsDiff {
			data, err := diff(src, res)
			if err != nil {
				return fmt.Errorf("computing diff: %s", err)
			}
			fmt.Printf("diff %s gofmt/%s\n", filename, filename)
			out.Write(data)
		}
	}

	if !goimportsList && !goimportsWrite && !goimportsDiff {
		_, err = out.Write(res)
	}

	return err
}

func visitFile(path string, f os.FileInfo, err error) error {
	if err == nil && isGoFile(f) {
		err = processFile(path, nil, os.Stdout, false)
	}
	if err != nil {
		report(err)
	}
	return nil
}

func walkDir(path string) {
	filepath.Walk(path, visitFile)
}

func diff(b1, b2 []byte) (data []byte, err error) {
	f1, err := ioutil.TempFile("", "gofmt")
	if err != nil {
		return
	}
	defer os.Remove(f1.Name())
	defer f1.Close()

	f2, err := ioutil.TempFile("", "gofmt")
	if err != nil {
		return
	}
	defer os.Remove(f2.Name())
	defer f2.Close()

	f1.Write(b1)
	f2.Write(b2)

	data, err = exec.Command("diff", "-u", f1.Name(), f2.Name()).CombinedOutput()
	if len(data) > 0 {
		// diff exits with a non-zero status when the files don't match.
		// Ignore that failure as long as we get output.
		err = nil
	}
	return
}
