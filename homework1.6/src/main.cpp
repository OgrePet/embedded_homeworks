#include <Arduino.h>

#define READ_PIN 9

void setup() {
  pinMode(READ_PIN, INPUT);
  analogReadResolution(12);
  analogSetAttenuation(ADC_ATTENDB_MAX);
  Serial.begin(115200);
}

void loop() {
  int val = analogRead(READ_PIN);
  float volt = ( val * 3.3 ) / 4096.0;
  int readMillivolts = analogReadMilliVolts(READ_PIN);

  int millivoltDiff = abs(readMillivolts - (volt * 1000));

  float error = (float)millivoltDiff / ((float)readMillivolts / 100.0);

  Serial.printf("pin value - Raw: %d, Volt Calculated %.3f, MilliVolt Read: %d, Error %.2f%%\n", val, volt, readMillivolts, error);
  delay(100);
}