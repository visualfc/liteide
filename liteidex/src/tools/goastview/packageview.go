// Copyright 2011-2012 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"fmt"
	"go/ast"
	"go/parser"
	"go/token"
	"io"
	"io/ioutil"
	"os"
	"tools/goastview/doc"
)

const (
	tag_package      = "p"
	tag_type         = "t"
	tag_struct       = "s"
	tag_interface    = "i"
	tag_value        = "v"
	tag_const        = "c"
	tag_func         = "f"
	tag_value_folder = "+v"
	tag_const_folder = "+c"
	tag_func_folder  = "+f"
	tag_type_method  = "tm"
	tag_type_factor  = "tf"
	tag_type_value   = "tv"
)

type PackageView struct {
	fset *token.FileSet
	pdoc *doc.PackageDoc
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
	return fmt.Sprintf("%d,%d,%d", index, pos.Line, pos.Column)
}

func NewFilePackage(filename string) (*PackageView, error) {
	p := new(PackageView)
	p.fset = token.NewFileSet()
	file, err := parser.ParseFile(p.fset, filename, nil, 0)
	if err != nil {
		return nil, err
	}
	p.pdoc = doc.NewFileDoc(file, true)
	return p, nil
}

func NewPackage(pkg *ast.Package, fset *token.FileSet) (*PackageView, error) {
	p := new(PackageView)
	p.fset = fset
	var importpath string = ""
	p.pdoc = doc.NewPackageDoc(pkg, importpath, true)
	return p, nil
}

func ParseFiles(fset *token.FileSet, filenames []string, mode parser.Mode) (pkgs map[string]*ast.Package, pkgsfiles []string, first error) {
	pkgs = make(map[string]*ast.Package)
	for _, filename := range filenames {
		if src, err := parser.ParseFile(fset, filename, nil, mode); err == nil {
			name := src.Name.Name
			pkg, found := pkgs[name]
			if !found {
				//pkg = &ast.Package{name, nil, make(map[string]*ast.File)}
				pkg = &ast.Package{name, nil, nil, make(map[string]*ast.File)}
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

func PrintFilesTree(filenames []string, w io.Writer) error {
	fset := token.NewFileSet()
	pkgs, pkgsfiles, err := ParseFiles(fset, filenames, 0)
	if err != nil {
		return err
	}
	AllFiles = pkgsfiles
	for i := 0; i < len(AllFiles); i++ {
		fmt.Fprintf(w, "@,%s\n", AllFiles[i])
	}
	for _, pkg := range pkgs {
		view, err := NewPackage(pkg, fset)
		if err != nil {
			return err
		}
		view.PrintTree(w)
	}
	return nil
}

func NewFilePackageSource(filename string, f *os.File) (*PackageView, error) {
	src, err := ioutil.ReadAll(f)
	if err != nil {
		return nil, err
	}
	p := new(PackageView)
	p.fset = token.NewFileSet()
	file, err := parser.ParseFile(p.fset, filename, src, 0)
	if err != nil {
		return nil, err
	}
	p.pdoc = doc.NewFileDoc(file, true)
	return p, nil
}

func (p *PackageView) PrintFuncs(w io.Writer, funcs []*doc.FuncDoc, level int, tag string, tag_folder string) {
	if len(tag_folder) > 0 && len(funcs) > 0 {
		fmt.Fprintf(w, "%d,%s,Functions\n", level, tag_folder)
		level++
	}
	for _, f := range funcs {
		pos := p.fset.Position(f.Decl.Pos())
		fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, f.Name, p.posText(pos))
	}
}

func (p *PackageView) PrintVars(w io.Writer, vars []*doc.ValueDoc, level int, tag string, tag_folder string) {
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
					fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, m.Names[i], p.posText(pos))
				}
			}
		}
	}
}
func (p *PackageView) PrintTypes(w io.Writer, types []*doc.TypeDoc, level int) {
	for _, d := range types {
		if d.Type == nil {
			continue
		}
		var tag string = tag_type
		if _, ok := d.Type.Type.(*ast.InterfaceType); ok {
			tag = tag_interface
		} else if _, ok := d.Type.Type.(*ast.StructType); ok {
			tag = tag_struct
		}
		pos := p.fset.Position(d.Type.Pos())
		fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, d.Type.Name, p.posText(pos))
		p.PrintTypeFields(w, d.Decl, level+1)
		p.PrintFuncs(w, d.Factories, level+1, tag_type_factor, "")
		p.PrintFuncs(w, d.Methods, level+1, tag_type_method, "")
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
				fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag_type_value, m.Name, p.posText(pos))
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
	fmt.Fprintf(w, "%d,%s,%s\n", level, tag_package, p.pdoc.PackageName)
}

func (p *PackageView) PrintPackage(w io.Writer, level int) {
	p.PrintHeader(w, level)
	level++
	p.PrintVars(w, p.pdoc.Vars, level, tag_value, tag_value_folder)
	p.PrintVars(w, p.pdoc.Consts, level, tag_const, tag_const_folder)
	p.PrintFuncs(w, p.pdoc.Funcs, level, tag_func, tag_func_folder)
	p.PrintTypes(w, p.pdoc.Types, level)
}

// level:tag:name:x:y
func (p *PackageView) PrintTree(w io.Writer) {
	p.PrintPackage(w, 0)
}
