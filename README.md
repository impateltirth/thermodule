# Thermodule

[![Build firmware](https://github.com/impateltirth/thermodule/actions/workflows/platformio.yml/badge.svg)](https://github.com/impateltirth/thermodule/actions/workflows/platformio.yml)

Thermal management system on ESP32: NTC thermistor temperature sensing, automatic PWM fan control with hysteresis across configurable temperature ranges, and real-time monitoring on an I2C LCD.

**Hardware:** ESP32 DevKit · 10k NTC thermistor · MOSFET-driven fan · 16x2 I2C LCD (PCF8574)
**Firmware:** C++ on PlatformIO (Arduino framework)

## Control law

Every 500 ms the firmware reads temperature and updates the fan:

| Temperature | Fan |
|---|---|
| ≤ setpoint − hysteresis (43 °C) | Off |
| Setpoint ± hysteresis band | Holds last duty (no chattering) |
| > setpoint + hysteresis | Ramps linearly to 100% at setpoint + 15 °C |
| ≥ 80 °C (max) | Full speed + `!! OVERHEAT !!` LCD alarm |

Duty changes are slew-rate limited so the fan never jumps abruptly. PWM runs at 25 kHz (above audible range) via the ESP32 LEDC peripheral.

## Pin map (ESP32 DevKit)

| Function | Pin | Notes |
|---|---|---|
| Thermistor ADC | GPIO34 | 10k NTC + 10k series, Steinhart-Hart conversion |
| Fan PWM | GPIO25 | LEDC → MOSFET gate, low-side fan drive |
| Fan tach | GPIO33 | Reserved — tachometer input (TODO) |
| I2C SDA / SCL | GPIO21 / GPIO22 | 16x2 LCD backpack (default addr `0x27`) |

All tunables (pins, setpoint, hysteresis, PWM params, thermistor constants) live in `include/config.h`. **Verify the pin map against your hardware before flashing** — especially the thermistor divider topology, which is documented in `config.h`.

## Firmware layout

```
├── platformio.ini      # esp32dev, Arduino framework, LCD library
├── include/
│   └── config.h        # ALL tunables: pins, control law, thermistor
└── src/
    ├── main.cpp        # setup + 500 ms control loop
    ├── temp_sensor.h/.cpp  # 16x-averaged ADC + Steinhart-Hart
    ├── fan.h/.cpp      # LEDC PWM, hysteresis, slew limiting, overheat alarm
    └── display.h/.cpp  # I2C LCD status UI
```

## Display

- Line 0: `Temp: 45.2 C` (or `!! OVERHEAT !!`)
- Line 1: `Fan:  65%`

Serial telemetry at 115200 baud:
`T=45.2C duty=166 alarm=0 sensor_fault=0`.

An invalid or rail-clamped thermistor reading is treated as a sensor fault: the
fan moves toward full duty, the LCD reports `SENSOR FAULT`, and telemetry marks
the fault explicitly.

## Building

```bash
pip install platformio   # or: brew install platformio
pio run -t upload        # builds, flashes, installs the LCD library
pio device monitor       # serial telemetry
```

## Status

- [x] Pin map and all tunables in `config.h`
- [x] Thermistor sensing with Steinhart-Hart conversion
- [x] Hysteresis + slew-limited PWM fan control, overheat alarm
- [x] I2C LCD status display + serial telemetry
- [ ] Verify thermistor divider topology and B-coefficient against your part
- [ ] Tachometer feedback (closed-loop RPM control)
- [ ] Tune setpoint/hysteresis against measured thermal response

## License

MIT — see [LICENSE](LICENSE).
