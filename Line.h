#ifndef LINE_H
#define LINE_H

#include <Arduino.h>
#include "driver/adc.h"

// LineSensors - raw analog sensor reader for ESP32 (ADC1)
// Supports 1 to 16 sensors. Raw values only (0..4095).
// Optimized with adc1_get_raw, IRAM, always_inline.

#define LINE_MAX_SENSORES 16

class LineSensors {
  private:
    int            _count;
    uint8_t        _gpio[LINE_MAX_SENSORES];
    adc1_channel_t _ch[LINE_MAX_SENSORES];

    static adc1_channel_t _toChannel(uint8_t gpio) {
      switch (gpio) {
        case 36: return ADC1_CHANNEL_0;
        case 37: return ADC1_CHANNEL_1;
        case 38: return ADC1_CHANNEL_2;
        case 39: return ADC1_CHANNEL_3;
        case 32: return ADC1_CHANNEL_4;
        case 33: return ADC1_CHANNEL_5;
        case 34: return ADC1_CHANNEL_6;
        case 35: return ADC1_CHANNEL_7;
        default: return ADC1_CHANNEL_5;
      }
    }

  public:
    // pins: GPIO array (ADC1 only). count: number of sensors.
    LineSensors(const int* pins, int count) {
      _count = (count > LINE_MAX_SENSORES) ? LINE_MAX_SENSORES : count;
      for (int i = 0; i < _count; i++) {
        _gpio[i] = (uint8_t)pins[i];
        _ch[i]   = _toChannel(_gpio[i]);
      }
    }

    // Configure ADC1. Call once in setup().
    void Initialize() {
      adc1_config_width(ADC_WIDTH_BIT_12);
      for (int i = 0; i < _count; i++) {
        adc1_config_channel_atten(_ch[i], ADC_ATTEN_DB_11);
      }
    }

    // Number of configured sensors.
    int Count() const { return _count; }

    // GPIO number of sensor idx, or -1 if out of range.
    int Pin(int idx) const {
      if (idx < 0 || idx >= _count) return -1;
      return _gpio[idx];
    }

    // Fill out[] with raw values. out must hold at least Count() slots.
    __attribute__((always_inline)) inline IRAM_ATTR
    void Read(int* out) {
      for (int i = 0; i < _count; i++) out[i] = adc1_get_raw(_ch[i]);
    }

    // Raw value of one sensor. Returns 0 if idx out of range.
    __attribute__((always_inline)) inline IRAM_ATTR
    int ReadOne(uint8_t idx) {
      if (idx >= _count) return 0;
      return adc1_get_raw(_ch[idx]);
    }

    // Print all values as [v0,v1,...] on one line.
    void Print() {
      int out[LINE_MAX_SENSORES];
      Read(out);
      Serial.print("[");
      for (int i = 0; i < _count; i++) {
        Serial.print(out[i]);
        if (i < _count - 1) Serial.print(",");
      }
      Serial.println("]");
    }

    // Same as Print() with prefix.
    void Print(const char* prefix) {
      Serial.print(prefix);
      Print();
    }
};

#endif