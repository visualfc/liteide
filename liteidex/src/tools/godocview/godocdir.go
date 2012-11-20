// gopkgview project godocdir.go
package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"path/filepath"
	"strconv"
	"strings"
	"text/template"
	"time"
)

var (
	fs FileSystem = OS // the underlying file system
)

// Fake package file and name for commands. Contains the command documentation.
const fakePkgFile = "doc.go"
const fakePkgName = "documentation"

func textFmt(w io.Writer, format string, x ...interface{}) {
	var buf bytes.Buffer
	fmt.Fprint(&buf, x)
	template.HTMLEscape(w, buf.Bytes())
}

func pathEscFmt(w io.Writer, format string, x ...interface{}) {
	switch v := x[0].(type) {
	case []byte:
		template.HTMLEscape(w, v)
	case string:
		template.HTMLEscape(w, []byte(filepath.ToSlash(v)))
	default:
		var buf bytes.Buffer
		fmt.Fprint(&buf, x)
		template.HTMLEscape(w, buf.Bytes())
	}
}

func htmlEscFmt(w io.Writer, format string, x ...interface{}) {
	switch v := x[0].(type) {
	case int:
		template.HTMLEscape(w, []byte(strconv.Itoa(v)))
	case []byte:
		template.HTMLEscape(w, v)
	case string:
		template.HTMLEscape(w, []byte(v))
	default:
		var buf bytes.Buffer
		fmt.Fprint(&buf, x)
		template.HTMLEscape(w, buf.Bytes())
	}
}

// Template formatter for "padding" format.
func paddingFmt(w io.Writer, format string, x ...interface{}) {
	for i := x[0].(int); i > 0; i-- {
		fmt.Fprint(w, `<td width="25"></td>`)
	}
}

// Template formatter for "time" format.
func timeFmt(w io.Writer, format string, x ...interface{}) {
	template.HTMLEscape(w, []byte(time.Unix(x[0].(int64)/1e9, 0).String()))
}

var fmap = template.FuncMap{
	"repeat": strings.Repeat,
}

func readTemplateData(name, data string) *template.Template {
	return template.Must(template.New(name).Funcs(fmap).Parse(data))
}

func readTemplateFile(name, path string) *template.Template {
	return template.Must(template.New(name).Funcs(fmap).ParseFiles(path))
}

func applyTemplate(t *template.Template, name string, data interface{}) []byte {
	var buf bytes.Buffer
	if err := t.Execute(&buf, data); err != nil {
		log.Printf("%s.Execute: %s", name, err)
	}
	return buf.Bytes()
}

type Info struct {
	Find string
	Best *DirEntry
	Dirs *DirList
}

type GodocDir struct {
	pkg *Directory
	cmd *Directory
}

func NewSourceDir(goroot string) *GodocDir {
	pkg := newDirectory(filepath.Join(goroot, "src", "pkg"), nil, -1)
	cmd := newDirectory(filepath.Join(goroot, "src", "cmd"), nil, -1)
	return &GodocDir{pkg, cmd}
}

func (dir *GodocDir) FindInfo(name string) *Info {
	max1, best1, list1 := FindDir(dir.pkg, name)
	max2, best2, list2 := FindDir(dir.cmd, name)
	var maxHeight int
	if max1 >= max2 {
		maxHeight = max1
	} else {
		maxHeight = max2
	}
	var best *DirEntry
	if best1 != nil {
		best = best1
		if best2 != nil {
			list2 = append(list2, *best2)
		}
	} else {
		best = best2
	}
	return &Info{name, best, &DirList{maxHeight, appendList(list1, list2)}}
}

func FindDir(dir *Directory, pkgname string) (maxHeight int, best *DirEntry, list []DirEntry) {
	if dir == nil {
		return
	}
	dirList := dir.listing(true)
	max := len(dirList.List)
	maxHeight = dirList.MaxHeight

	for i := 0; i < max; i++ {
		name := dirList.List[i].Name
		path := filepath.ToSlash(dirList.List[i].Path)
		if name == pkgname || path == pkgname {
			best = &dirList.List[i]
		} else if strings.Contains(path, pkgname) {
			list = append(list, dirList.List[i])
		}
	}
	return
}

func appendList(list1, list2 []DirEntry) []DirEntry {
	list := list1
	max := len(list2)
	for i := 0; i < max; i++ {
		list = append(list, list2[i])
	}
	return list
}

func NewListInfo(root string) *Info {
	dir := newDirectory(root, nil, -1)
	if dir == nil {
		return nil
	}
	return &Info{"", nil, dir.listing(true)}
}

func FindPkgInfo(root string, pkgname string) *Info {
	dir := newDirectory(root, nil, -1)
	if dir == nil {
		return nil
	}
	dirList := dir.listing(true)
	if pkgname == "*" {
		return &Info{pkgname, nil, dirList}
	}
	var best DirEntry
	var list []DirEntry
	max := len(dirList.List)
	for i := 0; i < max; i++ {
		name := dirList.List[i].Name
		path := filepath.ToSlash(dirList.List[i].Path)
		if name == pkgname || path == pkgname {
			best = dirList.List[i]
		} else if strings.Contains(path, pkgname) {
			list = append(list, dirList.List[i])
		}
	}
	return &Info{pkgname, &best, &DirList{dirList.MaxHeight, list}}
}

func (info *Info) GetPkgList(name, templateData string) []byte {
	data := readTemplateData(name, templateData)
	return applyTemplate(data, "pkglist", info)
}
