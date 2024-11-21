package logging

type ServiceType string

const (
	PUB    ServiceType = "PUBLISHER"
	SUB    ServiceType = "SUBSCRIBER"
	SERVER ServiceType = "SERVER"
	HEALTH ServiceType = "HEALTH"
)

type Endpoint string

const (
	ROOT      Endpoint = "/"
	ESP32TH   Endpoint = "/esp32-th"
	ESP32MB   Endpoint = "/esp32-mb"
	ESP32DOOR Endpoint = "/esp32-door"
	HEALTHZ   Endpoint = "/healthz"
	LIVENESS  Endpoint = "/liveness"
)

type LogRequest struct {
	ServiceName ServiceType
	Endpoint    Endpoint
	Level       string
	Message     string
}
