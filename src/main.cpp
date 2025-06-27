#include <Arduino.h>
#include "wifi_ota.h"
#include "display.h"
#include "serial_comm.h"
#include "input.h"

void setup(){
    pinMode(RGB_PIN, OUTPUT);
    display_init();
    
    if(wifi_connect()){
        syncTime();
        ota_setup();
    }
    
    serial_setup();   // handshake con MCU interno
    
    startDisplayTask();
    startInputTask();
}

void loop(){ //core 0

    //check if WiFi is connected
    if(!connectedWifi){
        ArduinoOTA.handle(); 
    }

    InputEvent ev;
    if(getInputEvent(ev,0)){     // no bloquea
        switch(ev.type){
            case KEYPAD_CHAR:
                Serial.printf("Btn%c\n", ev.data);
                if (ev.data == '1') {
                    textoCentrado("TestForSC", 80, ST77XX_RED, 1);
                } else if (ev.data == 'C') {
                    textoCentrado("          ", 80, ST77XX_RED, 1);
                }
                break;

            case ENC_ROT_LEFT:
                Serial.println("EncoderL");
                break;

            case ENC_ROT_RIGHT:
                Serial.println("EncoderR");
                break;

            case ENC_BTN_PRESS:
                Serial.println("EncoderSW");
                break;
        }
    }
    serial_loop();
}
