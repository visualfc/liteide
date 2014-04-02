// Copyright 2011-2014 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"fmt"
	"go/ast"
	"go/build"
	"go/parser"
	"go/printer"
	"go/token"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strconv"
	"strings"
	"time"

	"code.google.com/p/go.tools/go/gcimporter"
	"code.google.com/p/go.tools/go/types"
)

/*
liteide_stub type fmt
liteide_stub type -v c:\go\src\cmd\go\testdata\local
liteide_stub type -v std
liteide_stub type -v ...
*/

var cmdType = &Command{
	Run:       runType,
	UsageLine: "type",
	Short:     "golang type util",
	Long:      `golang type util`,
}

var (
	typeVerbose         bool
	typeAllowBinary     bool
	typeList            bool
	typeFileCursor      string
	typeFileCursorStdin bool
	typeFindUsages      bool
	typeFindInfo        bool
	typeFindDoc         bool
)

//func init
func init() {
	cmdType.Flag.BoolVar(&typeVerbose, "v", false, "verbose debugging")
	cmdType.Flag.BoolVar(&typeAllowBinary, "b", false, "import can be satisfied by a compiled package object without corresponding sources.")
	cmdType.Flag.BoolVar(&typeList, "list", false, "print type list")
	cmdType.Flag.StringVar(&typeFileCursor, "cursor", "", "file cursor \"file.go:pos\"")
	cmdType.Flag.BoolVar(&typeFileCursorStdin, "cursor_stdin", false, "cursor file use stdin")
	cmdType.Flag.BoolVar(&typeFindInfo, "info", false, "print file cursor info")
	cmdType.Flag.BoolVar(&typeFindUsages, "usages", false, "find file cursor usages")
}

type ObjKind int

const (
	ObjNone ObjKind = iota
	ObjPkgName
	ObjTypeName
	ObjInterface
	ObjStruct
	ObjConst
	ObjVar
	ObjField
	ObjFunc
	ObjMethod
	ObjLabel
	ObjBuiltin
	ObjNil
)

var ObjKindName = []string{"none", "package",
	"type", "interface", "struct",
	"const", "var", "field",
	"func", "method",
	"label", "builtin", "nil"}

func (k ObjKind) String() string {
	if k >= 0 && int(k) < len(ObjKindName) {
		return ObjKindName[k]
	}
	return "unkwnown"
}

var builtinInfoMap = map[string]string{
	"append":  "func append(slice []Type, elems ...Type) []Type",
	"copy":    "func copy(dst, src []Type) int",
	"delete":  "func delete(m map[Type]Type1, key Type)",
	"len":     "func len(v Type) int",
	"cap":     "func cap(v Type) int",
	"make":    "func make(Type, size IntegerType) Type",
	"new":     "func new(Type) *Type",
	"complex": "func complex(r, i FloatType) ComplexType",
	"real":    "func real(c ComplexType) FloatType",
	"imag":    "func imag(c ComplexType) FloatType",
	"close":   "func close(c chan<- Type)",
	"panic":   "func panic(v interface{})",
	"recover": "func recover() interface{}",
	"print":   "func print(args ...Type)",
	"println": "func println(args ...Type)",
	"error":   "type error interface {Error() string}",
}

func builtinInfo(id string) string {
	if info, ok := builtinInfoMap[id]; ok {
		return "builtin " + info
	}
	return "builtin " + id
}

func simpleType(src string) string {
	re, _ := regexp.Compile("[\\w\\./]+")
	return re.ReplaceAllStringFunc(src, func(s string) string {
		r := s
		if i := strings.LastIndex(s, "/"); i != -1 {
			r = s[i+1:]
		}
		if strings.Count(r, ".") > 1 {
			r = r[strings.Index(r, ".")+1:]
		}
		return r
	})
}

