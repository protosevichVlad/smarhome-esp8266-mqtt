#include "DHTesp.h"
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>   
#include <EEPROM.h>

#define DTH11_PIN 12 //D6
#define LED_PIN 0 //D3
#define BUTTON_PIN 2 //D4
#define MICRO_PIN 13 //D7
#define LED_SETTING_PIN 16 //D0
#define LED_FAILED_PIN 15 //D8
#define SIZE_ARRAY 20
#define EEPROM_SIZE 512
#define SIZE_STRING_EEPROM 32

#define SSID_ADDRESS 0
#define PASS_ADDRESS 32

#define MQTT_SERVER_ADDRESS 64
#define MQTT_PORT_ADDRESS 96
#define MQTT_USER_ADDRESS 128
#define MQTT_PASS_ADDRESS 160

#define LOCATION_ADDRESS 192
