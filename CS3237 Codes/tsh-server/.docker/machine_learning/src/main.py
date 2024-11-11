from datetime import datetime
import os

# Get the current datetime

# Path to update HausAI's CNN
CNN_OUTPUT_PATH = os.chdir("./CS3237\ Codes/tsh-server/.docker/machine_learning/ml_image/ml_image.txt")
#CNN_OUTPUT_PATH = "../ml_image/ml_image.txt"

ML_OUTPUT = "hello"

def update_ml(ML_OUTPUT_PATH, ML_OUTPUT):
    current_datetime = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    with open(ML_OUTPUT_PATH, "w") as file:
        file.write(current_datetime)
        file.write("\n")
        file.write(ML_OUTPUT)

update_ml(CNN_OUTPUT_PATH, ML_OUTPUT)
