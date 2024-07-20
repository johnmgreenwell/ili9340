# ILI9340 Driver

This library for the ILI9340 is no longer being developed, you can use the ILI9341 code as a drop-in replacement!

https://github.com/adafruit/Adafruit_ILI9341

Modified by John Greenwell to adapt driver for custom HAL support, 2024.

## Usage

For this modified version, the following hardware abstraction layer (HAL) requirements must be satisfied:

* A header file `hal.h` providing access to HAL namespace classes and methods.
* A `GPIO` class within the `HAL` namespace with the following methods:
  - Set pin mode: `pinMode(uint8_t mode)`
  - Write logic level to pin: `digitalWrite(uint8_t val)`
* A `SPI` class within the `HAL` namespace with the following methods:
  - Perform SPI data transfer: `transfer(uint8_t data)`
* A delay_ms() function in the HAL namespace that delays an accurate milliseconds to be used for timing.

Some further requirements may also be found. Typically, these will mirror the Arduino framework and should be added to `hal.h`.