// Copyright 2013 The rspace Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Doc is a simple document printer that produces the doc comments for its
// argument symbols, plus a link to the full documentation and a pointer to
// the source. It has a more Go-like UI than godoc. It can also search for
// symbols by looking in all packages, and case is ignored. For instance:
//	doc isupper
// will find unicode.IsUpper.
//
// The -pkg flag retrieves package-level doc comments only.
//
// Usage:
//	doc pkg.name   # "doc io.Writer"
//	doc pkg name   # "doc fmt Printf"
//	doc name       # "doc isupper" (finds unicode.IsUpper)
//	doc -pkg pkg   # "doc fmt"
//
// The pkg is the last element of the package path;
// no slashes (ast.Node not go/ast.Node).
//
// Flags
//	-c(onst) -f(unc) -i(nterface) -m(ethod) -s(truct) -t(ype) -v(ar)
// restrict hits to declarations of the corresponding kind.
// Flags
//	-doc -src -url
// restrict printing to the documentation, source path, or godoc URL.
package main

import (
	"bytes"
	"fmt"
	"go/ast"
	"go/parser"
	"go/printer"
	"go/token"
	"os"
	"path"
	"path/filepath"
	"regexp"
	"runtime"
	"strings"

	_ "code.google.com/p/go.tools/go/gcimporter"
	"code.google.com/p/go.tools/go/types"
)

const usageDoc = `Find documentation for names.
usage:
	doc pkg.name   # "doc io.Writer"
	doc pkg name   # "doc fmt Printf"
	doc name       # "doc isupper" finds unicode.IsUpper
	doc -pkg pkg   # "doc fmt"
	doc -r expr    # "doc -r '.*exported'"
pkg is the last component of any package, e.g. fmt, parser
name is the name of an exported symbol; case is ignored in matches.

The name may also be a regular expression to select which names
to match. In regular expression searches, case is ignored and
the pattern must match the entire name, so ".?print" will match
Print, Fprint and Sprint but not Fprintf.

Flags
	-c(onst) -f(unc) -i(nterface) -m(ethod) -s(truct) -t(ype) -v(ar)
restrict hits to declarations of the corresponding kind.
Flags
	-doc -src -url
restrict printing to the documentation, source path, or godoc URL.
Flag
	-r
takes a single argument (no package), a name or regular expression
to search for in all packages.
`

var cmdDoc = &Command{
	Run:       runDoc,
	UsageLine: "doc [pkg.name|pkg name|-pkg name]",
	Short:     "golang doc lookup",
	Long:      usageDoc,
}

var (
	// If none is set, all are set.
	docFlag       bool
	srcFlag       bool
	urlFlag       bool
	regexpFlag    bool
	fuzzyFlag     bool
	constantFlag  bool
	functionFlag  bool
	interfaceFlag bool
	methodFlag    bool
	packageFlag   bool
	structFlag    bool
	typeFlag      bool
	variableFlag  bool
)

func init() {
	cmdDoc.Flag.BoolVar(&docFlag, "doc", false, "restrict output to documentation only")
	cmdDoc.Flag.BoolVar(&srcFlag, "src", false, "restrict output to source file only")
	cmdDoc.Flag.BoolVar(&urlFlag, "url", false, "restrict output to godoc URL only")
	cmdDoc.Flag.BoolVar(&regexpFlag, "r", false, "single argument is a regular expression for a name")
	cmdDoc.Flag.BoolVar(&fuzzyFlag, "fuzzy", true, "fuzzy search for a name")

	cmdDoc.Flag.BoolVar(&constantFlag, "const", false, "show doc for consts only")
	cmdDoc.Flag.BoolVar(&functionFlag, "func", false, "show doc for funcs only")
	cmdDoc.Flag.BoolVar(&interfaceFlag, "interface", false, "show doc for interfaces only")
	cmdDoc.Flag.BoolVar(&methodFlag, "method", false, "show doc for methods only")
	cmdDoc.Flag.BoolVar(&packageFlag, "package", false, "show top-level package doc only")
	cmdDoc.Flag.BoolVar(&structFlag, "struct", false, "show doc for structs only")
	cmdDoc.Flag.BoolVar(&typeFlag, "type", false, "show doc for types only")
	cmdDoc.Flag.BoolVar(&variableFlag, "var", false, "show  doc for vars only")

	cmdDoc.Flag.BoolVar(&constantFlag, "c", false, "alias for -const")
	cmdDoc.Flag.BoolVar(&functionFlag, "f", false, "alias for -func")
	cmdDoc.Flag.BoolVar(&interfaceFlag, "i", false, "alias for -interface")
	cmdDoc.Flag.BoolVar(&methodFlag, "m", false, "alias for -method")
	cmdDoc.Flag.BoolVar(&packageFlag, "pkg", false, "alias for -package")
	cmdDoc.Flag.BoolVar(&structFlag, "s", false, "alias for -struct")
	cmdDoc.Flag.BoolVar(&typeFlag, "t", false, "alias for -type")
	cmdDoc.Flag.BoolVar(&variableFlag, "v", false, "alias for -var")
}

