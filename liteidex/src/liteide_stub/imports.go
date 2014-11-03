package main

import (
	"fmt"
	"go/parser"
	"go/token"
	"io/ioutil"
	"os"
	"strings"
)

var cmdImports = &Command{
	Run:       runImports,
	UsageLine: "imports [-stdin] file",
	Short:     "print go file imports",
	Long:      `print go file imports`,
}

var importsStdin bool

func init() {
	cmdImports.Flag.BoolVar(&importsStdin, "stdin", false, "input from stdin")
}

func runImports(cmd *Command, args []string) {
	if len(args) == 0 {
		cmd.Usage()
	}
	var f *os.File
	if importsStdin {
		f = os.Stdin
	}
	ims, err := parseFileImports(args[0], f)
	if err == nil {
		fmt.Println(strings.Join(ims, "\n"))
	}
}

func parseFileImports(filename string, f *os.File) (imports []string, err error) {
	var src interface{}
	if f != nil {
		src, err = ioutil.ReadAll(f)
		if err != nil {
			return nil, err
		}
	}
	fset := token.NewFileSet()
	file, err := parser.ParseFile(fset, filename, src, parser.ImportsOnly)
	if err != nil {
		return nil, err
	}
	for _, im := range file.Imports {
		impath := strings.Trim(im.Path.Value, "\"")
		impos := fmt.Sprintf("%d", fset.Position(im.Pos()).Line)
		if im.Name != nil {
			imports = append(imports, im.Name.Name+":"+impath+":"+impos)
		} else {
			names := strings.Split(impath, "/")
			imports = append(imports, names[len(names)-1]+":"+impath+":"+impos)
		}
	}
	return imports, nil
}
