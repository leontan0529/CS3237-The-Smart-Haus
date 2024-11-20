CREATE TABLE IF NOT EXISTS th_data (
    id SERIAL PRIMARY KEY,
    humidity NUMERIC,             -- Stores the humidity value
    temperature NUMERIC,          -- Stores the temperature value
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Timestamp of the data entry
);

CREATE TABLE IF NOT EXISTS mb_data (
    id SERIAL PRIMARY KEY,
    motion NUMERIC,               -- Stores the motion sensor value (infra-red value)
    brightness NUMERIC,           -- Stores the brightness value
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Timestamp of the data entry
);

CREATE TABLE IF NOT EXISTS door_data (
    id SERIAL PRIMARY KEY,
    intrusion NUMERIC,              -- Stores the image link as a numeric value (could be URL or another identifier)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Timestamp of the data entry
);

