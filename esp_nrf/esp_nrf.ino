#include <SPI.h>
#include <RF24.h>

#define CE_PIN   22
#define CSN_PIN  21

// Create an RF24 object
RF24 radio(CE_PIN, CSN_PIN);

// Set the RX address
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  
  // Initialize the radio
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);  // Set to minimal power
  radio.openReadingPipe(0, address);  // Set RX address
  radio.startListening();  // Set the radio to RX mode
}

void loop() {
  if (radio.available()) {
    char message[64] = "";  // Buffer to store incoming data
    radio.read(&message, sizeof(message));  // Read data
    Serial.print("Received message: ");
    Serial.println(message);  // Print received message
  }
}