func runType(cmd *Command, args []string) {
	if len(args) < 1 {
		cmd.Usage()
	}
	if typeVerbose {
		now := time.Now()
		defer func() {
			log.Println("time", time.Now().Sub(now))
		}()
	}

	w := NewPkgWalker(&build.Default)
	var cursor *FileCursor
	if typeFileCursor != "" {
		var cursorInfo FileCursor
		pos := strings.Index(typeFileCursor, ":")
		if pos != -1 {
			cursorInfo.fileName = typeFileCursor[:pos]
			if i, err := strconv.Atoi(typeFileCursor[pos+1:]); err == nil {
				cursorInfo.cursorPos = i
			}
		}
		if typeFileCursorStdin {
			src, err := ioutil.ReadAll(os.Stdin)
			if err == nil {
				cursorInfo.src = src
			}
		}
		cursor = &cursorInfo
	}
	if args[0] == "..." {
		for _, path := range paths("") {
			w.Import("", path, true, true, nil, nil)
		}
	} else if args[0] == "std" {
		for _, path := range stdPkg {
			w.Import("", path, true, true, nil, nil)
		}
	} else {
		for _, pkgName := range args {
			if pkgName == "." {
				pkgPath, err := os.Getwd()
				if err != nil {
					log.Fatalln(err)
				}
				pkgName = pkgPath
			}
			pkgInfo := &types.Info{}
			//pkgInfo.Types = make(map[ast.Expr]types.TypeAndValue)
			pkgInfo.Uses = make(map[*ast.Ident]types.Object)
			pkgInfo.Defs = make(map[*ast.Ident]types.Object)
			//pkgInfo.Selections = make(map[*ast.SelectorExpr]*types.Selection)
			//pkgInfo.Scopes = make(map[ast.Node]*types.Scope)
			//pkgInfo.Implicits = make(map[ast.Node]types.Object)
			pkg, err := w.Import("", pkgName, false, true, cursor, pkgInfo)
			if pkg == nil {
				log.Fatalln("error import path", err)
			}
			if cursor != nil {
				w.LookupCursor(pkg, pkgInfo, cursor)
			}
		}
	}
	return
}

type FileCursor struct {
	pkg       string
	fileName  string
	fileDir   string
	cursorPos int
	pos       token.Pos
	src       interface{}
	std       bool
}

func NewPkgWalker(context *build.Context) *PkgWalker {
	return &PkgWalker{
		context:         context,
		fset:            token.NewFileSet(),
		parsedFileCache: map[string]*ast.File{},
		imported:        map[string]*types.Package{"unsafe": types.Unsafe},
		gcimporter:      map[string]*types.Package{"unsafe": types.Unsafe},
	}
}

type PkgWalker struct {
	fset            *token.FileSet
	context         *build.Context
	current         *types.Package
	importing       types.Package
	parsedFileCache map[string]*ast.File
	imported        map[string]*types.Package // packages already imported
	gcimporter      map[string]*types.Package
}

func contains(list []string, s string) bool {
	for _, t := range list {
		if t == s {
			return true
		}
	}
	return false
}

func (w *PkgWalker) ParserPkg(name string, allowBinary bool, cursor *FileCursor, info *types.Info) (pkg *types.Package, err error) {
	return w.Import("", name, true, allowBinary, cursor, info)
}

