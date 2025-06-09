#include "serial_comm.h"
#include "wifi_ota.h"
#include "display.h"

static unsigned long lastMsgMillis=0;

void scanNetworks(){ /*  Copia exacta de tu scanNetworks()  */ }

void serial_setup(){
    textoCentrado("Conectando a Serial...", 94+9);
    Serial.begin(SerialBaudRate);
    Serial.setTimeout(200);
    delay(100);
    Serial.println("hello");
    //textoCentrado("Esperando respuesta...",60);
    while(!Serial.available()) delay(100);
    if(Serial.readStringUntil('\n')=="world"){
        connectedSerial=true;
        setStatus(SERIALST,true);
    }
    textoCentrado("Listo!",94+9+9);
    delay(300);
    tft.fillScreen(ST77XX_BLACK);
}

static void handleCommand(const String &msg){
    /*  Implementa parsing "UpdatePropsTitle:", "comm:scan", …  */
    /*  Usa funciones de display.cpp y wifi_ota.cpp             */
    if(msg.startsWith("UpdatePropsTitle:")){
        String title = msg.substring(17);
        tft.fillRect(0,50,130,400,ST77XX_BLACK);
        textoCentrado(title,50,ST77XX_WHITE,1);
    } else if(msg.startsWith("UpdatePropsArtist:")){
        String artist = msg.substring(18);
        tft.fillRect(0,75,130,20,ST77XX_BLACK);
        textoCentrado(artist,75,ST77XX_WHITE,1);
    } else if(msg.startsWith("UpdateState:")){
        String state = msg.substring(12);
        if(state=="Playing"){
            updateLEDColor(0,0,255); // Azul
            tft.fillRect(0,108,130,20,ST77XX_BLACK);
            textoCentrado("Reproduciendo",108,ST77XX_BLUE,1);
        } else if(state=="Paused"){
            updateLEDColor(255,255,0); // Amarillo
            tft.fillRect(0,108,130,20,ST77XX_YELLOW);
            textoCentrado("Pausado",108,ST77XX_YELLOW,1);
        } else if(state=="Stopped"){
            updateLEDColor(255,0,0); // Rojo
            tft.fillRect(0,108,130,20,ST77XX_RED);
            textoCentrado("Detenido",108,ST77XX_RED,1);
        }
    } else if(msg.startsWith("imhere")){
        lastMsgMillis = millis();
    } else if(msg.startsWith("comm:")){
        String comm = msg.substring(5);
        if(comm=="scan") scanNetworks();
        else if(comm=="diswf") {
            WiFi.disconnect();
            connectedWifi=false;
            setStatus(WIFIST,false);
            tft.fillScreen(ST77XX_BLACK);
            textoCentrado("WiFi Desconectado",60);
        } else if(comm=="wfstat") {
            wifi_stat(); // Implementa esta función en wifi_ota.cpp
        }
    }

}

void serial_loop(){
    ArduinoOTA.handle();           // mantener OTA
    if(Serial.available()){
        String line = Serial.readStringUntil('\n');
        handleCommand(line);
        lastMsgMillis = millis();
    }
}
