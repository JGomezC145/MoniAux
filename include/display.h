#pragma once
#include "globals.h"

// Estados que se pintan arriba-derecha
enum StatusType { WIFIST = 1, SERIALST = 2 };

void display_init();
void textoCentrado(const String &texto, int y,
                   uint16_t color = ST7735_WHITE, int size = 1);
void escribir(const String &texto, int x, int y,
              uint16_t color = ST7735_WHITE,
              uint16_t bg    = ST7735_BLACK, int size = 1);
void setStatus(StatusType c, bool ok);

// Tarea que actualiza hora y otros indicadores (se corre en core 1)
void startDisplayTask();
void getTemperature()
