// Copyright 2011-2014 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"text/template"
	"time"
)

var cmdDocView = &Command{
	Run:       runDocView,
	UsageLine: "docview [-mode] [-list|-find]",
	Short:     "golang docview util",
	Long:      `golang docview util`,
}

var docViewFind string
var docViewList string
var docViewMode string

func init() {
	cmdDocView.Flag.StringVar(&docViewFind, "find", "", "find package list, :pkg flag is best match")
	cmdDocView.Flag.StringVar(&docViewList, "list", "", "Print go packages list [pkg|cmd]")
	cmdDocView.Flag.StringVar(&docViewMode, "mode", "text", "Print mode [text|html|lite]")
}

func runDocView(cmd *Command, args []string) {
	if docViewFind == "" && docViewList == "" {
		cmd.Usage()
	}

	var template string
	var info *Info
	if len(docViewList) > 0 {
		info = NewListInfo(filepath.Join(goroot, "src", docViewList))
		switch docViewMode {
		case "html":
			template = listHTML
		case "lite":
			template = listLite
		case "text":
			template = listText
		default:
			template = listText
		}
	} else if len(docViewFind) > 0 {
		dir := NewSourceDir(goroot)
		info = dir.FindInfo(docViewFind)
		switch docViewMode {
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
		fmt.Fprintf(os.Stderr, "<error>\n")
		setExitStatus(3)
		exit()
	}
	contents := info.GetPkgList(docViewMode, template)
	fmt.Fprintf(os.Stdout, "%s", contents)
}

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

var listHTML = `<!-- Golang Package List -->
<p class="detail">
Need more packages? The
<a href="http://godashboard.appspot.com/package">Package Dashboard</a>
provides a list of <a href="/cmd/goinstall/">goinstallable</a> packages.
</p>
<h2 id="Subdirectories">Subdirectories</h2>
<p>
{{with .Dirs}}
	<p>
	<table class="layout">
	<tr>
	<th align="left" colspan="{{html .MaxHeight}}">Name</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{{range .List}}
		<tr>
		{{repeat "<td width=\"25\"></td>" .Depth}}
		<td align="left" colspan="{{html .Height}}"><a href="{{.Path}}">{{html .Name}}</a></td>
		<td></td>
		<td align="left">{{html .Synopsis}}</td>
		</tr>
	{{end}}
	</table>
	</p>
{{end}}`

var listText = `$list
{{with .Dirs}}
{{range .List}}{{.Path }}
{{end}}
{{end}}`

var listLite = `$list{{with .Dirs}}{{range .List}},{{.Path}}{{end}}{{end}}`

var findHTML = `<!-- Golang Package List -->
<p class="detail">
Need more packages? The
<a href="http://godashboard.appspot.com/package">Package Dashboard</a>
provides a list of <a href="/cmd/goinstall/">goinstallable</a> packages.
</p>
<h2 id="Subdirectories">Subdirectories</h2>
<table class="layout">
	<tr>
	<th align="left">Best</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	{{with .Best}}
		<tr>
		<td align="left"><a href="{{html .Path}}">{{.Path}}</a></td>
		<td></td>
		<td align="left">{{html .Synopsis}}</td>
		</tr>
	{{end}}	
	{{with .Dirs}}
	<tr>
	<th align="left">Match</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{{range .List}}
		<tr>
		<td align="left"><a href="{{html .Path}}">{{.Path}}</a></td>
		<td></td>
		<td align="left">{{html .Synopsis}}</td>
		</tr>
	{{end}}
	</table>
	</p>
{{end}}`

var findText = `$best
{{with .Best}}{{.Path}}{{end}}
$list
{{with .Dirs}}{{range .List}}{{.Path}}
{{end}}{{end}}`

var findLite = `$find,{{with .Best}}{{.Path}}{{end}}{{with .Dirs}}{{range .List}},{{.Path}}{{end}}{{end}}`
