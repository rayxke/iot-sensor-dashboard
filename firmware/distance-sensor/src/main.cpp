#include <Arduino.h>
#define TRIG_PIN 16 // GPIO pin for the trigger
#define ECHO_PIN 17 // GPIO pin for the echo

float duration, distance;

void setup() {  
	pinMode(TRIG_PIN, OUTPUT);  
	pinMode(ECHO_PIN, INPUT);  
	Serial.begin(115200);  
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
    delay(100);
}