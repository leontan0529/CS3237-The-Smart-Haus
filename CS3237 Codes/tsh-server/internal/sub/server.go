package sub

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

var subServer *http.Server

func StartServer() {
	s, err := initServer()
	if err != nil {
		log.Fatalf("Failed to start subscriber server: %v", err)
	}

	go func() {
		if err := s.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
			logging.Log(logging.LogRequest{
				ServiceName: logging.SUB,
				Level:       "ERROR",
				Message:     fmt.Sprintf("Failed to Serve: %v", err),
			})
			os.Exit(1)
		}
	}()
}

func StopServer(ctx context.Context) error {
	return subServer.Shutdown(ctx)
}

func initServer() (*http.Server, error) {
	logging.Log(logging.LogRequest{
		ServiceName: logging.SUB,
		Level:       "INFO",
		Message:     "Starting SUBSCRIBER Server on port 8080...",
	})

	api := &subAPI{}
	mux := http.NewServeMux()
	mux.HandleFunc("sub/data", api.humidity)
	mux.HandleFunc("sub/motion", api.motion)
	mux.HandleFunc("sub/brightness", api.brightness)
	mux.HandleFunc("sub/temperature", api.temperature)
	mux.HandleFunc("sub/images", api.images)

	server := &http.Server{
		Addr:              ":8080",
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second, // Adjust this value as needed
	}

	return server, nil
}
