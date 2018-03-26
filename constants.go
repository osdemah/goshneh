package goshneh

const (
	Successfull                   uint8 = 0
	ServiceDuplicateNameCollision uint8 = 1
	ServiceRegistrationFailed     uint8 = 2
	ClientFailure                 uint8 = 3
	EntryGroupFailure             uint8 = 4
	AllocationFailure             uint8 = 5
	BrowseFailure                 uint8 = 6
	ResolveFailure                uint8 = 7
)

var ErrorsString = [...]string{
	"",
	"Service Duplicate Name Collision",
	"Service Registration Failed",
	"Client Failure",
	"Entry Group Failure",
	"Allocation Failure",
	"Browse Failue",
	"Resolve Failue",
}

const (
	AlternativeNameOnCollision uint8 = 0
	DoNothingOnCollision       uint8 = 1
)

const (
	ServiceResolved uint8 = 1
	ServiceRemoved  uint8 = 2
)
