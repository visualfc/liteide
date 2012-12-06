// Copyright 2011-2012 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"regexp"
)

type GoProject struct {
	Name   string
	Values map[string][]string
	array  *PackageArray
}

func NewGoProjectWithFiles(files [][]byte) (*GoProject, error) {
	pro := new(GoProject)
	pro.Values = make(map[string][]string)

	for _, v := range files {
		pro.Values["GOFILES"] = append(pro.Values["GOFILES"], string(v))
	}
	return pro, nil
}

func NewGoProject(name string) (pro *GoProject, err error) {
	buf, e := ioutil.ReadFile(name)
	if e != nil {
		err = e
		return
	}
	pro = new(GoProject)
	pro.Name = name
	pro.Values = make(map[string][]string)

	pre, e := regexp.Compile("\\\\[\t| ]*[\r|\n]+[\t| ]*") //("\\\\[^a-z|A-Z|0-9|_|\r|\n]*[\r|\n]+[^a-z|A-Z|0-9|_|\r|\n]*")
	if e != nil {
		err = e
		return
	}
	all := pre.ReplaceAll(buf[:], []byte(" "))
	all = bytes.Replace(all, []byte("\r\n"), []byte("\n"), -1)
	lines := bytes.Split(all, []byte("\n"))

	for _, line := range lines {
		offset := 2
		line = bytes.Replace(line, []byte("\t"), []byte(" "), -1)
		if len(line) >= 1 && line[0] == '#' {
			continue
		}
		find := bytes.Index(line, []byte("+="))
		if find == -1 {
			offset = 1
			find = bytes.Index(line, []byte("="))
		}
		if find != -1 {
			k := bytes.TrimSpace(line[0:find])
			v := bytes.SplitAfter(line[find+offset:], []byte(" "))
			var vall []string
			if offset == 2 {
				vall = pro.Values[string(k)]
			}
			for _, nv := range v {
				nv2 := bytes.TrimSpace(nv)
				if len(nv2) != 0 && string(nv2) != "\\" {
					vall = append(vall, string(nv2))
				}
			}
			pro.Values[string(k)] = vall
		}
	}
	return
}

func (file *GoProject) Gofiles() []string {
	return file.Values["GOFILES"]
}

func (file *GoProject) AllPackage() (paks []string) {
	for i := 0; i < len(file.array.Data); i++ {
		if len(file.array.Data[i].files) > 0 {
			paks = append(paks, file.array.Data[i].pakname)
		}
	}
	return
}

func (file *GoProject) PackageFilesString(pakname string) []string {
	return file.Values[pakname]
}

func (file *GoProject) PackageFiles(pakname string) []string {
	return file.array.index(pakname).files
}

func (file *GoProject) TargetName() string {
	t := file.Target()
	_, name := path.Split(t)
	return name
}

func (file *GoProject) Target() string {
	v := file.Values["TARGET"]
	if len(v) >= 1 && len(v[0]) > 0 {
		return v[0]
	}
	if len(file.Name) == 0 {
		return "main"
	}
	return path.Base(file.Name)
}

func (file *GoProject) DestDir() (dir string) {
	v := file.Values["DESTDIR"]
	if len(v) >= 1 {
		dir = string(v[0])
	}
	t := file.Target()
	p, _ := path.Split(t)
	if len(p) > 0 {
		dir = path.Join(dir, p)
	}
	return
}

func (file *GoProject) ProjectDir() (dir string) {
	dir, _ = path.Split(file.Name)
	return
}

func build(gocmd string, gcfile string, opts []string, proFileName string, files []string, envv []string, dir string) error {
	args := []string{gocmd, "tool", gcfile, "-o", proFileName}
	for _, v := range opts {
		args = append(args, v)
	}
	for _, v := range files {
		args = append(args, string(v))
	}
	fmt.Println("\t", args)
	cmd := exec.Command(gocmd, args[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Dir = dir
	err := cmd.Run()
	if err != nil {
		fmt.Printf("Error, %s", err)
	}
	return err
}

func link(gocmd string, glfile string, opts []string, target string, ofile string, envv []string, dir string) error {
	args := []string{gocmd, "tool", glfile, "-o", target}
	for _, v := range opts {
		args = append(args, v)
	}
	args = append(args, ofile)
	fmt.Println("\t", args)
	cmd := exec.Command(gocmd, args[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Dir = dir
	err := cmd.Run()
	if err != nil {
		fmt.Printf("Error, %s", err)
	}
	return err
}

func pack(gocmd string, pkfile string, target string, ofile string, envv []string, dir string) error {
	args := []string{gocmd, "tool", pkfile, "grc", target, ofile}
	fmt.Println("\t", args)
	cmd := exec.Command(gocmd, args[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Dir = dir
	err := cmd.Run()
	if err != nil {
		fmt.Printf("Error, %s", err)
	}
	return err
}

func (file *GoProject) IsEmpty() bool {
	return len(file.Values) == 0
}

func (file *GoProject) Clean(clean string) error {
	dir := file.ProjectDir()
	if dir == "" {
		dir = "./"
	}
	files, err := ioutil.ReadDir(dir)
	var rfiles []string
	if err != nil {
		return err
	}
	for _, file := range files {
		ext := filepath.Ext(file.Name())
		if ext == ".8" || ext == ".5" || ext == ".6" {
			err := os.Remove(file.Name())
			if err == nil {
				rfiles = append(rfiles, file.Name())
			}
		}
	}
	if clean == "all" {
		target := file.Target()
		dest := file.DestDir()
		if len(dest) > 0 {
			target = path.Join(dest, target)
		}
		targets := []string{target, target + ".a", target + ".exe"}
		for _, v := range targets {
			err := os.Remove(v)
			if err == nil {
				rfiles = append(rfiles, v)
			}
		}
	}
	if len(rfiles) == 0 {
		fmt.Println("clean nothing")
	} else {
		for _, v := range rfiles {
			fmt.Println("remove", v)
		}
	}
	return nil
}

func (file *GoProject) MakeTarget(gobin *GoBin) error {
	all := file.AllPackage()
	for _, v := range all {
		if v == "documentation" {
			continue
		}
		fmt.Printf("build package %s:\n", v)
		target := v
		ofile := target + gobin.objext
		if v == "main" {
			target = file.TargetName()
			ofile = target + "_go_" + gobin.objext
		}
		err := build(gobin.gocmd, gobin.compiler, file.Values["GCOPT"], ofile, file.PackageFiles(v), os.Environ(), file.ProjectDir())
		if err != nil {
			return err
		}

		dest := file.DestDir()
		if len(dest) > 0 {
			//dest = path.Join(file.ProjectDir(), dest)
			os.MkdirAll(dest, 0777)
			target = path.Join(dest, target)
		}
		if string(v) == "main" {
			if filepath.Ext(target) != gobin.exeext {
				target = target + gobin.exeext
			}
			err := link(gobin.gocmd, gobin.link, file.Values["GLOPT"], target, ofile, os.Environ(), file.ProjectDir())
			if err != nil {
				return err
			}
			fmt.Printf("link target : %s\n", target)
		} else if *buildLib {
			target = target + gobin.pakext
			err := pack(gobin.gocmd, gobin.pack, target, ofile, os.Environ(), file.ProjectDir())
			if err != nil {
				return err
			}
			fmt.Printf("pack library : %s\n", target)
		}
	}
	return nil
}
