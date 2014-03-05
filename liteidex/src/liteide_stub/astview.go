// Copyright 2011-2014 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.
package main

import (
	"fmt"
	"go/ast"
	"go/doc"
	"go/parser"
	"go/token"
	"io"
	"io/ioutil"
	"os"
	"strings"

	"code.google.com/p/go.tools/go/types"
)

var cmdAstView = &Command{
	Run:       runAstView,
	UsageLine: "astview [-stdin] files...",
	Short:     "print go files astview",
	Long:      `print go files astview`,
}

var astViewStdin bool

func init() {
	cmdAstView.Flag.BoolVar(&astViewStdin, "stdin", false, "input from stdin")
}

func runAstView(cmd *Command, args []string) {
	if len(args) == 0 {
		cmd.Usage()
	}
	if astViewStdin {
		view, err := NewFilePackageSource(args[0], os.Stdin, true)
		if err != nil {
			fmt.Fprintf(os.Stderr, "astview: %s", err)
			setExitStatus(3)
			exit()
		}
		view.PrintTree(os.Stdout)
	} else {
		err := PrintFilesTree(args, os.Stdout, true)
		if err != nil {
			fmt.Fprintf(os.Stderr, "astview:%s", err)
			setExitStatus(3)
			exit()
		}
	}
}

const (
	tag_package        = "p"
	tag_imports_folder = "+m"
	tag_import         = "mm"
	tag_type           = "t"
	tag_struct         = "s"
	tag_interface      = "i"
	tag_value          = "v"
	tag_const          = "c"
	tag_func           = "f"
	tag_value_folder   = "+v"
	tag_const_folder   = "+c"
	tag_func_folder    = "+f"
	tag_factor_folder  = "+tf"
	tag_type_method    = "tm"
	tag_type_factor    = "tf"
	tag_type_value     = "tv"
)

type PackageView struct {
	fset *token.FileSet
	pdoc *doc.Package
	pkg  *ast.Package
	expr bool
}

var AllFiles []string

func (p *PackageView) posFileIndex(pos token.Position) int {
	var index = -1
	for i := 0; i < len(AllFiles); i++ {
		if AllFiles[i] == pos.Filename {
			index = i
			break
		}
	}
	if index == -1 {
		AllFiles = append(AllFiles, pos.Filename)
		index = len(AllFiles) - 1
	}
	return index
}

func (p *PackageView) posText(pos token.Position) (s string) {
	index := p.posFileIndex(pos)
	return fmt.Sprintf("%d:%d:%d", index, pos.Line, pos.Column)
}

func NewFilePackage(filename string) (*PackageView, error) {
	p := new(PackageView)
	p.fset = token.NewFileSet()
	file, err := parser.ParseFile(p.fset, filename, nil, 0)
	if err != nil {
		return nil, err
	}
	m := make(map[string]*ast.File)
	m[filename] = file
	pkg, err := ast.NewPackage(p.fset, m, nil, nil)
	if err != nil {
		return nil, err
	}
	p.pkg = pkg
	p.pdoc = doc.New(pkg, pkg.Name, doc.AllDecls)
	return p, nil
}

func NewPackage(pkg *ast.Package, fset *token.FileSet, expr bool) (*PackageView, error) {
	p := new(PackageView)
	p.fset = fset
	p.pkg = pkg
	p.pdoc = doc.New(pkg, pkg.Name, doc.AllDecls)
	p.expr = expr
	return p, nil
}

func ParseFiles(fset *token.FileSet, filenames []string, mode parser.Mode) (pkgs map[string]*ast.Package, pkgsfiles []string, first error) {
	pkgs = make(map[string]*ast.Package)
	for _, filename := range filenames {
		if src, err := parser.ParseFile(fset, filename, nil, mode); err == nil {
			name := src.Name.Name
			pkg, found := pkgs[name]
			if !found {
				pkg = &ast.Package{
					Name:  name,
					Files: make(map[string]*ast.File),
				}
				pkgs[name] = pkg
			}
			pkg.Files[filename] = src
			pkgsfiles = append(pkgsfiles, filename)
		} else {
			first = err
			return
		}
	}
	return
}

func PrintFilesTree(filenames []string, w io.Writer, expr bool) error {
	fset := token.NewFileSet()
	pkgs, pkgsfiles, err := ParseFiles(fset, filenames, 0)
	if err != nil {
		return err
	}
	AllFiles = pkgsfiles
	for i := 0; i < len(AllFiles); i++ {
		fmt.Fprintf(w, "@%s\n", AllFiles[i])
	}
	for _, pkg := range pkgs {
		view, err := NewPackage(pkg, fset, expr)
		if err != nil {
			return err
		}
		view.PrintTree(w)
	}
	return nil
}

func NewFilePackageSource(filename string, f *os.File, expr bool) (*PackageView, error) {
	src, err := ioutil.ReadAll(f)
	if err != nil {
		return nil, err
	}
	p := new(PackageView)
	p.fset = token.NewFileSet()
	p.expr = expr
	file, err := parser.ParseFile(p.fset, filename, src, 0)
	if err != nil {
		return nil, err
	}
	m := make(map[string]*ast.File)
	m[filename] = file
	pkg, err := ast.NewPackage(p.fset, m, nil, nil)
	if err != nil {
		return nil, err
	}

	p.pdoc = doc.New(pkg, pkg.Name, doc.AllDecls)
	return p, nil
}

