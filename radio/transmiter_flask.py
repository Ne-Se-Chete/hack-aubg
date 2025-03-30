import time
from pyrf24 import RF24, RF24_PA_MIN
from flask import Flask, request, jsonify
from flask_cors import CORS  # Import CORS
import threading
from datetime import datetime

# Define CE and CSN pins for Raspberry Pi
CE_PIN = 22
CSN_PIN = 0  # CE0 on SPI bus 0

# Initialize the radio
radio = RF24(CE_PIN, CSN_PIN)

# Set the address
address = b"00001"

# Configure the radio
radio.begin()
radio.set_pa_level(RF24_PA_MIN)
radio.open_tx_pipe(address)
radio.stopListening()  # Set to TX mode

# Initialize Flask app
app = Flask(__name__)

# Enable CORS for all routes
CORS(app)

# Global variable to store the message to send
message_to_send = b"---"

def send_message():
    """Send a message repeatedly."""
    global message_to_send
    while True:
        success = radio.write(message_to_send)
        if success:
            print(f"Message sent successfully: {message_to_send.decode()}")
        else:
            print("Message failed to send")
        time.sleep(1)

@app.route('/data', methods=['POST'])
def set_message():
    """Receive a new message with Id, Property, Value, Timestamp from Flask and set it to be sent."""
    global message_to_send
    data = request.get_json()  # Get the JSON data from the request
    
    # Ensure all the expected keys are present
    if all(key in data for key in ('Property', 'Value')):
        
        message = [
            int(data['Property']),  
            int(data['Value']),
            # data['Timestamp']  # Keep Timestamp as string (or convert it to a format if needed)
        ]

        # Convert the list into a comma-separated string
        message_to_send = ','.join(map(str, message)).encode()
        
        # Respond with the received data (optional confirmation)
        return jsonify({"status": "success", "message": f"Message set to: {message}"}), 200
    else:
        return jsonify({"status": "error", "message": "Missing required fields in the request"}), 400

@app.route('/')
def home():
    return "NRF24 Flask Server is running!"

if __name__ == "__main__":
    # Start the message sending in a separate thread
    threading.Thread(target=send_message, daemon=True).start()
    app.run(host='0.0.0.0', port=6969)
