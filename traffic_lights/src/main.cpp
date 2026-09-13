#include <Arduino.h>
#include "soc/gpio_reg.h"

#define BUTTON_PIN     4
#define RED_PIN     35
#define GREEN_PIN     38
#define YELLOW_PIN     36

enum class StoplightState : uint8_t { Allow, AllowEnding, StopWarning, FullStop, FullStopEnding, NonRegualted };


struct Config {
    static constexpr uint8_t kAllowTime = 3;
    static constexpr uint8_t kWarningTime = 2;
    static constexpr uint32_t kStopTime = 3;
    static constexpr uint32_t kDebounceMs = 50;
    static constexpr uint32_t kEndingTime = 2;
    static constexpr uint32_t kBlinkIntervalMs = 350; 
};

volatile StoplightState currentState = StoplightState::Allow;
StoplightState previousState = StoplightState::FullStopEnding;
unsigned long lastInterruptTime = 0;

unsigned long stateEnteredAt = 0;
unsigned long lastBlinkToggle = 0;
bool blinkOn = true;

void IRAM_ATTR handleButtonPress() {
    unsigned long currentTime = millis();

    if (currentTime - lastInterruptTime > Config::kDebounceMs) {
        lastInterruptTime = currentTime;
        currentState = (currentState == StoplightState::NonRegualted)
                           ? StoplightState::Allow
                           : StoplightState::NonRegualted;
    }
}

unsigned long stateDurationMs(StoplightState s) {
    switch (s) {
        case StoplightState::Allow:          return Config::kAllowTime * 1000UL;
        case StoplightState::AllowEnding:    return Config::kEndingTime * 1000UL;
        case StoplightState::StopWarning:    return Config::kWarningTime * 1000UL;
        case StoplightState::FullStop:       return Config::kStopTime * 1000UL;
        case StoplightState::FullStopEnding: return Config::kEndingTime * 1000UL;
        default:                              return 0; // NonRegualted: no auto timeout
    }
}

StoplightState nextNormalState(StoplightState s) {
    switch (s) {
        case StoplightState::Allow:          return StoplightState::AllowEnding;
        case StoplightState::AllowEnding:    return StoplightState::StopWarning;
        case StoplightState::StopWarning:    return StoplightState::FullStop;
        case StoplightState::FullStop:       return StoplightState::FullStopEnding;
        case StoplightState::FullStopEnding: return StoplightState::Allow;
        default:                              return StoplightState::NonRegualted;
    }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
            handleButtonPress,
            RISING);

  stateEnteredAt = millis();
  lastBlinkToggle = millis();
  blinkOn = true;
}

void loop() {
    unsigned long now = millis();
    StoplightState state = currentState;

    if (state != previousState) {
        // ISR toggled currentState since the last pass — re-baseline timing.
        previousState = state;
        stateEnteredAt = now;
        lastBlinkToggle = now;
        blinkOn = true;
    } else if (state != StoplightState::NonRegualted &&
               now - stateEnteredAt >= stateDurationMs(state)) {
        state = nextNormalState(state);
        currentState = state;
        previousState = state;
        stateEnteredAt = now;
        lastBlinkToggle = now;
        blinkOn = true;
    }

    if (now - lastBlinkToggle >= Config::kBlinkIntervalMs) {
        lastBlinkToggle = now;
        blinkOn = !blinkOn;
    }

    bool red = false, yellow = false, green = false;
    switch (state) {
        case StoplightState::Allow:          green = true;              break;
        case StoplightState::AllowEnding:    green = blinkOn;           break;
        case StoplightState::StopWarning:    yellow = true;             break;
        case StoplightState::FullStop:       red = true;                break;
        case StoplightState::FullStopEnding: red = true; yellow = true; break;
        case StoplightState::NonRegualted:   yellow = blinkOn;          break;
    }

    digitalWrite(RED_PIN,    red    ? HIGH : LOW);
    digitalWrite(GREEN_PIN,  green  ? HIGH : LOW);
    digitalWrite(YELLOW_PIN, yellow ? HIGH : LOW);
}