func (p *PackageView) printFuncsHelper(w io.Writer, funcs []*doc.Func, level int, tag string, tag_folder string) {
	for _, f := range funcs {
		pos := p.fset.Position(f.Decl.Pos())
		if p.expr {
			fmt.Fprintf(w, "%d,%s,%s,%s@%s\n", level, tag, f.Name, p.posText(pos), types.ExprString(f.Decl.Type))
		} else {
			fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, f.Name, p.posText(pos))
		}
	}
}

func (p *PackageView) PrintVars(w io.Writer, vars []*doc.Value, level int, tag string, tag_folder string) {
	if len(tag_folder) > 0 && len(vars) > 0 {
		if tag_folder == tag_value_folder {
			fmt.Fprintf(w, "%d,%s,Variables\n", level, tag_folder)
		} else if tag_folder == tag_const_folder {
			fmt.Fprintf(w, "%d,%s,Constants\n", level, tag_folder)
		}
		level++
	}
	for _, v := range vars {
		if v.Decl == nil {
			continue
		}
		for _, s := range v.Decl.Specs {
			if m, ok := s.(*ast.ValueSpec); ok {
				pos := p.fset.Position(m.Pos())
				for i := 0; i < len(m.Names); i++ {
					if p.expr && m.Type != nil {
						fmt.Fprintf(w, "%d,%s,%s,%s@%s\n", level, tag, m.Names[i], p.posText(pos), types.ExprString(m.Type))
					} else {
						fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, m.Names[i], p.posText(pos))
					}
				}
			}
		}
	}
}
func (p *PackageView) PrintTypes(w io.Writer, types []*doc.Type, level int) {
	for _, d := range types {
		if d.Decl == nil {
			continue
		}
		typespec := d.Decl.Specs[0].(*ast.TypeSpec)
		var tag = tag_type
		if _, ok := typespec.Type.(*ast.InterfaceType); ok {
			tag = tag_interface
		} else if _, ok := typespec.Type.(*ast.StructType); ok {
			tag = tag_struct
		}
		pos := p.fset.Position(d.Decl.Pos())
		fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, d.Name, p.posText(pos))
		p.PrintTypeFields(w, d.Decl, level+1)
		p.printFuncsHelper(w, d.Funcs, level+1, tag_type_factor, "")
		p.printFuncsHelper(w, d.Methods, level+1, tag_type_method, "")
		p.PrintVars(w, d.Vars, level, tag_value, tag_value_folder)
	}
}

func (p *PackageView) PrintTypeFields(w io.Writer, decl *ast.GenDecl, level int) {
	spec, ok := decl.Specs[0].(*ast.TypeSpec)
	if ok == false {
		return
	}
	switch d := spec.Type.(type) {
	case *ast.StructType:
		for _, list := range d.Fields.List {
			if list.Names == nil {
				continue
			}
			for _, m := range list.Names {
				pos := p.fset.Position(m.Pos())
				if list.Type != nil {
					fmt.Fprintf(w, "%d,%s,%s,%s@%s\n", level, tag_type_value, m.Name, p.posText(pos), types.ExprString(list.Type))
				} else {
					fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag_type_value, m.Name, p.posText(pos))
				}
			}
		}
	case *ast.InterfaceType:
		for _, list := range d.Methods.List {
			if list.Names == nil {
				continue
			}
			for _, m := range list.Names {
				pos := p.fset.Position(m.Pos())
				fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag_type_method, m.Name, p.posText(pos))
			}
		}
	}
}

func (p *PackageView) PrintHeader(w io.Writer, level int) {
	fmt.Fprintf(w, "%d,%s,%s\n", level, tag_package, p.pdoc.Name)
}

func (p *PackageView) PrintImports(w io.Writer, level int, tag, tag_folder string) {
	if tag_folder != "" && len(p.pdoc.Imports) > 0 {
		fmt.Fprintf(w, "%d,%s,%s\n", level, tag_folder, "Imports")
		level++
	}
	for _, name := range p.pdoc.Imports {
		vname := "\"" + name + "\""
		var ps []string
		for _, file := range p.pkg.Files {
			for _, v := range file.Imports {
				if v.Path.Value == vname {
					pos := p.fset.Position(v.Pos())
					ps = append(ps, p.posText(pos))
				}
			}
		}
		fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, name, strings.Join(ps, ";"))
	}
}

func (p *PackageView) PrintFuncs(w io.Writer, level int, tag_folder string) {
	hasFolder := false
	if len(p.pdoc.Funcs) > 0 {
		hasFolder = true
	}
	if !hasFolder {
		for _, d := range p.pdoc.Types {
			if len(d.Funcs) > 0 {
				hasFolder = true
				break
			}
		}
	}
	if !hasFolder {
		return
	}
	if len(tag_folder) > 0 {
		fmt.Fprintf(w, "%d,%s,Functions\n", level, tag_folder)
		level++
	}
	for _, d := range p.pdoc.Types {
		p.printFuncsHelper(w, d.Funcs, level, tag_type_factor, "")
	}
	p.printFuncsHelper(w, p.pdoc.Funcs, level, tag_func, tag_func_folder)
}

func (p *PackageView) PrintPackage(w io.Writer, level int) {
	p.PrintHeader(w, level)
	level++
	p.PrintImports(w, level, tag_import, tag_imports_folder)
	p.PrintVars(w, p.pdoc.Vars, level, tag_value, tag_value_folder)
	p.PrintVars(w, p.pdoc.Consts, level, tag_const, tag_const_folder)
	p.PrintFuncs(w, level, tag_func_folder)
	p.PrintTypes(w, p.pdoc.Types, level)
}

// level,tag,pos@info
func (p *PackageView) PrintTree(w io.Writer) {
	p.PrintPackage(w, 0)
}
