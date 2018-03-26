package goshneh

/*
#cgo LDFLAGS: -L/usr/local/lib -lavahi-client -lavahi-common
#include "service.h"
*/
import "C"

import "errors"

type Service struct {
	Name      string
	Type      string
	Domain    *string
	Host      *string
	Port      uint16
	Collision uint8
}

var context C.Context
var publishingServices chan Service = make(chan Service, 10)

var PublishedCallback func(service Service, err error)

func Publish(service Service) {
	publishingServices <- service
}

func c2GoStringPtr(s *C.char, output *string) {
	if s != nil {
		*output = C.GoString(s)
	}
	output = nil
}

//export publishedCallback
func publishedCallback(service *C.Service, err uint8, strerr *C.char) {
	if PublishedCallback != nil {
		var e error = nil
		if err != 0 {
			if strerr != nil {
				e = errors.New(ErrorsString[err] + ": " + C.GoString(strerr))
			} else {
				e = errors.New(ErrorsString[err])
			}

		}

		goService := Service{
			Name:      C.GoString(service.name),
			Type:      C.GoString(service._type),
			Port:      (uint16)(service.port),
			Collision: (uint8)(service.collision),
		}

		c2GoStringPtr(service.host, goService.Host)
		c2GoStringPtr(service.domain, goService.Domain)

		PublishedCallback(goService, e)
	}
}

//export publishing
func publishing(service **C.Service) bool {
	select {
	case s := <-publishingServices:
		*service = (*C.Service)(C.malloc(C.sizeof_Service))
		(*service).name = C.CString(s.Name)
		(*service)._type = C.CString(s.Type)
		if s.Domain != nil {
			(*service).domain = C.CString(*s.Domain)
		} else {
			(*service).domain = nil
		}
		if s.Host != nil {
			(*service).host = C.CString(*s.Host)
		} else {
			(*service).host = nil
		}
		(*service).port = (C.ushort)(s.Port)
		(*service).collision = (C.uchar)(s.Collision)
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
