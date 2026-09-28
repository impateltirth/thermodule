#pragma once

/*
 * Thermodule configuration — every tunable in one place.
 * ** Verify the pin map against your hardware before flashing. **
 */

/* ---- Pins (ESP32 DevKit) ---- */
#define PIN_THERMISTOR  34    // ADC1_CH6, input-only — safe for analog
#define PIN_FAN_PWM     25    // LEDC PWM -> MOSFET gate (low-side fan drive)
#define PIN_TACH        33    // TODO: fan tachometer input (not yet used)
#define PIN_I2C_SDA     21
#define PIN_I2C_SCL     22
#define LCD_I2C_ADDR    0x27  // PCF8574 backpack; use 0x3F if yours differs

/* ---- Thermistor: 10k NTC + 10k series resistor ----
 * Divider topology assumed: 3V3 -- [10k series] --+-- [NTC] -- GND,
 * with the ADC reading the midpoint (+). If your divider is flipped
 * (NTC to 3V3), invert the resistance calculation in temp_sensor.cpp.
 */
#define THERM_NOMINAL_R  10000.0f  // resistance at 25 C
#define THERM_NOMINAL_T  25.0f
#define THERM_B_COEFF    3950.0f   // B25/50 value from your thermistor datasheet
#define THERM_SERIES_R   10000.0f
#define ADC_VREF         3.3f

/* ---- Fan control law ----
 * Below (setpoint - hyst): fan off. Above (setpoint + hyst): duty ramps
 * linearly to 100% at (setpoint + ramp_span). Inside the hysteresis band
 * the duty holds its last value (no chattering). At max_temp: full speed
 * + LCD alarm, regardless of everything else.
 */
#define TEMP_SETPOINT_C    45.0f
#define TEMP_HYSTERESIS_C  2.0f
#define TEMP_RAMP_SPAN_C   15.0f   // full speed at setpoint + span
#define TEMP_MAX_C         80.0f   // safety cutoff
#define FAN_PWM_FREQ_HZ    25000   // above audible range
#define FAN_PWM_RES_BITS   8
#define FAN_MIN_DUTY       51      // ~20%: keeps the fan spinning once started
#define FAN_SLEW_PER_TICK  25      // max duty change per 500 ms control tick

/* ---- Timing ---- */
#define CONTROL_PERIOD_MS  500
