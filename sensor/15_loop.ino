void loop()
{
  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    if (time_button == 0)
    {
      time_button = millis();
    }
    else if (millis() - time_button > 1000) // 1s
    {
      time_button = millis();
      is_setting_mode = !is_setting_mode;
      if (is_setting_mode)
      {
        digitalWrite(LED_SETTING_PIN, HIGH);
        settingModeOn();
      }
      else
      {
        digitalWrite(LED_SETTING_PIN, LOW);
        settingModeOff();
      }
    }
  }
  else 
  {
    time_button = 0;
  }
  
  if (is_setting_mode)
  {
    server.handleClient();
    MDNS.update();
        
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED)
  {
    connectingToWifi();
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) 
  {
    mqttLoop();
  }
}
