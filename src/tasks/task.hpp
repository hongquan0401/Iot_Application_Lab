#ifdef ESP32

// basic function of board WEMOS D1
extern int flag;

void ledUpdate(void* pvParams);
void ledToggle(void *pvParam);
void sensorLight(void *pvParam);
void readDHT20(void *pvParam);

#endif