func (w *PkgWalker) Import(parentDir string, name string, ignoreFuncBodies bool, allowBinary bool, cursor *FileCursor, info *types.Info) (pkg *types.Package, err error) {
	if strings.HasPrefix(name, ".") && parentDir != "" {
		name = filepath.Join(parentDir, name)
	}
	pkg = w.imported[name]
	if pkg != nil {
		if pkg == &w.importing {
			return nil, fmt.Errorf("cycle importing package %q", name)
		}
		return pkg, nil
	}

	if typeVerbose {
		log.Println("parser pkg", name)
	}

	var bp *build.Package
	if filepath.IsAbs(name) {
		bp, err = w.context.ImportDir(name, 0)
	} else {
		bp, err = w.context.Import(name, "", 0)
	}

	checkName := name

	if bp.ImportPath == "." {
		checkName = bp.Name
	} else {
		checkName = bp.ImportPath
	}

	if err != nil {
		return nil, err
		//if _, nogo := err.(*build.NoGoError); nogo {
		//	return
		//}
		//return
		//log.Fatalf("pkg %q, dir %q: ScanDir: %v", name, info.Dir, err)
	}

	filenames := append(append([]string{}, bp.GoFiles...), bp.CgoFiles...)
	filenames = append(filenames, bp.TestGoFiles...)

	if name == "runtime" {
		n := fmt.Sprintf("zgoos_%s.go", w.context.GOOS)
		if !contains(filenames, n) {
			filenames = append(filenames, n)
		}

		n = fmt.Sprintf("zgoarch_%s.go", w.context.GOARCH)
		if !contains(filenames, n) {
			filenames = append(filenames, n)
		}
	}

	// Parse package files.
	var files []*ast.File
	for _, file := range filenames {
		var f *ast.File
		if cursor != nil && cursor.fileName == file {
			f, err = w.parseFile(bp.Dir, file, cursor.src)
			cursor.pos = token.Pos(w.fset.File(f.Pos()).Base()) + token.Pos(cursor.cursorPos)
			cursor.fileDir = bp.Dir
		} else {
			f, err = w.parseFile(bp.Dir, file, nil)
		}
		if err != nil && typeVerbose {
			log.Printf("error parsing package %s: %s\n", name, err)
		}
		files = append(files, f)
	}

	var xfiles []*ast.File
	for _, file := range bp.XTestGoFiles {
		var f *ast.File
		if cursor != nil && cursor.fileName == file {
			f, err = w.parseFile(bp.Dir, file, cursor.src)
			cursor.pos = token.Pos(w.fset.File(f.Pos()).Base()) + token.Pos(cursor.cursorPos)
			cursor.fileDir = bp.Dir
		} else {
			f, err = w.parseFile(bp.Dir, file, nil)
		}
		if err != nil && typeVerbose {
			log.Printf("error parsing package %s: %s\n", name, err)
		}
		xfiles = append(xfiles, f)
	}

	conf := types.Config{
		IgnoreFuncBodies: ignoreFuncBodies,
		FakeImportC:      true,
		Packages:         w.gcimporter,
		Import: func(imports map[string]*types.Package, name string) (pkg *types.Package, err error) {
			pkg = w.imported[name]
			if pkg != nil {
				return pkg, nil
			}
			if allowBinary && isStdPkg(name) {
				pkg = w.gcimporter[name]
				if pkg != nil && pkg.Complete() {
					return
				}
				pkg, err = gcimporter.Import(imports, name)
				if pkg != nil && pkg.Complete() {
					w.gcimporter[name] = pkg
					return
				}
			}
			return w.Import(bp.Dir, name, true, true, nil, nil)
		},
		Error: func(err error) {
			if typeVerbose {
				log.Println(err)
			}
		},
	}
	if pkg == nil {
		pkg, err = conf.Check(checkName, w.fset, files, info)
	}
	w.imported[name] = pkg

	if len(xfiles) > 0 {
		xpkg, _ := conf.Check(checkName+"_test", w.fset, xfiles, info)
		w.imported[checkName+"_test"] = xpkg
	}
	return
}

func (w *PkgWalker) parseFile(dir, file string, src interface{}) (*ast.File, error) {
	filename := filepath.Join(dir, file)
	f, _ := w.parsedFileCache[filename]
	if f != nil {
		return f, nil
	}

	var err error

	// generate missing context-dependent files.
	if w.context != nil && file == fmt.Sprintf("zgoos_%s.go", w.context.GOOS) {
		src := fmt.Sprintf("package runtime; const theGoos = `%s`", w.context.GOOS)
		f, err = parser.ParseFile(w.fset, filename, src, 0)
		if err != nil {
			log.Fatalf("incorrect generated file: %s", err)
		}
	}

	if w.context != nil && file == fmt.Sprintf("zgoarch_%s.go", w.context.GOARCH) {
		src := fmt.Sprintf("package runtime; const theGoarch = `%s`", w.context.GOARCH)
		f, err = parser.ParseFile(w.fset, filename, src, 0)
		if err != nil {
			log.Fatalf("incorrect generated file: %s", err)
		}
	}

	if f == nil {
		f, err = parser.ParseFile(w.fset, filename, src, parser.AllErrors) //|parser.ParseComments)
		if err != nil {
			return f, err
		}
	}

	w.parsedFileCache[filename] = f
	return f, nil
}

func (w *PkgWalker) LookupCursor(pkg *types.Package, pkgInfo *types.Info, cursor *FileCursor) {
	is := w.CheckIsImport(cursor)
	if is != nil {
		w.LookupImport(pkg, pkgInfo, cursor, is)
	} else {
		w.LookupObjects(pkg, pkgInfo, cursor)
	}
}

