package database

// Function to read all rows from the th_data table
func GetAllThData() ([]Esp32ThData, error) {
	query := `SELECT temperature, humidity, created_at FROM th_data`
	rows, err := db.Query(query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var thDataList []Esp32ThData
	for rows.Next() {
		var thData Esp32ThData
		err := rows.Scan(&thData.Temperature, &thData.Humidity, &thData.CreatedAt)
		if err != nil {
			return nil, err
		}
		thDataList = append(thDataList, thData)
	}

	return thDataList, nil
}

// Function to read all rows from the mb_data table
func GetAllMbData() ([]Esp32MbData, error) {
	query := `SELECT motion, brightness, created_at FROM th_data`
	rows, err := db.Query(query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var mbDataList []Esp32MbData
	for rows.Next() {
		var mbData Esp32MbData
		err := rows.Scan(&mbData.Motion, &mbData.Brightness, &mbData.CreatedAt)
		if err != nil {
			return nil, err
		}
		mbDataList = append(mbDataList, mbData)
	}

	return mbDataList, nil
}
