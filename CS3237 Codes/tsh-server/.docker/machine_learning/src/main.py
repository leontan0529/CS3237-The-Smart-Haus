import os
import pandas as pd
from data.fetch_data import fetch_data
from data.preprocess import preprocess_data
from models.clustering import perform_clustering
from models.random_forest import train_random_forest
from models.linear_regression import train_linear_regression
import datetime
import sys
import re
import requests
from apscheduler.schedulers.background import BackgroundScheduler
from models.cnn import train_cnn
from recognition import recognize_face
import threading
import time
import joblib
from models.cnn import load_model

sys.path.append('/Users/constanceow/Downloads/CS3237_ML/src/src')
import constants

def check_image_exists(url):
    """Check if an image exists at the given URL."""
    try:
        response = requests.head(url)
        return response.status_code == 200  # Return True if the image exists
    except Exception as e:
        print(f"Error checking image: {e}")
        return False
    
def download_image(url, save_path):
    """Download an image from a URL and save it to a specified path."""
    try:
        response = requests.get(url)
        response.raise_for_status()  # Raise an error for bad responses
        with open(save_path, 'wb') as file:
            file.write(response.content)
        print(f"Image downloaded and saved to {save_path}")
    except Exception as e:
        print(f"Error downloading image: {e}")

def process_and_recognize(model):
    """Continuously check for an image and save it if found."""
    image_url = 'http://localhost:8081/esp32-im'  # URL to check for image
    image_save_path = '/Users/constanceow/Downloads/CS3237_ML/training_capture/image.jpg'  # Path to save the downloaded image

    while True:
        if check_image_exists(image_url):
            print("Image found at URL. Downloading...")
            download_image(image_url, image_save_path)  # Download the image
            print(f"Image saved to {image_save_path}")

            # Call the recognition function after downloading the image
            check_and_recognize_faces(model)  # Recognize faces in the downloaded image

        time.sleep(5)  # Check every 5 seconds (adjust as needed)

def check_and_recognize_faces():
    """Check for images in the specified directory and recognize faces."""
    image_path = '/Users/constanceow/Downloads/CS3237_ML/training_capture/captured_image.jpg'  # Path to the saved image
    result_file_path = 'results.txt'

    CNN_OUTPUT = ""

    # Check if the saved image exists
    if os.path.exists(image_path):
        print(f"Processing image: {image_path}")

        # Recognize the face in the image
        known_faces = {
            0: "User A",
            1: "User B",
            2: "User C",
        }
        
        # Call recognize_face function and capture result
        CNN_OUTPUT = recognize_face(image_path, known_faces, model)
        
    else:
        print(f"No image found at {image_path} for recognition.")
    
    return CNN_OUTPUT

def train_models():
    best_rf_model_path = constants.BEST_RF_MODEL_PATH
    best_lr_model_path = constants.BEST_LR_MODEL_PATH

    # print(f'Best Random Forest Model Path: {best_rf_model_path}')
    # print(f'Best Linear Regression Model Path: {best_lr_model_path}')

    # Fetch data from APIs
    data_th = fetch_data('http://localhost:8081/esp32-th')
    data_mb = fetch_data('http://localhost:8081/esp32-mb')


    # Check if either DataFrame is empty
    if data_th.empty or data_mb.empty:
        print("One or both datasets are empty. Exiting.")
        return

    # Save fetched data to CSV files
    data_th.to_csv('data_th.csv', index=False)
    data_mb.to_csv('data_mb.csv', index=False)

    # Preprocess the fetched datasets
    data_th_cleaned, data_mb_cleaned, df = preprocess_data(data_th, data_mb)

    # Combine features for clustering
    df = pd.concat([data_th_cleaned, data_mb_cleaned], axis=1)

    # Check for NaNs again before clustering
    if df.isnull().values.any():
        df = df.dropna()  # Handle remaining NaNs if necessary

    # Perform clustering (if needed)
    clustered_data = perform_clustering(df)

    # Define features and target variable for regression models
    X = df[['hour']]
    y = df['brightness']
    
    pattern = r'_(\d{8}_\d{6})\.pkl$'
    base_model_name = re.sub(pattern, '', os.path.basename(best_rf_model_path))

    model_directory = constants.MODEL_PATH
    os.makedirs(model_directory, exist_ok=True)

    new_timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

    model_save_path_rfc = os.path.join(model_directory, f'{base_model_name}_{new_timestamp}.pkl')
    model_save_path_lr = os.path.join(model_directory.replace('RF', 'LR'), f'trained_LR_model_{new_timestamp}.pkl')

    # Train Random Forest Classifier
    train_random_forest(X, y, model_save_path_rfc)

    # Train Linear Regression Model
    train_linear_regression(X, y, model_save_path_lr)

    # print("Model training completed.")

    return df , data_th_cleaned, data_mb_cleaned  # Return the DataFrame

