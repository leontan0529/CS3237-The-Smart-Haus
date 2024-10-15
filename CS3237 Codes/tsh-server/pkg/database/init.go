package database

import (
	"database/sql"
	"log"
	"os"

	_ "github.com/lib/pq"
)

var db *sql.DB

func Init() {
	// Set up the PostgreSQL connection string
	connStr := os.Getenv("DB_URL")

	// Open a connection to the database
	var err error
	db, err = sql.Open("postgres", connStr)
	if err != nil {
		log.Fatalf("Failed to open PostgreSQL connection: %v", err)
	}

	// Verify the connection
	err = db.Ping()
	if err != nil {
		log.Fatalf("Failed to connect to PostgreSQL: %v", err)
	}
}
