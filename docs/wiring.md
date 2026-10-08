# Wiring schematic

```mermaid
flowchart LR
    V33["ESP32 3V3"] --> RS["10 kΩ series resistor"]
    RS --> ADC["GPIO34 / ADC"]
    ADC --> NTC["10 kΩ NTC thermistor"]
    NTC --> GND["GND"]
    P25["GPIO25 / 25 kHz PWM"] --> RG["Gate resistor"]
    RG --> MOSFET["Logic-level N-MOSFET"]
    FAN["DC fan"] --> MOSFET
    SUPPLY["Fan supply +"] --> FAN
    MOSFET --> GND
    P21["GPIO21 / SDA"] --> LCD["PCF8574 LCD"]
    P22["GPIO22 / SCL"] --> LCD
```

Use a flyback diode when required by the fan/load, join the fan-supply and
ESP32 grounds, and verify MOSFET current/voltage ratings before assembly.
The firmware assumes the divider orientation shown above.
