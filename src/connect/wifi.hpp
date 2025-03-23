#include <global.hpp>

extern WiFiClient espClient;
extern PubSubClient client;

// MQTT function
void reconnectMQTT();
void taskMQTT(void* pvParams);
bool publishData(const String &feedName, String message);

// Connect Wifi function
void taskWifi(void* pvParams);