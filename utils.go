package goshneh

//#include "service.h"
import "C"

import "errors"

func c2GoStringPtr(s *C.char, output *string) {
	if s != nil {
		temp := C.GoString(s)
		output = &temp
	}
	output = nil
}

func constructError(err uint8, strerr *C.char) error {
	if err != 0 {
		if strerr != nil {
			return errors.New(ErrorsString[err] + ": " + C.GoString(strerr))
		} else {
			return errors.New(ErrorsString[err])
		}
	}
	return nil
}

func cService2goService(service *C.Service) Service {
	goService := Service{
		Name:      C.GoString(service.name),
		Type:      C.GoString(service._type),
		Port:      (uint16)(service.port),
		Collision: (uint8)(service.collision),
	}

	c2GoStringPtr(service.host, goService.Host)
	c2GoStringPtr(service.domain, goService.Domain)

	return goService
}
