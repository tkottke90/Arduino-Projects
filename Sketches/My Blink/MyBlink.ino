#include <Arduino.h>

const int led = LED_BUILTIN;
bool pinState = false;

void setup() {
    pinMode(led,OUTPUT);
}

void loop() {
    if (pinState) {
        digitalWrite(led, HIGH);
        pinState = !pinState;
    } else {
        digitalWrite(led, LOW);
        pinState = !pinState;
    }

    delay(1000);
}