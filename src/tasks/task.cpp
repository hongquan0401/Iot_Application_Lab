#include "../../include/global.hpp"
#include "../connect/wifi.hpp"

int flag = 0;
void ledToggle(void *pvParam)
{
  while (true)
  {
    if (flag)
    {
      digitalWrite(A0, 1);
    //   Serial.println("ON");
      flag = 0;
    }
    else
    {
      digitalWrite(A0, 0);
    //   Serial.println("OFF");
      flag = 1;
    }
    vTaskDelay(pdMS_TO_TICKS(delay_toggle_led));
  }
}

void sensorLight(void *pvParam) {
  while (true)
  {
    uint32_t val = analogRead(A1);
    Serial.println(val);
    vTaskDelay(pdMS_TO_TICKS(delay_read_adc));
  }
}

void readDHT20(void *pvParam)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  DHT20* dht = (DHT20*)pvParam;
  uint8_t count = 0;
  while (true)
  {
      // READ DATA
      int status = dht->read();
      if (count % 5 == 0)
      {
        count = 0;
        Serial.print('\n');
        Serial.println("Type\tHumidity (%)\tTemp (°C)\tStatus");
      }
      count++;
// 
      // Print to Serial
      Serial.print("DHT20 \t");
      //  DISPLAY DATA, sensor has only one decimal.
      Serial.print(dht->getHumidity(), 1);
      String a(dht->getHumidity(), 1);
      Serial.print("\t\t");
      Serial.print(dht->getTemperature(), 1);
      Serial.print("\t\t");
      switch (status)
      {
        case DHT20_OK:
          Serial.print("OK");
          break;
        case DHT20_ERROR_CHECKSUM:
          Serial.print("Checksum error");
          break;
        case DHT20_ERROR_CONNECT:
          Serial.print("Connect error");
          break;
        case DHT20_MISSING_BYTES:
          Serial.print("Missing bytes");
          break;
        case DHT20_ERROR_BYTES_ALL_ZERO:
          Serial.print("All bytes read zero");
          break;
        case DHT20_ERROR_READ_TIMEOUT:
          Serial.print("Read time out");
          break;
        case DHT20_ERROR_LASTREAD:
          Serial.print("Error read too fast");
          break;
        default:
          Serial.print("Unknown error");
          break;
      }
      Serial.println();
    // Seriallize data 
    StaticJsonDocument<300> jsonDoc;
    jsonDoc["humidity"] = String(dht->getHumidity(), 2);
    jsonDoc["temperature"] = String(dht->getTemperature(), 2);
    String data_pub;
    serializeJson(jsonDoc, data_pub);
    
    publishData(MQTT_TELEMETRY, data_pub);

    vTaskDelay(pdMS_TO_TICKS(delay_DHT20_read));
  }
}