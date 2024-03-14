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

int counter = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

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
   
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // Ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  // Initialize the indicator LED pin
  pinMode(indicatorLED, OUTPUT);
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // Turn on the indicator LED
  digitalWrite(indicatorLED, HIGH);
  // Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  // Turn off the indicator LED
  digitalWrite(indicatorLED, LOW);

  counter++;

  delay(500);
}