# Task 1.1 – Switching ON Lights

## System Description

An Arduino Nano 33 IoT based lighting system for an elderly resident's home.
When a push button is pressed, the porch light activates for 30 seconds and
the hallway light activates for 60 seconds. The system is designed to assist
the resident in safely entering the home during low-light conditions.

## Hardware

- Arduino Nano 33 IoT
- 2x LEDs (porch light: pin 8, hallway light: pin 9)
- Push button (pin 4, INPUT_PULLUP)
- Resistors, breadboard, jumper wires

## Code Structure

### Configuration (`#define` block)

Declares pin assignments and light duration constants. All hardware and
timing parameters are isolated here for easy modification.

### `setup()`

Initialises pin modes for both lights and the button. Sets both lights to
OFF as the default state on power-up.

### `loop()`

Monitors the button state. On a valid press, both lights are turned ON and
a timer is started. The porch light is turned OFF after 30 seconds and the
hallway light after 60 seconds. A `running` flag prevents re-triggering
while a sequence is active.
