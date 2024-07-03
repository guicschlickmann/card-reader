#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "KIKI";
const char* password = "36582706";

constexpr uint8_t RST_PIN = D1;
constexpr uint8_t SS_PIN = D2;
#define relay_pin D3

MFRC522 rfid(SS_PIN, RST_PIN);

String tag;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(relay_pin , OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    if (tag == "115177248") {
      Serial.println("Acesso garantido!");
      digitalWrite(relay_pin, LOW);
      delay(5000);
      digitalWrite(relay_pin, HIGH);
    } else {
      Serial.println("Acesso negado!");
    }
    
    // Enviar dados para o servidor
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String serverPath = "http://192.168.0.13:3000/api/rfid";
      http.begin(client, serverPath); // Use WiFiClient object here
      http.addHeader("Content-Type", "application/json");
      String jsonData = "{\"tag\": \"" + tag + "\"}";
      int httpResponseCode = http.POST(jsonData);
      
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }

    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
