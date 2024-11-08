-- insert_data.sql
CREATE TABLE IF NOT EXISTS th_data (
    id SERIAL PRIMARY KEY,
    humidity NUMERIC,             -- Stores the humidity value
    temperature NUMERIC,          -- Stores the temperature value
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Timestamp of the data entry
);

INSERT INTO th_data (humidity, temperature) VALUES (45.5, 22.3);
INSERT INTO th_data (humidity, temperature) VALUES (50.1, 23.1);
INSERT INTO th_data (humidity, temperature) VALUES (47.8, 21.9);