func runDoc(cmd *Command, args []string) {

	if !(constantFlag || functionFlag || interfaceFlag || methodFlag || packageFlag || structFlag || typeFlag || variableFlag) { // none set
		constantFlag = true
		functionFlag = true
		methodFlag = true
		// Not package! It's special.
		typeFlag = true
		variableFlag = true
	}
	if !(docFlag || srcFlag || urlFlag) {
		docFlag = true
		srcFlag = true
		urlFlag = true
	}
	var pkg, name string
	switch len(args) {
	case 1:
		if packageFlag {
			pkg = args[0]
		} else if regexpFlag {
			name = args[0]
		} else if strings.Contains(args[0], ".") {
			pkg, name = split(args[0])
		} else {
			name = args[0]
		}
	case 2:
		if packageFlag {
			cmd.Usage()
		}
		pkg, name = args[0], args[1]
	default:
		cmd.Usage()
	}
	if strings.Contains(pkg, "/") {
		fmt.Fprintf(os.Stderr, "doc: package name cannot contain slash (TODO)\n")
		os.Exit(2)
	}
	for _, path := range paths(pkg) {
		lookInDirectory(path, name)
	}
}

var slash = string(filepath.Separator)
var slashDot = string(filepath.Separator) + "."
var goRootSrcPkg = filepath.Join(runtime.GOROOT(), "src", "pkg")
var goRootSrcCmd = filepath.Join(runtime.GOROOT(), "src", "cmd")
var goPaths = splitGopath()

func split(arg string) (pkg, name string) {
	dot := strings.IndexRune(arg, '.') // We know there's one there.
	return arg[0:dot], arg[dot+1:]
}

func paths(pkg string) []string {
	pkgs := pathsFor(runtime.GOROOT(), pkg)
	for _, root := range goPaths {
		pkgs = append(pkgs, pathsFor(root, pkg)...)
	}
	return pkgs
}

func splitGopath() []string {
	gopath := os.Getenv("GOPATH")
	if gopath == "" {
		return nil
	}
	return strings.Split(gopath, string(os.PathListSeparator))
}

// pathsFor recursively walks the tree looking for possible directories for the package:
// those whose basename is pkg.
func pathsFor(root, pkg string) []string {
	root = path.Join(root, "src")
	pkgPaths := make([]string, 0, 10)
	visit := func(pathName string, f os.FileInfo, err error) error {
		if err != nil {
			return nil
		}
		// One package per directory. Ignore the files themselves.
		if !f.IsDir() {
			return nil
		}
		// No .hg or other dot nonsense please.
		if strings.Contains(pathName, slashDot) {
			return filepath.SkipDir
		}
		// Is the last element of the path correct
		if pkg == "" || filepath.Base(pathName) == pkg {
			pkgPaths = append(pkgPaths, pathName)
		}
		return nil
	}

	filepath.Walk(root, visit)
	return pkgPaths
}

// lookInDirectory looks in the package (if any) in the directory for the named exported identifier.
func lookInDirectory(directory, name string) {
	fset := token.NewFileSet()
	pkgs, _ := parser.ParseDir(fset, directory, nil, parser.ParseComments) // Ignore the error.
	for _, pkg := range pkgs {
		if pkg.Name == "main" || strings.HasSuffix(pkg.Name, "_test") {
			continue
		}
		doPackage(pkg, fset, name)
	}
}

// prefixDirectory places the directory name on the beginning of each name in the list.
func prefixDirectory(directory string, names []string) {
	if directory != "." {
		for i, name := range names {
			names[i] = filepath.Join(directory, name)
		}
	}
}

// File is a wrapper for the state of a file used in the parser.
// The parse tree walkers are all methods of this type.
type File struct {
	fset       *token.FileSet
	name       string // Name of file.
	ident      string // Identifier we are searching for.
	regexp     *regexp.Regexp
	pathPrefix string // Prefix from GOROOT/GOPATH.
	urlPrefix  string // Start of corresponding URL for golang.org or godoc.org.
	file       *ast.File
	comments   ast.CommentMap
	objs       map[*ast.Ident]types.Object
	doPrint    bool
	found      bool
	allFiles   []*File // All files in the package.
}

const godocOrg = "http://godoc.org"

