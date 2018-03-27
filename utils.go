package goshneh

import "C"

func c2GoStringPtr(s *C.char, output *string) {
	if s != nil {
		temp := C.GoString(s)
		output = &temp
	}
	output = nil
}
