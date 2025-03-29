from picamera2 import Picamera2
from time import sleep

picam2 = Picamera2()

# Configure the camera (you can also pass in custom resolution, etc.)
picam2.configure(picam2.create_still_configuration())

# Start camera
picam2.start()
sleep(2)  # Give the camera time to adjust

# Capture and save image
picam2.capture_file("test_image.jpg")

print("Image saved!")
