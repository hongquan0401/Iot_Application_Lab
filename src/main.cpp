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
  delay(2000);

  // Add task
  xTaskCreate(taskWifi, "Wifi", 4096, nullptr, 0, nullptr);
  xTaskCreate(taskMQTT, "MQTT", 4096, nullptr, 1, nullptr);
  xTaskCreate(readDHT20, "dddd", 4096, dht, 0, nullptr);
  // xTaskCreate(readDHT20, "Read DHT20", 1024*4, nullptr, 1, nullptr);
  // xTaskCreate(sensorLight, "Read Light Sensor", 4096, nullptr, 1, nullptr);
  xTaskCreate(ledToggle, "Blinky Led", 4096, nullptr, 2, nullptr);
  }

  void loop()
  {
    // do nothing
  }
