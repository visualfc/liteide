// Copyright 2011-2015 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

var cmdJsonFmt = &Command{
	Run:       runJsonFmt,
	UsageLine: "jsonfmt",
	Short:     "json format util",
	Long:      `json format util.`,
}

var (
	jsonFmtList    bool
	jsonFmtCompact bool
	jsonFmtWrite   bool
	jsonFmtDiff    bool
	jsonTabWidth   int
	jsonTabIndent  bool
)

func init() {
	cmdJsonFmt.Flag.BoolVar(&jsonFmtList, "l", false, "list files whose formatting differs")
	cmdJsonFmt.Flag.BoolVar(&jsonFmtCompact, "c", false, "compact json")
	cmdJsonFmt.Flag.BoolVar(&jsonFmtWrite, "w", false, "write result to (source) file instead of stdout")
	cmdJsonFmt.Flag.BoolVar(&jsonFmtDiff, "d", false, "display diffs instead of rewriting files")
	cmdJsonFmt.Flag.IntVar(&jsonTabWidth, "tabwidth", 4, "tab width")
	cmdJsonFmt.Flag.BoolVar(&jsonTabIndent, "tabs", false, "indent with tabs")
}

func runJsonFmt(cmd *Command, args []string) {
	opt := &JsonFmtOption{}
	opt.List = jsonFmtList
	opt.Compact = jsonFmtCompact
	opt.IndentTab = jsonTabIndent
	opt.TabWidth = jsonTabWidth
	opt.Write = jsonFmtWrite
	opt.Diff = jsonFmtDiff

	if len(args) == 0 {
		if err := processJsonFile("<standard input>", os.Stdin, os.Stdout, true, opt); err != nil {
			reportJsonError(err)
		}
	} else {
		for _, path := range args {
			switch dir, err := os.Stat(path); {
			case err != nil:
				reportJsonError(err)
			case dir.IsDir():
				filepath.Walk(path, func(path string, f os.FileInfo, err error) error {
					if err == nil && isJsonFile(f) {
						err = processJsonFile(path, nil, os.Stdout, false, opt)
					}
					if err != nil {
						reportJsonError(err)
					}
					return nil
				})
			default:
				if err := processJsonFile(path, nil, os.Stdout, false, opt); err != nil {
					reportJsonError(err)
				}
			}
		}
	}
}

type JsonFmtOption struct {
	List      bool
	Compact   bool
	Format    bool
	Write     bool
	Diff      bool
	IndentTab bool
	TabWidth  int
}

func isJsonFile(f os.FileInfo) bool {
	// ignore non-Go files
	name := f.Name()
	return !f.IsDir() && !strings.HasPrefix(name, ".") && strings.HasSuffix(name, ".json")
}

func reportJsonError(err error) {
	fmt.Fprintf(os.Stderr, "%s\n", err)
	os.Exit(2)
}

func processJson(filename string, src []byte, opt *JsonFmtOption) ([]byte, error) {
	if opt.Compact {
		var out bytes.Buffer
		err := json.Compact(&out, src)
		if err != nil {
			return nil, err
		}
		return out.Bytes(), nil
	} else {
		var out bytes.Buffer
		var err error
		if opt.IndentTab {
			err = json.Indent(&out, src, "", "\t")
		} else {
			var indent string
			for i := 0; i < opt.TabWidth; i++ {
				indent += " "
			}
			err = json.Indent(&out, src, "", indent)
		}
		if err != nil {
			return nil, err
		}
		return out.Bytes(), nil
	}
	return src, nil
}

func processJsonFile(filename string, in io.Reader, out io.Writer, stdin bool, opt *JsonFmtOption) error {
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

	res, err := processJson(filename, src, opt)
	if err != nil {
		return err
	}

	if !bytes.Equal(src, res) {
		// formatting has changed
		if opt.List {
			fmt.Fprintln(out, filename)
		}
		if opt.Write {
			err = ioutil.WriteFile(filename, res, 0)
			if err != nil {
				return err
			}
		}
		if opt.Diff {
			data, err := diffJson(src, res)
			if err != nil {
				return fmt.Errorf("computing diff: %s", err)
			}
			fmt.Printf("diff %s json/%s\n", filename, filename)
			out.Write(data)
		}
	}

	if !opt.List && !opt.Write && !opt.Diff {
		_, err = out.Write(res)
	}

	return err
}

func diffJson(b1, b2 []byte) (data []byte, err error) {
	f1, err := ioutil.TempFile("", "json")
	if err != nil {
		return
	}
	defer os.Remove(f1.Name())
	defer f1.Close()

	f2, err := ioutil.TempFile("", "json")
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
