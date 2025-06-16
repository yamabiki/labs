#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "wifi_setup.h"
#include "web_server.h"

#define LED1 14  // D5v     
#define LED2 0   // D3
#define LED3 2   // D4
#define BUTTON 4 // D2
#define COMMAND_J 0b01001010

bool reverseOrder = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

void setup() {
    Serial.begin(115200); 

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);

    setupWiFi();     
    setupWebServer();
}

void loop() {
    server.handleClient();

    // Кнопка
    bool buttonState = digitalRead(BUTTON);
    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime > debounceDelay)) {
        Serial.write(COMMAND_J); // Надіслати сигнал іншому ESP
        lastDebounceTime = millis();
    }
    lastButtonState = buttonState;

    // UART прийом
    if (Serial.available()) {
        char c = Serial.read();
        if (c == COMMAND_J) {
            reverseOrder = !reverseOrder;
            Serial.println("UART: зміна напрямку по J");
        }
    }

    // Миготіння
    if (!reverseOrder) {
        blinkLeds(LED1, LED2, LED3);
    } else {
        blinkLeds(LED3, LED2, LED1);
    }
}

void blinkLeds(int led1, int led2, int led3) {
    digitalWrite(led1, HIGH);
    delay(300);
    digitalWrite(led1, LOW);

    digitalWrite(led2, HIGH);
    delay(300);
    digitalWrite(led2, LOW);

    digitalWrite(led3, HIGH);
    delay(300);
    digitalWrite(led3, LOW);
}
