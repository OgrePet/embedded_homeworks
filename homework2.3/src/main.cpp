#include <stdio.h>
#include <array>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_timer.h"

struct Config {
    static constexpr auto kLedPins = std::to_array<gpio_num_t>({GPIO_NUM_16, GPIO_NUM_3, GPIO_NUM_15});
    static constexpr auto kBlinkIntervals = std::to_array<int64_t>({1000000, 500000, 20000});
};

extern "C" void app_main(void)
{
    int64_t now = esp_timer_get_time();
    std::array<int64_t, Config::kBlinkIntervals.size()> pinLastToggle;
    pinLastToggle.fill(now);
    
    for (gpio_num_t pin : Config::kLedPins) {
        gpio_reset_pin(pin);
        gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    }
   
    while (1) {
        int64_t now = esp_timer_get_time();

        for (int i = 0; i < Config::kLedPins.size(); i++) {
            int64_t lastToggle = pinLastToggle[i];
            if (now - lastToggle >= Config::kBlinkIntervals[i]) {
                gpio_num_t pin = Config::kLedPins[i];
                gpio_set_level(pin, !gpio_get_level(pin));
                pinLastToggle[i] = now;
            }
        }

        vTaskDelay(1);
    }
}