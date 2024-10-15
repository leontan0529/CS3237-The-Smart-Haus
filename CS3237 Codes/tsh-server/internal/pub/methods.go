package pub

import "net/http"

type pubAPI struct{}

func (h *pubAPI) humidity(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *pubAPI) motion(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *pubAPI) brightness(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *pubAPI) temperature(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

// TODO: to determine necessity of this
func (h *pubAPI) images(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}
