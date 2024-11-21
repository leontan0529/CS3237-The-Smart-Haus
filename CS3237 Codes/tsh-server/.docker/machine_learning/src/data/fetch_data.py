import requests
import pandas as pd

# Function to fetch data from a given endpoint
def fetch_data(url):
    try:
        response = requests.get(url)
        response.raise_for_status()  # Raise an error for bad responses (4xx or 5xx)
        return pd.DataFrame(response.json())  # Convert JSON response to DataFrame
    except requests.exceptions.RequestException as e:
        print(f"Error fetching data from {url}: {e}")
        return pd.DataFrame()  # Return an empty DataFrame on error

# Fetch data from esp32-th and esp32-mb endpoints
data_th = fetch_data('http://localhost:8081/esp32-th')
data_mb = fetch_data('http://localhost:8081/esp32-mb')

# Check if either DataFrame is empty
if data_th.empty or data_mb.empty:
    print("One or both datasets are empty. Exiting.")