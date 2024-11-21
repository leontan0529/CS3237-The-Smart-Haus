from flask import Flask, request
import os
import sys

app = Flask(__name__)

def req(request):
   print(request.__dict__, file=sys.stderr)

# Set the directory to save uploaded images
UPLOAD_FOLDER = r'C:\Users\Manoj\Downloads'  # Use raw string to handle backslashes
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/upload', methods=['POST'])
def upload_image():
    if 'imageFile' not in request.files:
        return 'No file part', 400
    
    file = request.files['imageFile']
    if file.filename == '':
        return 'No selected file', 400
    
    if file:
        file_path = os.path.join(UPLOAD_FOLDER, file.filename)
        file.save(file_path)
        #req(request)
        return f'File successfully uploaded to {file_path}', 200
        

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # Listen on all interfaces

