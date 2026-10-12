Quick Start

#include "Line.h"

const int pins[] = { 33 };
LineSensors sensors(pins, 1);

void setup() {
Serial.begin(115200);
sensors.Initialize();
}

void loop() {
int value = sensors.ReadOne(0);
Serial.println(value);
delay(100);
}

Multiple Sensors

#include "Line.h"

const int pins[] = { 32, 33, 34, 35 };
LineSensors sensors(pins, 4);

void setup() {
Serial.begin(115200);
sensors.Initialize();
}

void loop() {
int values[4];
sensors.Read(values);
Serial.println(values[0]);
Serial.println(values[1]);
Serial.println(values[2]);
Serial.println(values[3]);
delay(100);
}

API

LineSensors(pins, count) - create with GPIO array and number of sensors
Initialize() - configure ADC1, call once in setup()
ReadOne(idx) - raw value (0..4095) of one sensor
Read(out) - fill array with all sensor values
Print() - print all values as [v0,v1,...]
Count() - number of sensors
Pin(idx) - GPIO number of a sensor

Notes

Only ADC1 pins: 32, 33, 34, 35, 36, 37, 38, 39
Raw values only, no calibration, no thresholds
Optimized for speed: adc1_get_raw + IRAM + always_inline
