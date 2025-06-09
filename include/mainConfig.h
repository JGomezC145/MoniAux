#pragma once
#include "secret.h" // eliminar para usar el propio secreto


// --------------------
// Misc config:
// --------------------
#define DeviceID "jg-moniaux"  // Identificador del dispositivo

#define SerialBaudRate 115200  // Velocidad del puerto serie


// ----------------------------------
// Configuración WiFi
// ----------------------------------
extern const char* ssid;      // <--  
extern const char* password;  // <--  

// -> WiFi subconfig 
#define NTP_SERVER "pool.ntp.org"  // Servidor NTP para sincronización de hora
#define TIMEZONE_OFFSET -21600  // Offset de zona horaria (UTC -6)

#define HostName "moniaux" // Nombre del host para el dispositivo WiFi y posible mDNS


// --------------------
// Configuración de pines TFT (SPI2 en ESP32-S3)
// --------------------
#define TFT_SCLK 6
#define TFT_MOSI 4
#define TFT_MISO -1
#define TFT_CS 7
#define TFT_DC 2
#define TFT_RST 3

// --------------------
// Configuración de Pantalla TFT
// --------------------
#define TFT_WIDTH 128
#define TFT_HEIGHT 128
#define SPI_SPEED 40000000  // Velocidad SPI para TFT