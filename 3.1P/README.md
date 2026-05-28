# Task 3.1P — Terrarium Sunlight Notifier

Arduino Nano 33 IoT reads light levels via a BH1750 sensor and sends
IFTTT Webhook notifications when sunlight starts or stops hitting the terrarium.

## Hardware

- Arduino Nano 33 IoT
- BH1750 light sensor
- Breadboard + jumper wires

## Wiring (I2C)

| BH1750 | Arduino |
| ------ | ------- |
| VCC    | 3.3V    |
| GND    | GND     |
| SDA    | A4      |
| SCL    | A5      |

## How It Works

- Polls light level every second
- If lux drops ≤ 30 → fires `lost_sunlight` IFTTT event (once)
- If lux rises > 30 → fires `received_sunlight` IFTTT event (once)
- Edge detection via `was_dark` flag prevents duplicate triggers

## Setup

1. Install libraries: `BH1750`, `WiFiNINA` via Arduino Library Manager
2. Open `task3_1_trigger.ino`
3. Set your WiFi credentials and IFTTT Webhook key in the config section
4. Flash to Arduino and monitor via Serial (9600 baud)

## Dependencies

- [BH1750 Library](https://github.com/claws/BH1750)
- [WiFiNINA](https://github.com/arduino-libraries/WiFiNINA)
- IFTTT Maker Webhooks
