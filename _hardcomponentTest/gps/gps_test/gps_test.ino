#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "PANIS_GPS_TEST";
const char* password = "qwerty123";

WebServer server(80);

static const int RXPin = 19, TXPin = 18;
static const uint32_t GPSBaud = 4800;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Create access point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point created");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Start the server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // Start the GPS
  ss.begin(GPSBaud);
  Serial.println("GPS started");
}

void loop() {
  server.handleClient();

  // Read data from the GPS
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  // Handle GPS data
  if (gps.location.isValid()) {
    // Update webpage with GPS data
    updateWebPage();
  }

}

void handleRoot() {
  String content = "<html><head><meta http-equiv='refresh' content='1'></head><body>";
  content += "<h1>GPS Data</h1>";
  content += "<p>Latitude: ";
  content += gps.location.lat(), 6;
  content += "</p>";
  content += "<p>Longitude: ";
  content += gps.location.lng(), 6;
  content += "</p>";
  // Add more data as needed

  // Check for errors
  if (gps.charsProcessed() < 10) {
    content += "<p style='color:red;'>WARNING: No GPS data. Check wiring.</p>";
  }

  content += "</body></html>";
  server.send(200, "text/html", content);
}

void updateWebPage() {
  String content = "<html><head><meta http-equiv='refresh' content='1'></head><body>";
  content += "<h1>GPS Data</h1>";
  content += "<p>Latitude: ";
  content += gps.location.lat(), 6;
  content += "</p>";
  content += "<p>Longitude: ";
  content += gps.location.lng(), 6;
  content += "</p>";
  // Add more data as needed

  // Check for errors
  if (gps.charsProcessed() < 10) {
    content += "<p style='color:red;'>WARNING: No GPS data. Check wiring.</p>";
  }

  content += "</body></html>";
  server.send(200, "text/html", content);
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
