from flask import Flask, request, jsonify

app = Flask(__name__)

# Route to accept the POST message from the ESP32
@app.route('/door_open', methods=['POST'])
def handle_door_open():
    # Read the plain text data sent in the POST request
    door_message = request.data.decode('utf-8')
    
    # Print the received message for debugging/logging purposes
    print(f"Received door message: {door_message}")
    
    # Validate the message content
    if door_message == "Door opened":
        response = {"status": "Success", "message": "Door open notification received"}
        return jsonify(response), 200
    else:
        response = {"status": "Error", "message": "Invalid message content"}
        return jsonify(response), 400

if __name__ == '__main__':
    # Run the app on the local server, listening on all IP addresses and on port 8080
    app.run(host='0.0.0.0', port=8080)