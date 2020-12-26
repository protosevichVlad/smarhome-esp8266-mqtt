String mqtt_client_id = "esp_sensor_" + String(random(1, 100)); // client id
String location = "home/Room1/";
String location1 = "home/Room2/";

const long utcOffsetInSeconds = 3 * 3600;

long Time = millis();
long time_button = millis();
long Time1 = millis();
int address = 0;

bool is_setting_mode = false;

IPAddress    apIP(10, 10, 10, 1); 

// wifi
WiFiUDP ntpUDP;  
WiFiClient wclient;
ESP8266WebServer server(80);

// mqtt
PubSubClient client(wclient); 

//real time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//send
DHTesp dht;    
Adafruit_BMP280 bme;

float array_temperature[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float array_pressure[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float array_temperature1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float array_humidity1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
