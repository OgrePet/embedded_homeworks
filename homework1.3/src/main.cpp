#include <Arduino.h>

#define RED_PIN 4
#define BLUE_PIN 5

#define BLINK_TIMES 3
#define BLINK_DELAY 300

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void blink_siren(int pin) {
  for (int i = 0; i < BLINK_TIMES ; i++) {
    digitalWrite(pin, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(pin, LOW);
    delay(BLINK_DELAY);
  }
}

void loop() {  
  blink_siren(RED_PIN);
  delay(10);
  blink_siren(BLUE_PIN);
}
