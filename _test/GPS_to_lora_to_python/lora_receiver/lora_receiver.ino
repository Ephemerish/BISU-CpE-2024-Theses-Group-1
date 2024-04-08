/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2
#define indicatorLED 13 // Define the pin for the indicator LED

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  // Setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  // Replace the LoRa.begin(---E-) argument with your location's frequency 
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
   
  // Change sync word (0xF3) to match the sender
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // Ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  // Initialize the indicator LED pin
  pinMode(indicatorLED, OUTPUT);
}

void loop() {
  // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Turn on the indicator LED
    digitalWrite(indicatorLED, HIGH);
    // Received a packet
    Serial.print("Received packet '");

    // Variables to store parsed data
    int P, ST;
    float Lat, Lng;
    String dateTime, S;

    // Read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 

      // Parse the data
      int colonIndex = LoRaData.indexOf(':');
      if (colonIndex != -1) {
        String key = LoRaData.substring(0, colonIndex);
        String value = LoRaData.substring(colonIndex + 2); // Skip colon and space
        
        if (key.equals("P")) {
          P = value.toInt();
        } else if (key.equals("ST")) {
          ST = value.toInt();
        } else if (key.equals("L")) {
          int commaIndex = value.indexOf(',');
          if (commaIndex != -1) {
            Lat = value.substring(5, commaIndex).toFloat();
            Lng = value.substring(commaIndex + 6).toFloat();
          }
        } else if (key.equals("D/T")) {
          dateTime = value;
        } else if (key.equals("S")) {
          S = value;
        }
      }
    }

    // Print parsed data
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.println("Parsed data:");
    Serial.print("P: ");
    Serial.println(P);
    Serial.print("ST: ");
    Serial.println(ST);
    Serial.print("Lat: ");
    Serial.println(Lat, 10);
    Serial.print("Lng: ");
    Serial.println(Lng, 10);
    Serial.print("D/T: ");
    Serial.println(dateTime);
    Serial.print("S: ");
    Serial.println(S);

    // Turn off the indicator LED
    digitalWrite(indicatorLED, LOW);
  }
}