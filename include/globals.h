#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "mainConfig.h"

// ====== Objetos compartidos ======
extern Adafruit_ST7735 tft;

extern bool otaON;
extern bool connectedWifi;
extern bool connectedSerial;
extern String horaActual; // Variable global para almacenar la hora
extern String horaActualNew; // Variable temporal para almacenar la hora
extern uint8_t hora;
extern uint8_t minutos;

// LED RGB builtin (esp32-s3) 
#define RGB_PIN RGB_BUILTIN
void updateLEDColor(int r, int g, int b, int brightness = 255); // Actualiza el color del LED RGB
