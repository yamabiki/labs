#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>

// WiFi Credentials
const char* ssid     = "ш";
const char* password = "12345678";

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600 * 3, 60000);

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    lcd.setCursor(0, 0);
    lcd.print("~ Connecting...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("~ WiFi Connected");
    delay(1000);
    lcd.clear();
    
    timeClient.begin();
    configTime(3600 * 3, 0, "europe.pool.ntp.org"); 
}

void loop() {
    timeClient.update();
    
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime(&epochTime);

    int rawHour = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    int seconds = timeClient.getSeconds();
    
    int year = ptm->tm_year + 1900; 
    int month = ptm->tm_mon + 1;    
    int date = ptm->tm_mday;       
    int day = ptm->tm_wday;      

    String ampm = (rawHour >= 12) ? "PM" : "AM";
    int hour = rawHour % 12;
    if (hour == 0) hour = 12;

    String weekDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    String currentDay = weekDays[day];

    lcd.setCursor(0, 0);
    lcd.printf("~ %02d:%02d:%02d %s", hour, minutes, seconds, ampm.c_str());
    
    lcd.setCursor(0, 1);
    lcd.printf("~ %02d/%02d/%04d %s", date, month, year, currentDay.c_str());
    
    delay(1000);
}