#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <Arduino.h>
#include <sstream>
#include <iomanip>

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

String intToMacAddress(uint64_t intValue) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');

    for (int i = 5; i >= 0; --i) {
        oss << std::setw(2) << ((intValue >> (i * 8)) & 0xFF);
        if (i > 0) oss << ":";
    }

    return String(oss.str().c_str());
}

String intToHexString(uint64_t num) {
  return String(num, HEX);
}

uint64_t hexStringToInt(String hexString) {
  // Convert the hex string to a c-string
  const char* hexChars = hexString.c_str();
  // Convert the hex string to a uint64_t
  return strtoull(hexChars, NULL, 16);
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  String macAddress = WiFi.macAddress();

  uint64_t intValue = macAddressToInt(macAddress);
  Serial.print("MAC Address: ");
  Serial.println(macAddress);

  Serial.print("Integer Value: ");
  Serial.println(intValue);

  String hexvalue = intToHexString(intValue);
  Serial.print("Hex Value: ");
  Serial.println(hexvalue);
  
  uint64_t intValueConverted = hexStringToInt(hexvalue);
  Serial.print("Integer Value Converted: ");
  Serial.println(intValueConverted);

  String newMacAddress = intToMacAddress(intValueConverted);
  Serial.print("Converted back to MAC Address: ");
  Serial.println(newMacAddress);
}
 
void loop(){

}