// doPackage analyzes the single package constructed from the named files, looking for
// the definition of ident.
func doPackage(pkg *ast.Package, fset *token.FileSet, ident string) {
	var files []*File
	found := false
	for name, astFile := range pkg.Files {
		if packageFlag && astFile.Doc == nil {
			continue
		}
		file := &File{
			fset:     fset,
			name:     name,
			ident:    ident,
			file:     astFile,
			comments: ast.NewCommentMap(fset, astFile, astFile.Comments),
		}
		if regexp.QuoteMeta(ident) != ident {
			// It's a regular expression.
			var err error
			file.regexp, err = regexp.Compile("^(?i:" + ident + ")$")
			if err != nil {
				fmt.Fprintf(os.Stderr, "regular expression `%s`:", err)
				os.Exit(2)
			}
		}
		switch {
		case strings.HasPrefix(name, goRootSrcPkg):
			file.urlPrefix = "http://golang.org/pkg"
			file.pathPrefix = goRootSrcPkg
		case strings.HasPrefix(name, goRootSrcCmd):
			file.urlPrefix = "http://golang.org/cmd"
			file.pathPrefix = goRootSrcCmd
		default:
			file.urlPrefix = godocOrg
			for _, path := range goPaths {
				p := filepath.Join(path, "src")
				if strings.HasPrefix(name, p) {
					file.pathPrefix = p
					break
				}
			}
		}
		files = append(files, file)
		if found {
			continue
		}
		file.doPrint = false
		if packageFlag {
			file.pkgComments()
		} else {
			ast.Walk(file, file.file)
			if file.found {
				found = true
			}
		}
	}

	if !found {
		return
	}

	// Type check to build map from name to type.
	defs := make(map[*ast.Ident]types.Object)
	uses := make(map[*ast.Ident]types.Object)

	// By providing the Context with our own error function, it will continue
	// past the first error. There is no need for that function to do anything.
	config := types.Config{
		Error: func(error) {},
	}
	info := &types.Info{
		Defs: defs,
		Uses: uses,
	}
	path := ""
	var astFiles []*ast.File
	for name, astFile := range pkg.Files {
		if path == "" {
			path = name
		}
		astFiles = append(astFiles, astFile)
	}
	config.Check(path, fset, astFiles, info) // Ignore errors.

	// We need to search all files for methods, so record the full list in each file.
	for _, file := range files {
		file.allFiles = files
	}
	for _, file := range files {
		file.doPrint = true
		file.objs = uses
		if packageFlag {
			file.pkgComments()
		} else {
			ast.Walk(file, file.file)
		}
	}
}

// Visit implements the ast.Visitor interface.
func (f *File) Visit(node ast.Node) ast.Visitor {
	switch n := node.(type) {
	case *ast.GenDecl:
		// Variables, constants, types.
		for _, spec := range n.Specs {
			switch spec := spec.(type) {
			case *ast.ValueSpec:
				if constantFlag && n.Tok == token.CONST || variableFlag && n.Tok == token.VAR {
					for _, ident := range spec.Names {
						if f.match(ident.Name) {
							f.printNode(n, ident, f.nameURL(ident.Name))
							break
						}
					}
				}
			case *ast.TypeSpec:
				// If there is only one Spec, there are probably no parens and the
				// comment we want appears before the type keyword, bound to
				// the GenDecl. If the Specs are parenthesized, the comment we want
				// is bound to the Spec. Hence we dig into the GenDecl to the Spec,
				// but only if there are no parens.
				node := ast.Node(n)
				if n.Lparen.IsValid() {
					node = spec
				}
				if f.match(spec.Name.Name) {
					if typeFlag {
						f.printNode(node, spec.Name, f.nameURL(spec.Name.Name))
					} else {
						switch spec.Type.(type) {
						case *ast.InterfaceType:
							if interfaceFlag {
								f.printNode(node, spec.Name, f.nameURL(spec.Name.Name))
							}
						case *ast.StructType:
							if structFlag {
								f.printNode(node, spec.Name, f.nameURL(spec.Name.Name))
							}
						}
					}
					if f.doPrint && f.objs[spec.Name] != nil && f.objs[spec.Name].Type() != nil {
						ms := types.NewMethodSet(f.objs[spec.Name].Type()) //.Type().MethodSet()
						if ms.Len() == 0 {
							ms = types.NewMethodSet(types.NewPointer(f.objs[spec.Name].Type())) //.MethodSet()
						}
						f.methodSet(ms)
					}
				}
			case *ast.ImportSpec:
				continue // Don't care.
			}
		}
	case *ast.FuncDecl:
		// Methods, top-level functions.
		if f.match(n.Name.Name) {
			n.Body = nil // Do not print the function body.
			if methodFlag && n.Recv != nil {
				f.printNode(n, n.Name, f.methodURL(n.Recv.List[0].Type, n.Name.Name))
			} else if functionFlag && n.Recv == nil {
				f.printNode(n, n.Name, f.nameURL(n.Name.Name))
			}
		}
	}
	return f
}

