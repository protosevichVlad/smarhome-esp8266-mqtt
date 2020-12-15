#include "DHTesp.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>   

#define DTH11_PIN 12 //D6
#define LED_PIN 0 //D3
#define BUZER_PIN 2 //D4
#define MICRO_PIN 13 //D7
#define SIZE_ARRAY 20

const char *ssid =  "........";  // Имя вайфай точки доступа
const char *pass =  "........"; // Пароль от точки доступа

const char *mqtt_server = "........"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "........"; // Логиn от сервер
const char *mqtt_pass = "........"; // Пароль от сервера

const char *mqtt_client_id = "esp_sensor_1"; // client id
const String location = "home/Room1/";
const String location1 = "home/Room2/";

const long utcOffsetInSeconds = 3 * 3600;

long Time = millis();
long Time1 = millis();


float array_temperature[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float array_pressure[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float array_temperature1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float array_humidity1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void TempSend();
void TempSend1();
void callback(char* topic, byte* payload, unsigned int length);
void playMusic();
void addToArray(float*, float);
void sendArray(float*, String);

DHTesp dht;
WiFiClient wclient;
WiFiUDP ntpUDP;      
PubSubClient client(wclient); 
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
Adafruit_BMP280 bme;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  if (!bme.begin(0x76)) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");                                         
  }

  timeClient.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZER_PIN, OUTPUT);
  pinMode(MICRO_PIN, INPUT);

  dht.setup(DTH11_PIN, DHTesp::DHT11);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) 
  {
    if (!client.connected()) 
    {
      Serial.println("Connecting to MQTT server");
      if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass, "status", 2, true, "0"))
      {
        Serial.println("Connected to MQTT server");
        client.subscribe("test/led");
        client.subscribe("home/music");
      } 
      else 
      {
        Serial.println("Could not connect to MQTT server");   
      }
    }

    if (client.connected())
    {
      client.loop();
      TempSend();
      TempSend1();
     
      int micro = digitalRead(MICRO_PIN);
      if (micro == HIGH)
      {
        timeClient.update();
        client.publish("home/micro", timeClient.getFormattedTime().c_str());
      }
    } 
  }
}

void TempSend(){
  if (millis() - Time > 30000)
  {  
    float pressure = bme.readPressure();
    float temperature = bme.readTemperature();
    
    client.publish((location + String("temperature")).c_str(), String(temperature).c_str()); // отправляем в топик для термодатчика значение температуры
    Serial.println(temperature);
    client.publish((location + String("pressure")).c_str(), String(pressure).c_str());
    Serial.println(pressure);
    
    addToArray(array_temperature, temperature);
    sendArray(array_temperature, (location + String("arrayTemperature")).c_str());
    
    addToArray(array_pressure, pressure);
    sendArray(array_pressure, (location + String("arrayPressure")).c_str());
    Time = millis();
  }  
}

void TempSend1(){
  if (millis() - Time1 > 30000)
  {  
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    
    client.publish((location1 + String("temperature")).c_str(), String(temperature).c_str()); // отправляем в топик для термодатчика значение температуры
    Serial.println(temperature);
    client.publish((location1 + String("humidity")).c_str(), String(humidity).c_str());
    Serial.println(humidity);
    
    addToArray(array_temperature1, temperature);
    sendArray(array_temperature1, (location1 + String("arrayTemperature")).c_str());
    
    addToArray(array_humidity1, humidity);
    sendArray(array_humidity1, (location1 + String("arrayHumidity")).c_str());
    Time1 = millis();
  }  
}

void addToArray(float* arr, float value)
{
  for (int i = 1; i < SIZE_ARRAY; i++) 
  {
    arr[i - 1] = arr[i];
  }
  arr[SIZE_ARRAY - 1] = value;
}

void sendArray(float* arr, String top) {
  String array_str = "[";
  for (int i = 0; i < SIZE_ARRAY - 1; i++) 
  {
    array_str += String(arr[i]) + ", ";
  }
  array_str += String(arr[SIZE_ARRAY - 1]) + "]";
  Serial.print(top); 
  Serial.print(array_str);
  Serial.println();
  
  client.publish(top.c_str(), array_str.c_str());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  if(String(topic) == String("test/led")) 
  {
    int onInt = String((char*)payload).toInt();
    Serial.print("onInt");
    Serial.println(onInt);
    if (onInt == 1) 
    {
       digitalWrite(LED_PIN, HIGH);
    } 
    else 
    {
      digitalWrite(LED_PIN, LOW);
    }
  }

  if(String(topic) == String("home/music"))
  {
    int music = String((char*)payload).toInt();
    Serial.print("music");
    Serial.println(music);
    if (music == 1) 
    {
      playMusic();   
    } 
  }
  Serial.println();
}

void playMusic()
{
  tone(BUZER_PIN, 440, 250);
  delay(250);
  tone(BUZER_PIN, 165, 250);
  delay(250);
  tone(BUZER_PIN, 440, 250);
  delay(250);
  delay(250);
  
  tone(BUZER_PIN, 440, 250);
  delay(250);
  tone(BUZER_PIN, 165, 250);
  delay(250);
  tone(BUZER_PIN, 440, 250);
  delay(250);
  delay(250);
  
  tone(BUZER_PIN, 330, 125);
  delay(125);
  tone(BUZER_PIN, 294, 125);
  delay(125);
  tone(BUZER_PIN, 262, 125);
  delay(124);
  tone(BUZER_PIN, 494, 125);
  delay(125);
  tone(BUZER_PIN, 440, 125);
  delay(125);
  tone(BUZER_PIN, 494, 125);
  delay(125);
  tone(BUZER_PIN, 262, 125);
  delay(125);
  tone(BUZER_PIN, 294, 125);
  delay(125);
  
  tone(BUZER_PIN, 330, 250);
  delay(250);
  tone(BUZER_PIN, 165, 250);
  delay(250);
  tone(BUZER_PIN, 440, 250);
  delay(250);
  delay(250);
  delay(3000);
}
