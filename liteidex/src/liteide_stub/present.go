// Copyright 2013 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"fmt"
	"html/template"
	"io"
	"os"
	"path/filepath"

	"code.google.com/p/go.tools/present"
)

var cmdPresent = &Command{
	Run:       runPresent,
	UsageLine: "present [mode] [list | find <name> ]",
	Short:     "golang present util",
	Long:      `golang present util`,
}

var presentVerifyOnly bool
var presentInput string
var presentStdout bool
var presentOutput string

func init() {
	cmdPresent.Flag.BoolVar(&presentVerifyOnly, "v", false, "verify present only")
	cmdPresent.Flag.BoolVar(&presentStdout, "stdout", false, "output use std output")
	cmdPresent.Flag.StringVar(&presentInput, "input", "i", "input golang present file")
	cmdPresent.Flag.StringVar(&presentOutput, "o", "", "output html file name")
}

func runPresent(cmd *Command, args []string) {
	if presentInput == "" || !isDoc(presentInput) {
		cmd.Usage()
	}

	if presentVerifyOnly {
		err := VerifyDoc(presentInput)
		if err != nil {
			fmt.Fprintf(os.Stderr, "present:%s", err)
			setExitStatus(3)
			exit()
		}
		return
	}
	w := os.Stdout
	if !presentStdout {
		if presentOutput == "" {
			presentOutput = presentInput + ".html"
		}
		ext := filepath.Ext(presentOutput)
		if ext != ".htm" && ext != ".html" {
			presentOutput += ".html"
		}
		var err error
		w, err = os.Create(presentOutput)
		if err != nil {
			fmt.Fprintf(os.Stderr, "present:%s", err)
			setExitStatus(3)
			exit()
		}
	}
	err := RenderDoc(w, presentInput)
	if err != nil {
		fmt.Fprintf(os.Stderr, "present:%s", err)
		setExitStatus(3)
		exit()

	}
}

var extensions = map[string]string{
	".slide":   "slides.tmpl",
	".article": "article.tmpl",
}

var extensions_tmpl = map[string]string{
	".slide":   slides_tmpl,
	".article": article_tmpl,
}

func isDoc(path string) bool {
	_, ok := extensions[filepath.Ext(path)]
	return ok
}

func VerifyDoc(docFile string) error {
	doc, err := parse(docFile, 0)
	if err != nil {
		return err
	}
	dir := filepath.Dir(docFile)
	return verify_doc(dir, doc)
}

// renderDoc reads the present file, builds its template representation,
// and executes the template, sending output to w.
func renderDoc(w io.Writer, base, docFile string) error {
	// Read the input and build the doc structure.
	doc, err := parse(docFile, 0)
	if err != nil {
		return err
	}

	// Find which template should be executed.
	ext := filepath.Ext(docFile)
	contentTmpl, ok := extensions[ext]
	if !ok {
		return fmt.Errorf("no template for extension %v", ext)
	}

	// Locate the template file.
	actionTmpl := filepath.Join(base, "templates/action.tmpl")
	contentTmpl = filepath.Join(base, "templates", contentTmpl)

	// Read and parse the input.
	tmpl := present.Template()
	tmpl = tmpl.Funcs(template.FuncMap{"playable": playable})
	if _, err := tmpl.ParseFiles(actionTmpl, contentTmpl); err != nil {
		return err
	}
	// Execute the template.
	return doc.Render(w, tmpl)
}

func RenderDoc(w io.Writer, docFile string) error {
	// Read the input and build the doc structure.
	doc, err := parse(docFile, 0)
	if err != nil {
		return err
	}

	// Find which template should be executed.
	ext := filepath.Ext(docFile)
	contentTmpl, ok := extensions_tmpl[ext]
	if !ok {
		return fmt.Errorf("no template for extension %v", ext)
	}

	// Locate the template file.
	actionTmpl := action_tmpl //filepath.Join(base, "templates/action.tmpl")
	// Read and parse the input.
	tmpl := present.Template()
	tmpl = tmpl.Funcs(template.FuncMap{"playable": playable})
	if tmpl, err = tmpl.New("action").Parse(actionTmpl); err != nil {
		return err
	}
	if tmpl, err = tmpl.New("content").Parse(contentTmpl); err != nil {
		return err
	}

	// Execute the template.
	return doc.Render(w, tmpl)
}

func parse(name string, mode present.ParseMode) (*present.Doc, error) {
	f, err := os.Open(name)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	return present.Parse(f, name, 0)
}

func playable(c present.Code) bool {
	return present.PlayEnabled && c.Play
}

func isSkipURL(url string) bool {
	if filepath.HasPrefix(url, "http://") {
		return true
	}
	if filepath.HasPrefix(url, "https://") {
		return true
	}
	return false
}

func verify_path(root string, url string) error {
	if isSkipURL(url) {
		return nil
	}
	path := url
	if !filepath.IsAbs(url) {
		path = filepath.Join(root, path)
	}
	_, err := os.Stat(path)
	if err != nil {
		return err
	}
	return nil
}

