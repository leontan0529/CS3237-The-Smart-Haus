import os
import pandas as pd

CACHE_FILE_PATH = 'data_cache.txt'  # Path for your cache file

def get_cached_length():
    """Retrieve the cached length from the file."""
    if os.path.exists(CACHE_FILE_PATH):
        with open(CACHE_FILE_PATH, 'r') as file:
            return int(file.read().strip())
    return 0  # Return 0 if cache file does not exist

def update_cache(length):
    """Update the cache file with the new length."""
    with open(CACHE_FILE_PATH, 'w') as file:
        file.write(str(length))

def should_retrain():
    """Check if retraining is needed based on CSV data."""
    
    # Load previously saved datasets from CSV files
    try:
        data_th = pd.read_csv('data_th.csv')
        data_mb = pd.read_csv('data_mb.csv')
        
        # Check if either DataFrame is empty
        if data_th.empty or data_mb.empty:
            print("One or both datasets are empty. Exiting.")
            return False
        
        # Get current lengths of fetched data
        current_length = len(data_th) + len(data_mb)
        
        # Compare with cached length
        cached_length = get_cached_length()
        
        if current_length > cached_length:
            update_cache(current_length)  # Update cache with new length
            return True  # Retrain is needed
        
        return False  # No retraining needed
    
    except FileNotFoundError:
        print("CSV files not found. Retraining will be required.")
        return True  # If files do not exist, we should retrain
