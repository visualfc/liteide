package main

import (
	"fmt"
	"os"

	"github.com/visualfc/gotools/command"
	"github.com/visualfc/gotools/pkgs"
)

func main() {
	fmt.Println("liteidex", os.Args)
	cdrv_main(os.Args)
}

var (
	Env = os.Environ()
)

func init() {
	//	command.Register(types.Command)
	//	command.Register(jsonfmt.Command)
	//	command.Register(finddoc.Command)
	//	command.Register(runcmd.Command)
	//	command.Register(docview.Command)
	//	command.Register(astview.Command)
	//	command.Register(goimports.Command)
	//	command.Register(gopresent.Command)
	//	command.Register(goapi.Command)
	command.Register(pkgs.Command)
}
