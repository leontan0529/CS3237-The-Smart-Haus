package health

import (
	"fmt"
	"net/http"
	"tsh-server/pkg/logging"
)

type healthAPI struct{}

func (h *healthAPI) readinessProbe(w http.ResponseWriter, r *http.Request) {
	// Add your readiness check logic here
	logging.Log(logging.LogRequest{
		ServiceName: logging.HEALTH,
		Endpoint:    logging.HEALTHZ,
		Level:       "INFO",
		Message:     fmt.Sprintf("Received a health check from: %v", r.Host),
	})
	w.WriteHeader(http.StatusOK)
}

func (h *healthAPI) livenessProbe(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	logging.Log(logging.LogRequest{
		ServiceName: logging.HEALTH,
		Endpoint:    logging.LIVENESS,
		Level:       "INFO",
		Message:     fmt.Sprintf("Received a liveness check from: %v", r.Host),
	})
	w.WriteHeader(http.StatusOK)
}
