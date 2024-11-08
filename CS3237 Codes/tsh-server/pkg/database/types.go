package database

import "time"

type Esp32MbData struct {
	Motion     float64   `json:"motion"`
	Brightness float64   `json:"brightness"`
	CreatedAt  time.Time `json:"created_at"`
}

type Esp32ThData struct {
	Temperature float64   `json:"temperature"`
	Humidity    float64   `json:"humidity"`
	CreatedAt   time.Time `json:"created_at"`
}
