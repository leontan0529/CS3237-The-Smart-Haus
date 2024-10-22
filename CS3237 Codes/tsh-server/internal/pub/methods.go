package pub

import (
	"encoding/json"
	"fmt"
	"net/http"
	"tsh-server/pkg/database"
	"tsh-server/pkg/logging"
)

type pubAPI struct{}

func (h *pubAPI) thEsp32(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ESP32TH,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Received an invalid HTTP Request: %v", r.Method),
		})
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	thDataList, err := database.GetAllThData()
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ESP32TH,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Database retrieval of temperature/humidity data failed: %v", err),
		})
		http.Error(w, "Failed to retrieve temperature/humidity data", http.StatusInternalServerError)
		return
	}

	err = json.NewEncoder(w).Encode(thDataList)
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ESP32TH,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Failed to respond back: %v", err),
		})
		return
	}
}

func (h *pubAPI) mbEsp32(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ESP32MB,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Received an invalid HTTP Request: %v", r.Method),
		})
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	mbDataList, err := database.GetAllMbData()
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ESP32MB,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Database retrieval of motion/brightness data failed: %v", err),
		})
		http.Error(w, "Failed to retrieve temperature/humidity data", http.StatusInternalServerError)
		return
	}

	err = json.NewEncoder(w).Encode(mbDataList)
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ESP32MB,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Failed to respond back: %v", err),
		})
		return
	}
}

func (h *pubAPI) imagesEsp32(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *pubAPI) all(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}
