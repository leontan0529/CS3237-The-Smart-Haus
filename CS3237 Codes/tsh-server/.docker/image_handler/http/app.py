from flask import Flask, request
import numpy as np
import cv2
import os
from datetime import datetime

app = Flask(__name__)

GENERIC_FOLDER = '/images'
os.makedirs(GENERIC_FOLDER, exist_ok=True)

def grafana_img(img):
    # Save the file with a generic filename in the created directory
    save_dupe_path = os.path.join(GENERIC_FOLDER)
    os.makedirs(save_dupe_path, exist_ok=True)
    imagename = 'image.jpeg'
    cv2.imwrite(os.path.join(save_dupe_path, imagename), img)
    print("Updated Grafana Image.") # debug

@app.route('/upload', methods=['POST','GET'])
def upload():
    received = request
    img = None
    if received.files:
        print(received.files['imageFile'])
        #convert string of image data to uint8
        file  = received.files['imageFile']
        nparr = np.fromstring(file.read(), np.uint8)

        # decode image
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        grafana_img(img)

        return "[SUCCESS] Image Received", 200
    else:
        return "[FAILED] Image Not Received", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9091)