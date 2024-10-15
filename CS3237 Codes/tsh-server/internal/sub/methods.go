package sub

import "net/http"

type subAPI struct{}

func (h *subAPI) humidity(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *subAPI) motion(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *subAPI) brightness(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

func (h *subAPI) temperature(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}

// TODO: to determine necessity of this
func (h *subAPI) images(w http.ResponseWriter, r *http.Request) {
	// Add your liveness check logic here
	w.WriteHeader(http.StatusOK)
}
