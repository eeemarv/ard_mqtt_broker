#include <Arduino.h>
#include <.env.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <TinyMqtt.h>
#include <W5500lwIP.h>
#include <ENC28J60lwIP.h>

#ifndef MQTT_PORT
  #define MQTT_PORT 1883
#endif
#ifndef ETH_RST_PIN 
  #define ETH_RST_PIN 16
#endif
#ifndef ETH_CS_PIN
  #define ETH_CS_PIN 5
#endif
#ifndef SPI_FREQ
  #define SPI_FREQ 8000000
#endif
#ifndef W5500
  #define ENC29J60
#endif
#define SERIAL_BAUD 115200

byte mac[] = {0x00, 0xAA, 0xBB, 0xCD, 0xDE, 0x03};
const IPAddress ip(192, 168, 0, 20);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress mask(255, 255, 255, 0);

#ifdef W5500
  Wiznet5500lwIP eth(ETH_CS_PIN);
#else
  ENC28J60lwIP eth(ETH_CS_PIN);
#endif

MqttBroker mqttBroker(MQTT_PORT);

void setup() {
  delay(200);

  Serial.begin(SERIAL_BAUD);

  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("ESP8266 Core version: ");
  Serial.println(ESP.getCoreVersion());
  Serial.println("ESP8266 CPU clock freq:");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" Mhz");

  Serial.println("Turning WiFi Off");
  WiFi.mode(WIFI_OFF);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(SPI_FREQ);

  Serial.println("Reset Wiz Ethernet...");
  pinMode(ETH_RST_PIN, OUTPUT);
  digitalWrite(ETH_RST_PIN, HIGH);
  delay(500);
  digitalWrite(ETH_RST_PIN, LOW);
  delay(50);
  digitalWrite(ETH_RST_PIN, HIGH);
  delay(500);

  eth.setDefault();
  eth.config(ip, gateway, mask, gateway);

  if (!eth.begin()) {
    Serial.println("ethernet hardware not found ... sleeping");
    while(1){
      delay(1000);
    };
  }

  Serial.print("connecting ethernet");
  while (!eth.connected()) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("ethernet ip address: ");
  Serial.println(eth.localIP());
  Serial.print("ethernet subnetMask: ");
  Serial.println(eth.subnetMask());
  Serial.print("ethernet gateway: ");
  Serial.println(eth.gatewayIP());
  mqttBroker.begin();
}

void loop() {
  mqttBroker.loop();
}
