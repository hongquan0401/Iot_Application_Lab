#include "wifi.hpp"

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT function
void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...");
        String clientID = "f74f6c80-07c3-11f0-a887-6d1a184f2bb5";
        if (client.connect(clientID.c_str(), TOKEN_GATEWAY, ""))
        {
            Serial.println("MQTT reconnect success.");
        }
        else{
            Serial.print("MQTT connection failed, rc=");
            Serial.print(String(client.state()));
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }   
}
void taskMQTT(void* pvParams)
{   
    // Check wifi connection
    while (WiFi.status() != WL_CONNECTED )
    {
        vTaskDelay(pdMS_TO_TICKS(delay_connect));
    }
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setKeepAlive(30);
    Serial.println("MQTT connetion success");
    while (true)
  {
    if (!client.connected())
    {
      reconnectMQTT();
    }
    client.loop();
    vTaskDelay(delay_mqtt / portTICK_PERIOD_MS);
  }
}
bool publishData(const String &feedName, String message)
{
    String topic = feedName;
    // Serial.print("Publishing to topic: ");
    // Serial.print(feedName);
    // Serial.print("Status: ");

    if(client.publish(topic.c_str(), message.c_str(), 1))
    {
        Serial.print("Publish success: ");
        Serial.println(message);
        return true;
    }
    else
    {
        Serial.println("Publish fail!");
        return false;
    }
}

// Connect Wifi function
void taskWifi(void* pvParams)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    uint8_t wifiRetry = 0;
    while (WiFi.status() != WL_CONNECTED && wifiRetry < 20)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        wifiRetry++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    }

    Serial.println("[INFO] WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(String(WiFi.localIP()));

    while (true)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
        Serial.print("[WARN] Lost WiFi! Attempting to reconnect...");
        uint8_t retryCount = 0;

        while (WiFi.status() != WL_CONNECTED && retryCount < 20)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            retryCount++;
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("[INFO] WiFi Reconnected!");
        }
        else
        {
            Serial.print("[ERROR] WiFi reconnect failed. Restarting in 5s...");
            vTaskDelay(pdMS_TO_TICKS(delay_connect_wifi));
            ESP.restart();
        }
        }
        vTaskDelay(pdMS_TO_TICKS(delay_connect_wifi));
    }
}