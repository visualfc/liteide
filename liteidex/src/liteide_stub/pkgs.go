// Copyright 2011-2015 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"encoding/json"
	"fmt"
	"go/build"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strings"
	"sync"
)

var cmdPkgs = &Command{
	Run:       runPkgs,
	UsageLine: "pkgs",
	Short:     "print liteide_stub version",
	Long:      `Version prints the liteide_stub version.`,
}

var (
	pkgsList       bool
	pkgsJson       bool
	pkgsFind       string
	pkgsStd        bool
	pkgsPkgOnly    bool
	pkgsSkipGoroot bool
)

func init() {
	cmdPkgs.Flag.BoolVar(&pkgsList, "list", false, "list all package")
	cmdPkgs.Flag.BoolVar(&pkgsJson, "json", false, "json format")
	cmdPkgs.Flag.BoolVar(&pkgsStd, "std", false, "std library")
	cmdPkgs.Flag.BoolVar(&pkgsPkgOnly, "pkg", false, "pkg only")
	cmdPkgs.Flag.BoolVar(&pkgsSkipGoroot, "skip_goroot", false, "skip goroot")
	cmdPkgs.Flag.StringVar(&pkgsFind, "find", "", "find package by name")
}

func runPkgs(cmd *Command, args []string) {
	runtime.GOMAXPROCS(runtime.NumCPU())
	if len(args) != 0 {
		cmd.Usage()
	}
	//pkgIndexOnce.Do(loadPkgsList)
	var pp PathPkgsIndex
	pp.LoadIndex()
	pp.Sort()
	if pkgsList {
		for _, pi := range pp.indexs {
			for _, pkg := range pi.pkgs {
				if pkgsPkgOnly && pkg.IsCommand() {
					continue
				}
				if pkgsJson {
					var p GoPackage
					p.copyBuild(pkg)
					b, err := json.MarshalIndent(&p, "", "\t")
					if err == nil {
						os.Stdout.Write(b)
						os.Stdout.Write([]byte{'\n'})
					}
				} else {
					fmt.Println(pkg.ImportPath)
				}
			}
		}
	} else if pkgsFind != "" {
		for _, pi := range pp.indexs {
			for _, pkg := range pi.pkgs {
				if pkg.Name == pkgsFind {
					if pkgsPkgOnly && pkg.IsCommand() {
						continue
					}
					if pkgsJson {
						var p GoPackage
						p.copyBuild(pkg)
						b, err := json.MarshalIndent(p, "", "\t")
						if err == nil {
							os.Stdout.Write(b)
							os.Stdout.Write([]byte{'\n'})
						}
					} else {
						fmt.Println(pkg.Name)
					}
					break
				}
			}
		}
	}
}

