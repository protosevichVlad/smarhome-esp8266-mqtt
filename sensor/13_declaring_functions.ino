//wi-fi
void connectingToWifi();
void settingModeOn();
void settingModeOff();

//
void indexPage(); // http://10.10.10.1/
void wifiSettingPage(); // http://10.10.10.1/wifi
void mqttSettingPage(); // http://10.10.10.1/mqtt
void locationSettingPage(); // http://10.10.10.1/location
void settingModeOffPage(); // http://10.10.10.1/off
void getAvailableNetworksPage(); // http://10.10.10.1/getAvailableNetworks
void notFoundPage();

//mqtt
void mqttSetup();
void mqttLoop();
void connectingToMqtt();
void mqttSubscribe();
void callback(char* topic, byte* payload, unsigned int length);

// send
void send0();
void send1();
void addToArray(float*, float);
void sendArray(float*, String);

// EEPROM
void setupEEPROM();
String readFrom(int address);
void writeTo(int address, String data);
