import time
from pyrf24 import RF24, RF24_PA_MIN

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

def send_message():
    """Send a message repeatedly."""
    while True:
        message = b"Hello Woddd"
        success = radio.write(message)
        if success:
            print("Message sent successfully: Hello World")
        else:
            print("Message failed to send")
        time.sleep(1)

if __name__ == "__main__":
    try:
        send_message()
    except KeyboardInterrupt:
        print("Transmission stopped.")
        radio.power = False
