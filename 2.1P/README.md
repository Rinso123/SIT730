# Task 2.1P — Temperature & Light Data to the Web

Arduino Nano 33 IoT reads temperature and light levels, then pushes
data to ThingSpeak every 15 seconds via HTTP POST.

## Hardware

| Component      | Pin |
| -------------- | --- |
| DHT11 (data)   | D2  |
| LDR (analogue) | A0  |

## ThingSpeak Channel Fields

| Field   | Data                           |
| ------- | ------------------------------ |
| Field 1 | Temperature (°C)               |
| Field 2 | Light Level (0–1023)           |
| Field 3 | Alarm Flag (0 = OK, 1 = Alert) |

## Libraries Required

- `DHT sensor library` — Adafruit
- `ThingSpeak` — MathWorks
- `WiFiNINA` — Arduino

## Setup

1. Install the libraries above via Arduino IDE Library Manager
2. Set your WiFi credentials in `ssid[]` and `password[]`
3. Replace `SECRET_CH_ID` and `SECRET_WRITE_APIKEY` with your
   ThingSpeak channel details
4. Flash to Arduino Nano 33 IoT

## Alarm Thresholds (SIT730)

| Sensor      | Min   | Max   |
| ----------- | ----- | ----- |
| Temperature | 18 °C | 30 °C |
| Light       | 100   | 900   |

Field 3 is set to `1` if any reading breaches these limits.
