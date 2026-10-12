#ifndef LINE_H
#define LINE_H

#include <Arduino.h>
#include "driver/adc.h"

// ============================================================
// LINE SENSORS - UNIVERSAL RAW ANALOG LIBRARY (MAX OPTIMIZACION)
// ============================================================
// Libreria generica para sensado analogico crudo en ESP32.
// Soporta cualquier cantidad de sensores (pines ADC1).
//
// Uso:
//   const int pines[] = { 33 };
//   LineSensors sensores(pines, 1);
//
// Optimizaciones aplicadas:
//   - adc1_get_raw (sin HAL de Arduino)
//   - IRAM_ATTR (sin cache-miss)
//   - always_inline (sin overhead de llamada)
//   - Sin calibracion, sin umbrales, 100% crudo
// ============================================================

#define LINE_MAX_SENSORES 16

class LineSensors {
  private:
    int            _numSensores;
    uint8_t        _gpio[LINE_MAX_SENSORES];
    adc1_channel_t _ch[LINE_MAX_SENSORES];

    // Mapeo GPIO -> canal ADC1
    static adc1_channel_t _gpioAChannel(uint8_t gpio) {
      switch (gpio) {
        case 36: return ADC1_CHANNEL_0;
        case 37: return ADC1_CHANNEL_1;
        case 38: return ADC1_CHANNEL_2;
        case 39: return ADC1_CHANNEL_3;
        case 32: return ADC1_CHANNEL_4;
        case 33: return ADC1_CHANNEL_5;
        case 34: return ADC1_CHANNEL_6;
        case 35: return ADC1_CHANNEL_7;
        default: return ADC1_CHANNEL_5;   // fallback
      }
    }

  public:
    // =========================================================
    // CONSTRUCTOR
    // =========================================================
    LineSensors(const int* pines, int numSensores) {
      _numSensores = (numSensores > LINE_MAX_SENSORES)
                     ? LINE_MAX_SENSORES : numSensores;

      for (int i = 0; i < _numSensores; i++) {
        _gpio[i] = (uint8_t)pines[i];
        _ch[i]   = _gpioAChannel(_gpio[i]);
      }
    }

    // =========================================================
    // INICIALIZACION (llamar una vez en setup)
    // =========================================================
    void Initialize() {
      adc1_config_width(ADC_WIDTH_BIT_12);
      for (int i = 0; i < _numSensores; i++) {
        adc1_config_channel_atten(_ch[i], ADC_ATTEN_DB_11);
      }
    }

    // =========================================================
    // GETTERS
    // =========================================================
    int GetCount() const { return _numSensores; }

    int GetPin(int idx) const {
      if (idx < 0 || idx >= _numSensores) return -1;
      return _gpio[idx];
    }

    // =========================================================
    // LECTURA PRINCIPAL (MAXIMA OPTIMIZACION)
    // Rellena out[] con los valores crudos (0..4095) de cada sensor
    // =========================================================
    __attribute__((always_inline)) inline IRAM_ATTR
    void Read(int* out) {
      for (int i = 0; i < _numSensores; i++) {
        out[i] = adc1_get_raw(_ch[i]);
      }
    }

    // =========================================================
    // LECTURA INDIVIDUAL (MAXIMA OPTIMIZACION)
    // Devuelve el valor crudo de un sensor concreto
    // =========================================================
    __attribute__((always_inline)) inline IRAM_ATTR
    int ReadOne(uint8_t idx) {
      if (idx >= _numSensores) return 0;
      return adc1_get_raw(_ch[idx]);
    }

    // =========================================================
    // PRINT - Imprime todos los sensores en una linea
    // =========================================================
    void Print() {
      int out[LINE_MAX_SENSORES];
      Read(out);

      Serial.print("[");
      for (int i = 0; i < _numSensores; i++) {
        Serial.print(out[i]);
        if (i < _numSensores - 1) Serial.print(",");
      }
      Serial.println("]");
    }

    void Print(const char* prefix) {
      Serial.print(prefix);
      Print();
    }
};

#endif