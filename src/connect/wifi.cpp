#include "wifi.hpp"

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT function
void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...\n");
        String clientID = "f74f6c80-07c3-11f0-a887-6d1a184f2bb5";
        if (client.connect(clientID.c_str(), TOKEN_GATEWAY, ""))
        {
            Serial.println("MQTT connect success.");
            if (client.subscribe(MQTT_SUBCRIBE_TOPIC)) Serial.println("Subcribe success.");
            else Serial.println("Fail.");
        }
        else{
            Serial.print("MQTT connection failed, rc=");
            Serial.print(String(client.state()));
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }   
}

void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.printf("[MQTT Message Arrive]: %s\n", topic);
    // v1/devices/me/rpc/request/15
    String str_topic(topic);
    // get last idx
    int idx = str_topic.lastIndexOf('/');
    String requestID = str_topic.substring(idx + 1);
    String msg;
    for (unsigned int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }
    Serial.print("[MQTT] Payload: ");
    Serial.println(msg);
    StaticJsonDocument<100> docRequest;
    DeserializationError error = deserializeJson(docRequest, msg);
    if (error)
    {
        Serial.print("[ERROR] deserializeJson() failed: ");
        Serial.println(error.f_str());
        return;
    }
    String method = docRequest["method"].as<String>();
    bool state = docRequest["params"];
    StaticJsonDocument<100> doc;
    if (method == "setState")
    {
        if (state) digitalWrite(A0, HIGH);
        else digitalWrite(A0, LOW);
        doc["method"] = String("setState");
        doc["params"] = state;
    }
    else
    {
        doc["method"] = String("LEDState");
        doc["params"] = (digitalRead(A0)? true : false);
    }

    // Serialize Json to publish (ACK)
    doc["LEDState"] = state;
    String data;
    serializeJson(doc, data);
    String mqtt_topic = String("v1/devices/me/rpc/response/" + requestID);
    publishData(mqtt_topic, data);
    return;
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
    client.setCallback(callback);
    Serial.println("check point");
    
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
        Serial.println(topic);
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
bool subcriptData()
{
    if (client.subscribe(MQTT_TELEMETRY))
    {
        Serial.println("Subcripted success.");
        Serial.println();
        return true;
    }
    else Serial.println("Subcripted Failed.");
    return false;
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
    IPAddress ip = WiFi.localIP();
    Serial.printf("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

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