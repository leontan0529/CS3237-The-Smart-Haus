package logging

import (
	"log"
	"os"
	"path/filepath"
)

func Init() {
	var output *os.File
	var err error

	// Get log file path from environment variable
	logFilePath := os.Getenv("LOG_FILE_PATH")
	if logFilePath == "" {
		logFilePath = "./logs/logs.txt" // default path
	}

	// Ensure the directory exists
	err = os.MkdirAll(filepath.Dir(logFilePath), os.ModePerm)
	if err != nil {
		log.Fatalf("Failed to create log directory: %v", err)
	}

	// Open the log file
	output, err = os.OpenFile(logFilePath, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0600)
	if err != nil {
		log.Fatalf("Failed to open log file for writing: %v", err)
	}

	InfoLogger = outputLog(output, "<NEW LOG ENTRY>\n->[INFO] ")
	WarningLogger = outputLog(output, "<NEW LOG ENTRY>\n->[WARNING] ")
	ErrorLogger = outputLog(output, "<NEW LOG ENTRY>\n->[ERROR] ")
	DebugLogger = outputLog(output, "<NEW LOG ENTRY>\n->[DEBUG] ")
}

func outputLog(output *os.File, prefix string) func(string) {
	logger := log.New(output, prefix, log.Ldate|log.Ltime)
	return func(message string) {
		logger.Println(message)
	}
}
