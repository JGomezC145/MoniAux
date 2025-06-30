#pragma once
#include "globals.h"

void serial_setup();                 // baud + handshake “hello/world”
void serial_loop();                  // llama en loop()
void scanNetworks();                 // comando "comm:scan"
void updateTimeClient(const String &timeStr);
