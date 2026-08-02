#include <Arduino.h>

enum class LedState : uint8_t { Off, On };

enum class Mode : uint8_t { Blinking, AlwaysOn, AlwaysOff };

struct Config {
    static constexpr uint8_t kLedPin = 16;
    static constexpr uint8_t kButtonPin = 4;
    static constexpr uint32_t kBlinkIntervalMs = 500;
    static constexpr uint32_t kDebounceMs = 50;
    static constexpr uint32_t kStatsEveryIterations = 1000;
    static constexpr uint32_t kSerialBaud = 115200;
};

class Led {
public:
    explicit constexpr Led(uint8_t pin) : pin_(pin), state_(LedState::Off) {}

    void init() {
        pinMode(pin_, OUTPUT);
        set(LedState::Off);
    }

    void set(LedState state) {
        state_ = state;
        digitalWrite(pin_, state == LedState::On ? HIGH : LOW);
    }

    void toggle() {
        set(state_ == LedState::On ? LedState::Off : LedState::On);
    }

private:
    uint8_t pin_;
    LedState state_;
};

static volatile bool buttonPressed = false;

static void onButtonPress() {
    buttonPressed = true;
}

static Mode nextMode(Mode mode) {
    switch (mode) {
        case Mode::Blinking:  return Mode::AlwaysOn;
        case Mode::AlwaysOn:  return Mode::AlwaysOff;
        case Mode::AlwaysOff: return Mode::Blinking;
    }
    return Mode::Blinking;
}

static Led& getLed() {
    static Led led(Config::kLedPin);
    return led;
}

void setup() {
    Serial.begin(Config::kSerialBaud);

    getLed().init();

    pinMode(Config::kButtonPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(Config::kButtonPin), onButtonPress, RISING);
}

void loop() {
    static Mode mode = Mode::Blinking;
    static uint32_t lastToggleBlinkMs = 0;
    static uint32_t lastPressMs = 0;
    static uint32_t iterations = 0;
    static uint32_t batchStartUs = micros();

    if (buttonPressed) {
        buttonPressed = false;
        const uint32_t nowMs = millis();
        if (nowMs - lastPressMs >= Config::kDebounceMs) {
            lastPressMs = nowMs;
            mode = nextMode(mode);

            switch (mode) {
                case Mode::Blinking:
                    lastToggleBlinkMs = nowMs;
                    getLed().set(LedState::On);
                    break;
                case Mode::AlwaysOn:
                    getLed().set(LedState::On);
                    break;
                case Mode::AlwaysOff:
                    getLed().set(LedState::Off);
                    break;
            }
        }
    }

    if (mode == Mode::Blinking) {
        const uint32_t nowMs = millis();
        if (nowMs - lastToggleBlinkMs >= Config::kBlinkIntervalMs) {
            lastToggleBlinkMs = nowMs;
            getLed().toggle();
        }
    }

    if (++iterations >= Config::kStatsEveryIterations) {
        const uint32_t batchUs = micros() - batchStartUs;
        Serial.printf("Avg loop iteration: %lu ns (over %lu iterations)\n",
                      static_cast<unsigned long>(batchUs * 1000UL / iterations),
                      static_cast<unsigned long>(iterations));
        iterations = 0;
        batchStartUs = micros();
    }
}
