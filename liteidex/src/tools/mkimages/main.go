package main

import (
	"encoding/xml"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var (
	root = "../../../"
)

func main() {
	var p Process
	p.ProcessDir(filepath.Join(root, "src/liteapp"))
	p.ProcessDir(filepath.Join(root, "src/plugins"))
	p.Export(filepath.Join(root, "deploy/images/default"))
}

type Process struct {
	rccs []RCC
}

func (p *Process) ProcessDir(dir string) {
	filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if filepath.Ext(path) == ".qrc" {
			p.ProcessQrc(path)
		}
		return nil
	})
}

type QResource struct {
	Prefix string   `xml:"prefix,attr"`
	Files  []string `xml:"file"`
}

type RCC struct {
	Dir      string
	DirName  string
	FileName string
	XMLName  xml.Name  `xml:"RCC"`
	Resource QResource `xml:"qresource"`
}

func (p *Process) ProcessQrc(path string) error {
	data, err := ioutil.ReadFile(path)
	if err != nil {
		return err
	}
	var rcc RCC
	err = xml.Unmarshal(data, &rcc)
	if err != nil {
		log.Println(err)
		return err
	}
	rcc.Dir, rcc.FileName = filepath.Split(path)
	_, rcc.DirName = filepath.Split(filepath.Clean(rcc.Dir))
	p.rccs = append(p.rccs, rcc)
	log.Println("->", path)
	return nil
}

func (p *Process) Export(path string) error {
	for _, rcc := range p.rccs {
		log.Println(rcc.Dir, rcc.DirName)
	}
	return nil
}
