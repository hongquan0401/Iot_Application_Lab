#include <Arduino.h>
#include "../include/defines.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// put function declarations here:
int flag = 0;
void ledToggle(void* pvParam) {
  while (true)
  {
    if (flag) {
      digitalWrite(A0, 1);
      Serial.println("ON");
      flag = 0;
    }
    else {
      digitalWrite(A0, 0);
      Serial.println("OFF");
      flag = 1;
    }
    vTaskDelay(pdMS_TO_TICKS(delay_toggle_led));
  }
}

void sensorLight(void* pvParam) {
  while (true)
  {
    uint32_t val = analogRead(A1);
    Serial.println(val);
    vTaskDelay(pdMS_TO_TICKS(delay_read_adc));
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);
  xTaskCreate(sensorLight, "Read Light Sensor", 4096, nullptr, 1, nullptr);
  xTaskCreate(ledToggle, "Blinky Led", 4096, nullptr, 1, nullptr);
  vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:
}