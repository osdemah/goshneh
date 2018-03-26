package goshneh

// #include "service.h"
import "C"
import "errors"

var browsingTypes chan string = make(chan string, 10)

var ResolvedCallback func(service Service, err error)
var RemovedCallback func(service Service)

func Browse(t string) {
	browsingTypes <- t
}

//export browsing
func browsing(typeToBrowse **C.char) bool {
	select {
	case t := <-browsingTypes:
		*typeToBrowse = C.CString(t)
		return true
	default:
		return false
	}
}

//export browseCallback
func browseCallback(service *C.Service, event uint8, err uint8, strerr *C.char) {
	if event != ServiceRemoved && ResolvedCallback != nil {
		var e error = nil
		if err != 0 {
			if strerr != nil {
				e = errors.New(ErrorsString[err] + ": " + C.GoString(strerr))
			} else {
				e = errors.New(ErrorsString[err])
			}

		}

		goService := Service{
			Name: C.GoString(service.name),
			Type: C.GoString(service._type),
			Port: (uint16)(service.port),
		}

		c2GoStringPtr(service.host, goService.Host)
		c2GoStringPtr(service.domain, goService.Domain)

		ResolvedCallback(goService, e)
	}

	if event == ServiceRemoved && RemovedCallback != nil {
		goService := Service{
			Name: C.GoString(service.name),
			Type: C.GoString(service._type),
		}

		c2GoStringPtr(service.domain, goService.Domain)

		RemovedCallback(goService)

	}
}
