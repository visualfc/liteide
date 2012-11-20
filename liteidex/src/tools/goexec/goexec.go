package main

import (
	"fmt"
	"os"
	"os/exec"
	"path"
	"strings"
)

func main() {
	args := os.Args[1:]

	if len(args) < 1 {
		fmt.Println("Usage: goexec [-w work_path] <program_name> [arguments...]")
		os.Exit(0)
	}

	var workPath string
	var fileName string

	if args[0] == "-w" {
		if len(args) < 3 {
			fmt.Println("Usage: goexec [-w work_path] <program_name> [arguments...]")
			os.Exit(0)
		}
		workPath = args[1]
		fileName = args[2]
		args = args[2:]
		if len(workPath) > 0 && workPath[0] == '.' {
			wd, err := os.Getwd()
			if err == nil {
				workPath = path.Join(wd, workPath)
			}
		}
	} else {
		workPath, _ = os.Getwd()
		fileName = args[0]
	}
	filePath, err := exec.LookPath(fileName)
	if err != nil {
		filePath, err = exec.LookPath("./" + fileName)
	}
	if err != nil {
		fmt.Println(err)
		wait_exit()
	}

	fmt.Println("---- Starting Process", filePath, strings.Join(args[1:], " "), "----")

	cmd := exec.Command(filePath, args[1:]...)
	cmd.Dir = workPath
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	err = cmd.Run()

	if err != nil {
		fmt.Println("\n---- End Process", err, "----")
	} else {
		fmt.Println("\n---- End Process", "exit status 0", "----")
	}

	wait_exit()
}

func wait_exit() {
	fmt.Println("\nPress enter key to continue")
	var s = [256]byte{}
	os.Stdin.Read(s[:])
	os.Exit(0)
}
