#pragma once
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "globals.h"

bool wifi_connect();                 // devuelve true si conecta
void ota_setup();                    // configura y arranca OTA
void syncTime();                     // fuerza NTP (usa constantes en mainConfig.h)
extern NTPClient timeClient;         // para quien necesite leer la hora
void wifi_stat();                    // imprime datos de conexión WiFi en Serial
