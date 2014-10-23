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
	typeFileCursor      string
	typeFileCursorStdin bool
	typeFindUse         bool
	typeFindDef         bool
	typeFindInfo        bool
	typeFindDoc         bool
)

//func init
func init() {
	cmdType.Flag.BoolVar(&typeVerbose, "v", false, "verbose debugging")
	cmdType.Flag.BoolVar(&typeAllowBinary, "b", false, "import can be satisfied by a compiled package object without corresponding sources.")
	cmdType.Flag.StringVar(&typeFileCursor, "cursor", "", "file cursor \"file.go:pos\"")
	cmdType.Flag.BoolVar(&typeFileCursorStdin, "cursor_stdin", false, "cursor file use stdin")
	cmdType.Flag.BoolVar(&typeFindInfo, "info", false, "find cursor info")
	cmdType.Flag.BoolVar(&typeFindDef, "def", false, "find cursor define")
	cmdType.Flag.BoolVar(&typeFindUse, "use", false, "find cursor usages")
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
		conf := &PkgConfig{IgnoreFuncBodies: true, AllowBinary: true, WithTestFiles: true}
		for _, path := range paths("") {
			w.Import("", path, conf)
		}
	} else if args[0] == "std" {
		conf := &PkgConfig{IgnoreFuncBodies: true, AllowBinary: true, WithTestFiles: true}
		for _, path := range stdPkg {
			w.Import("", path, conf)
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
			conf := &PkgConfig{IgnoreFuncBodies: true, AllowBinary: true, WithTestFiles: true}
			if cursor != nil {
				conf.Cursor = cursor
				conf.IgnoreFuncBodies = false
				conf.Info = &types.Info{
					Uses: make(map[*ast.Ident]types.Object),
					Defs: make(map[*ast.Ident]types.Object),
					//Types : make(map[ast.Expr]types.TypeAndValue)
					Selections: make(map[*ast.SelectorExpr]*types.Selection),
					//Scopes : make(map[ast.Node]*types.Scope)
					//Implicits : make(map[ast.Node]types.Object)
				}
			}
			pkg, err := w.Import("", pkgName, conf)
			if pkg == nil {
				log.Fatalln("error import path", err)
			}
			if cursor != nil && (typeFindInfo || typeFindDef || typeFindUse) {
				w.LookupCursor(pkg, conf.Info, cursor)
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
}

type PkgConfig struct {
	IgnoreFuncBodies bool
	AllowBinary      bool
	WithTestFiles    bool
	Cursor           *FileCursor
	Info             *types.Info
	Files            map[string]*ast.File
	TestFiles        map[string]*ast.File
	XTestFiles       map[string]*ast.File
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

func (w *PkgWalker) isBinaryPkg(pkg string) bool {
	return isStdPkg(pkg)
}

func (w *PkgWalker) Import(parentDir string, name string, conf *PkgConfig) (pkg *types.Package, err error) {
	defer func() {
		err := recover()
		if err != nil && typeVerbose {
			log.Println(err)
		}
	}()

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
	if conf.WithTestFiles {
		filenames = append(filenames, bp.TestGoFiles...)
	}

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

	parserFiles := func(filenames []string, cursor *FileCursor) (files []*ast.File) {
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
		return
	}
	files := parserFiles(filenames, conf.Cursor)
	xfiles := parserFiles(bp.XTestGoFiles, conf.Cursor)

	typesConf := types.Config{
		IgnoreFuncBodies: conf.IgnoreFuncBodies,
		FakeImportC:      true,
		Packages:         w.gcimporter,
		Import: func(imports map[string]*types.Package, name string) (pkg *types.Package, err error) {
			if pkg != nil {
				return pkg, nil
			}
			if conf.AllowBinary && w.isBinaryPkg(name) {
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
			return w.Import(bp.Dir, name, &PkgConfig{IgnoreFuncBodies: true, AllowBinary: true, WithTestFiles: false})
		},
		Error: func(err error) {
			if typeVerbose {
				log.Println(err)
			}
		},
	}
	if pkg == nil {
		pkg, err = typesConf.Check(checkName, w.fset, files, conf.Info)
	}
	w.imported[name] = pkg

	if len(xfiles) > 0 {
		xpkg, _ := typesConf.Check(checkName+"_test", w.fset, xfiles, conf.Info)
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
	//kind := ObjPkgName
	//fmt.Println(kind, true)

	if typeFindDef {
		fmt.Println(w.fset.Position(is.Pos()))
	}
	if typeFindInfo {
		fmt.Println("package", fpath)
	}
	if !typeFindUse {
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

func parserObjKind(obj types.Object) (ObjKind, error) {
	var kind ObjKind
	switch t := obj.(type) {
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
		return ObjNone, fmt.Errorf("unknown obj type %T", obj)
	}
	return kind, nil
}

func (w *PkgWalker) LookupStructFromField(info *types.Info, cursorPkg *types.Package, cursorObj types.Object, cursorPos token.Pos) types.Object {
	if info == nil {
		conf := &PkgConfig{
			IgnoreFuncBodies: true,
			AllowBinary:      true,
			WithTestFiles:    true,
			Info: &types.Info{
				Defs: make(map[*ast.Ident]types.Object),
			},
		}
		w.imported[cursorPkg.Path()] = nil
		pkg, _ := w.Import("", cursorPkg.Path(), conf)
		if pkg != nil {
			info = conf.Info
		}
	}
	for _, obj := range info.Defs {
		if obj == nil {
			continue
		}
		if _, ok := obj.(*types.TypeName); ok {
			if t, ok := obj.Type().Underlying().(*types.Struct); ok {
				for i := 0; i < t.NumFields(); i++ {
					if t.Field(i).Pos() == cursorPos {
						return obj
					}
				}
			}
		}
	}
	return nil
}

func (w *PkgWalker) lookupNamedMethod(named *types.Named, name string) (types.Object, *types.Named) {
	if iface, ok := named.Underlying().(*types.Interface); ok {
		for i := 0; i < iface.NumMethods(); i++ {
			fn := iface.Method(i)
			if fn.Name() == name {
				return fn, named
			}
		}
		for i := 0; i < iface.NumEmbeddeds(); i++ {
			if obj, na := w.lookupNamedMethod(iface.Embedded(i), name); obj != nil {
				return obj, na
			}
		}
		return nil, nil
	}
	if istruct, ok := named.Underlying().(*types.Struct); ok {
		for i := 0; i < named.NumMethods(); i++ {
			fn := named.Method(i)
			if fn.Name() == name {
				return fn, named
			}
		}
		for i := 0; i < istruct.NumFields(); i++ {
			field := istruct.Field(i)
			if !field.Anonymous() {
				continue
			}
			if typ, ok := field.Type().(*types.Named); ok {
				if obj, na := w.lookupNamedMethod(typ, name); obj != nil {
					return obj, na
				}
			}
		}
	}
	return nil, nil
}

func (w *PkgWalker) LookupObjects(pkg *types.Package, pkgInfo *types.Info, cursor *FileCursor) {
	var cursorObj types.Object
	var cursorSelection *types.Selection
	var cursorObjIsDef bool
	//lookup defs

	_ = cursorObjIsDef
	if cursorObj == nil {
		for sel, obj := range pkgInfo.Selections {
			if cursor.pos >= sel.Sel.Pos() && cursor.pos <= sel.Sel.End() {
				cursorObj = obj.Obj()
				cursorSelection = obj
				break
			}
		}
	}
	if cursorObj == nil {
		for id, obj := range pkgInfo.Defs {
			if cursor.pos >= id.Pos() && cursor.pos <= id.End() {
				cursorObj = obj
				cursorObjIsDef = true
				break
			}
		}
	}
	_ = cursorSelection
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
	kind, err := parserObjKind(cursorObj)
	if err != nil {
		log.Fatalln(err)
	}
	if kind == ObjField {
		if cursorObj.(*types.Var).Anonymous() {
			if named, ok := cursorObj.Type().(*types.Named); ok {
				cursorObj = named.Obj()
			}
		}
	}
	cursorPkg := cursorObj.Pkg()
	cursorPos := cursorObj.Pos()
	var fieldTypeInfo *types.Info
	var fieldTypeObj types.Object
	if cursorPkg == pkg {
		fieldTypeInfo = pkgInfo
	}
	cursorIsInterfaceMethod := false
	var cursorInterfaceTypeName string
	if kind == ObjMethod && cursorSelection != nil && cursorSelection.Recv() != nil {
		sig := cursorObj.(*types.Func).Type().Underlying().(*types.Signature)
		if _, ok := sig.Recv().Type().Underlying().(*types.Interface); ok {
			named := cursorSelection.Recv().(*types.Named)
			obj, typ := w.lookupNamedMethod(named, cursorObj.Name())
			if obj != nil {
				cursorObj = obj
			}
			if typ != nil {
				cursorPkg = typ.Obj().Pkg()
				cursorInterfaceTypeName = typ.Obj().Name()
			}
			cursorIsInterfaceMethod = true
		}
	}

	if cursorPkg != nil && cursorPkg != pkg &&
		kind != ObjPkgName && w.isBinaryPkg(cursorPkg.Path()) {
		conf := &PkgConfig{
			IgnoreFuncBodies: true,
			AllowBinary:      true,
			WithTestFiles:    true,
			Info: &types.Info{
				Defs: make(map[*ast.Ident]types.Object),
			},
		}
		pkg, _ := w.Import("", cursorPkg.Path(), conf)
		if pkg != nil {
			if cursorIsInterfaceMethod {
				for _, obj := range conf.Info.Defs {
					if obj == nil {
						continue
					}
					if fn, ok := obj.(*types.Func); ok {
						if fn.Name() == cursorObj.Name() {
							if sig, ok := fn.Type().Underlying().(*types.Signature); ok {
								if named, ok := sig.Recv().Type().(*types.Named); ok {
									if named.Obj() != nil && named.Obj().Name() == cursorInterfaceTypeName {
										cursorPos = obj.Pos()
										break
									}
								}
							}
						}
					}
				}
			} else {
				for _, obj := range conf.Info.Defs {
					if obj != nil && obj.String() == cursorObj.String() {
						cursorPos = obj.Pos()
						break
					}
				}
			}
		}
		if kind == ObjField || cursorIsInterfaceMethod {
			fieldTypeInfo = conf.Info
		}
	}
	if kind == ObjField {
		fieldTypeObj = w.LookupStructFromField(fieldTypeInfo, cursorPkg, cursorObj, cursorPos)
	}
	if typeFindDef {
		fmt.Println(w.fset.Position(cursorPos))
	}
	if typeFindInfo {
		if kind == ObjField && fieldTypeObj != nil {
			typeName := fieldTypeObj.Name()
			if fieldTypeObj.Pkg() != nil && fieldTypeObj.Pkg() != pkg {
				typeName = fieldTypeObj.Pkg().Name() + "." + fieldTypeObj.Name()
			}
			fmt.Println(typeName, simpleType(cursorObj.String()))
		} else if kind == ObjBuiltin {
			fmt.Println(builtinInfo(cursorObj.Name()))
		} else if cursorIsInterfaceMethod {
			fmt.Println(strings.Replace(simpleType(cursorObj.String()), "(interface)", cursorPkg.Name()+"."+cursorInterfaceTypeName, 1))
		} else {
			fmt.Println(simpleType(cursorObj.String()))
		}
	}
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
	if !typeFindUse {
		return
	}
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
	return nil
}

func (w *PkgWalker) nodeString(node interface{}) string {
	if node == nil {
		return ""
	}
	var b bytes.Buffer
	printer.Fprint(&b, w.fset, node)
	return b.String()
}
