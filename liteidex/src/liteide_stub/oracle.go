// Copyright 2011-2015 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"fmt"
	"go/build"
	"log"
	"os"
	"runtime"

	"golang.org/x/tools/oracle"
)

//The mode argument determines the query to perform:

//	callees	  	show possible targets of selected function call
//	callers	  	show possible callers of selected function
//	callgraph 	show complete callgraph of program
//	callstack 	show path from callgraph root to selected function
//	describe  	describe selected syntax: definition, methods, etc
//	freevars  	show free variables of selection
//	implements	show 'implements' relation for selected type
//	peers     	show send/receive corresponding to selected channel op
//	referrers 	show all refs to entity denoted by selected identifier
//	what		show basic information about the selected syntax node

var cmdOracle = &Command{
	Run:       runOracle,
	UsageLine: "oracle",
	Short:     "golang oracle util",
	Long:      `golang oracle util.`,
}

var (
	oraclePos     string
	oracleReflect bool
)

func init() {
	cmdOracle.Flag.StringVar(&oraclePos, "pos", "", "filename:#offset")
	cmdOracle.Flag.BoolVar(&oracleReflect, "reflect", false, "Analyze reflection soundly (slow).")
}

func runOracle(cmd *Command, args []string) {
	if len(args) < 2 {
		cmd.Usage()
	}
	if os.Getenv("GOMAXPROCS") == "" {
		n := runtime.NumCPU()
		if n < 4 {
			n = 4
		}
		runtime.GOMAXPROCS(n)
	}
	mode := args[0]
	args = args[1:]
	if args[0] == "." {
		pkgPath, err := os.Getwd()
		if err != nil {
			log.Fatalln(err)
		}
		pkg, err := build.Default.ImportDir(pkgPath, 0)
		if err != nil {
			log.Fatalln(err)
		}
		args = pkg.GoFiles
		//log.Println(pkg.ImportPath)
		if pkg.ImportPath != "." && pkg.ImportPath != "" {
			args = []string{pkg.ImportPath}
		}
	}
	res, err := oracle.Query(args, mode, oraclePos, nil, &build.Default, oracleReflect)
	if err != nil {
		fmt.Fprintf(os.Stderr, "oracle: %s.\n", err)
		os.Exit(2)
	}
	res.WriteTo(os.Stdout)
}