func (w *PkgWalker) LookupImport(pkg *types.Package, pkgInfo *types.Info, cursor *FileCursor, is *ast.ImportSpec) {
	fpath, err := strconv.Unquote(is.Path.Value)
	if err != nil {
		return
	}
	fbase := fpath
	pos := strings.LastIndexAny(fpath, "./-\\")
	if pos != -1 {
		fbase = fpath[pos+1:]
	}
	fid := fpath + "." + fbase
	kind := ObjPkgName
	fmt.Println(kind)
	fmt.Println("package", fpath)
	fmt.Println(w.fset.Position(is.Pos()))
	if !typeFindUsages {
		return
	}
	var usages []int
	for id, obj := range pkgInfo.Uses {
		if obj != nil && obj.Id() == fid { //!= nil && cursorObj.Pos() == obj.Pos() {
			usages = append(usages, int(id.Pos()))
		}
	}
	(sort.IntSlice(usages)).Sort()
	for _, pos := range usages {
		fmt.Println(w.fset.Position(token.Pos(pos)))
	}

}

func (w *PkgWalker) LookupObjects(pkg *types.Package, pkgInfo *types.Info, cursor *FileCursor) {
	var cursorObj types.Object
	var kind ObjKind
	//lookup defs
	for id, obj := range pkgInfo.Defs {
		if cursor.pos >= id.Pos() && cursor.pos <= id.End() {
			cursorObj = obj
			break
		}
	}
	if cursorObj == nil {
		for id, obj := range pkgInfo.Uses {
			if cursor.pos >= id.Pos() && cursor.pos <= id.End() {
				cursorObj = obj
				break
			}
		}
	}
	if cursorObj == nil {
		return
	}
	switch t := cursorObj.(type) {
	case *types.PkgName:
		kind = ObjPkgName
	case *types.Const:
		kind = ObjConst
	case *types.TypeName:
		kind = ObjTypeName
		switch t.Type().Underlying().(type) {
		case *types.Interface:
			kind = ObjInterface
		case *types.Struct:
			kind = ObjStruct
		}
	case *types.Var:
		kind = ObjVar
		if t.IsField() {
			kind = ObjField
		}
	case *types.Func:
		kind = ObjFunc
		if sig, ok := t.Type().(*types.Signature); ok {
			if sig.Recv() != nil {
				kind = ObjMethod
			}
		}
	case *types.Label:
		kind = ObjLabel
	case *types.Builtin:
		kind = ObjBuiltin
	case *types.Nil:
		kind = ObjNil
	default:
		log.Fatalf("unknown obj type %T", cursorObj)
	}
	cursorPkg := cursorObj.Pkg()
	cursorPos := cursorObj.Pos()
	var fieldTypeInfo *types.Info
	var fieldTypeObj types.Object
	if cursorPkg == pkg {
		fieldTypeInfo = pkgInfo
	}
	if cursorPkg != nil && cursorPkg != pkg &&
		kind != ObjPkgName && isStdPkg(cursorPkg.Path()) {
		ti := &types.Info{}
		ti.Defs = make(map[*ast.Ident]types.Object)
		//ti.Types = make(map[ast.Expr]types.TypeAndValue)
		pkg, _ := w.Import("", cursorPkg.Path(), true, true, nil, ti)
		if pkg != nil {
			for _, obj := range ti.Defs {
				if obj != nil && obj.String() == cursorObj.String() {
					cursorPos = obj.Pos()
					break
				}
			}
		}
		if kind == ObjField {
			fieldTypeInfo = ti
		}
	}
	if kind == ObjField {
		if fieldTypeInfo == nil {
			ti := &types.Info{}
			ti.Defs = make(map[*ast.Ident]types.Object)
			//ti.Types = make(map[ast.Expr]types.TypeAndValue)
			//ti.Implicits = make(map[ast.Node]types.Object)
			w.imported[cursorPkg.Path()] = nil
			pkg, _ := w.Import("", cursorPkg.Path(), true, true, nil, ti)
			if pkg != nil {
				fieldTypeInfo = ti
			}
		}
		//log.Println(fieldTypeInfo.Implicits)
	loop:
		for _, obj := range fieldTypeInfo.Defs {
			if obj == nil {
				continue
			}
			if _, ok := obj.(*types.TypeName); ok {
				//if obj.Type().(*types.TypeName)
				if t, ok := obj.Type().Underlying().(*types.Struct); ok {
					for i := 0; i < t.NumFields(); i++ {
						if t.Field(i).Pos() == cursorPos {
							fieldTypeObj = obj
							break loop
						}
					}
				}
			}
		}
	}
	fmt.Println(kind)
	if kind == ObjField && fieldTypeObj != nil {
		fmt.Println(simpleType(fieldTypeObj.String()))
		fmt.Println(simpleType(cursorObj.String()))
	} else if kind == ObjBuiltin {
		fmt.Println(builtinInfo(cursorObj.Name()))
	} else {
		fmt.Println(simpleType(cursorObj.String()))
	}
	fmt.Println(w.fset.Position(cursorPos))
	//if f, ok := w.parsedFileCache[w.fset.Position(cursorPos).Filename]; ok {
	//	for _, d := range f.Decls {
	//		if inRange(d, cursorPos) {
	//			if fd, ok := d.(*ast.FuncDecl); ok {
	//				fd.Body = nil
	//			}
	//			commentMap := ast.NewCommentMap(w.fset, f, f.Comments)
	//			commentedNode := printer.CommentedNode{Node: d}
	//			if comments := commentMap.Filter(d).Comments(); comments != nil {
	//				commentedNode.Comments = comments
	//			}
	//			var b bytes.Buffer
	//			printer.Fprint(&b, w.fset, &commentedNode)
	//			b.Write([]byte("\n\n")) // Add a blank line between entries if we print documentation.
	//			log.Println(w.nodeString(d))
	//		}
	//	}
	//}

	if !typeFindUsages {
		return
	}
	_ = kind
	var usages []int
	if kind == ObjPkgName {
		for id, obj := range pkgInfo.Uses {
			if obj != nil && obj.Id() == cursorObj.Id() { //!= nil && cursorObj.Pos() == obj.Pos() {
				usages = append(usages, int(id.Pos()))
			}
		}
	} else {
		for id, obj := range pkgInfo.Defs {
			if obj == cursorObj { //!= nil && cursorObj.Pos() == obj.Pos() {
				usages = append(usages, int(id.Pos()))
			}
		}
		for id, obj := range pkgInfo.Uses {
			if obj == cursorObj { //!= nil && cursorObj.Pos() == obj.Pos() {
				usages = append(usages, int(id.Pos()))
			}
		}
	}
	(sort.IntSlice(usages)).Sort()
	for _, pos := range usages {
		fmt.Println(w.fset.Position(token.Pos(pos)))
	}
}

