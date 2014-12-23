// Copyright 2011-2015 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"fmt"
	"os"
	"os/exec"
	"strings"
)

var cmdExec = &Command{
	Run:       runExec,
	UsageLine: "exec [-w work_path] <program_name> [arguments...]",
	Short:     "execute program",
	Long:      `execute program and arguments`,
}

var execWorkPath string
var execWaitEnter bool

func init() {
	cmdExec.Flag.StringVar(&execWorkPath, "w", "", "work path")
	cmdExec.Flag.BoolVar(&execWaitEnter, "e", true, "wait enter and continue")
}

func runExec(cmd *Command, args []string) {
	if len(args) == 0 {
		cmd.Usage()
	}
	if execWorkPath == "" {
		var err error
		execWorkPath, err = os.Getwd()
		if err != nil {
			fmt.Fprintf(os.Stderr, "liteide_stub exec: os.Getwd() false\n")
			setExitStatus(3)
			exit()
			return
		}
	}
	fileName := args[0]

	filePath, err := exec.LookPath(fileName)
	if err != nil {
		filePath, err = exec.LookPath("./" + fileName)
	}
	if err != nil {
		fmt.Fprintf(os.Stderr, "liteide_stub exec: file %s not found\n", fileName)
		setExitStatus(3)
		exit()
	}

	fmt.Println("Starting Process", filePath, strings.Join(args[1:], " "), "...")

	command := exec.Command(filePath, args[1:]...)
	command.Dir = execWorkPath
	command.Stdin = os.Stdin
	command.Stdout = os.Stdout
	command.Stderr = os.Stderr

	err = command.Run()

	if err != nil {
		fmt.Println("\nEnd Process", err)
	} else {
		fmt.Println("\nEnd Process", "exit status 0")
	}

	exitWaitEnter()
}

func exitWaitEnter() {
	if !execWaitEnter {
		return
	}
	fmt.Println("\nPress enter key to continue")
	var s = [256]byte{}
	os.Stdin.Read(s[:])
	setExitStatus(0)
	exit()
}