// A Package describes a single package found in a directory.
type GoPackage struct {
	// Note: These fields are part of the go command's public API.
	// See list.go.  It is okay to add fields, but not to change or
	// remove existing ones.  Keep in sync with list.go
	Dir         string `json:",omitempty"` // directory containing package sources
	ImportPath  string `json:",omitempty"` // import path of package in dir
	Name        string `json:",omitempty"` // package name
	Doc         string `json:",omitempty"` // package documentation string
	Target      string `json:",omitempty"` // install path
	Goroot      bool   `json:",omitempty"` // is this package found in the Go root?
	Standard    bool   `json:",omitempty"` // is this package part of the standard Go library?
	Stale       bool   `json:",omitempty"` // would 'go install' do anything for this package?
	Root        string `json:",omitempty"` // Go root or Go path dir containing this package
	ConflictDir string `json:",omitempty"` // Dir is hidden by this other directory

	// Source files
	GoFiles        []string `json:",omitempty"` // .go source files (excluding CgoFiles, TestGoFiles, XTestGoFiles)
	CgoFiles       []string `json:",omitempty"` // .go sources files that import "C"
	IgnoredGoFiles []string `json:",omitempty"` // .go sources ignored due to build constraints
	CFiles         []string `json:",omitempty"` // .c source files
	CXXFiles       []string `json:",omitempty"` // .cc, .cpp and .cxx source files
	MFiles         []string `json:",omitempty"` // .m source files
	HFiles         []string `json:",omitempty"` // .h, .hh, .hpp and .hxx source files
	SFiles         []string `json:",omitempty"` // .s source files
	SwigFiles      []string `json:",omitempty"` // .swig files
	SwigCXXFiles   []string `json:",omitempty"` // .swigcxx files
	SysoFiles      []string `json:",omitempty"` // .syso system object files added to package

	// Cgo directives
	CgoCFLAGS    []string `json:",omitempty"` // cgo: flags for C compiler
	CgoCPPFLAGS  []string `json:",omitempty"` // cgo: flags for C preprocessor
	CgoCXXFLAGS  []string `json:",omitempty"` // cgo: flags for C++ compiler
	CgoLDFLAGS   []string `json:",omitempty"` // cgo: flags for linker
	CgoPkgConfig []string `json:",omitempty"` // cgo: pkg-config names

	// Dependency information
	Imports []string `json:",omitempty"` // import paths used by this package
	Deps    []string `json:",omitempty"` // all (recursively) imported dependencies

	// Error information
	Incomplete bool `json:",omitempty"` // was there an error loading this package or dependencies?

	// Test information
	TestGoFiles  []string `json:",omitempty"` // _test.go files in package
	TestImports  []string `json:",omitempty"` // imports from TestGoFiles
	XTestGoFiles []string `json:",omitempty"` // _test.go files outside package
	XTestImports []string `json:",omitempty"` // imports from XTestGoFiles

	// Unexported fields are not part of the public API.
	build        *build.Package
	pkgdir       string // overrides build.PkgDir
	imports      []*Package
	deps         []*Package
	gofiles      []string // GoFiles+CgoFiles+TestGoFiles+XTestGoFiles files, absolute paths
	sfiles       []string
	allgofiles   []string             // gofiles + IgnoredGoFiles, absolute paths
	target       string               // installed file for this package (may be executable)
	fake         bool                 // synthesized package
	forceBuild   bool                 // this package must be rebuilt
	forceLibrary bool                 // this package is a library (even if named "main")
	cmdline      bool                 // defined by files listed on command line
	local        bool                 // imported via local path (./ or ../)
	localPrefix  string               // interpret ./ and ../ imports relative to this prefix
	exeName      string               // desired name for temporary executable
	coverMode    string               // preprocess Go source files with the coverage tool in this mode
	coverVars    map[string]*CoverVar // variables created by coverage analysis
	omitDWARF    bool                 // tell linker not to write DWARF information
}

// CoverVar holds the name of the generated coverage variables targeting the named file.
type CoverVar struct {
	File string // local file name
	Var  string // name of count struct
}

func (p *GoPackage) copyBuild(pp *build.Package) {
	p.build = pp

	p.Dir = pp.Dir
	p.ImportPath = pp.ImportPath
	p.Name = pp.Name
	p.Doc = pp.Doc
	p.Root = pp.Root
	p.ConflictDir = pp.ConflictDir
	// TODO? Target
	p.Goroot = pp.Goroot
	p.Standard = p.Goroot && p.ImportPath != "" && !strings.Contains(p.ImportPath, ".")
	p.GoFiles = pp.GoFiles
	p.CgoFiles = pp.CgoFiles
	p.IgnoredGoFiles = pp.IgnoredGoFiles
	p.CFiles = pp.CFiles
	p.CXXFiles = pp.CXXFiles
	p.MFiles = pp.MFiles
	p.HFiles = pp.HFiles
	p.SFiles = pp.SFiles
	p.SwigFiles = pp.SwigFiles
	p.SwigCXXFiles = pp.SwigCXXFiles
	p.SysoFiles = pp.SysoFiles
	p.CgoCFLAGS = pp.CgoCFLAGS
	p.CgoCPPFLAGS = pp.CgoCPPFLAGS
	p.CgoCXXFLAGS = pp.CgoCXXFLAGS
	p.CgoLDFLAGS = pp.CgoLDFLAGS
	p.CgoPkgConfig = pp.CgoPkgConfig
	p.Imports = pp.Imports
	p.TestGoFiles = pp.TestGoFiles
	p.TestImports = pp.TestImports
	p.XTestGoFiles = pp.XTestGoFiles
	p.XTestImports = pp.XTestImports
}

type PathPkgsIndex struct {
	indexs []*PkgsIndex
}

