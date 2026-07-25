#include <Arduino.h>

#define READ_PIN 9
#define BUTTON_PIN 4
#define LED_PIN 16

#define MODE_AUTO 0
#define MODE_ON 1
#define MODE_OFF 2

#define TURN_ON_TRIGGER 3000
#define TURN_OFF_TRIGGER 3200

bool lastButtonState = LOW;
bool isAutoTurnedOn = false;
int currentMode = MODE_AUTO;

void blink_led(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);    
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void setup() {
  pinMode(READ_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int light_value = analogRead(READ_PIN);
  float volt = ( light_value * 3.3 ) / 4096.0;
    
  bool currentButtonState = digitalRead(BUTTON_PIN);
  bool wasModeChanges = false;
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    currentMode = (currentMode + 1) % 3;            
    wasModeChanges = true;
    Serial.print("Режим змінено на: ");
    Serial.println(currentMode);
  }
  lastButtonState = currentButtonState;

  if (wasModeChanges) {
    blink_led(currentMode + 1);
  }

  delay(10);
  if (currentMode == MODE_ON) {
    digitalWrite(LED_PIN, HIGH);
    isAutoTurnedOn = false;
  }

  if (currentMode == MODE_OFF) {
    digitalWrite(LED_PIN, LOW);
    isAutoTurnedOn = false;
  }  

  if (currentMode == MODE_AUTO)
  {
    if (!isAutoTurnedOn && light_value < TURN_ON_TRIGGER) {
      isAutoTurnedOn = true;
    }

    if (isAutoTurnedOn && light_value > TURN_OFF_TRIGGER) {
      isAutoTurnedOn = false;
    }
    digitalWrite(LED_PIN, isAutoTurnedOn ? HIGH : LOW);
  }  
}
