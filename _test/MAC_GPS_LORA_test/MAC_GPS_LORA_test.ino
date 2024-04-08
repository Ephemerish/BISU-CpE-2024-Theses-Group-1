#include <cmath>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <TimeLib.h>
#include <Arduino.h>
#include <sstream>
#include <iomanip>
#include <SoftwareSerial.h>

// LoRa pin definitions
#define ss 5
#define rst 14
#define dio0 2
#define indicatorLED 13 // Indicator LED pin

// GPS serial connection
#define GPS_SERIAL Serial2

uint64_t macAddressToInt(const String& macAddress);

// Coordinated Universal Time
const int UTC = 8;
// macAddress
const String macAddress = WiFi.macAddress();
const String DeviceID = MacAddressToHexString(macAddress);

// The TinyGPSPlus object
TinyGPSPlus gps;

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

  // GPS serial connection setup
  GPS_SERIAL.begin(9600);
  delay(3000);
}

void loop() {
  while (GPS_SERIAL.available() > 0) {
    // if (gps.encode(GPS_SERIAL.read())) {
    //   sendData();
    // } 
    char c = GPS_SERIAL.read();
    Serial.print(c);
  }
}

void sendData() {
  static int counter = 0;
  digitalWrite(indicatorLED, HIGH);

  // Construct the GPS message
  String message = "P: ";
  message += String(counter);

  message += ", ST: ";
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

  message += ", S: ";
  message += DeviceID;

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