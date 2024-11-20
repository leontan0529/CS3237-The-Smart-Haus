import pandas as pd
from sklearn.cluster import KMeans

def perform_clustering(data):
    # Combine features for clustering
    X = data[['humidity', 'temperature', 'hour', 'motion']]

    # Check if X contains any NaN values
    if X.isnull().values.any():
        print("Error: Input features contain NaN values.")
        return None  # Return None or handle as needed

    # Apply K-Means clustering
    kmeans = KMeans(n_clusters=3)
    data['cluster'] = kmeans.fit_predict(X)

    return data
