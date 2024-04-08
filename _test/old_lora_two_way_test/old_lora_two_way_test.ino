/*
  Bi-directional LED control (duplex communications)
  Requires LoRa Library by Sandeep Mistry - https://github.com/sandeepmistry/arduino-LoRa
  sendMessage & onReceive functions based upon "LoRaDuplexCallback" code sample by Tom Igoe
*/

// Include required libraries
#include <WiFi.h>
#include <Arduino.h>
#include <sstream>
#include <iomanip>
#include <SPI.h>
#include <LoRa.h>

uint64_t macAddressToInt(const String& macAddress);

// Change sync word (0xF3) to match the sender
// The sync word assures you don't get LoRa messages from other LoRa transceivers
// Ranges from 0-0xFF
const uint8_t SyncWord = 0x01;
// Predefined encryption key
const uint8_t encryptionKey = 0x24;

// Define the pins used by the LoRa module
const int csPin = 5;     // LoRa radio chip select
const int resetPin = 32;  // LoRa radio reset
const int irqPin = 2;    // Must be a hardware interrupt pin

// LED connection
const int greenIndicatorLED = 13;
const int redIndicatorLED = 12;
const int blueIndicatorLED = 14;

// Outgoing message variable
String outMessage;

// Message counter
byte msgCount = 1;

// Receive message variables
String contents = "";

// Source and destination addresses
uint64_t localAddress = macAddressToInt(WiFi.macAddress());
uint64_t destination = 0x94e6863c7ce8;                // destination to send to - 0xffffffffffffffff for broadcast mode

long lastSendTime = 0;                                // last send time
int interval = 5000;                                  // interval between sends 

void setup() {
  // Set LED as output
  pinMode(greenIndicatorLED, OUTPUT);
  pinMode(redIndicatorLED, OUTPUT);
  pinMode(blueIndicatorLED, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("LoRa Duplex with callback");

  // Setup LoRa module
  LoRa.setPins(csPin, resetPin, irqPin);

  // Start LoRa module at local frequency
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // // Change sync word (0xF3) to match the sender
  // // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // // Ranges from 0-0xFF
  // LoRa.setSyncWord(SyncWord);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    String message = "HeLoRa World!";   // send a message
    sendMessage(message);
    lastSendTime = millis();
  }
  onReceive(LoRa.parsePacket());            // timestamp the message
}

// Send LoRa Packet with encrypted message
void sendMessage(String outgoing) {
  digitalWrite(greenIndicatorLED, HIGH);
  // Encrypt outgoing message
  String encryptedMessage = encryptString(outgoing);

  LoRa.beginPacket();             // start packet
  for (int i = 0; i < 8; i++) {   // encrypt destination address
    LoRa.write((destination >> (i * 8)) ^ encryptionKey);
  }
  for (int i = 0; i < 8; i++) {   // encrypt local address
    LoRa.write((localAddress >> (i * 8)) ^ encryptionKey);
  }
  LoRa.write(msgCount ^ encryptionKey);           // add encrypted message ID
  LoRa.write(encryptedMessage.length() ^ encryptionKey);  // add encrypted payload length
  LoRa.print(encryptedMessage);          // add encrypted payload
  LoRa.endPacket();               // finish packet and send it
  
  String message = "Message " + String(msgCount) + " sent: " + outgoing;
  Serial.println(message);
  
  msgCount++;                     // increment message ID
  digitalWrite(greenIndicatorLED, LOW);
}

// Receive Callback Function with decryption
void onReceive(int packetSize) {
  if (packetSize == 0) return;  // if there's no packet, return
  digitalWrite(blueIndicatorLED, HIGH);

  // Read packet header bytes and decrypt them:
  uint64_t recipient = 0;
  for (int i = 0; i < 8; i++) {
    recipient |= ((uint64_t)(LoRa.read() ^ encryptionKey) << (i * 8)); // reassemble recipient address
  }
  uint64_t sender = 0;
  for (int i = 0; i < 8; i++) {
    sender |= ((uint64_t)(LoRa.read() ^ encryptionKey) << (i * 8)); // reassemble sender address
  }
  byte incomingMsgId = LoRa.read() ^ encryptionKey;   // decrypt incoming msg ID
  byte incomingLength = LoRa.read() ^ encryptionKey;  // decrypt incoming msg length

  String incoming = "";  // decrypted payload of packet
  while (LoRa.available()) {        // can't use readString() in callback, so
    incoming += (char)(LoRa.read() ^ encryptionKey);  // decrypt bytes one by one
  }

  if (incomingLength != incoming.length()) {  // check length for error
      Serial.println("error: message length does not match length - " + String(incomingLength) + " != " + String(incoming.length()));
      digitalWrite(blueIndicatorLED, LOW);
      return;  // skip rest of function
  }


  // If the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFFFFFFFFFFFFFFFF) {
    Serial.print("Sent to: 0x" + String(recipient, HEX));
    Serial.println(" - This message is not for me.");
    digitalWrite(blueIndicatorLED, LOW);
    return;  // skip rest of function
  }

  // If message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();

  digitalWrite(blueIndicatorLED, LOW);
}

uint64_t macAddressToInt(const String& macAddress) {
    uint64_t result = 0;
    std::istringstream iss(macAddress.c_str());
    iss >> std::hex;

    for (int i = 0; i < 6; ++i) {
        int byte;
        iss >> byte;
        result = (result << 8) | byte;
        if (i < 5) iss.ignore(1, ':'); // Assuming MAC address is formatted as xx:xx:xx:xx:xx:xx
    }

    return result;
}

// Function to encrypt a string
String encryptString(String message) {
  String encryptedMessage = "";
  for (int i = 0; i < message.length(); i++) {
    encryptedMessage += char(message.charAt(i) ^ encryptionKey);
  }
  return encryptedMessage;
}

// Function to decrypt a string
String decryptString(String encryptedMessage) {
  String decryptedMessage = "";
  for (int i = 0; i < encryptedMessage.length(); i++) {
    decryptedMessage += char(encryptedMessage.charAt(i) ^ encryptionKey);
  }
  return decryptedMessage;
}