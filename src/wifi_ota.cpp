#include "wifi_ota.h"
#include "display.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, TIMEZONE_OFFSET, 60000);

bool otaON     = false;
bool connectedWifi   = false;
bool connectedSerial = false;

bool wifi_connect(){
    
    textoCentrado("Conectando a WiFi...", 94);
    WiFi.begin(ssid,password);
    WiFi.setHostname(HostName);
    WiFi.setAutoReconnect(true);
    WiFi.mode(WIFI_STA);

    int retry=0;
    while(WiFi.status()!=WL_CONNECTED && retry<20){
        delay(500); retry++;
    }
    connectedWifi = WiFi.status()==WL_CONNECTED;
    setStatus(WIFIST, connectedWifi);

    // if not connected, show error and reason
    if(!connectedWifi){
        Serial.println("\nError de conexión WiFi");
        tft.fillScreen(ST77XX_RED);
        Serial.println(WiFi.status());
        // Show the reason for the failure
        switch(WiFi.status()){
            case WL_NO_SSID_AVAIL: Serial.println("No SSID available"); break;
            case WL_CONNECT_FAILED: Serial.println("Connection failed"); break;
            case WL_CONNECTION_LOST: Serial.println("Connection lost"); break;
            case WL_DISCONNECTED: Serial.println("Disconnected"); break;
            default: Serial.println("Unknown error"); break;
        }
        return false;
    }

    return connectedWifi;
}

void syncTime(){
    timeClient.begin();
    while(!timeClient.update()){ timeClient.forceUpdate(); }
}

static void onOtaStart(){
    otaON=true;
    textoCentrado("OTA Iniciada",30);
}
static void onOtaEnd(){ ESP.restart(); }
static void onOtaError(ota_error_t){ ESP.restart(); }

void ota_setup(){
    String HostNameOTA = String(HostName) + "OTA";
    ArduinoOTA.setHostname(HostNameOTA.c_str());
    ArduinoOTA.onStart(onOtaStart);
    ArduinoOTA.onEnd(onOtaEnd);
    ArduinoOTA.onError(onOtaError);
    ArduinoOTA.begin();
}

void wifi_stat() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi OK");
        Serial.println("Getting data...");
        delay(100);
        // obtiene la ip, ssid, rssi y mac del modulo wifi, además datos como AutoReconnect, BSSID, Channel, GatewayIP, Hostname, SubnetMask, WiFiMode, WiFiPhyMode
        String ip = WiFi.localIP().toString();
        String ssid = WiFi.SSID();
        String rssi = String(WiFi.RSSI());
        String mac = WiFi.macAddress();
        String autoReconnect = String(WiFi.getAutoReconnect());
        String bssid = WiFi.BSSIDstr();
        String channel = String(WiFi.channel());
        String gatewayIP = WiFi.gatewayIP().toString();
        String hostname = WiFi.getHostname();
        String subnetMask = WiFi.subnetMask().toString();
        String wifiMode = String(WiFi.getMode());
        
        Serial.println("---Start Data---");
        Serial.println("IP: " + ip);
        Serial.println("SSID: " + ssid);
        Serial.println("RSSI: " + rssi);
        Serial.println("MAC: " + mac);
        Serial.println("AutoReconnect: " + autoReconnect);
        Serial.println("BSSID: " + bssid);
        Serial.println("Channel: " + channel);
        Serial.println("Gateway IP: " + gatewayIP);
        Serial.println("Hostname: " + hostname);
        Serial.println("Subnet Mask: " + subnetMask);
        Serial.println("WiFi Mode: " + wifiMode);
        Serial.println("---End Data---");


        
    } else {
        Serial.println("WiFi Not Connected");
    }
}
