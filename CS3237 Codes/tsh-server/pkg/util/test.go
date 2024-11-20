package util

// Helper function to check if a string contains a substring
func Contains(str, substr string) bool {
	return len(str) >= len(substr) && str[:len(substr)] == substr
}
