// liteapp.go
package main

/*
extern void cdrv_init(void *fn);
extern int cdrv_main(int argc,char** argv);
//extern void cdrv_cb(void *cb, void *id, void *reply, int size, int flag, void* ctx);

static void cdrv_init_ex()
{
	extern int godrv_call(void* id,int id_size, void* args, int args_size, void* cb, void* ctx);
	cdrv_init(&godrv_call);
}

static void cdrv_cb(void *cb, void *id, int id_size, void *reply, int size, int flag, void* ctx)
{
	typedef void (*DRV_CALLBACK)(void *id, int id_size, void *reply, int len, int flag, void *ctx);
    ((DRV_CALLBACK)(cb))(id,id_size,reply,size,flag,ctx);
}

#cgo windows LDFLAGS: -L../../liteide/bin -lliteapp
#cgo linux LDFLAGS: -L../../liteide/bin -lliteapp
#cgo openbsd LDFLAGS: -L../../liteide/bin -lliteapp
#cgo darwin LDFLAGS: -L../../liteide/bin/liteide.app/Contents/MacOS
*/
import "C"
import (
	"bytes"
	"log"
	"strings"
	"unsafe"

	"github.com/visualfc/gotools/command"
)

func cdrv_main(args []string) int {
	argc := len(args)
	var cargs []*C.char
	for _, arg := range args {
		size := len(arg)
		data := make([]C.char, size+1)
		for i := 0; i < size; i++ {
			data[i] = (C.char)(arg[i])
		}
		data[size] = 0
		cargs = append(cargs, &data[0])
	}
	C.cdrv_init_ex()
	return int(C.cdrv_main(C.int(argc), &cargs[0]))
}

func cdrv_cb(cb unsafe.Pointer, id []byte, reply []byte, flag int, ctx unsafe.Pointer) {
	if len(reply) == 0 {
		C.cdrv_cb(cb, unsafe.Pointer(&id[0]), C.int(len(id)), nil, 0, C.int(flag), ctx)
	} else {
		C.cdrv_cb(cb, unsafe.Pointer(&id[0]), C.int(len(id)), unsafe.Pointer(&reply[0]), C.int(len(reply)), C.int(flag), ctx)
	}
}

//export godrv_call
func godrv_call(id unsafe.Pointer, id_size C.int, args unsafe.Pointer, size C.int, cb unsafe.Pointer, ctx unsafe.Pointer) C.int {
	return C.int(go_call(C.GoBytes(id, id_size), C.GoBytes(args, size), cb, ctx))
}

type app_writer struct {
	id   []byte
	cb   unsafe.Pointer
	ctx  unsafe.Pointer
	flag int
}

func (w *app_writer) Write(p []byte) (n int, err error) {
	n = len(p)
	log.Println(string(w.id), string(p))
	cdrv_cb(w.cb, w.id, p, w.flag, w.ctx)
	return
}

var (
	buf bytes.Buffer
)

func init() {
	command.Stdin = &buf
}

type Context struct {
	buf bytes.Buffer
}

var (
	contextMap = make(map[unsafe.Pointer]*Context)
)

func go_call(id []byte, args []byte, cb unsafe.Pointer, ctx unsafe.Pointer) int {
	if string(id) == "stdin" {
		if context, ok := contextMap[ctx]; ok {
			context.buf.Write(args)
		}
		return 0
	}
	for _, cmd := range command.CommandList() {
		if cmd == string(id) {
			go func() {
				var arguments []string
				arguments = append(arguments, string(id))
				if len(args) > 0 {
					for _, opt := range strings.Split(string(args), ";;") {
						if len(opt) > 0 {
							arguments = append(arguments, opt)
						}
					}
				}
				context := &Context{}
				contextMap[ctx] = context
				//start
				cdrv_cb(cb, id, nil, 0, ctx)
				//run command
				err := command.RunArgs(arguments,
					&context.buf,
					&app_writer{id, cb, ctx, 1},
					&app_writer{id, cb, ctx, 2},
				)
				//finished
				if err == nil {
					cdrv_cb(cb, id, nil, 3, ctx)
				} else {
					cdrv_cb(cb, id, []byte(err.Error()), 4, ctx)
				}
			}()
			return 0
		}
	}
	return -1
}
