import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestRegressor
from sklearn.metrics import mean_squared_error, r2_score
import pickle
import sys


def train_random_forest(X, y, model_save_path):
    # Split the dataset into training and testing sets.
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    # Normalize features.
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)

    # Train Random Forest Model.
    rf_model = RandomForestRegressor(n_estimators=100)
    rf_model.fit(X_train_scaled, y_train)

    # Predictions and evaluation for Linear Regression Model.
    y_pred_lr = rf_model.predict(X_test_scaled)

    # Print evaluation metrics.
    # print(f'RF Mean Squared Error: {mean_squared_error(y_test, y_pred_lr)}')
    # print(f'RF R^2 Score: {r2_score(y_test, y_pred_lr)}')

    mse_value = mean_squared_error(y_test, y_pred_lr)  # Calculate MSE
    r2_value = r2_score(y_test, y_pred_lr)  # Calculate R^2 Score
    # model_save_path = sys.argv[1]

    # Save the trained model to a file.
    with open(model_save_path, 'wb') as file:
        pickle.dump(rf_model, file)

    log_file_path = model_save_path + '.log'
    with open(log_file_path, 'w') as log_file:
        log_file.write(f'Mean Squared Error: {mse_value}\n')
        log_file.write(f'R^2 Score: {r2_value}\n')

    print(f"Model saved to {model_save_path}")

