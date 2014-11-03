// Copyright 2013 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bufio"
	"bytes"
	"fmt"
	"go/ast"
	"go/parser"
	"go/printer"
	"go/scanner"
	"go/token"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"runtime"
	"strconv"
	"strings"
	"sync"

	"code.google.com/p/go.tools/astutil"
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

func initModes() {
	initParserMode()
	initPrinterMode()
}

func initParserMode() {
	parserMode = parser.Mode(0)
	if goimportsComments {
		parserMode |= parser.ParseComments
	}
	if goimportsAllErrors {
		parserMode |= parser.AllErrors
	}
}

func initPrinterMode() {
	printerMode = printer.UseSpaces
	if goimportsTabIndent {
		printerMode |= printer.TabIndent
	}
}

func isGoFile(f os.FileInfo) bool {
	// ignore non-Go files
	name := f.Name()
	return !f.IsDir() && !strings.HasPrefix(name, ".") && strings.HasSuffix(name, ".go")
}

// If in == nil, the source is the contents of the file with the given filename.
func processFile(filename string, in io.Reader, out io.Writer, stdin bool) error {
	initModesOnce.Do(initModes)

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

	file, adjust, err := goimportParse(fileSet, filename, src, stdin)
	if err != nil {
		return err
	}

	_, err = fixImports(fileSet, file)
	if err != nil {
		return err
	}

	sortImports(fileSet, file)
	imps := astutil.Imports(fileSet, file)

	var spacesBefore []string // import paths we need spaces before
	if len(imps) == 1 {
		// We have just one block of imports. See if any are in different groups numbers.
		lastGroup := -1
		for _, importSpec := range imps[0] {
			importPath, _ := strconv.Unquote(importSpec.Path.Value)
			groupNum := importGroup(importPath)
			if groupNum != lastGroup && lastGroup != -1 {
				spacesBefore = append(spacesBefore, importPath)
			}
			lastGroup = groupNum
		}

	}

	var buf bytes.Buffer
	err = (&printer.Config{Mode: printerMode, Tabwidth: goimportsTabWidth}).Fprint(&buf, fileSet, file)
	if err != nil {
		return err
	}
	res := buf.Bytes()
	if adjust != nil {
		res = adjust(src, res)
	}
	if len(spacesBefore) > 0 {
		res = addImportSpaces(bytes.NewReader(res), spacesBefore)
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

// parse parses src, which was read from filename,
// as a Go source file or statement list.
func goimportParse(fset *token.FileSet, filename string, src []byte, stdin bool) (*ast.File, func(orig, src []byte) []byte, error) {
	// Try as whole source file.
	file, err := parser.ParseFile(fset, filename, src, parserMode)
	if err == nil {
		return file, nil, nil
	}
	// If the error is that the source file didn't begin with a
	// package line and this is standard input, fall through to
	// try as a source fragment.  Stop and return on any other error.
	if !stdin || !strings.Contains(err.Error(), "expected 'package'") {
		return nil, nil, err
	}

	// If this is a declaration list, make it a source file
	// by inserting a package clause.
	// Insert using a ;, not a newline, so that the line numbers
	// in psrc match the ones in src.
	psrc := append([]byte("package p;"), src...)
	file, err = parser.ParseFile(fset, filename, psrc, parserMode)
	if err == nil {
		adjust := func(orig, src []byte) []byte {
			// Remove the package clause.
			// Gofmt has turned the ; into a \n.
			src = src[len("package p\n"):]
			return matchSpace(orig, src)
		}
		return file, adjust, nil
	}
	// If the error is that the source file didn't begin with a
	// declaration, fall through to try as a statement list.
	// Stop and return on any other error.
	if !strings.Contains(err.Error(), "expected declaration") {
		return nil, nil, err
	}

	// If this is a statement list, make it a source file
	// by inserting a package clause and turning the list
	// into a function body.  This handles expressions too.
	// Insert using a ;, not a newline, so that the line numbers
	// in fsrc match the ones in src.
	fsrc := append(append([]byte("package p; func _() {"), src...), '}')
	file, err = parser.ParseFile(fset, filename, fsrc, parserMode)
	if err == nil {
		adjust := func(orig, src []byte) []byte {
			// Remove the wrapping.
			// Gofmt has turned the ; into a \n\n.
			src = src[len("package p\n\nfunc _() {"):]
			src = src[:len(src)-len("}\n")]
			// Gofmt has also indented the function body one level.
			// Remove that indent.
			src = bytes.Replace(src, []byte("\n\t"), []byte("\n"), -1)
			return matchSpace(orig, src)
		}
		return file, adjust, nil
	}

	// Failed, and out of options.
	return nil, nil, err
}

func cutSpace(b []byte) (before, middle, after []byte) {
	i := 0
	for i < len(b) && (b[i] == ' ' || b[i] == '\t' || b[i] == '\n') {
		i++
	}
	j := len(b)
	for j > 0 && (b[j-1] == ' ' || b[j-1] == '\t' || b[j-1] == '\n') {
		j--
	}
	if i <= j {
		return b[:i], b[i:j], b[j:]
	}
	return nil, nil, b[j:]
}

// matchSpace reformats src to use the same space context as orig.
// 1) If orig begins with blank lines, matchSpace inserts them at the beginning of src.
// 2) matchSpace copies the indentation of the first non-blank line in orig
//    to every non-blank line in src.
// 3) matchSpace copies the trailing space from orig and uses it in place
//   of src's trailing space.
func matchSpace(orig []byte, src []byte) []byte {
	before, _, after := cutSpace(orig)
	i := bytes.LastIndex(before, []byte{'\n'})
	before, indent := before[:i+1], before[i+1:]

	_, src, _ = cutSpace(src)

	var b bytes.Buffer
	b.Write(before)
	for len(src) > 0 {
		line := src
		if i := bytes.IndexByte(line, '\n'); i >= 0 {
			line, src = line[:i+1], line[i+1:]
		} else {
			src = nil
		}
		if len(line) > 0 && line[0] != '\n' { // not blank
			b.Write(indent)
		}
		b.Write(line)
	}
	b.Write(after)
	return b.Bytes()
}

var impLine = regexp.MustCompile(`^\s+(?:\w+\s+)?"(.+)"`)

func addImportSpaces(r io.Reader, breaks []string) []byte {
	var out bytes.Buffer
	sc := bufio.NewScanner(r)
	inImports := false
	done := false
	for sc.Scan() {
		s := sc.Text()

		if !inImports && !done && strings.HasPrefix(s, "import") {
			inImports = true
		}
		if inImports && (strings.HasPrefix(s, "var") ||
			strings.HasPrefix(s, "func") ||
			strings.HasPrefix(s, "const") ||
			strings.HasPrefix(s, "type")) {
			done = true
			inImports = false
		}
		if inImports && len(breaks) > 0 {
			if m := impLine.FindStringSubmatch(s); m != nil {
				if m[1] == string(breaks[0]) {
					out.WriteByte('\n')
					breaks = breaks[1:]
				}
			}
		}

		fmt.Fprintln(&out, s)
	}
	return out.Bytes()
}
