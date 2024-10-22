package sub

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"tsh-server/pkg/logging"
)

type subAPI struct{}

func (s *subAPI) thEsp32(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ESP32TH,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Received an invalid HTTP Request: %v", r.Method),
		})
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ESP32TH,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Unable to read POST Request Body: %v", err),
		})
		http.Error(w, "Unable to read request body", http.StatusBadRequest)
		return
	}
	defer r.Body.Close()

	fmt.Println(string(body))
	var data Esp32ThData
	err = json.Unmarshal(body, &data)
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ESP32TH,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Invalid JSON format: %v", err),
		})
		http.Error(w, "Invalid JSON format", http.StatusBadRequest)
		return
	}

	// Log the received data for debugging purposes
	logging.Log(logging.LogRequest{
		ServiceName: logging.SUB,
		Endpoint:    logging.ESP32TH,
		Level:       "INFO",
		Message:     fmt.Sprintf("Received Data : %+v\n", data),
	})

	// You can add more processing logic here if needed
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("Data received successfully"))
}

func (s *subAPI) mbEsp32(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ESP32MB,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Received an invalid HTTP Request: %v", r.Method),
		})
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ESP32MB,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Unable to read POST Request Body: %v", err),
		})
		http.Error(w, "Unable to read request body", http.StatusBadRequest)
		return
	}
	defer r.Body.Close()

	fmt.Println(string(body))
	var data Esp32MbData
	err = json.Unmarshal(body, &data)
	if err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ESP32MB,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Invalid JSON format: %v", err),
		})
		http.Error(w, "Invalid JSON format", http.StatusBadRequest)
		return
	}

	// Log the received data for debugging purposes
	logging.Log(logging.LogRequest{
		ServiceName: logging.SUB,
		Endpoint:    logging.ESP32MB,
		Level:       "INFO",
		Message:     fmt.Sprintf("Received Data : %+v\n", data),
	})

	// You can add more processing logic here if needed
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("Data received successfully"))
}

// TODO: to determine necessity of this
func (s *subAPI) imagesEsp32(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}
