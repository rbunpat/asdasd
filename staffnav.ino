#include "TinyGPS.h"
#include <HardwareSerial.h>
#include "WiFiClient.h"
#include <ArduinoJson.h>

const int ultraSonicTrigger = 5;
const int ultraSonicEcho = 18;
const int ultraSonicMaxDistance = 200;

const float soundSpeed = 0.034;

const int vibrationMotorPin = 22;

const int gpsRX = 22;
const int gpsTX = 21;

bool debugOption = true;

long duration;
float distance;

const char* wifiSSID = "Wokwi-GUEST";
const char* wifiPassword = "";
const char* navServer = "159.65.6.49";
const char* navServerWithPath = "159.65.6.49:3000/api/updatePosition";
const char* apiKey = "e1c0bd3793f0fc6c965570ae84946b6bf1284df9e7a56b7d92d9bc73b8e2df0c";

static const uint32_t gpsBaudRate = 9600;

TinyGPSPlus gps;
HardwareSerial ss(2);

WiFiClient client;



void setup() {
  Serial.begin(115200);

  pinMode(ultraSonicTrigger, OUTPUT);
  pinMode(ultraSonicEcho, INPUT);

  if (debugOption) {
    Serial.println("Ultrasonic Sensor Initialized");
  }

  pinMode(vibrationMotorPin, OUTPUT);

  if (debugOption) {
    Serial.println("Vibration Motor Initialized");
  }

  ss.begin(gpsBaudRate, SERIAL_8N1, gpsRX, gpsTX, false);

  if (debugOption) {
    Serial.println("GPS Initialized");
  }

  WiFi.begin(wifiSSID, wifiPassword);

  if (debugOption) {
    Serial.println("Wifi Connecting");
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  if (debugOption) {
    Serial.println("Wifi Connected");
  }


  client.setInsecure();

  sendLocation(123.2, 44.5);



}

void loop() {
  float currentDistance = getDistance();

  if (currentDistance <= 40) {
    triggerVibrationHigh();
  } else if (currentDistance <= 55) {
    triggerVibrationMedium();
  } else if (currentDistance <= 70) {
    triggerVibrationLow();
  }
}

void triggerVibrationLow() {
  digitalWrite(vibrationMotorPin, 1);
  delay(1000);
  digitalWrite(vibrationMotorPin, 0);
}

void triggerVibrationMedium() {
  digitalWrite(vibrationMotorPin, 1);
  delay(500);
  digitalWrite(vibrationMotorPin, 0);
}

void triggerVibrationHigh() {
  digitalWrite(vibrationMotorPin, 1);
  delay(200);
  digitalWrite(vibrationMotorPin, 0);
}

float getDistance() {
  digitalWrite(ultraSonicTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(ultraSonicTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraSonicTrigger, LOW);

  duration = pulseIn(ultraSonicEcho, HIGH);

  distance = duration * soundSpeed / 2;

  return distance;
}

float getCurrentLocation() {
  float lat, lng;

  lat = lng = 0;

  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lng = gps.location.lng();
  }

  return lat, lng;
}



void sendLocation(float lat, float lng) {
  StaticJsonDocument<200> doc;

  // Create a JSON document
  doc["lat"] = lat;
  doc["lon"] = lng;
  doc["apiKey"] = apiKey;

  // Serialize the JSON document to a string
  String jsonStr;
  serializeJson(doc, jsonStr);

  // Make the POST request
  String url = String(navServerWithPath);
  // if (url.startsWith("/")) {
  //   url = url.substring(1);
  // }

  if (client.connect(navServer, 3000)) {
    client.print("POST /" + url + " HTTP/1.1\r\n" +
                 "Host: " + navServer + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + jsonStr.length() + "\r\n\r\n" +
                 jsonStr);

    if (debugOption) {
      Serial.println("Sent POST request with location data");
    }
  } else {
    if (debugOption) {
      Serial.println("Failed to connect to server");
    }
  }

  // Wait for the response (you may need to adjust the delay depending on your server's response time)
  delay(1000);

  // Read and print the server's response
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect
  client.stop();
}

