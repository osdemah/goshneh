package goshneh

/*
#cgo LDFLAGS: -lavahi-client -lavahi-common
#include "service.h"
*/
import "C"

type Service struct {
	Name   string
	Type   string
	Domain *string
	Host   *string
	Port   uint16
}

var context C.Context
var publishingServices chan Service = make(chan Service, 10)

func Publish(service Service) {
	publishingServices <- service
}

//export publishing
func publishing(service *C.Service) bool {
	select {
	case s := <-publishingServices:
		service = (*C.Service)(C.malloc(C.sizeof_Service))
		service.name = C.CString(s.Name)
		service._type = C.CString(s.Type)
		if s.Domain != nil {
			service.domain = C.CString(*s.Domain)
		} else {
			service.domain = nil
		}
		if s.Host != nil {
			service.host = C.CString(*s.Host)
		} else {
			service.host = nil
		}
		service.port = (C.ushort)(s.Port)
		return true
	default:
		return false
	}
}

func Setup() {
	C.setup(&context)
}

func Run() {
	go C.run(&context)
}

func Clean() {
	C.clean(&context)
}

func Quit() {
	C.quit(&context)
}
