package main

import (
	"bytes"
	"flag"
	"image"
	"image/draw"
	"image/png"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var (
	root = "../../../"
)

var (
	flagRoot string
)

func init() {
	flag.StringVar(&flagRoot, "root", "", "setup liteide root")
}

func main() {
	flag.Parse()
	if flagRoot != "" {
		root = flagRoot
	}
	root, _ := filepath.Abs(root)
	log.Println("process png files in liteide root", root)
	var p Process
	p.ProcessDir(filepath.Join(root, "src/api"))
	p.ProcessDir(filepath.Join(root, "src/liteapp"))
	p.ProcessDir(filepath.Join(root, "src/plugins"))
	p.ProcessDir(filepath.Join(root, "deploy"))
	p.ProcessDir(filepath.Join(root, "src/utils"))
	p.ProcessDir(filepath.Join(root, "src/3rdparty"))
}

type Process struct {
}

func (p *Process) ProcessDir(dir string) {
	filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if filepath.Ext(path) == ".png" {
			err := p.ProcessPng(path)
			if err != nil {
				log.Println(path, err)
			}
		}
		return nil
	})
}

func (p *Process) ProcessPng(path string) error {
	f, err := os.Open(path)
	if err != nil {
		return err
	}
	stat, err := f.Stat()
	if err != nil {
		return err
	}
	var buf bytes.Buffer
	srcImage, err := png.Decode(f)
	if err != nil {
		return err
	}

	dstImage := image.NewNRGBA(srcImage.Bounds())
	draw.Draw(dstImage, dstImage.Bounds(), srcImage, srcImage.Bounds().Min, draw.Src)

	err = png.Encode(&buf, dstImage)
	if err != nil {
		return err
	}

	data, err := ioutil.ReadFile(path)
	if err != nil {
		return err
	}
	if bytes.Compare(data, buf.Bytes()) != 0 {
		log.Println("update png", path, len(data), buf.Len())
		ioutil.WriteFile(path, buf.Bytes(), stat.Mode())
	}
	return nil
}
