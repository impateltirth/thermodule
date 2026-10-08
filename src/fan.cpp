#include <Arduino.h>
#include <esp_arduino_version.h>
#include "fan.h"
#include "config.h"

#define FAN_LEDC_CHANNEL 0

static void writeDuty(uint8_t duty)
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(PIN_FAN_PWM, duty);
#else
    ledcWrite(FAN_LEDC_CHANNEL, duty);
#endif
}

void Fan::begin()
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(PIN_FAN_PWM, FAN_PWM_FREQ_HZ, FAN_PWM_RES_BITS);
#else
    ledcSetup(FAN_LEDC_CHANNEL, FAN_PWM_FREQ_HZ, FAN_PWM_RES_BITS);
    ledcAttachPin(PIN_FAN_PWM, FAN_LEDC_CHANNEL);
#endif
    writeDuty(0);
}

uint8_t Fan::update(float tempC)
{
    uint8_t target = duty_;  // default: hold (inside hysteresis band)
    alarm_ = false;
    sensor_fault_ = !isfinite(tempC);

    if (sensor_fault_) {
        // A missing or invalid sensor must fail safe: command full cooling.
        target = 255;
        alarm_ = true;
    } else if (tempC >= TEMP_MAX_C) {
        target = 255;
        alarm_ = true;
    } else if (tempC >= TEMP_SETPOINT_C + TEMP_HYSTERESIS_C) {
        float span = TEMP_RAMP_SPAN_C - TEMP_HYSTERESIS_C;
        float x = (tempC - TEMP_SETPOINT_C - TEMP_HYSTERESIS_C) / span;
        if (x > 1.0f)
            x = 1.0f;
        target = FAN_MIN_DUTY + (uint8_t)(x * (255 - FAN_MIN_DUTY));
    } else if (tempC <= TEMP_SETPOINT_C - TEMP_HYSTERESIS_C) {
        target = 0;
    }

    // Slew-rate limit so the fan never jumps abruptly.
    if (target > duty_)
        duty_ = (target - duty_ > FAN_SLEW_PER_TICK) ? duty_ + FAN_SLEW_PER_TICK : target;
    else if (target < duty_)
        duty_ = (duty_ - target > FAN_SLEW_PER_TICK) ? duty_ - FAN_SLEW_PER_TICK : target;

    writeDuty(duty_);
    return duty_;
}
