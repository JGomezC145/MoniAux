#include <Arduino.h>
#include "wifi_ota.h"
#include "display.h"
#include "serial_comm.h"

void setup(){
    pinMode(RGB_PIN, OUTPUT);
    display_init();
    
    if(wifi_connect()){
        syncTime();
        ota_setup();
    }
    
    serial_setup();   // handshake con MCU interno
    
    //startDisplayTask();
}

void loop(){ //core 0
    serial_loop();
}
