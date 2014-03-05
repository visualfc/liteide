package main

import (
	"fmt"
	"runtime"
)

const (
	liteide_stub_version = "1.0"
)

var cmdVersion = &Command{
	Run:       runVersion,
	UsageLine: "version",
	Short:     "print liteide_stub version",
	Long:      `Version prints the liteide_stub version.`,
}

func runVersion(cmd *Command, args []string) {
	if len(args) != 0 {
		cmd.Usage()
	}

	fmt.Printf("liteide_stub version %s %s/%s\n", liteide_stub_version, runtime.GOOS, runtime.GOARCH)
}
