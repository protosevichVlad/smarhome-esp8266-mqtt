void setup()
{
  Serial.begin(115200);  
  Serial.println();
  Serial.println("Start");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);
  
  setupEEPROM();

  WiFi.mode(WIFI_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); 

  mqttSetup();

  if (!bme.begin(0x76)) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");                                         
  }

  timeClient.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_SETTING_PIN, OUTPUT);
  pinMode(LED_FAILED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(MICRO_PIN, INPUT);

  

  dht.setup(DTH11_PIN, DHTesp::DHT11);
  
}
