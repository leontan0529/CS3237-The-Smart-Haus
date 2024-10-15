package health

import "net/http"

type healthAPI struct{}

func (h *healthAPI) readinessProbe(w http.ResponseWriter, r *http.Request) {
	// Add your readiness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *healthAPI) livenessProbe(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}
