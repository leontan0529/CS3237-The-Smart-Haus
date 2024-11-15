package pub

import (
	"context"
	"errors"
	"fmt"
	"log"
	"net/http"
	"os"
	"time"
	"tsh-server/pkg/logging"
)

var pubServer *http.Server

func StartServer() {
	s, err := initServer()
	if err != nil {
		log.Fatalf("Failed to start publisher server: %v", err)
	}

	go func() {
		if err := s.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
			logging.Log(logging.LogRequest{
				ServiceName: logging.PUB,
				Endpoint:    logging.ROOT,
				Level:       "ERROR",
				Message:     fmt.Sprintf("Failed to Serve: %v", err),
			})
			os.Exit(1)
		}
	}()
}

func StopServer(ctx context.Context) error {
	return pubServer.Shutdown(ctx)
}

func initServer() (*http.Server, error) {
	logging.Log(logging.LogRequest{
		ServiceName: logging.PUB,
		Endpoint:    logging.ROOT,
		Level:       "INFO",
		Message:     "Starting PUBLISHER Server on port 8081...",
	})

	api := &pubAPI{}
	mux := http.NewServeMux()
	mux.HandleFunc("/esp32-th", api.thEsp32)
	mux.HandleFunc("/esp32-mb", api.mbEsp32)
	mux.HandleFunc("/esp32-images", api.imagesEsp32)
	mux.HandleFunc("/all", api.all)

	pubServer := &http.Server{
		Addr:              ":8081",
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second, // Adjust this value as needed
	}

	return pubServer, nil
}