func (p *PathPkgsIndex) LoadIndex() {
	var wg sync.WaitGroup
	var context = build.Default
	if pkgsStd {
		context.GOPATH = ""
	}
	var srcDirs []string
	goroot := context.GOROOT
	gopath := context.GOPATH
	context.GOPATH = ""

	if !pkgsSkipGoroot {
		//go1.4 go/src/
		//go1.3 go/src/pkg; go/src/cmd
		_, err := os.Stat(filepath.Join(goroot, "src/pkg/runtime"))
		if err == nil {
			for _, v := range context.SrcDirs() {
				if strings.HasSuffix(v, "pkg") {
					srcDirs = append(srcDirs, v[:len(v)-3]+"cmd")
				}
				srcDirs = append(srcDirs, v)
			}
		} else {
			srcDirs = append(srcDirs, filepath.Join(goroot, "src"))
		}
	}

	context.GOPATH = gopath
	context.GOROOT = ""
	for _, v := range context.SrcDirs() {
		srcDirs = append(srcDirs, v)
	}
	context.GOROOT = goroot
	for _, path := range srcDirs {
		pi := &PkgsIndex{}
		p.indexs = append(p.indexs, pi)
		pkgsGate.enter()
		f, err := os.Open(path)
		if err != nil {
			pkgsGate.leave()
			fmt.Fprint(os.Stderr, err)
			continue
		}
		children, err := f.Readdir(-1)
		f.Close()
		pkgsGate.leave()
		if err != nil {
			fmt.Fprint(os.Stderr, err)
			continue
		}
		for _, child := range children {
			if child.IsDir() {
				wg.Add(1)
				go func(path, name string) {
					defer wg.Done()
					pi.loadPkgsPath(&wg, path, name)
				}(path, child.Name())
			}
		}
	}
	wg.Wait()
}

func (p *PathPkgsIndex) Sort() {
	for _, v := range p.indexs {
		v.sort()
	}
}

type PkgsIndex struct {
	sync.Mutex
	pkgs []*build.Package
}

func (p *PkgsIndex) sort() {
	sort.Sort(PkgSlice(p.pkgs))
}

type PkgSlice []*build.Package

func (p PkgSlice) Len() int {
	return len([]*build.Package(p))
}

func (p PkgSlice) Less(i, j int) bool {
	if p[i].IsCommand() && !p[j].IsCommand() {
		return true
	} else if !p[i].IsCommand() && p[j].IsCommand() {
		return false
	}
	return p[i].ImportPath < p[j].ImportPath
}

func (p PkgSlice) Swap(i, j int) {
	p[i], p[j] = p[j], p[i]
}

// pkgsgate protects the OS & filesystem from too much concurrency.
// Too much disk I/O -> too many threads -> swapping and bad scheduling.
// gate is a semaphore for limiting concurrency.
var pkgsGate = make(gate, 8)

func (p *PkgsIndex) loadPkgsPath(wg *sync.WaitGroup, root, pkgrelpath string) {
	importpath := filepath.ToSlash(pkgrelpath)
	dir := filepath.Join(root, importpath)

	pkgsGate.enter()
	defer pkgsGate.leave()
	pkgDir, err := os.Open(dir)
	if err != nil {
		return
	}
	children, err := pkgDir.Readdir(-1)
	pkgDir.Close()
	if err != nil {
		return
	}
	// hasGo tracks whether a directory actually appears to be a
	// Go source code directory. If $GOPATH == $HOME, and
	// $HOME/src has lots of other large non-Go projects in it,
	// then the calls to importPathToName below can be expensive.
	hasGo := false
	for _, child := range children {
		name := child.Name()
		if name == "" {
			continue
		}
		if c := name[0]; c == '.' || ('0' <= c && c <= '9') {
			continue
		}
		if strings.HasSuffix(name, ".go") {
			hasGo = true
		}
		if child.IsDir() {
			if strings.HasPrefix(name, ".") || strings.HasPrefix(name, "_") || name == "testdata" {
				continue
			}
			wg.Add(1)
			go func(root, name string) {
				defer wg.Done()
				p.loadPkgsPath(wg, root, name)
			}(root, filepath.Join(importpath, name))
		}
	}
	if hasGo {
		buildPkg, err := build.ImportDir(dir, 0)
		if err == nil {
			if buildPkg.ImportPath == "." {
				buildPkg.ImportPath = filepath.ToSlash(pkgrelpath)
				buildPkg.Root = root
				buildPkg.Goroot = true
			}
			p.Lock()
			p.pkgs = append(p.pkgs, buildPkg)
			p.Unlock()
		}
	}
}
