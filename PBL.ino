#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// WiFi credentials
const char* ssid = "vivo V23 5G";
const char* password = "123456789";
// Google Script Web App URL
const char* scriptURL = "https://script.google.com/macros/s/AKfycbzKjHmd-THxJjC9HA0CktsqADLB7iep1Hhvjvu50J2snKxlh2Viw8YOPJF-hj6v5fsa9A/exec";
// Grove Ultrasonic SIG pin
#define SIG_PIN 13
// Timing
const unsigned long sendInterval = 10000; // 10 seconds
unsigned long previousMillis = 0;
void setup() {
Serial.begin(115200);
delay(1000);
Serial.println("ESP32 Grove Ultrasonic Data Logger");
// Connect to WiFi
WiFi.begin(ssid, password);
Serial.print("Connecting to WiFi");
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println();
Serial.print("Connected to WiFi with IP: ");
Serial.println(WiFi.localIP());
}
void loop() {
unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= sendInterval) {
previousMillis = currentMillis;
// Measure distance
float distance_cm = measureDistance();
Serial.print("Distance: ");
Serial.print(distance_cm);
Serial.println(" cm");
// Send to Google Sheets
sendDataToGoogleSheets(distance_cm);
}
}
float measureDistance() {
long duration;
float distance;
// Trigger pulse
pinMode(SIG_PIN, OUTPUT);
digitalWrite(SIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(SIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(SIG_PIN, LOW);
// Read echo
pinMode(SIG_PIN, INPUT);
duration = pulseIn(SIG_PIN, HIGH);
// Convert to cm
distance = duration * 0.034 / 2;
return distance;
}
void sendDataToGoogleSheets(float distance) {
if (WiFi.status() != WL_CONNECTED) {
Serial.println("WiFi not connected");
return;
}
setup();
loop();
sendDataToGoogleSheets()
HTTPClient http;
http.begin();
http.addHeader("Content-Type", "application/json");
StaticJsonDocument<200> doc;
doc["distance"] = distance;
String jsonString;
serializeJson(doc, jsonString);
int httpResponseCode = http.POST(jsonString);
if (httpResponseCode > 0) {
String response = http.getString();
Serial.println("HTTP Response code: " + String(httpResponseCode));
Serial.println("Response: " + response);
} else {
Serial.print("Error sending POST: ");
Serial.println(httpResponseCode);
}
http.end();
}