/* Thermodule — ESP32 thermal management.
 *
 * Loop: read temperature -> compute fan duty with hysteresis ->
 *        drive PWM (LEDC) -> update I2C LCD -> serial telemetry.
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* ---------------- Configuration ---------------- */
constexpr float TEMP_FAN_MIN_C   = 30.0f;  // fan starts ramping
constexpr float TEMP_FAN_MAX_C   = 60.0f;  // fan reaches 100%
constexpr float TEMP_HYSTERESIS_C = 2.0f;  // anti-oscillation band

constexpr uint8_t PIN_FAN_PWM   = 25;
constexpr uint8_t PIN_TEMP_ADC  = 34;
constexpr uint8_t PIN_I2C_SDA   = 21;
constexpr uint8_t PIN_I2C_SCL   = 22;
constexpr uint8_t LCD_I2C_ADDR  = 0x27;

constexpr uint8_t  LEDC_CHANNEL   = 0;
constexpr uint32_t LEDC_FREQ_HZ   = 25000; // 25 kHz: above audible range
constexpr uint8_t  LEDC_RES_BITS  = 8;

constexpr uint32_t LOOP_PERIOD_MS = 500;

/* ---------------- State ---------------- */
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);
static uint8_t fan_duty = 0; // 0-255

/* TODO: implement for your temperature sensor.
 * Options: NTC thermistor via ADC + Steinhart-Hart (recommended),
 *          DS18B20 (OneWire), DHT22/SHT31. */
float read_temperature_c()
{
    // Placeholder: replace with real sensor code.
    // Example NTC sketch:
    //   int raw = analogRead(PIN_TEMP_ADC);
    //   ... Steinhart-Hart ...
    return 25.0f;
}

static uint8_t compute_fan_duty(float temp_c)
{
    // Hysteresis: only change direction of travel past the band edges.
    static bool ramping_up = true;

    if (ramping_up && temp_c < TEMP_FAN_MIN_C - TEMP_HYSTERESIS_C)
        ramping_up = false;
    else if (!ramping_up && temp_c > TEMP_FAN_MIN_C + TEMP_HYSTERESIS_C)
        ramping_up = true;

    if (temp_c <= TEMP_FAN_MIN_C)
        return ramping_up ? 0 : fan_duty; // hold while inside hysteresis band
    if (temp_c >= TEMP_FAN_MAX_C)
        return 255;

    float fraction = (temp_c - TEMP_FAN_MIN_C) / (TEMP_FAN_MAX_C - TEMP_FAN_MIN_C);
    return (uint8_t)(fraction * 255.0f);
}

static void update_lcd(float temp_c)
{
    lcd.setCursor(0, 0);
    lcd.print("Temp ");
    lcd.print(temp_c, 1);
    lcd.print((char)223); // degree symbol
    lcd.print("C   ");

    lcd.setCursor(0, 1);
    lcd.print("Fan ");
    lcd.print((fan_duty * 100) / 255);
    lcd.print("%   ");
    lcd.print(fan_duty == 0 ? "IDLE" : fan_duty == 255 ? "MAX " : "AUTO");
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Thermodule boot");

    ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ, LEDC_RES_BITS);
    ledcAttachPin(PIN_FAN_PWM, LEDC_CHANNEL);
    ledcWrite(LEDC_CHANNEL, 0);

    analogReadResolution(12);
    Serial.println("ts_ms,temp_c,fan_duty");
}

void loop()
{
    static uint32_t last_run = 0;
    if (millis() - last_run < LOOP_PERIOD_MS)
        return;
    last_run = millis();

    float temp_c = read_temperature_c();
    fan_duty = compute_fan_duty(temp_c);
    ledcWrite(LEDC_CHANNEL, fan_duty);
    update_lcd(temp_c);

    Serial.print(millis());
    Serial.print(',');
    Serial.print(temp_c, 2);
    Serial.print(',');
    Serial.println(fan_duty);
}
