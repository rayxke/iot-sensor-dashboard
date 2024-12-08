#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

#define TRIG_PIN 16 // GPIO pin for the trigger
#define ECHO_PIN 17 // GPIO pin for the echo


void init_sensor_pins() {
    esp_rom_gpio_pad_select_gpio(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
}

uint32_t pulseIn(gpio_num_t pin, int level, uint32_t timeout_us) {
    int64_t start_time = 0, end_time = 0;

    // Ensure the pin is configured as an input
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    // Wait for any previous pulse to end
    int64_t wait_start = esp_timer_get_time();
    while (gpio_get_level(pin) == level) {
        if ((esp_timer_get_time() - wait_start) > timeout_us) {
            return 0; // Timeout waiting for the signal to change
        }
    }

    // Wait for the pulse to start
    wait_start = esp_timer_get_time();
    while (gpio_get_level(pin) != level) {
        if ((esp_timer_get_time() - wait_start) > timeout_us) {
            return 0; // Timeout waiting for the pulse to start
        }
    }
    start_time = esp_timer_get_time();

    // Measure the pulse width
    while (gpio_get_level(pin) == level) {
        if ((esp_timer_get_time() - start_time) > timeout_us) {
            return 0; // Timeout while measuring pulse
        }
    }
    end_time = esp_timer_get_time();

    // Calculate and return the pulse duration in microseconds
    return (uint32_t)(end_time - start_time);
}

float measure_distance() {
    gpio_set_level(TRIG_PIN, 0); // Ensure trigger is low
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(TRIG_PIN, 1); // Send trigger pulse
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(TRIG_PIN, 0);

    float duration_us = pulseIn(ECHO_PIN, 1, 30000);
    float distance_cm = (duration_us / 2.0) / 29.1; // Convert to cm
    return distance_cm;
}


void app_main() {
    init_sensor_pins();
    while (1) {
        float distance = measure_distance();
        ESP_LOGI("Distance Sensor", "Measured Distance: %.2f cm\n", distance);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for a second before next reading
    }
}