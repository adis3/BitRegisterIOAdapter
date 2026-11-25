#ifndef THERMOSTAT_HYSTERESIS_TESTS_STUBS_ARDUINO_H_
#define THERMOSTAT_HYSTERESIS_TESTS_STUBS_ARDUINO_H_
#include <stdint.h>
#include <stdio.h>
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
void pinMode(uint8_t pin, int mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
#endif  // THERMOSTAT_HYSTERESIS_TESTS_STUBS_ARDUINO_H_
