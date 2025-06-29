#include "serial_comm.h"
#include "wifi_ota.h"
#include "display.h"

static unsigned long lastMsgMillis=0;

void scanNetworks() {
	delay(1000);
    
    Serial.println("Escaneando redes WiFi...");
    
    int numRedes = WiFi.scanNetworks();
    
    if (numRedes == 0) {
        Serial.println("No se encontraron redes WiFi.");
    } else {
        Serial.println("Redes WiFi encontradas:");
        Serial.println("------------------------------------------------------");
        Serial.println(" No. |  SSID                 | Señal (dBm) | Tipo");
        Serial.println("------------------------------------------------------");
        
        for (int i = 0; i < numRedes; ++i) {
            Serial.printf(" %2d  |  %-20s |  %4d dBm  |  %s\n",
                          i + 1,
                          WiFi.SSID(i).c_str(),
                          WiFi.RSSI(i),
                          WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Abierta" : "Cifrada");
        }
        
        Serial.println("------------------------------------------------------");
    }
    
    WiFi.scanDelete();
}

void serial_setup(){
    textoCentrado("Conectando a Serial...", 103);
    Serial.begin(SerialBaudRate);
    Serial.setTimeout(200);
    delay(100);
    Serial.println("hello");
    //textoCentrado("Esperando respuesta...",60);
    //while(!Serial.available()) delay(100);
    /*if(Serial.readStringUntil('\n')=="world"){
        connectedSerial=true;
        setStatus(SERIALST,true);
    }*/
    delay(100);
    textoCentrado("Listo!",112);
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
        } else if (comm.startsWith("conwf")) {
            // cuando se envie el comando "conwf" se conecta a la red suministrada
            // formato: comm:conwf SSID;PASS
            String mensaje = msg;
            String ssid = mensaje.substring(11, mensaje.indexOf(';'));
            String pass = mensaje.substring(mensaje.indexOf(';') + 1);
            WiFi.begin(ssid.c_str(), pass.c_str());
            int retry = 0;
            Serial.println("Conectando a " + ssid);
            tft.fillScreen(ST77XX_BLACK);
            textoCentrado("Conectando a WiFi...", 20);
            escribir(ssid, 0, 35);
            escribir("Password: " + pass, 0, 45);
            while (WiFi.status() != WL_CONNECTED && retry < 20) {
                delay(500);
                Serial.print(".");
                retry++;
            }
            
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\nWiFi Conectado!");
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                connectedWifi = true;
                setStatus(WIFIST, true);
                tft.fillScreen(ST77XX_BLACK);
                textoCentrado("WiFi OK!", 94);
                textoCentrado("Conectado a " + String(ssid), 104);
                textoCentrado("IP: " + WiFi.localIP().toString(), 114);
                textoCentrado("Obteniendo hora...", 94);
                timeClient.begin();
                while (!timeClient.update()) {
                    timeClient.forceUpdate();
                    delay(500);
                }
                hora = timeClient.getHours();
                minutos = timeClient.getMinutes() - 1;
                horaActualNew = (hora < 10 ? "0" + String(hora) : String(hora)) + ":" +
                    (minutos < 10 ? "0" + String(minutos) : String(minutos));
                delay(2000);
            }
            else {
                Serial.println("\nError de conexión WiFi");
                tft.fillScreen(ST77XX_RED);
                textoCentrado("WiFi Error!", 30);
                Serial.println(WiFi.status());
                setStatus(WIFIST, false);
                return;
            }

        } else if(comm=="wfstat") {
            wifi_stat(); // Implementa esta función en wifi_ota.cpp
        } else if (comm == "restart") {
				updateLEDColor(0, 255, 0); // Verde
				otaON = true;
				tft.fillScreen(ST77XX_BLACK);
				textoCentrado("Reiniciando...", 60, ST77XX_RED, 1);
				delay(2000);
				updateLEDColor(0, 0, 0); // Apagar LED
				ESP.restart();
		} else if (comm=="clearScreen") {
        tft.fillScreen(ST77XX_BLACK);
        } else if (comm.startsWith("setRGB:")) {
            // Actualizar color del LED RGB formato de entrada: "setRGB:R,G,B,bright" brigh es opcional

            String rgbValues = comm.substring(7);
            int comma1 = rgbValues.indexOf(',');
            int comma2 = rgbValues.indexOf(',', comma1 + 1);
            int comma3 = rgbValues.indexOf(',', comma2 + 1);
            if (comma1 != -1 && comma2 != -1) {
                int r = rgbValues.substring(0, comma1).toInt();
                int g = rgbValues.substring(comma1 + 1, comma2).toInt();
                int b = rgbValues.substring(comma2 + 1, comma3).toInt();
                int bright = (comma3 != -1) ? rgbValues.substring(comma3 + 1).toInt() : 255; // Valor por defecto de brillo
                updateLEDColor(r, g, b, bright);
                Serial.printf("Color LED actualizado: R=%d, G=%d, B=%d, Brightness=%d\n", r, g, b, bright);
            } else {
                Serial.println("Formato de comando setRGB inválido. Use: setRGB:R,G,B[,bright]");
            }
        } else if (comm == "forceUpdateTime") {
            // Forzar actualización de la hora
            timeClient.forceUpdate();
            hora = timeClient.getHours();
            minutos = timeClient.getMinutes() - 1;
            horaActualNew = (hora < 10 ? "0" + String(hora) : String(hora)) + ":" +
                            (minutos < 10 ? "0" + String(minutos) : String(minutos));
            Serial.println("Hora actualizada: " + horaActualNew);
            // Actualiza solo la hora sin borrar toda la pantalla
            if (horaActualNew != horaActual) {
                horaActual = horaActualNew;
                tft.fillRect(10, 16, 108, 20, ST77XX_BLACK);
                textoCentrado(horaActual, 23, ST77XX_WHITE, 3);
            }
            
        } else {
            Serial.println("Comando desconocido: " + comm);
        }
    } else if(msg.startsWith("settime:")) {
        String timeStr = msg.substring(8);
        updateTimeClient(timeStr); // Actualiza la hora en timeClient
    } else {
        Serial.println("Comando no reconocido: " + msg);
    } 

}

