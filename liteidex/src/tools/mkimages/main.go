package main

import (
	"encoding/xml"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"
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

func (rcc *RCC) IsEmtpy() bool {
	return len(rcc.Resource.Files) == 0
}

func (rcc *RCC) ImagesFiles() (images []string) {
	for _, file := range rcc.Resource.Files {
		if strings.HasPrefix(file, "images/") {
			image := filepath.Join(rcc.Dir, file)
			_, err := os.Lstat(image)
			if err != nil {
				log.Println("warning, not find image", image)
			}
			images = append(images, image)
		}
	}
	return
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
	return nil
}

func (p *Process) Export(outdir string) error {
	for _, rcc := range p.rccs {
		p.ExportQrc(outdir, rcc)
	}
	return nil
}

func (p *Process) ExportQrc(outdir string, rcc RCC) error {
	images := rcc.ImagesFiles()
	if len(images) == 0 {
		log.Println("skip empty rcc", rcc.FileName)
		return nil
	}
	outpath := filepath.Join(outdir, rcc.DirName)
	os.MkdirAll(outpath, 0777)
	for _, file := range images {
		err := CopyFileTo(file, outpath)
		log.Println(file, err)
	}
	return nil
}

func CopyFileTo(source string, outdir string) (err error) {
	_, name := filepath.Split(source)
	return CopyFile(source, filepath.Join(outdir, name))
}

func CopyFile(source string, dest string) (err error) {
	sourcefile, err := os.Open(source)
	if err != nil {
		return err
	}
	defer sourcefile.Close()
	destfile, err := os.Create(dest)
	if err != nil {
		return err
	}
	defer destfile.Close()
	_, err = io.Copy(destfile, sourcefile)
	if err == nil {
		sourceinfo, err := os.Stat(source)
		if err != nil {
			err = os.Chmod(dest, sourceinfo.Mode())
		}
	}
	return
}
