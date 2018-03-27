package goshneh

//#include "service.h"
import "C"

import "errors"

var publishingServices chan Service = make(chan Service, 10)

var PublishedCallback func(service Service, err error)

func Publish(service Service) {
	publishingServices <- service
}

//export publishedCallback
func publishedCallback(service *C.Service, err uint8, strerr *C.char) {
	// OPTIMIZE: Isn't better to call callbacks in another goroutine to prevent avahi loop be blocked?
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
