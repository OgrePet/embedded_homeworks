#include <Arduino.h>

#define BUTTON_PIN 3

int16_t counter_left = 0;
int16_t counter_left_last = 0;

void IRAM_ATTR reaction_left() {
  counter_left++;
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);  
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), reaction_left, FALLING);
}

void loop() {
  int counter = counter_left;
  if (counter > counter_left_last)
  {    
    Serial.printf("Counter %d, triggered %d times from last\n", counter, counter - counter_left_last);
    counter_left_last = counter;    
  }
  
}