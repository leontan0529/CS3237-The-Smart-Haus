import pandas as pd

def preprocess_data(data_th, data_mb):
    # Preprocess temperature and humidity data
    data_th['created_at_th'] = pd.to_datetime(data_th['created_at']).dt.tz_convert('UTC')  # Ensure it's in UTC
    data_th['hour'] = data_th['created_at_th'].dt.hour  # Extract hour

    # Preprocess motion and brightness data
    if 'created_at' in data_mb.columns:
        data_mb['created_at_mb'] = pd.to_datetime(data_mb['created_at']).dt.tz_convert('UTC')  # Ensure it's in UTC
        data_mb['hour'] = data_mb['created_at_mb'].dt.hour  # Extract hour

    # Check for NaN values in relevant columns before proceeding
    relevant_columns_th = ['humidity', 'temperature', 'hour', 'created_at_th']  # Include 'created_at_th'
    relevant_columns_mb = ['motion', 'brightness', 'hour', 'created_at_mb']  # Include 'created_at_mb'

    # Drop rows with NaN values in temperature/humidity dataset
    data_th_cleaned = data_th[relevant_columns_th].dropna()
    
    # Drop rows with NaN values in motion dataset
    data_mb_cleaned = data_mb[relevant_columns_mb].dropna()

    # Combine cleaned datasets for clustering
    df = pd.concat([data_th_cleaned, data_mb_cleaned], axis=1)

    # Check for NaN values after concatenation
    if df.isnull().values.any():
        # print("Warning: Data contains NaN values after concatenation.")
        df = df.dropna()  # Drop any remaining NaN values

    # print("Columns in df:", df.columns.tolist())
    # print(df)
    return data_th_cleaned, data_mb_cleaned, df  # Return df as well