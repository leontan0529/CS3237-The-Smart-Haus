package database

// Function to insert temperature and humidity data into the th_data table
func InsertThData(temperature, humidity float64) error {
	query := `INSERT INTO th_data (temperature, humidity) VALUES ($1, $2)`
	_, err := db.Exec(query, temperature, humidity)
	return err
}

// Function to insert motion and brightness data into the mb_data table
func InsertMbData(motion, brightness int64) error {
	query := `INSERT INTO mb_data (motion, brightness) VALUES ($1, $2)`
	_, err := db.Exec(query, motion, brightness)
	return err
}

// Function to insert image link into the image_data table
func InsertImageData(url string) error {
	query := `INSERT INTO image_data (image_link) VALUES ($1)`
	_, err := db.Exec(query, url)
	return err
}
