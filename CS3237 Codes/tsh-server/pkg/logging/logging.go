package logging

import (
	"fmt"
	"runtime"
)

var (
	// InfoLogger logs informational messages.
	// This function is initialized in the Init function and is used to log messages with the INFO level.
	InfoLogger func(string)

	// WarningLogger logs warning messages.
	// This function is initialized in the Init function and is used to log messages with the WARNING level.
	WarningLogger func(string)

	// ErrorLogger logs error messages.
	// This function is initialized in the Init function and is used to log messages with the ERROR level.
	ErrorLogger func(string)

	// DebugLogger logs debug messages.
	// This function is initialized in the Init function and is used to log messages with the DEBUG level.
	DebugLogger func(string)

	// debugMode indicates if the debug mode is enabled.
	// When set to true, additional debug information may be logged.
	// This variable can be controlled to toggle detailed logging for debugging purposes.
	debugMode = false
)

func Log(req LogRequest) {
	// Format the log message to include additional context
	formattedMessage := formatLog(req)

	switch req.Level {
	case "INFO":
		InfoLogger(formattedMessage)
	case "WARNING":
		WarningLogger(formattedMessage)
	case "ERROR":
		ErrorLogger(formattedMessage)
	case "DEBUG":
		DebugLogger(formattedMessage)
	}
}

func formatLog(req LogRequest) string {
	pc, filename, line, _ := runtime.Caller(3)
	return fmt.Sprintf(
		"\n->%s\n->[%s:%d]\n->[%s] [%s] [%s] %s: %s\n",
		runtime.FuncForPC(pc).Name(),
		filename,
		line,
		req.ServiceName, // Service name
		req.Level,       // Log level
		req.Message,     // The actual log message
	)
}
