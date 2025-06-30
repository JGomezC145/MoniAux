#pragma once
#include "globals.h"
#include <Arduino.h>
#include <queue>

// --- eventos que puede generar el subsistema de entrada ---
enum InputEventType {
    KEYPAD_CHAR,          // data: char (tecla)
    ENC_ROT_LEFT,
    ENC_ROT_RIGHT,
    ENC_BTN_PRESS
};

struct InputEvent {
    InputEventType type;
    char           data;     // solo usado si type == KEYPAD_CHAR
};

#define ENC_STEP 4 // pasos por click del encoder

void startInputTask();                 // crea la tarea en Core 1
bool getInputEvent(InputEvent& ev,
                   TickType_t timeout = 0);   // lee de la cola
