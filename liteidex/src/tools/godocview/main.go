// godocview project main.go
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
)

var (
	goroot = flag.String("goroot", runtime.GOROOT(), "Go root directory")
	find   = flag.String("find", "", "find package list, :pkg flag is best match")
	list   = flag.String("list", "", "Print go packages list [pkg|cmd]")
	mode   = flag.String("mode", "text", "Print mode [text|html|lite]")
)

func usage() {
	fmt.Fprintf(os.Stderr,
		"usage: godocview -find pkgname\n"+
			"       godocview -find build\t:find go/build package\n")
	flag.PrintDefaults()
	os.Exit(2)
}

func main() {
	flag.Usage = usage
	flag.Parse()

	if len(*find) == 0 && len(*list) == 0 {
		flag.Usage()
	}

	var template string
	var info *Info
	if len(*list) > 0 {
		info = NewListInfo(filepath.Join(*goroot, "src", *list))
		switch *mode {
		case "html":
			template = listHTML
		case "lite":
			template = listLite
		case "text":
			template = listText
		default:
			template = listText
		}
	} else if len(*find) > 0 {
		dir := NewSourceDir(*goroot)
		info = dir.FindInfo(*find)
		switch *mode {
		case "html":
			template = findHTML
		case "lite":
			template = findLite
		case "text":
			template = findText
		default:
			template = findText
		}
	}
	if info == nil {
		fmt.Fprintf(os.Stderr, "<error>")
		os.Exit(2)
	}
	contents := info.GetPkgList(*mode, template)
	fmt.Println(string(contents))

	os.Exit(0)
}
