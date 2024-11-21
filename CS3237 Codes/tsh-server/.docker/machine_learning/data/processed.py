import pandas as pd

# Assuming you have already preprocessed your data
def save_cleaned_data(data_th_cleaned, data_mb_cleaned):
    # Save temperature and humidity cleaned data
    data_th_cleaned.to_csv('data/processed/cleaned_temperature_humidity.csv', index=False)
    # Save motion and brightness cleaned data
    data_mb_cleaned.to_csv('data/processed/cleaned_motion_brightness.csv', index=False)

# Example usage after preprocessing
save_cleaned_data(data_th_cleaned, data_mb_cleaned)