if __name__ == '__main__':
    # Train CNN once before starting recognition loop
    cnn_data_dir = '/Users/constanceow/Downloads/CS3237_ML'  # Adjust this path accordingly.
    resume_training = True  # Change this based on your requirement
    #train_cnn(data_dir=cnn_data_dir, num_epochs=15, resume_training=resume_training)
    model = load_model() # Train your CNN model once and get the trained model
    lr_data_dir = '/Users/constanceow/Downloads/CS3237_ML'
    CNN_OUTPUT = check_and_recognize_faces()
    model = joblib.load(constants.BEST_LR_MODEL_PATH)
    # Fetch data from APIs
    data_th = fetch_data('http://localhost:8081/esp32-th')
    data_mb = fetch_data('http://localhost:8081/esp32-mb')
    

    currDf = pd.read_csv('data_mb.csv')
    df, data_th_cleaned, data_mb_cleaned = train_models()

    # Print columns for debugging
    # print("Columns in data_mb_cleaned:", data_mb_cleaned.columns.tolist())

    # Ensure 'created_at_mb' is in datetime format
    if 'created_at_mb' not in data_mb_cleaned.columns:
        raise KeyError("'created_at_mb' column not found in data_mb_cleaned.")

    data_mb_cleaned['created_at_mb'] = pd.to_datetime(data_mb_cleaned['created_at_mb'])

    # Check if 'created_at' exists in currDf
    if 'created_at' not in currDf.columns:
        raise KeyError("'created_at' column not found in currDf.")

    currDf['created_at'] = pd.to_datetime(currDf['created_at'])

    # Use the first timestamp from data_mb_cleaned for reference
    if not data_mb_cleaned.empty:
        reference_time = data_mb_cleaned['created_at_mb'].iloc[0]  # Get the first timestamp
    else:
        raise ValueError("data_mb_cleaned is empty. Cannot determine reference time.")


    # Filter currDf based on the reference time
    all_brightness_at_time = currDf[
        (currDf['created_at'] >= reference_time - pd.Timedelta(minutes=5)) & 
        (currDf['created_at'] <= reference_time + pd.Timedelta(minutes=5))
    ]['brightness']

    # Calculate mean and standard deviation of brightness
    # mean_brightness = all_brightness_at_time.mean()
    # sd_brightness = all_brightness_at_time.std()

    # Check if either DataFrame is empty before prediction
    if data_th_cleaned.empty or data_mb_cleaned.empty:
        print("One or both datasets are empty. Exiting.")
    else:
        # Combine features for predicting
        new_data = pd.concat([data_th_cleaned, data_mb_cleaned], axis=1)
        new_data = new_data.dropna()
        expected_features = ['hour']
        new_data_filtered = new_data[expected_features]
        # print(new_data_filtered)

        # Make predictions
        y_pred = model.predict(new_data_filtered)
        # print(y_pred[0])
        # print(mean_brightness)
        # print(sd_brightness)
        
        REGRESSION_OUTPUT = ""
        # Determine action based on prediction
        if 200 > y_pred[0] > 50: 
            REGRESSION_OUTPUT="reccomend to turn off lights"
        elif y_pred[0] > 200: 
            REGRESSION_OUTPUT="In 2 hours, the brightness level is expected to be 50 lux. Please turn on the lights."
        elif y_pred[0] < 50:
            REGRESSION_OUTPUT="no reccomendation."

    


# HausAI Default Intro

DEFAULT_INTRO = "Welcome back home, Sir.\nI am HausAI, your home assistant. Here is a report for you:"

current_datetime = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

print (current_datetime)
print (DEFAULT_INTRO)
print(CNN_OUTPUT)
print("\n")
print (current_datetime)
print (DEFAULT_INTRO)
print(REGRESSION_OUTPUT)

   
# scheduler = BackgroundScheduler()

# scheduler.add_job(train_models, 'cron', minute=0)

# scheduler.start()

# print("Scheduler started. Waiting for scheduled jobs and image recognition...")

# try:
#     while True:
#         time.sleep(1)  # Sleep to reduce CPU usage

# except (KeyboardInterrupt, SystemExit):
#     print("Shutting down...")
#     scheduler.shutdown()

