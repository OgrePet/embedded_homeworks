#include <Arduino.h>

#define BOOT_BUTTON_PIN 0
#define BUTTON_PIN 4
#define LED1_PIN 16
#define LED2_PIN 3

#define MODE_SLOW 0
#define MODE_FAST 1

bool lastButtonState = LOW;
bool lastBootButtonState = LOW;
int currentMode = MODE_SLOW;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);  
}

const int slowLightPeriod = 500;
const int fastLightPeriod = 200;

void lightLEDs(int mode) {
  int millisFromStart = millis();
  int lightPeriod = slowLightPeriod;
  if (mode == MODE_FAST) {
    lightPeriod = fastLightPeriod;
  }
  bool turnOn = ((millisFromStart / lightPeriod) % 2) == 0;
  digitalWrite(LED1_PIN, turnOn ? HIGH : LOW);
  digitalWrite(LED2_PIN, turnOn ? HIGH : LOW);
}

void loop() {  
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    currentMode = MODE_SLOW;
  }
  lastButtonState = currentButtonState;

  bool currentBootButtonState = digitalRead(BOOT_BUTTON_PIN);

  if (currentBootButtonState == LOW && lastBootButtonState == HIGH) {
    currentMode = MODE_FAST;
  }
  lastBootButtonState = currentBootButtonState;

  delay(10);

  lightLEDs(currentMode);
}

