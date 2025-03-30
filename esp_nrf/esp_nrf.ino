#include <SPI.h>
#include <RF24.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Base64.h>  // Include the Base64 library for encoding

#define CE_PIN   22
#define CSN_PIN  21

// Set your Wi-Fi credentials
const char* ssid = "MyNet1";
const char* password = "samsungs20";

// Create an RF24 object
RF24 radio(CE_PIN, CSN_PIN);

// Set the RX address
const byte address[6] = "00001";

// Buffer to store the received payload
char payload[256];  // Increase the size of the payload buffer

// Define the endpoint URL
const char* serverUrl = "http://192.168.84.164:8080/services/ts/nst-one/gen/edm/api/entities/ReadingService.ts";

// Basic Authentication details
const char* username = "admin";
const char* passwordAuth = "admin";

// To hold the count of received messages
int messageCount = 0;
String receivedMessages = "";  // To hold the concatenated messages

// Store the last sent data to compare with new data
String lastSentData = "";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");

  // Initialize the radio
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);  // Set to minimal power
  radio.openReadingPipe(0, address);  // Set RX address
  radio.startListening();  // Set the radio to RX mode
}

void loop() {
  uint8_t pipe;

  // Check if there's a payload available, and get the pipe number
  if (radio.available(&pipe)) {
    uint8_t bytes = radio.getPayloadSize();  // Get the size of the payload
    
    // Read the payload from the FIFO
    radio.read(&payload, bytes);
    
    // Null-terminate the string to avoid printing garbage
    payload[bytes] = '\0';
    
    // Print the received data
    Serial.print(F("Received "));
    Serial.print(bytes);  // Print the size of the payload
    Serial.print(F(" bytes on pipe "));
    Serial.print(pipe);  // Print the pipe number
    Serial.print(F(": "));
    Serial.println(payload);  // Print the payload's value

    // If message is not "---", store it
    if (String(payload) != "---") {
      messageCount++;
      receivedMessages += String(payload) + ";";  // Store the message with a separator
      
      // If we have received 10 messages, check if they are new and send them to the server
      // Only send if the received data is different from the last sent data
      if (receivedMessages != lastSentData) {
        sendDataToServer(receivedMessages);
        lastSentData = receivedMessages;  // Update lastSentData to the new sent data
      }
      messageCount = 0;  // Reset the count after sending
      receivedMessages = "";  // Clear the stored messages
    }
  }
}

void sendDataToServer(const String& data) {
  HTTPClient http;

  // Start the HTTP request
  http.begin(serverUrl);

  // Basic Authentication: Add the Authorization header with base64 encoded username:password
  String authHeader = "Basic " + base64::encode(String(username) + ":" + String(passwordAuth));
  http.addHeader("Authorization", authHeader);  // Add the Authorization header

  // Specify the content-type and other necessary headers
  http.addHeader("Content-Type", "application/json");

  // Parse the received data and format it as JSON
  String jsonPayload = createJsonPayload(data);

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonPayload);
  
  // Check the response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }

  // End the HTTP request
  http.end();
}

String createJsonPayload(const String& data) {
  // Create a JSON object
  String json = "{";

  int startIndex = 0;
  int endIndex = data.indexOf(";");

  while (endIndex != -1) {
    String message = data.substring(startIndex, endIndex);  // Get one message
    
    // Parse the message, which is assumed to be comma-separated (property,value)
    int propertyEnd = message.indexOf(",");
    if (propertyEnd != -1) {
      String property = message.substring(0, propertyEnd);
      String value = message.substring(propertyEnd + 1);
      
      // Add the JSON object with correct key-value pairs for each message
      json += "\"Property\": \"" + property + "\", \"Value\": \"" + value + "\",";
    }
    
    startIndex = endIndex + 1;
    endIndex = data.indexOf(";", startIndex);
  }

  // Remove last comma and close the JSON object
  if (json.length() > 1) {
    json.remove(json.length() - 1);
  }
  json += "}";

  return json;
}
