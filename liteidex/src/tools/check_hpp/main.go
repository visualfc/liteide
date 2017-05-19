package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"
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
	log.Println("process hpp files in liteide root", root)
	var p Process
	p.ProcessDir(filepath.Join(root, "src/api"))
	p.ProcessDir(filepath.Join(root, "src/liteapp"))
	p.ProcessDir(filepath.Join(root, "src/plugins"))
	p.ProcessDir(filepath.Join(root, "src/utils"))
	p.ProcessDir(filepath.Join(root, "src/3rdparty"))
}

type Process struct {
}

func (p *Process) ProcessDir(dir string) {
	filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if filepath.Ext(path) == ".h" {
			p.ProcessHpp(path)
		}
		return nil
	})
}

func (p *Process) ProcessHpp(path string) error {
	f, err := os.Open(path)
	if err != nil {
		log.Printf("error open file %q, %v\n", path, err)
		return err
	}
	stat, _ := f.Stat()
	data, err := ioutil.ReadAll(f)
	f.Close()
	if err != nil {
		log.Println(err)
		return err
	}

	lines := strings.Split(string(data), "\n")
	var hppDef string
	hppDefMap := make(map[int]string)
	for n, line := range lines {
		if strings.HasPrefix(line, "#ifndef ") && hppDef == "" {
			hppDef = strings.TrimSpace(line[7:])
			if !(strings.HasPrefix(hppDef, "_") || strings.HasSuffix(hppDef, "_")) {
				return nil
			}
			hppDefMap[n] = line
		} else if hppDef != "" {
			if strings.HasPrefix(line, "#define ") {
				if strings.TrimSpace(line[8:]) == hppDef {
					hppDefMap[n] = line
				}
			} else if strings.HasPrefix(line, "#endif") {
				if strings.Contains(line, hppDef) {
					hppDefMap[n] = line
				}
			}
		}
	}
	if len(hppDefMap) != 3 {
		err := fmt.Errorf("error, check hpp %q head %q no match", path, hppDef)
		log.Println(err)
		return err
	}
	newDef := strings.Trim(hppDef, "_")
	fmt.Printf("> process hpp %q head %v => %v\n", path, hppDef, newDef)
	for n, line := range hppDefMap {
		newline := strings.Replace(line, hppDef, newDef, -1)
		fmt.Printf("%v %q => %q\n", n, line, newline)
		lines[n] = newline
	}
	err = ioutil.WriteFile(path, []byte(strings.Join(lines, "\n")), stat.Mode())
	if err != nil {
		log.Fatalf("error write file %q, %v", path, err)
		return err
	}

	return nil
}
