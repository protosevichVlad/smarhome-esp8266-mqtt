#include <Wire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h> 

#define BUTTON_PIN 13 // D7
#define ARRAY_SIZE 5

const char *ssid =  "ZTE_a791e4";  // Имя вайфай точки доступа
const char *pass =  "Protosevich_Famili"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.6"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "vlad"; // Логиn от сервер
const char *mqtt_pass = "2358"; // Пароль от сервера

const char *mqtt_client_id = "esp_dashboard"; // client id

String* rooms = new String[ARRAY_SIZE];

String* temperatureData = new String[ARRAY_SIZE];
String* humidityData = new String[ARRAY_SIZE];
String* pressureData = new String[ARRAY_SIZE];

const long utcOffsetInSeconds = 3 * 3600;
byte current_room = 0;
String string_with_data = "";

byte nextRoom();
void updateScreen();
String getSensor(String topic);
short getIndexRoom(String room);
String getRoomFromTopic(String topic);
void callback(char* topic, byte* payload, unsigned int length);

unsigned long Time;
unsigned long time_button;
unsigned long time_shift;
short shift_index = 0;

bool display_time = true;

WiFiClient wclient;
WiFiUDP ntpUDP;      
PubSubClient client(wclient); 
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
LiquidCrystal_I2C lcd ( 0x27, 16 , 2 ) ;

void setup()
{
  lcd.init();
  lcd.backlight();  
  Serial.begin(115200);
  Serial.println();
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  pinMode(BUTTON_PIN, INPUT);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  timeClient.begin();

  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    rooms[i] = "";
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server");
      if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass, "status", 2, true, "0")) {
        Serial.println("Connected to MQTT server");
        client.subscribe("home/+/temperature");
        client.subscribe("home/+/humidity");
        client.subscribe("home/+/pressure");
      } 
      else
      {
        Serial.println("Could not connect to MQTT server");   
      }
    }

    if (client.connected()){
      client.loop();
    }
  }

  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    if (time_button == 0)
    {
      display_time = !display_time;
      time_button = millis();
    }
    else
    {
      if (millis() - time_button > 2000) //2s 
      {
        current_room = nextRoom();
        updateScreen();
        time_button = millis();
      }
    }
  }
  else
  {
    time_button = 0;
  }

  if (display_time)
  {
    timeClient.update();
    lcd.setCursor(0, 0);
    lcd.print(timeClient.getFormattedTime() + "   " + timeClient.getFullDate());
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor((16 - rooms[current_room].length()) / 2, 0);
    lcd.print(rooms[current_room]);
  }

  if (millis() - time_shift > 750)
  {
    time_shift = millis();
    lcd.setCursor(0, 1);
    lcd.print(string_with_data.substring(shift_index) + string_with_data.substring(0, shift_index));
    shift_index++;
    if (shift_index >= string_with_data.length())
    {
      shift_index = 0;
    }
  }
}

byte nextRoom()
{
  if (current_room + 1 < ARRAY_SIZE)
  {
    if (rooms[current_room + 1] != "")
    {
      return current_room + 1;
    }
  }
  
  return 0;
}

String getRoomFromTopic(String topic)
{
  int startIndex = -1;
  int endIndex = 0;
  for (int i = 0; i < topic.length(); i++)
  {
    if (topic[i] == '/')
    {
      if (startIndex == -1)
      {
        startIndex = i + 1;
      }
      else
      {
        endIndex = i;
        break;
      } 
    }
  }

  return topic.substring(startIndex, endIndex);
}

short getIndexRoom(String room)
{
  short index = -1;
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    if (room == rooms[i])
    {
      index = i;
      break;
    }
  }

  if (index == -1)
  {
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      if (rooms[i] == "")
      {
        rooms[i] = room;
        index = i;
        break;
      }
    }
  }

  return index;
}

String getSensor(String topic)
{
  int index = 0;
  for (int i = topic.length() - 1; i >= 0; i--)
  {
    if (topic[i] == '/')
    {
      index = i + 1;
      break;
    }
  }
  
  return topic.substring(index);
}

void updateScreen()
{
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    Serial.print(temperatureData[i]);
    Serial.print(" ");
    Serial.print(humidityData[i]);
    Serial.print(" ");
    Serial.println(pressureData[i]);
  }
  
  lcd.setCursor(0, 1);
  lcd.print("                ");

  string_with_data = "";
  if (temperatureData[current_room] != "")
  {
    string_with_data += temperatureData[current_room] + "C ";
  }

  if (humidityData[current_room] != "")
  {
    string_with_data += humidityData[current_room] + "% ";
  }

  if (pressureData[current_room] != "")
  {
    string_with_data += pressureData[current_room] + "Pa ";
  }

  if (string_with_data.length() < 16)
  {
    string_with_data = string_with_data + "                ";
    string_with_data = string_with_data.substring(0, 16);
  }
  

  lcd.setCursor(0, 1);
  lcd.print(string_with_data);
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String payloadStr = "";
  for (int i = 0; i < length; i++) 
  {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr); // выводим в сериал порт значение полученных данных
  String room = getRoomFromTopic(String(topic));
  short index = getIndexRoom(room);
  String sensor = getSensor(String(topic));

  if (sensor == "temperature")
  {
    temperatureData[index] = payloadStr;
  }

  if (sensor == "humidity")
  {
    humidityData[index] = payloadStr;
  }

  if (sensor == "pressure")
  {
    pressureData[index] = payloadStr;
  }

  updateScreen();
}
