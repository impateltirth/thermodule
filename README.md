# Thermodule

Thermal management system on ESP32: temperature sensing, automatic PWM fan control across configurable temperature ranges, and real-time monitoring on an I2C LCD.

**Stack:** ESP32 · C++ · PWM · I2C · PlatformIO (Arduino framework)

## Features

- Temperature sensing with configurable thresholds
- Automatic fan speed regulation via PWM (LEDC) across temperature ranges
- Real-time I2C LCD dashboard: temperature, fan duty %, status
- Hysteresis to avoid fan speed oscillation at threshold boundaries
- Serial telemetry for logging/plotting

## Repository layout

```
thermodule/
├── platformio.ini   # ESP32 dev board, Arduino framework
├── src/
│   └── main.cpp     # Sensor read → control logic → PWM + LCD + serial
└── docs/            # TODO: add wiring photo / enclosure shots
```

## Getting started

1. Install [PlatformIO](https://platformio.org/) (VS Code extension recommended).
2. Connect your ESP32 board.
3. `pio run -t upload && pio device monitor`

## Configuration

All tuning lives at the top of `src/main.cpp`:

```cpp
constexpr float TEMP_FAN_MIN_C = 30.0f;  // fan starts ramping here
constexpr float TEMP_FAN_MAX_C = 60.0f;  // fan hits 100% here
constexpr float TEMP_HYSTERESIS_C = 2.0f;
```

Pin map (change to match your wiring):

| Function | Default pin | Notes |
|---|---|---|
| Fan PWM | GPIO 25 | LEDC channel 0, 25 kHz |
| Temp sensor | GPIO 34 (ADC) | TODO: set for your sensor |
| I2C SDA / SCL | GPIO 21 / 22 | LCD at 0x27, 16×2 |

## Temperature sensor

`read_temperature_c()` is stubbed — drop in your sensor:
- **NTC thermistor** via ADC + Steinhart–Hart (recommended, matches the analog theme)
- **DS18B20** via OneWire/DallasTemperature
- **DHT22 / SHT31** if you already have one on hand

## TODO

- [ ] Implement `read_temperature_c()` for your sensor
- [ ] Confirm pin map against your wiring
- [ ] Tune thresholds/hysteresis for your thermal load
- [ ] Add `docs/` photos of the build

## License

MIT — see [LICENSE](LICENSE).
