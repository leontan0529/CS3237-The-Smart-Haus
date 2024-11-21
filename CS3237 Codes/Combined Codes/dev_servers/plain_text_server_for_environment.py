from flask import Flask, request, jsonify

app = Flask(__name__)

# Route to handle JSON POST requests
@app.route('/data', methods=['POST'])
def receive_data():
    # Check if the request contains JSON
    if request.is_json:
        # Parse the JSON data
        data = request.get_json()
        
        # Process the data as needed
        # Here, we'll just echo it back as a response for demonstration
        response = {
            "message": "Data received successfully",
            "data": data
        }
        return jsonify(response), 200
    else:
        # If the request does not contain JSON, return an error
        return jsonify({"error": "Request must be in JSON format"}), 400

if __name__ == '__main__':
    # Run the server on http://localhost:5000
    app.run(host='0.0.0.0', port=8080)
