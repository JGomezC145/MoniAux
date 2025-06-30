#include "display.h"
#include "images2.h"
#include "wifi_ota.h"
#include "globals.h"
#include <SPI.h>

// === Variables definidas aquí ===
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// get temperature from the internal core sensor

void getTemperature() {
    float temp = (float)temperatureRead(); // Convertir a grados Celsius
    textoCentrado("Temp Chip: " + String(temp, 1) + " C", 70, ST77XX_RED, 1);
}

// --------------------
// helpers
// --------------------
void updateLEDColor(int r,int g,int b, int brightness) { 
	r = (r * brightness) / 255; // Ajusta el brillo
	g = (g * brightness) / 255; // same
	b = (b * brightness) / 255; // same

	neopixelWrite(RGB_PIN,r,g,b); 

}

void textoCentrado(const String &txt,int y,uint16_t color,int sz){
    int x=(128-(txt.length()*(6*sz)))/2;
    tft.setTextSize(sz);
    tft.setTextColor(color,ST7735_BLACK);
    tft.setCursor(x,y);
    tft.print(txt);
}

void escribir(const String &txt,int x,int y,uint16_t color,uint16_t bg,int sz){
    tft.setTextSize(sz);
    tft.setTextColor(color,bg);
    tft.setCursor(x,y);
    tft.print(txt);
}

void setStatus(StatusType c,bool ok){
    if (c == 1) {
		if (ok) {
			tft.drawRGBBitmap(113, 0, sta_wifiok, 15, 15);
			//Serial.println("WiFi update status: OK");
		}
		else {
			tft.drawRGBBitmap(113, 0, sta_wifibad, 15, 15);
			//Serial.println("WiFi update status: BAD");
		}
	}
	else if (c == 2) {
		if (ok) {
			tft.drawRGBBitmap(95, 0, sta_serialok, 15, 15);
			//Serial.println("Serial update status: OK");
		}
		else {
			tft.drawRGBBitmap(95, 0, sta_serialbad, 15, 15);
			//Serial.println("Serial update status: BAD");
		}
	}
}

static void TaskDisplay(void *pvParameters) { // core 1
    (void)pvParameters; // Evita warning de parámetro no usado
	static unsigned long lastNTPUpdate = 0;
	static unsigned long lastStatusUpdate = 0;

	while (1) {
		if (!otaON) {
			if (millis() - lastNTPUpdate > 1000) { // Actualiza cada 60s
				timeClient.update();
                //getTemperature();
				// hora actual en formato HH:MM
				hora = timeClient.getHours();
				minutos = timeClient.getMinutes();
				horaActualNew = (hora < 10 ? "0" + String(hora) : String(hora)) + ":" +
					(minutos < 10 ? "0" + String(minutos) : String(minutos));
				lastNTPUpdate = millis();

				// Actualiza la hora solo si ha cambiado
				if (horaActualNew != horaActual) {
					horaActual = horaActualNew;
					// Actualiza solo la hora sin borrar toda la pantalla
					tft.fillRect(10, 16, 108, 20, ST77XX_BLACK);
					textoCentrado(horaActual, 23, ST77XX_WHITE, 3);
				}
				
			}

			// cada 5 segundos verifica si hay cambios en el estado de WiFi o Serial
			if (millis() - lastStatusUpdate > 5000) {
				// Actualiza el estado de WiFi y Serial
				if (connectedWifi) {
					setStatus(WIFIST, true); // WiFi OK
					String ip = WiFi.localIP().toString(); // Si WiFi está conectado, obtiene la IP
					textoCentrado(ip, 70, ST77XX_WHITE, 1); // Muestra la IP en la pantalla
				} else {
					setStatus(WIFIST, false); // WiFi BAD
					textoCentrado("              ", 70, ST77XX_WHITE, 1); // elimina la IP de la pantalla
				}
				if (connectedSerial) {
					setStatus(SERIALST, true); // Serial OK
				} else {
					setStatus(SERIALST, false); // Serial BAD
				}
				lastStatusUpdate = millis();
			}
			
		}

		delay(1000);
	}
    
}

void updateTime() {
	static unsigned long lastNTPUpdate = 0;
	if (!otaON) {
		if (millis() - lastNTPUpdate > 5000) { // Actualiza cada 60s
			timeClient.update();
			getTemperature();
			// hora actual en formato HH:MM
			hora = timeClient.getHours();
			minutos = timeClient.getMinutes();
			horaActualNew = (hora < 10 ? "0" + String(hora) : String(hora)) + ":" +
				(minutos < 10 ? "0" + String(minutos) : String(minutos));
			lastNTPUpdate = millis();

			// Actualiza la hora solo si ha cambiado
			if (horaActualNew != horaActual) {
				horaActual = horaActualNew;
				// Actualiza solo la hora sin borrar toda la pantalla
				tft.fillRect(10, 16, 108, 20, ST77XX_BLACK);
				textoCentrado(horaActual, 23, ST77XX_WHITE, 3);
			}
		}
	}

}

// Inicializa HW y crea la tarea en core 1
void display_init(){
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
    tft.initR(INITR_GREENTAB);
    tft.setSPISpeed(SPI_SPEED);
    tft.setRotation(0);
    tft.fillScreen(ST77XX_BLACK);
    // splash
    tft.drawRGBBitmap(0,0,initImage,TFT_WIDTH,TFT_HEIGHT);
    textoCentrado("Inicializando...", 85);
}

void startDisplayTask(){
    xTaskCreatePinnedToCore(TaskDisplay,
        "TaskDisplay",
        10000,
        NULL,
        1,
        NULL,
        1);
}
