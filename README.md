# ESP32-S3 LED Detection using ADC

This project uses a push button to turn an LED on and off and checks whether the LED is actually connected using the ADC on an ESP32-S3.

The LED is controlled through a GPIO output, and the ADC measures the voltage from the LED’s ground side to determine if the LED is present or missing.

---

## Hardware Used
- ESP32-S3-WROOM-1
- LED
- 330Ω resistor
- Push button
- Breadboard and jumper wires

---

## Pin Configuration

| Function | GPIO | Notes |
|--------|------|------|
| LED Output | GPIO7 | Drives LED ON/OFF |
| Button Input | GPIO13 | Internal pull-up enabled |
| ADC Sense | GPIO5 | ADC1_CHANNEL_4 |
| Ground | GND | Common ground |

-------------------------------------------------------------

## Wiring Connections

### LED Circuit
GPIO7 ──► LED (anode)
LED (cathode) ──► 330Ω ──► GND

### ADC Sensing 
LED cathode / GND node ──► GPIO5 (ADC1_CH4)

### Button
GPIO13 ──► Push Button ──► GND


------------------------------------------------------------------

## How Detection Works

- The button toggles the LED ON/OFF.
- When the LED is ON, the ADC reads voltage from the LED ground side.
- Based on the measured voltage, the firmware determines whether the LED is connected.
- A threshold comparison is used to decide LED presence.