func (w *PkgWalker) CheckIsImport(cursor *FileCursor) *ast.ImportSpec {
	if cursor.fileDir == "" {
		return nil
	}
	file, _ := w.parseFile(cursor.fileDir, cursor.fileName, cursor.src)
	if file == nil {
		return nil
	}
	for _, is := range file.Imports {
		if inRange(is, cursor.pos) {
			return is
		}
	}
	//for _, di := range file.Decls {
	//	if d, ok := di.(*ast.GenDecl); ok {
	//		if inRange(d, cursor.pos) {
	//			switch d.Tok {
	//			case token.IMPORT:
	//				for _, sp := range d.Specs {
	//					if inRange(sp, cursor.pos) {
	//						return sp.(*ast.ImportSpec)
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	return nil
}

// func (w *PkgWalker) LookupInfo(pkg *types.Package, info *typeFileCursor) {
//	file, _ := w.parseFile(info.fileDir, info.fileName, info.src)
//	if file == nil {
//		return
//	}
//	log.Println("looup info", info)

//	log.Println(pkg.Scope().Child(0).Child(0).Child(0).Names())

//	f := w.fset.File(file.Pos())
//	typeInfo, err := w.lookupFile(pkg, file, token.Pos(f.Base())+info.pos-1)
//	if typeInfo == nil {
//		log.Println("error lookup", err)
//	} else {
//		log.Println(typeInfo, w.fset.Position(typeInfo.Obj.Pos()), err)
//	}
//}

func (w *PkgWalker) nodeString(node interface{}) string {
	if node == nil {
		return ""
	}
	var b bytes.Buffer
	printer.Fprint(&b, w.fset, node)
	return b.String()
}

//type TypeObj struct {
//	Kind     Kind           //type kind
//	Name     string         //expr name
//	TypeName string         //type name
//	Expr     ast.Expr       //expr
//	Pkg      *types.Package //expr package
//	Obj      types.Object   //object
//	TypeObj  types.Object   //type object
//	IsType   bool
//}

//func (i *TypeObj) String() string {
//	return i.Name + "," + i.TypeName
//}