void serial_loop(){
    if(Serial.available()){
        String line = Serial.readStringUntil('\n');
        handleCommand(line);
        lastMsgMillis = millis();
    }
    
    
}

void updateTimeClient(const String &timeStr) {
    Serial.println("====StartDebug: updateTimeClient====");
    Serial.println("Str recibido: " + timeStr);
    Serial.println("Str Length: " + String(timeStr.length()));
    Serial.println("Str pos 2: " + String(timeStr[2]));
    bool isValidFormat = (timeStr.length() == 5 && timeStr[2] == ':');
    Serial.println("Formato válido: " + String(isValidFormat ? "Sí" : "No"));
    Serial.println("==============EndDebug==============");

    if (timeStr.length() == 5 && timeStr[2] == ':') { // formato HH:MM
        hora = timeStr.substring(0, 2).toInt();
        minutos = timeStr.substring(3, 5).toInt();
        horaActualNew = (hora < 10 ? "0" + String(hora) : String(hora)) + ":" +
                        (minutos < 10 ? "0" + String(minutos) : String(minutos));
        Serial.println("Hora actualizada: " + horaActualNew);

        // Actualiza la hora solo si ha cambiado
				if (horaActualNew != horaActual) {
					horaActual = horaActualNew;
					// Actualiza solo la hora sin borrar toda la pantalla
					tft.fillRect(10, 16, 108, 20, ST77XX_BLACK);
					textoCentrado(horaActual, 23, ST77XX_WHITE, 3);
				}

    } else {
        Serial.println("Formato de hora inválido. Use HH:MM.");
    }
}

