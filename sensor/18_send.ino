void send0(){
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

  int micro = digitalRead(MICRO_PIN);
  if (micro == HIGH)
  {
    timeClient.update();
    client.publish((location + String("micro")).c_str(), timeClient.getFormattedTime().c_str());
  }
}

void send1(){
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
