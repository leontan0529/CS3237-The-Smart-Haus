package sub

type HumidityData struct {
	Value float64 `json:"value"`
}

type MotionData struct {
	Value float64 `json:"value"`
}

type BrightnessData struct {
	Value float64 `json:"value"`
}

type TemperatureData struct {
	Value float64 `json:"value"`
}

// TODO: Decide to store on ESP32 side or transmit images via some RAW format and process in server
type ImageData struct {
	Value string `json:"value"`
}
