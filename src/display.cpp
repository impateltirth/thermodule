#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "display.h"
#include "config.h"

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);

static void printPadded(const char *s)
{
    uint8_t n = 0;
    while (*s && n < 16) {
        lcd.print(*s++);
        n++;
    }
    while (n++ < 16)
        lcd.print(' ');
}

void Display::begin()
{
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    printPadded("Thermodule");
    lcd.setCursor(0, 1);
    printPadded("Booting...");
}

void Display::show(float tempC, uint8_t duty, bool alarm)
{
    char line[17];

    lcd.setCursor(0, 0);
    if (alarm) {
        printPadded("!! OVERHEAT !!");
    } else {
        snprintf(line, sizeof(line), "Temp: %4.1f C", (double)tempC);
        printPadded(line);
    }

    lcd.setCursor(0, 1);
    const char *state = (duty == 0) ? "OFF" : (duty >= 255 ? "MAX" : "   ");
    snprintf(line, sizeof(line), "Fan: %3d%% %s", (duty * 100) / 255, state);
    printPadded(line);
}
