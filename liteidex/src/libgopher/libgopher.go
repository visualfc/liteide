package main

/*
//flag: 1 stdout, 2 stderr, 0 finished
typedef struct {char *data; int size;} TString;
typedef int (*CB)(void *ctx, int flag, char *data, int size);
static int WriteContext(void *ctx, void *cb, int flag, char *data, int size)
{
	return ((CB)cb)(ctx,flag,data,size);
}

#include <stdlib.h>
*/
import "C"

import (
	"bytes"
	"io"
	"os"
	"strings"
	"unsafe"

	"github.com/visualfc/gotools/astview"
	"github.com/visualfc/gotools/command"
	"github.com/visualfc/gotools/docview"
	"github.com/visualfc/gotools/finddoc"
	"github.com/visualfc/gotools/gofmt"
	"github.com/visualfc/gotools/gopresent"
	"github.com/visualfc/gotools/jsonfmt"
	"github.com/visualfc/gotools/pkgs"
	"github.com/visualfc/gotools/runcmd"
	"github.com/visualfc/gotools/types"
)

type Context struct {
	ctx  unsafe.Pointer
	cb   unsafe.Pointer
	flag C.int
}

func (c *Context) Write(data []byte) (n int, err error) {
	if c.flag == 0 {
		return int(C.WriteContext(c.ctx, c.cb, c.flag, nil, 0)), nil
	}
	cdata := C.CBytes(data)
	defer C.free(unsafe.Pointer(cdata))
	return int(C.WriteContext(c.ctx, c.cb, c.flag, (*C.char)(cdata), C.int(len(data)))), nil
}

func (c *Context) Finished(err error) {
	if err == nil {
		C.WriteContext(c.ctx, c.cb, 0, nil, 0)
		return
	}
	s := err.Error()
	if s == "" {
		s = "unknown error"
	}
	cdata := C.CString(s)
	defer C.free(unsafe.Pointer(cdata))
	C.WriteContext(c.ctx, c.cb, 0, cdata, C.int(len(s)))
}

//export Setenv
func Setenv(key *C.char, key_size C.int, value *C.char, value_size C.int) {
	os.Setenv(C.GoStringN(key, key_size), C.GoStringN(value, value_size))
}

//export InvokeAsync
func InvokeAsync(id string, args string, sep string, sin string, ctx unsafe.Pointer, cb unsafe.Pointer) {
	stdout := &Context{ctx, cb, 1}
	stderr := &Context{ctx, cb, 2}
	go func() {
		err := InvokeHelper(id, args, sep, bytes.NewBufferString(sin), stdout, stderr)
		stdout.Finished(err)
	}()
}

//export Invoke
func Invoke(id string, args string, sep string, sin string, sout *string, serr *string) int32 {
	var err error
	var stdout bytes.Buffer
	var stderr bytes.Buffer
	err = InvokeHelper(id, args, sep, bytes.NewBufferString(sin), &stdout, &stderr)
	*sout = stdout.String()
	*serr = stderr.String()
	if err != nil {
		*serr = err.Error()
		return -1
	}
	return 0
}

func InvokeCommand(id string, args string, sep string, stdin string) (sout string, serr string, err error) {
	var stdout bytes.Buffer
	var stderr bytes.Buffer
	err = InvokeHelper(id, args, sep, bytes.NewBufferString(stdin), &stdout, &stderr)
	sout = stdout.String()
	serr = stderr.String()
	return
}

func InvokeHelper(id string, args string, sep string, stdin io.Reader, stdout io.Writer, stderr io.Writer) error {
	for _, cmd := range command.CommandList() {
		if cmd == string(id) {
			var arguments []string
			arguments = append(arguments, id)
			if len(args) > 0 {
				if sep == "" {
					sep = " "
				}
				for _, opt := range strings.Split(args, sep) {
					if len(opt) > 0 {
						arguments = append(arguments, opt)
					}
				}
			}
			err := command.RunArgs(arguments,
				stdin,
				stdout,
				stderr,
			)
			return err
		}
	}
	return os.ErrInvalid
}

func init() {
	command.Register(types.Command)
	command.Register(jsonfmt.Command)
	command.Register(finddoc.Command)
	command.Register(runcmd.Command)
	command.Register(docview.Command)
	command.Register(astview.Command)
	command.Register(gofmt.Command)
	command.Register(gopresent.Command)
	command.Register(pkgs.Command)
}

func main() {}
