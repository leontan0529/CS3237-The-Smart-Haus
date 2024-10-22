package main

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"
	"tsh-server/internal/health"
	"tsh-server/internal/pub"
	"tsh-server/internal/sub"

	"tsh-server/pkg/database"
	"tsh-server/pkg/logging"
)

func main() {
	// Initialize the logging
	logging.Init()
	database.Init()

	// Start the health check server
	health.StartServer()

	// Start the subscriber server
	pub.StartServer()

	// Start the publisher server
	sub.StartServer()

	// Register signals Channel to receive the SIGTERM and SIGINT Signals
	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, os.Interrupt, syscall.SIGINT, syscall.SIGTERM)

	// Information to signify on logs that Server has started
	logging.Log(logging.LogRequest{
		ServiceName: logging.SERVER,
		Endpoint:    logging.ROOT,
		Level:       "INFO",
		Message:     "Listening for Stop Signal...",
	})

	// Wait for a stop signal
	<-sigs
	logging.Log(logging.LogRequest{
		ServiceName: logging.SERVER,
		Endpoint:    logging.ROOT,
		Level:       "INFO",
		Message:     "Conducting Graceful Shutdown",
	})

	// Create a context with a timeout for the graceful shutdown
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	// Stop the subscriber server
	if err := pub.StopServer(ctx); err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.PUB,
			Endpoint:    logging.ROOT,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Failed to stop PUBLISHER server: %v", err),
		})
	}

	// Stop the publisher server
	if err := sub.StopServer(ctx); err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.SUB,
			Endpoint:    logging.ROOT,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Failed to stop SUBSCRIBER server: %v", err),
		})
	}

	// Stop the health check server
	if err := health.StopServer(ctx); err != nil {
		logging.Log(logging.LogRequest{
			ServiceName: logging.HEALTH,
			Endpoint:    logging.ROOT,
			Level:       "ERROR",
			Message:     fmt.Sprintf("Failed to stop HEALTH server: %v", err),
		})
	}

	// Optionally wait for other background processes to finish
	<-ctx.Done()

	// Final statement to show SCRIBE has exited
	logging.Log(logging.LogRequest{
		ServiceName: logging.SERVER,
		Endpoint:    logging.ROOT,
		Level:       "INFO",
		Message:     "Server Exited Properly",
	})
}
