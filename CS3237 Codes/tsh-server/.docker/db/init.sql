CREATE TABLE IF NOT EXISTS sensor_data (
    id SERIAL PRIMARY KEY,
    humidity NUMERIC,             -- Stores the humidity value
    motion NUMERIC,               -- Stores the motion sensor value (infra-red value)
    brightness NUMERIC,           -- Stores the brightness value
    temperature NUMERIC,          -- Stores the temperature value
    image_link TEXT,              -- Stores the image link as a numeric value (could be URL or another identifier)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Timestamp of the data entry
);

