void mqttSetup()
{
  client.setServer(mqtt_server.c_str(), mqtt_port.toInt());
  client.setCallback(callback);
}

void mqttLoop()
{
  if (!client.connected()) 
  {
    connectingToMqtt();
  }

  if (client.connected())
  {
    client.loop();
    send0();
    send1();
  }
}

void connectingToMqtt()
{
  Serial.println("Connecting to MQTT server");
  if (client.connect(mqtt_client_id.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), "status", 2, true, "0"))
  {
    Serial.println("Connected to MQTT server");
    mqttSubscribe();
  } 
  else 
  {
    Serial.println("Could not connect to MQTT server");   
  }
}

void mqttSubscribe()
{
  client.subscribe((location + String("led")).c_str());
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
  
  if(String(topic) == location + String("led")) 
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
  
  Serial.println();
}
