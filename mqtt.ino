#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <time.h>
#include <PubSubClient.h> 

const char* ssid     = "ш";
const char* password = "12345678";

const char* mqtt_server = "192.168.183.86"; 
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP8266_Clock_MQTT";       
const char* mqtt_time_topic = "clock/set_time";          

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient espClient;
PubSubClient client(espClient);

String weekDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    if (String(topic) == mqtt_time_topic) {
        unsigned long newEpochTime = message.toInt(); 
        if (newEpochTime > 0) {
            configTime(0, 0, nullptr, nullptr);  // Скинути NTP
            timeval tv;
            tv.tv_sec = newEpochTime;
            tv.tv_usec = 0;
            settimeofday(&tv, nullptr); 

            Serial.print("Time updated via MQTT to: ");
            Serial.println(newEpochTime);
            lcd.setCursor(0, 1);
            lcd.print("Time set via MQTT!");
            delay(1500); 
            lcd.clear(); 
        } else {
            Serial.println("Invalid time received via MQTT.");
            lcd.setCursor(0, 1);
            lcd.print("MQTT time error!");
            delay(1500);
            lcd.clear();
        }
    }
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqtt_client_id)) {
            Serial.println("connected");
            client.subscribe(mqtt_time_topic); 
            Serial.print("Subscribed to: ");
            Serial.println(mqtt_time_topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            delay(5000);
        }
    }
}

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
    
    configTime(10800, 0, "europe.pool.ntp.org");  // Налаштування NTP UTC+3

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback); 
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop(); 
    
    time_t now = time(nullptr);
    struct tm *ptm = localtime(&now);

    int hour = ptm->tm_hour;
    int minute = ptm->tm_min;
    int second = ptm->tm_sec;

    String ampm = (hour >= 12) ? "PM" : "AM";
    int hour12 = hour % 12;
    if (hour12 == 0) hour12 = 12;

    lcd.setCursor(0, 0);
    lcd.printf("~ %02d:%02d:%02d %s", hour12, minute, second, ampm.c_str());

    lcd.setCursor(0, 1);
    lcd.printf("~ %02d/%02d/%04d %s", ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900, weekDays[ptm->tm_wday].c_str());

    delay(1000);
}
