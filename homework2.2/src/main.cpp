#include <Arduino.h>

#define CTRL_PIN 4
#define READ_PIN 5

#define TOGGLE_PERIOD 5000000
#define DEBOUNCE_PERIOD 20000 

uint32_t lastToggleUs;
uint32_t lastToHighUs;
uint32_t lastToLowUs;
uint8_t lastState;
volatile uint32_t switchTimeUs = 0;      
volatile bool     switched     = false;
int switchesToHighCount = 0;
int switchesToLowCount = 0;
uint32_t aggregatedDelaysToHigh = 0;
uint32_t aggregatedDelaysToLow = 0;

void IRAM_ATTR onRelayEdge() {
    uint32_t now = micros();
    if (now - switchTimeUs > DEBOUNCE_PERIOD) {
        switchTimeUs = now;
        switched = true;
    }    
}

void setup() {
  Serial.begin(115200);
  pinMode(CTRL_PIN, OUTPUT);
  pinMode(READ_PIN, INPUT_PULLUP);
  lastToggleUs = micros();
  lastState = HIGH;
  lastToHighUs = micros();
  digitalWrite(CTRL_PIN, lastState);  

  attachInterrupt(digitalPinToInterrupt(READ_PIN), onRelayEdge, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t now = micros();

  if (switched) {
    if (lastState == HIGH) {
      switchesToHighCount++;
      uint32_t delay = switchTimeUs - lastToHighUs;
      aggregatedDelaysToHigh += delay;  
      Serial.printf("Interrupt detect time %d, toggle relay time %d relay state %d\n", switchTimeUs, lastToHighUs, lastState);
      Serial.printf("Toggles to %d count  %d, delay %d, averageDelay to HIGH %d \n", lastState,  switchesToHighCount, delay, aggregatedDelaysToHigh / switchesToHighCount);
    } else {
      switchesToLowCount++;
      uint32_t delay = switchTimeUs - lastToLowUs;
      aggregatedDelaysToLow += delay;  
      Serial.printf("Interrupt detect time %d, toggle relay time %d relay state %d\n", switchTimeUs, lastToLowUs, lastState);
      Serial.printf("Toggles to %d count  %d, delay %d, averageDelay to LOW %d \n", lastState,  switchesToLowCount, delay, aggregatedDelaysToLow / switchesToLowCount);
    }
    switched = false;
  }

  if (now - lastToggleUs > TOGGLE_PERIOD) {
    lastState = lastState == HIGH ? LOW : HIGH;
    digitalWrite(CTRL_PIN, lastState);
    lastToggleUs = now;
    if (lastState == HIGH) {
      lastToHighUs = now;
    } else {
      lastToLowUs = now;
    }
  }
}