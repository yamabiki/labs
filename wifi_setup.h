#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <ESP8266WiFi.h>

const char* ssid = "ш";     
const char* password = "12345678";

void setupWiFi() {
    Serial.print("Підключення до WiFi");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi підключено!");
    Serial.print("IP адреса: ");
    Serial.println(WiFi.localIP());
}

#endif // WIFI_SETUP_H
