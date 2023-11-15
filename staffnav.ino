#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WifiClientSecure.h>

#define ultraSonicTrigger = 5;
#define ultraSonicEcho = 18;
#define ultraSonicMaxDistance = 200;

#define soundSpeed = 0.034;

#define vibrationMotorPin = 22;

#define gpsRX = 22;
#define gpsTX = 21;

bool debugOption = false;

long duration;
float distance;

const char* wifiSSID = "bruh";
const char* wifiPassword = "asd";
const char* navServer = "hamburgz.online";
const char* navServerWithPath = "hamburgz.online/api/updatePosition";

static const uint32_t gpsBaudRate = 9600;

TinyGPSPlus gps;
HardwareSerial ss(2);

WifiClientSecure client;



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

  while (WiFi,status() != WL_CONNECTED) {
    delay(1000);
  }

  if (debugOption) {
    Serial.println("Wifi Connected");
  }


  client.setInsecure();



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

void getDistance() {
  digitalWrite(ultraSonicTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(ultraSonicTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraSonicTrigger, LOW);

  duration = pulseIn(ultraSonicEcho, HIGH);

  distance = duration * soundSpeed / 2;

  return distance;
}

void getCurrentLocation() {
  float lat, lng;

  lat = lng = 0;

  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lon = gps.location.lng();
  }

  return [lat, lng]
}

