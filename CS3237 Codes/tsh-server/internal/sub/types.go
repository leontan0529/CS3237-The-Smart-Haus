package sub

type Esp32ThData struct {
	Temperature float64 `json:"Temperature,string"`
	Humidity    float64 `json:"Humidity,string"`
}

type Esp32MbData struct {
	Motion     int64 `json:"Motion,string"`
	Brightness int64 `json:"Brightness,string"`
}

// TODO: Decide to store on ESP32 side or transmit images via some RAW format and process in server
type Esp32DoorData struct {
	Intrusion int64 `json:"Intrusion,string"`
}
