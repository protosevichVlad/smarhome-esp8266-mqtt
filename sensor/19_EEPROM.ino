void setupEEPROM()
{
  ssid = readFrom(SSID_ADDRESS);
  pass = readFrom(PASS_ADDRESS);

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("PASSWORD: ");
  Serial.println(pass);

  mqtt_server = readFrom(MQTT_SERVER_ADDRESS); 
  mqtt_port = readFrom(MQTT_PORT_ADDRESS); 
  mqtt_user = readFrom(MQTT_USER_ADDRESS); 
  mqtt_pass = readFrom(MQTT_PASS_ADDRESS);

  Serial.println("MQTT: ");
  Serial.print("mqtt_server: ");
  Serial.println(mqtt_server);
  Serial.print("mqtt_port: ");
  Serial.println(mqtt_port);
  Serial.print("mqtt_user: ");
  Serial.println(mqtt_user);
  Serial.print("mqtt_pass: ");
  Serial.println(mqtt_pass);

  location = readFrom(LOCATION_ADDRESS);
  Serial.print("location: ");
  Serial.println(location);
}

String readFrom(int address)
{
  EEPROM.begin(EEPROM_SIZE);
  String result = "";
  for (int i = address; i < address + SIZE_STRING_EEPROM; i++)
  {
    if (EEPROM.read(i) == 0)
    {
      break;
    }
    result += char(EEPROM.read(i));
  }
  EEPROM.end();
  return result;
}

void writeTo(int address, String data)
{
  EEPROM.begin(EEPROM_SIZE);  
  for (int i = address; i < address + SIZE_STRING_EEPROM; i++)
  {
    if (i - address >= data.length())
    {
      EEPROM.write(i, 0);
    }
    else
    {
      EEPROM.write(i, data[i - address]);
    }
    EEPROM.commit();
  }
  EEPROM.end();
}
