import io
import logging
import socketserver
import serial
import serial.tools.list_ports
from http import server
from threading import Condition
from urllib.parse import urlparse, parse_qs
from picamera2 import Picamera2
from picamera2.encoders import JpegEncoder
from picamera2.outputs import FileOutput

class ArduinoConnection:
    def __init__(self, baud_rate=9600, timeout=1):
        self.port = self.find_arduino()
        self.connection = None
        if self.port:
            self.connection = serial.Serial(self.port, baud_rate, timeout=timeout)
            print(f"Connected to Arduino on {self.port}")
        else:
            print("No Arduino found.")

    @staticmethod
    def find_arduino():
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if any(x in port.description for x in ["Arduino", "CH340", "ttyUSB", "ttyACM", "USB"]):
                return port.device
        return None

    def send(self, message):
        if self.connection:
            self.connection.write(f"{message}\n".encode("utf-8"))

    def close(self):
        if self.connection:
            self.connection.close()

class StreamingOutput(io.BufferedIOBase):
    def __init__(self):
        self.frame = None
        self.condition = Condition()

    def write(self, buf):
        with self.condition:
            self.frame = buf
            self.condition.notify_all()

class StreamingHandler(server.BaseHTTPRequestHandler):
    saved_value = None

    def do_GET(self):        
        parsed_path = urlparse(self.path)
        query_params = parse_qs(parsed_path.query)

        if parsed_path.path == "/stream":
            self.handle_stream()
        elif "dir" in query_params:
            self.handle_dir(query_params["dir"][0])
        elif "arm" in query_params:
            self.handle_arm(query_params["arm"][0])
        else:
            self.send_error(404)
            self.end_headers()

    def handle_stream(self):
        self.send_response(200)
        self.send_header("Age", 0)
        self.send_header("Cache-Control", "no-cache, private")
        self.send_header("Pragma", "no-cache")
        self.send_header("Content-Type", "multipart/x-mixed-replace; boundary=FRAME")
        self.end_headers()
        try:
            while True:
                with output.condition:
                    output.condition.wait()
                    frame = output.frame
                self.wfile.write(b"--FRAME\r\n")
                self.send_header("Content-Type", "image/jpeg")
                self.send_header("Content-Length", str(len(frame)))
                self.end_headers()
                self.wfile.write(frame)
                self.wfile.write(b"\r\n")
        except Exception as e:
            logging.warning("Streaming client disconnected: %s", str(e))

    def handle_dir(self, value):
        if value in ["1", "2", "3", "4"]:
            StreamingHandler.saved_value = value
            print(f"Saved value: {StreamingHandler.saved_value}")
            arduino.send(f"dir:{value}")
            self.send_response(200)
            self.send_header("Content-Type", "text/plain")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
            self.send_header("Access-Control-Allow-Headers", "Content-Type")
            self.end_headers()
            self.wfile.write(f"Saved value: {StreamingHandler.saved_value}".encode("utf-8"))
        else:
            self.send_error(400, "Invalid value for dir")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
            self.send_header("Access-Control-Allow-Headers", "Content-Type")
            self.end_headers()

    def handle_arm(self, value):  
        if value in ["1", "2", "3"]: #right, left, toggle
            print(f"Arm movement: {value}")
            arduino.send(f"arm:{value}")
        else:
            self.send_error(400, "Invalid value for arm")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
            self.send_header("Access-Control-Allow-Headers", "Content-Type")
            self.end_headers()
            return

        self.send_response(200)
        self.send_header("Content-Type", "text/plain")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
        self.wfile.write(f"Arm movement: {value}".encode("utf-8"))

class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
    allow_reuse_address = True
    daemon_threads = True

arduino = ArduinoConnection()

picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (480, 360)}))
output = StreamingOutput()
picam2.start_recording(JpegEncoder(), FileOutput(output))

try:
    address = ("0.0.0.0", 7123)
    server = StreamingServer(address, StreamingHandler)
    print("Server running on http://0.0.0.0:7123")
    server.serve_forever()
finally:
    picam2.stop_recording()
    arduino.close()
