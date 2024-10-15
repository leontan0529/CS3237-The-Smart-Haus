package health

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

var healthServer *http.Server

func StartServer() {
	s, err := initServer()
	if err != nil {
		log.Fatalf("Failed to start health check server: %v", err)
	}

	go func() {
		if err := s.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
			logging.Log(logging.LogRequest{
				ServiceName: logging.HEALTH,
				Level:       "ERROR",
				Message:     fmt.Sprintf("Failed to Serve: %v", err),
			})
			os.Exit(1)
		}
	}()
}

func StopServer(ctx context.Context) error {
	return healthServer.Shutdown(ctx)
}

func initServer() (*http.Server, error) {
	logging.Log(logging.LogRequest{
		ServiceName: logging.HEALTH,
		Level:       "INFO",
		Message:     "Starting HEALTH Server on port 8080...",
	})

	api := &healthAPI{}
	mux := http.NewServeMux()
	mux.HandleFunc("/healthz", api.readinessProbe)
	mux.HandleFunc("/liveness", api.livenessProbe)

	server := &http.Server{
		Addr:              ":9000",
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second, // Adjust this value as needed
	}

	return server, nil
}
