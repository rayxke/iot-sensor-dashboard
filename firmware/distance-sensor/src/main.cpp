#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"
#define TRIG_PIN 16 // GPIO pin for the trigger
#define ECHO_PIN 17 // GPIO pin for the echo

float duration, distance;


void setup() {  
	pinMode(TRIG_PIN, OUTPUT);  
	pinMode(ECHO_PIN, INPUT);  
	Serial.begin(115200); 
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(MY_SSID, MY_PASSWORD);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
} 

void sendDataToServer(float distance) {
  HTTPClient http;

  // Replace with your server's URL
  http.begin(SERVER_IPADDRESS);

  // Prepare JSON data
  String jsonData = "{\"timestamp\": " + String(millis()) + ", \"distance\": " + String(distance) + "}";

  // Set HTTP header
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request
  int httpCode = http.POST(jsonData);

  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


void loop() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration*.0343)/2;
    Serial.print("Distance: ");
    Serial.println(distance);
    sendDataToServer(distance);
    delay(100);
}