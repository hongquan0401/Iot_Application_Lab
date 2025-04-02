#include "../include/global.hpp"
#include "tasks/task.hpp"
#include "connect/wifi.hpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(A0, OUTPUT); // For LED
  // pinMode(A1, INPUT); // For DHT11 ...
  Wire.begin(21, 22); // For I2C DHT20
  DHT20* dht = new DHT20(&Wire);
  if (!dht->begin()) {
    Serial.println("Failed to initialize DHT20 sensor!");
    while (1);
  }
  Serial.println("DHT20 initialized successfully.");
  xTaskCreate(taskWifi, "Wifi", 4096, nullptr, 0, nullptr);
  xTaskCreate(taskMQTT, "MQTT", 4096, nullptr, 0, nullptr);
  // StaticJsonDocument<300> jsonDoc;
  // jsonDoc["LEDState"] = String(digitalRead(A0));
  // String data_pub;
  // serializeJson(jsonDoc, data_pub);
  // publishData(MQTT_TELEMETRY, data_pub);
  // subcriptData();
  delay(2000);

  // Add task
  // xTaskCreate(ledUpdate, "led update", 2048, nullptr, 0, nullptr);
  xTaskCreate(readDHT20, "dddd", 4096, dht, 0, nullptr);
  // xTaskCreate(ledToggle, "Blinky Led", 4096, nullptr, 2, nullptr);
  // xTaskCreate(ledControl, "led", 2048, nullptr, 0, nullptr);
  }

  void loop()
  {
    // do nothing
  }
