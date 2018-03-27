package goshneh

// #include "service.h"
import "C"

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
	// OPTIMIZE: Isn't better to call callbacks in another goroutine to prevent avahi loop be blocked?
	if event != ServiceRemoved && ResolvedCallback != nil {
		ResolvedCallback(cService2goService(service), constructError(err, strerr))
	}

	if event == ServiceRemoved && RemovedCallback != nil {
		RemovedCallback(cService2goService(service))
	}
}
