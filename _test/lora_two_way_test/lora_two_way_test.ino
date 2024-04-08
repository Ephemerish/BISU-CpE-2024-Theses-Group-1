/*

*/

// Include required libraries
#include <WiFi.h>
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <cmath>
#include <TinyGPSPlus.h>
#include <TimeLib.h>
#include <Arduino.h>
#include <sstream>
#include <iomanip>

// Definitions
#define GPS_SERIAL Serial2 // GPS serial connection

// Function declarations
uint64_t macAddressToInt(const String& macAddress);
String encryptString(String message);
String decryptString(String encryptedMessage);

// void sendMessageTask(void *pvParameters) {
//     for (;;) {
//         // Send messages
//         sendMessage(constructMessage());
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust the delay as needed
//     }
// }

// void onReceiveTask(void *pvParameters) {
//     for (;;) {
//         // Receive messages
//         onReceive(LoRa.parsePacket());
//         vTaskDelay(pdMS_TO_TICKS(50)); // Adjust the delay as needed
//     }
// }

// Constants
const uint8_t SyncWord = 0x01; // Sync word to match the sender (0xF3)
const uint8_t encryptionKey = 0x24; // Predefined encryption key
const int UTC = +8; // Coordinated Universal Time

// Pin definitions
const int csPin = 5; // LoRa radio chip select
const int resetPin = 32; // LoRa radio reset
const int irqPin = 2; // Hardware interrupt pin

// LED pins
const int greenIndicatorLED = 13;
const int redIndicatorLED = 12;
const int blueIndicatorLED = 14;

// Objects
TinyGPSPlus gps; // The TinyGPSPlus object

// Variables
String outMessage; // Outgoing message variable
uint32_t msgCount = 1; // Message counter
String contents; // Receive message variables
uint64_t localAddress = macAddressToInt(WiFi.macAddress()); // Local address
// my reciver 0x94e6863c7ce8
uint64_t destination = 0xffffffffffffffff; // destination to send to - 0xffffffffffffffff for broadcast mode
long lastSendTime = 0; // Last send time
int interval = 5000; // Interval between sends

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial);

  // Initialize hardware and setup tasks
  // xTaskCreate(sendMessageTask, "SendMessageTask", 4096, NULL, 1, NULL);
  // xTaskCreate(onReceiveTask, "OnReceiveTask", 4096, NULL, 1, NULL);

  // Initialize LEDs
  pinMode(greenIndicatorLED, OUTPUT);
  pinMode(redIndicatorLED, OUTPUT);
  pinMode(blueIndicatorLED, OUTPUT);

  // Initialize LoRa module
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(433E6)) { // Start LoRa at 433 MHz for Asia
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");

  // GPS serial connection setup
  GPS_SERIAL.begin(9600);
  Serial.println("GPS Initializing OK!");
  
  sendMessage(String(localAddress, HEX) + " just started!");

}

void loop() {
  // Send messages at the defined interval
  while (GPS_SERIAL.available() > 0) {
    if (gps.encode(GPS_SERIAL.read())) {
      if(gps.location.isUpdated() || gps.satellites.isUpdated()) {
        //if(gps.location.isValid() && gps.satellites.isValid())
        {
          digitalWrite(greenIndicatorLED, HIGH);
          uint32_t currentTime = millis();
          sendMessage(constructMessage());
          uint32_t elapsedTime = millis() - currentTime;
          Serial.print(" Elapsed time: ");
          Serial.println(elapsedTime);
          digitalWrite(greenIndicatorLED, LOW);
        }
      }
    }
  }
  onReceive(LoRa.parsePacket());
}


String constructMessage() {
  // Construct the GPS message
  String message = "ST: ";
  if (gps.satellites.isValid()) {
    message += String(gps.satellites.value());
  } else {
    message += "INV";
  }

  message += ", L: ";
  if (gps.location.isValid()) {
    message += "Lat: ";
    message += String(gps.location.lat(), 9); // Format latitude with 10 decimal places
    message += ", Lng: ";
    message += String(gps.location.lng(), 9); // Format longitude with 10 decimal places
  } else {
    message += "INV";
  }
  
  message += ", D/T: ";
  if (gps.date.isValid() && gps.time.isValid()) {
    tmElements_t tm;
    tm.Year = gps.date.year() - 1970;
    tm.Month = gps.date.month();
    tm.Day = gps.date.day();
    tm.Hour = gps.time.hour();
    tm.Minute = gps.time.minute();
    tm.Second = gps.time.second();

    time_t gpsTime = makeTime(tm);
    gpsTime += UTC * 3600; // Adjust for +8 timezone

    message += String(month(gpsTime));
    message += "/";
    message += String(day(gpsTime));
    message += "/";
    message += String(year(gpsTime));
    message += " ";
    message += String(hour(gpsTime));
    message += ":";
    message += String(minute(gpsTime));
    message += ":";
    message += String(second(gpsTime));
  } else {
    message += "INV";
  }

  return message;
}

void sendMessage(String outgoing) {
  String encryptedMessage = encryptString(outgoing);

  // Start packet and write encrypted data
  LoRa.beginPacket();
  for (int i = 0; i < 8; i++) {
    LoRa.write((destination >> (i * 8)) ^ encryptionKey);
  }
  for (int i = 0; i < 8; i++) {
    LoRa.write((localAddress >> (i * 8)) ^ encryptionKey);
  }
  for (int i = 0; i < 4; i++) {
    LoRa.write((msgCount >> (i * 8)) ^ encryptionKey);
  }

  LoRa.write(encryptedMessage.length() ^ encryptionKey);
  LoRa.print(encryptedMessage);
  LoRa.endPacket();

  Serial.print("Message " + String(msgCount) + " sent: " + outgoing);
  msgCount++;
  digitalWrite(greenIndicatorLED, LOW);
  
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;
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

  uint32_t incomingMsgId = 0;
  for (int i = 0; i < 4; i++) {
    incomingMsgId |= ((uint32_t)(LoRa.read() ^ encryptionKey) << (i * 8)); // reassemble sender address
  }

  byte incomingLength = LoRa.read() ^ encryptionKey;

  // Decrypt payload
  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)(LoRa.read() ^ encryptionKey);
  }

  // Check for errors in message length
  if (incomingLength != incoming.length()) {
    Serial.println("error: message length does not match length - " + String(incomingLength) + " != " + String(incoming.length()));
    digitalWrite(blueIndicatorLED, LOW);
    return;
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

// Helper functions
uint64_t macAddressToInt(const String& macAddress) {
  uint64_t result = 0;
  std::istringstream iss(macAddress.c_str());
  iss >> std::hex;
  for (int i = 0; i < 6; ++i) {
    int byte;
    iss >> byte;
    result = (result << 8) | byte;
    if (i < 5) iss.ignore(1, ':');
  }
  return result;
}

String encryptString(String message) {
  String encryptedMessage = "";
  for (int i = 0; i < message.length(); i++) {
    encryptedMessage += char(message.charAt(i) ^ encryptionKey);
  }
  return encryptedMessage;
}

String decryptString(String encryptedMessage) {
  String decryptedMessage = "";
  for (int i = 0; i < encryptedMessage.length(); i++) {
    decryptedMessage += char(encryptedMessage.charAt(i) ^ encryptionKey);
  }
  return decryptedMessage;
}
