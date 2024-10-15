package logging

type ServiceType string

const (
	PUB    ServiceType = "PUB"
	SUB    ServiceType = "SUB"
	SERVER ServiceType = "SERVER"
	HEALTH ServiceType = "HEALTH"
)

type LogRequest struct {
	ServiceName ServiceType
	Level       string
	Message     string
}