//func (w *PkgWalker) lookupFile(pkg *types.Package, file *ast.File, p token.Pos) (*TypeObj, error) {
//	if inRange(file.Name, p) {
//		return &TypeObj{Kind: KindPackage,
//			Name:     file.Name.Name,
//			TypeName: "package",
//			Expr:     file.Name,
//			Obj:      nil,
//			TypeObj:  nil,
//			Pkg:      pkg}, nil
//	}
//	for _, di := range file.Decls {
//		switch d := di.(type) {
//		case *ast.GenDecl:
//			if inRange(d, p) {
//				return w.lookupDecl(pkg, file, d, p, false)
//			}
//		case *ast.FuncDecl:
//			//if inRange(d, p) {
//			//	info, err := w.lookupDecl(d, p, false)
//			//	if info != nil && info.Kind == KindBranch {
//			//		return w.lookupLabel(d.Body, info.Name)
//			//	}
//			//	return info, err
//			//}
//			//if d.Body != nil && inRange(d.Body, p) {
//			//	return w.lookupStmt(d.Body, p)
//			//}
//		default:
//			return nil, fmt.Errorf("un parser decl %T", di)
//		}
//	}
//	return nil, fmt.Errorf("lookupFile %v", w.fset.Position(p))
//}

//func (w *PkgWalker) lookupDecl(pkg *types.Package, file *ast.File, di ast.Decl, p token.Pos, local bool) (*TypeObj, error) {
//	switch d := di.(type) {
//	case *ast.GenDecl:
//		switch d.Tok {
//		case token.IMPORT:
//			for _, sp := range d.Specs {
//				is := sp.(*ast.ImportSpec)
//				fpath, err := strconv.Unquote(is.Path.Value)
//				if err != nil {
//					return nil, err
//				}
//				if inRange(sp, p) {
//					importPkg, _ := w.Import("", fpath, true, true, nil)
//					return &TypeObj{Kind: KindImport,
//						Name:     fpath,
//						TypeName: "import",
//						Expr:     is.Path,
//						Obj:      nil,
//						TypeObj:  nil,
//						Pkg:      importPkg}, nil
//				}
//			}
//		case token.CONST:
//			for _, sp := range d.Specs {
//				if inRange(sp, p) {
//					return w.lookupConst(pkg, file, sp.(*ast.ValueSpec), p, local)
//				}
//			}
//			return nil, nil
//		case token.TYPE:
//			for _, sp := range d.Specs {
//				if inRange(sp, p) {
//					return w.lookupType(pkg, sp.(*ast.TypeSpec), p, local)
//				} else {
//					w.lookupType(pkg, sp.(*ast.TypeSpec), p, local)
//				}
//			}
//		case token.VAR:
//			for _, sp := range d.Specs {
//				if inRange(sp, p) {
//					return w.lookupVar(pkg, sp.(*ast.ValueSpec), p, local)
//				} else {
//					w.lookupVar(pkg, sp.(*ast.ValueSpec), p, local)
//				}
//			}
//			return nil, nil
//		default:
//			return nil, fmt.Errorf("unknown token type %d %T in GenDecl", d.Tok, d)
//		}
//	case *ast.FuncDecl:
//		//if d.Type.Params != nil {
//		//	for _, fd := range d.Type.Params.List {
//		//		if inRange(fd, p) {
//		//			return w.lookupExprInfo(fd.Type, p)
//		//		}
//		//		for _, ident := range fd.Names {
//		//			if inRange(ident, p) {
//		//				info, err := w.lookupExprInfo(fd.Type, p)
//		//				if err == nil {
//		//					return &TypeInfo{Kind: KindParam, X: ident, Name: ident.Name, T: info.T, Type: info.Type}, nil
//		//				}
//		//			}
//		//			typ, err := w.varValueType(fd.Type, 0)
//		//			if err == nil {
//		//				w.localvar[ident.Name] = &ExprType{T: typ, X: ident}
//		//			} else if apiVerbose {
//		//				log.Println(err)
//		//			}
//		//		}
//		//	}
//		//}
//		//if d.Type.Results != nil {
//		//	for _, fd := range d.Type.Results.List {
//		//		if inRange(fd, p) {
//		//			return w.lookupExprInfo(fd.Type, p)
//		//		}
//		//		for _, ident := range fd.Names {
//		//			typ, err := w.varValueType(fd.Type, 0)
//		//			if err == nil {
//		//				w.localvar[ident.Name] = &ExprType{T: typ, X: ident}
//		//			}
//		//		}
//		//	}
//		//}
//		//if d.Recv != nil {
//		//	for _, fd := range d.Recv.List {
//		//		if inRange(fd, p) {
//		//			return w.lookupExprInfo(fd.Type, p)
//		//		}
//		//		for _, ident := range fd.Names {
//		//			w.localvar[ident.Name] = &ExprType{T: w.nodeString(fd.Type), X: ident}
//		//		}
//		//	}
//		//}
//		//if inRange(d.Body, p) {
//		//	return w.lookupStmt(d.Body, p)
//		//}
//		//var fname = d.Name.Name
//		//kind := KindFunc
//		//if d.Recv != nil {
//		//	recvTypeName, imp := baseTypeName(d.Recv.List[0].Type)
//		//	if imp {
//		//		return nil, nil
//		//	}
//		//	fname = recvTypeName + "." + d.Name.Name
//		//	kind = KindMethod
//		//}
//		//return &TypeInfo{Kind: kind, X: d.Name, Name: fname, T: d.Type, Type: w.nodeString(w.namelessType(d.Type))}, nil
//	default:
//		return nil, fmt.Errorf("unhandled %T, %#v\n", di, di)
//	}
//	return nil, fmt.Errorf("not lookupDecl %v %T", w.nodeString(di), di)
//}

