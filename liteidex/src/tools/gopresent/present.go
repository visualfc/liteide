// Copyright 2013 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"code.google.com/p/go.tools/present"
	"fmt"
	"html/template"
	"io"
	//"log"
	"os"
	"path/filepath"
)

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
