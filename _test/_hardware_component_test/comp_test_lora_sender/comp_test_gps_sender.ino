#include <SPI.h>
#include <LoRa.h>
#include <TimeLib.h>
#include <cmath>

// LoRa pin definitions
#define ss 5
#define rst 14
#define dio0 2
#define indicatorLED 13 // Indicator LED pin

// Coordinated Universal Time
const int UTC = 8;

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  // LoRa initialization
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  // Indicator LED pin setup
  pinMode(indicatorLED, OUTPUT);

  delay(3000);
}

void loop() {
  sendData();
}

void sendData() {
  static int counter = 0;
  digitalWrite(indicatorLED, HIGH);

  // Construct the GPS message
  String message = "Packet Counter: ";
  message += String(counter);

  Serial.print(message);
  Serial.println("");

  // Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  digitalWrite(indicatorLED, LOW);

  counter++;

  delay(500);
}