//func (w *PkgWalker) lookupImportPath(file *ast.File, name string) string {
//	for _, is := range file.Imports {
//		fpath, _ := strconv.Unquote(is.Path.Value)
//		if (is.Name != nil && is.Name.Name == name) || path.Base(fpath) == name {
//			return fpath
//		}
//	}
//	return ""
//}

//func (w *PkgWalker) lookupConstType(pkg *types.Package, file *ast.File, vs *ast.ValueSpec) types.Object {
//	switch expr := vs.Type.(type) {
//	case *ast.Ident:
//		return pkg.Scope().Lookup(types.ExprString(expr))
//	case *ast.SelectorExpr:
//		importPath := w.lookupImportPath(file, types.ExprString(expr.X))
//		importPkg, _ := w.Import("", importPath, true, true, nil)
//		if importPkg != nil {
//			return importPkg.Scope().Lookup(expr.Sel.Name)
//		}
//	default:
//		log.Fatalf("lookupConst type %T", vs.Type)
//	}
//	return nil
//}

//func (w *PkgWalker) lookupConst(pkg *types.Package, file *ast.File, vs *ast.ValueSpec, p token.Pos, local bool) (*TypeObj, error) {
//	if inRange(vs.Type, p) {
//		//return w.lookupExprInfo(vs.Type, p)
//		obj := w.lookupConstType(pkg, file, vs)
//		return &TypeObj{Kind: KindConst,
//			Name:     types.ExprString(vs.Type),
//			TypeName: obj.Name(),
//			Expr:     vs.Type,
//			Obj:      obj,
//			TypeObj:  obj,
//			Pkg:      pkg,
//			IsType:   true}, nil
//	}
//	for _, ident := range vs.Names {
//		if inRange(ident, p) {
//			obj := pkg.Scope().Lookup(ident.Name)
//			typeObj := w.lookupConstType(pkg, file, vs)
//			if obj != nil {
//				return &TypeObj{Kind: KindConst,
//					Name:     ident.Name,
//					TypeName: types.ExprString(vs.Type),
//					Expr:     ident,
//					Obj:      obj,
//					TypeObj:  typeObj,
//					Pkg:      pkg}, nil

//			}
//		}
//	}
//	for _, expr := range vs.Values {
//		if inRange(expr, p) {
//			return w.lookupExpr(pkg, expr, p, local)
//		}
//	}
//	return nil, fmt.Errorf("error looup const %v", w.fset.Position(p))
//}

//func (w *PkgWalker) lookupVar(pkg *types.Package, vs *ast.ValueSpec, p token.Pos, local bool) (*TypeObj, error) {
//	return nil, nil
//}

//func (w *PkgWalker) lookupType(pkg *types.Package, vs *ast.TypeSpec, p token.Pos, local bool) (*TypeObj, error) {
//	return nil, nil
//}

//func (w *PkgWalker) lookupExpr(pkg *types.Package, expr ast.Expr, p token.Pos, local bool) (*TypeObj, error) {
//	switch v := expr.(type) {
//	case *ast.Ident:
//		obj := pkg.Scope().Lookup(v.Name)
//		if obj == nil {
//			obj = types.Universe.Lookup(v.Name)
//		}
//		log.Println(obj, v.Name)
//	}
//	return nil, fmt.Errorf("error lookupExpr %v %T", w.nodeString(expr), expr)
//}
