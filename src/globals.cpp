/*
* WARNING: This file must be used to define global variables in memory.
*/

#include "globals.h"

// definición (memoria real)
uint8_t  hora            = 0;
uint8_t  minutos         = 0;
String   horaActual      = "";
String   horaActualNew   = "";
unsigned long lastNTPUpdate = 0;