func (f *File) match(name string) bool {
	// name must  be exported.
	if !ast.IsExported(name) {
		return false
	}
	if fuzzyFlag {
		return strings.Contains(name, f.ident)
	}
	if f.regexp == nil {
		return strings.ToLower(name) == strings.ToLower(f.ident)
	}
	return f.regexp.MatchString(name)
}

func (f *File) printNode(node, ident ast.Node, url string) {
	if !f.doPrint {
		f.found = true
		return
	}
	fmt.Printf("%s%s%s", url, f.sourcePos(f.fset.Position(ident.Pos())), f.docs(node))
}

func (f *File) docs(node ast.Node) []byte {
	if !docFlag {
		return nil
	}
	commentedNode := printer.CommentedNode{Node: node}
	if comments := f.comments.Filter(node).Comments(); comments != nil {
		commentedNode.Comments = comments
	}
	var b bytes.Buffer
	printer.Fprint(&b, f.fset, &commentedNode)
	b.Write([]byte("\n\n")) // Add a blank line between entries if we print documentation.
	return b.Bytes()
}

func (f *File) pkgComments() {
	doc := f.file.Doc
	if doc == nil {
		return
	}
	url := ""
	if urlFlag {
		url = f.packageURL() + "\n"
	}
	docText := ""
	if docFlag {
		docText = fmt.Sprintf("package %s\n%s\n\n", f.file.Name.Name, doc.Text())
	}
	fmt.Printf("%s%s%s", url, f.sourcePos(f.fset.Position(doc.Pos())), docText)
}

func (f *File) packageURL() string {
	s := strings.TrimPrefix(f.name, f.pathPrefix)
	// Now we have a path with a final file name. Drop it.
	if i := strings.LastIndex(s, slash); i > 0 {
		s = s[:i+1]
	}
	return f.urlPrefix + s
}

func (f *File) packageName() string {
	s := strings.TrimPrefix(f.name, f.pathPrefix)
	// Now we have a path with a final file name. Drop it.
	if i := strings.LastIndex(s, slash); i > 0 {
		s = s[:i+1]
	}
	s = strings.Trim(s, slash)
	return filepath.ToSlash(s)
}

func (f *File) sourcePos(posn token.Position) string {
	if !srcFlag {
		return ""
	}
	return fmt.Sprintf("%s:%d:\n", posn.Filename, posn.Line)
}

func (f *File) nameURL(name string) string {
	if !urlFlag {
		return ""
	}
	return fmt.Sprintf("%s#%s\n", f.packageURL(), name)
}

func (f *File) methodURL(typ ast.Expr, name string) string {
	if !urlFlag {
		return ""
	}
	var b bytes.Buffer
	printer.Fprint(&b, f.fset, typ)
	typeName := b.Bytes()
	if len(typeName) > 0 && typeName[0] == '*' {
		typeName = typeName[1:]
	}
	return fmt.Sprintf("%s#%s.%s\n", f.packageURL(), typeName, name)
}

// Here follows the code to find and print a method (actually a method set, because
// we want to do only one redundant tree walk, not one per method).
// It should be much easier than walking the whole tree again, but that's what we must do.
// TODO.

type method struct {
	index int // Which doc to write. (Keeps the results sorted)
	*types.Selection
}

type methodVisitor struct {
	*File
	methods []method
	docs    []string
}

func (f *File) methodSet(set *types.MethodSet) {
	// Build the set of things we're looking for.
	methods := make([]method, 0, set.Len())
	docs := make([]string, set.Len())
	for i := 0; i < set.Len(); i++ {
		if ast.IsExported(set.At(i).Obj().Name()) {
			m := method{
				i,
				set.At(i),
			}
			methods = append(methods, m)
		}
	}
	if len(methods) == 0 {
		return
	}
	// Collect the docs.
	for _, file := range f.allFiles {
		visitor := &methodVisitor{
			File:    file,
			methods: methods,
			docs:    docs,
		}
		ast.Walk(visitor, file.file)
		methods = visitor.methods
	}
	// Print them in order. The incoming method set is sorted by name.
	for _, doc := range docs {
		if doc != "" {
			fmt.Print(doc)
		}
	}
}

// Visit implements the ast.Visitor interface.
func (visitor *methodVisitor) Visit(node ast.Node) ast.Visitor {
	switch n := node.(type) {
	case *ast.FuncDecl:
		for i, method := range visitor.methods {
			// If this is the right one, the position of the name of its identifier will match.
			if method.Obj().Pos() == n.Name.Pos() {
				n.Body = nil // TODO. Ugly - don't print the function body.
				visitor.docs[method.index] = fmt.Sprintf("%s", visitor.File.docs(n))
				// If this was the last method, we're done.
				if len(visitor.methods) == 1 {
					return nil
				}
				// Drop this one from the list.
				visitor.methods = append(visitor.methods[:i], visitor.methods[i+1:]...)
				return visitor
			}
		}
	}
	return visitor
}
