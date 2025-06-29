#include <Arduino.h>
#include "wifi_ota.h"
#include "display.h"
#include "serial_comm.h"
#include "input.h"

int testencoder = 50;

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
    if(connectedWifi){
        ArduinoOTA.handle(); 
    }

    InputEvent ev;
    if(getInputEvent(ev,0)){     // no bloquea
        textoCentrado("           ", 95, ST77XX_GREEN, 1);
        switch(ev.type){
            case KEYPAD_CHAR:
                Serial.printf("Btn%c\n", ev.data);
                // textoCentrado(String("Btn%c", ev.data), 95, ST77XX_GREEN, 1);
                if (ev.data == '1') {
                    textoCentrado("TestForSC", 80, ST77XX_RED, 1);
                } else if (ev.data == 'C') {
                    textoCentrado("          ", 80, ST77XX_RED, 1);
                }

                switch (ev.data){  // switch on char
                    case '1': // va desde 1 a 9 luego salta a A, B, C
                        textoCentrado("Btn1", 95, ST77XX_GREEN, 1);
                        break;
                    case '2':
                        textoCentrado("Btn2", 95, ST77XX_GREEN, 1);
                        break;
                    case '3':
                        textoCentrado("Btn3", 95, ST77XX_GREEN, 1);
                        break;
                    case '4':
                        textoCentrado("Btn4", 95, ST77XX_GREEN, 1);
                        break;
                    case '5':
                        textoCentrado("Btn5", 95, ST77XX_GREEN, 1);
                        break;
                    case '6':
                        textoCentrado("Btn6", 95, ST77XX_GREEN, 1);
                        break;
                    case '7':
                        textoCentrado("Btn7", 95, ST77XX_GREEN, 1);
                        break;
                    case '8':
                        textoCentrado("Btn8", 95, ST77XX_GREEN, 1);
                        break;
                    case '9':
                        textoCentrado("Btn9", 95, ST77XX_GREEN, 1);
                        break;
                    case 'A':
                        textoCentrado("BtnA", 95, ST77XX_GREEN, 1);
                        break;
                    case 'B':
                        textoCentrado("BtnB", 95, ST77XX_GREEN, 1);
                        break;
                    case 'C':
                        textoCentrado("BtnC", 95, ST77XX_GREEN, 1);
                        break;                    
                    default:
                        break;
                }
                break;

            case ENC_ROT_LEFT:
                Serial.println("EncoderL");
                textoCentrado("EncoderL", 95, ST77XX_GREEN, 1);
                break;

            case ENC_ROT_RIGHT:
                Serial.println("EncoderR");
                textoCentrado("EncoderR", 95, ST77XX_GREEN, 1);
                break;

            case ENC_BTN_PRESS:
                Serial.println("EncoderSW");
                textoCentrado("EncoderSW", 95, ST77XX_GREEN, 1);
                break;
        }
    }
    serial_loop();
}