func verify_doc(root string, doc *present.Doc) error {
	for _, section := range doc.Sections {
		for _, elem := range section.Elem {
			switch i := elem.(type) {
			case present.Image:
				if err := verify_path(root, i.URL); err != nil {
					return fmt.Errorf("! .image %s not exist", i.URL)
				}
			}
		}
	}
	return nil
}

var action_tmpl = `
{/*
This is the action template.
It determines how the formatting actions are rendered.
*/}

{{define "section"}}
  <h{{len .Number}} id="TOC_{{.FormattedNumber}}">{{.FormattedNumber}} {{.Title}}</h{{len .Number}}>
  {{range .Elem}}{{elem $.Template .}}{{end}}
{{end}}

{{define "list"}}
  <ul>
  {{range .Bullet}}
    <li>{{style .}}</li>
  {{end}}
  </ul>
{{end}}

{{define "text"}}
  {{if .Pre}}
  <div class="code"><pre>{{range .Lines}}{{.}}{{end}}</pre></div>
  {{else}}
  <p>
    {{range $i, $l := .Lines}}{{if $i}}{{template "newline"}}
    {{end}}{{style $l}}{{end}}
  </p>
  {{end}}
{{end}}

{{define "code"}}
  <div class="code{{if playable .}} playground{{end}}" contenteditable="true" spellcheck="false">{{.Text}}</div>
{{end}}

{{define "image"}}
<div class="image">
  <img src="{{.URL}}"{{with .Height}} height="{{.}}"{{end}}{{with .Width}} width="{{.}}"{{end}}>
</div>
{{end}}

{{define "iframe"}}
<iframe src="{{.URL}}"{{with .Height}} height="{{.}}"{{end}}{{with .Width}} width="{{.}}"{{end}}></iframe>
{{end}}

{{define "link"}}<p class="link"><a href="{{.URL}}" target="_blank">{{style .Label}}</a></p>{{end}}

{{define "html"}}{{.HTML}}{{end}}
`

var article_tmpl = `
{/* This is the article template. It defines how articles are formatted. */}

{{define "root"}}
<!DOCTYPE html>
<html>
  <head>
    <title>{{.Title}}</title>
    <link type="text/css" rel="stylesheet" href="static/article.css">
    <meta charset='utf-8'>
  </head>

  <body>
    <div id="topbar" class="wide">
      <div class="container">
        <div id="heading">{{.Title}}
          {{with .Subtitle}}{{.}}{{end}}
        </div>
      </div>
    </div>
    <div id="page" class="wide">
      <div class="container">
        {{with .Sections}}
          <div id="toc">
            {{template "TOC" .}}
          </div>
        {{end}}

        {{range .Sections}}
          {{elem $.Template .}}
        {{end}}{{/* of Section block */}}

        <h2>Authors</h2>
        {{range .Authors}}
          <div class="author">
            {{range .Elem}}{{elem $.Template .}}{{end}}
          </div>
        {{end}}
      </div>
    </div>
    <script src='/play.js'></script>
  </body>
</html>
{{end}}

{{define "TOC"}}
  <ul>
  {{range .}}
    <li><a href="#TOC_{{.FormattedNumber}}">{{.Title}}</a></li>
    {{with .Sections}}{{template "TOC" .}}{{end}}
  {{end}}
  </ul>
{{end}}

{{define "newline"}}
{{/* No automatic line break. Paragraphs are free-form. */}}
{{end}}
`

var slides_tmpl = `
{/* This is the slide template. It defines how presentations are formatted. */}

{{define "root"}}
<!DOCTYPE html>
<html>
  <head>
    <title>{{.Title}}</title>
    <meta charset='utf-8'>
    <script src='static/slides.js'></script>
  </head>

  <body style='display: none'>

    <section class='slides layout-widescreen'>
      
      <article>
        <h1>{{.Title}}</h1>
        {{with .Subtitle}}<h3>{{.}}</h3>{{end}}
        {{if not .Time.IsZero}}<h3>{{.Time.Format "2 January 2006"}}</h3>{{end}}
        {{range .Authors}}
          <div class="presenter">
            {{range .TextElem}}{{elem $.Template .}}{{end}}
          </div>
        {{end}}
      </article>
      
  {{range $i, $s := .Sections}}
  <!-- start of slide {{$s.Number}} -->
      <article>
      {{if $s.Elem}}
        <h3>{{$s.Title}}</h3>
        {{range $s.Elem}}{{elem $.Template .}}{{end}}
      {{else}}
        <h2>{{$s.Title}}</h2>
      {{end}}
      </article>
  <!-- end of slide {{$i}} -->
  {{end}}{{/* of Slide block */}}

      <article>
        <h3>Thank you</h1>
        {{range .Authors}}
          <div class="presenter">
            {{range .Elem}}{{elem $.Template .}}{{end}}
          </div>
        {{end}}
      </article>

  </body>
  {{if .PlayEnabled}}
  <script src='/play.js'></script>
  {{end}}
</html>
{{end}}

{{define "newline"}}
<br>
{{end}}
`
