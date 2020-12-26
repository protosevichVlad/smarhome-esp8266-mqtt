void connectingToWifi()
{
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");
  digitalWrite(LED_FAILED_PIN, HIGH);
  WiFi.begin(ssid.c_str(), pass.c_str());

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
  Serial.println("WiFi connected");
  digitalWrite(LED_FAILED_PIN, LOW);
}

void settingModeOff()
{
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  server.stop();
  is_setting_mode = false;
  connectingToWifi();
}

void settingModeOn()
{
  is_setting_mode = true;
  WiFi.softAP("ESP8266_1", "11111111");
  Serial.println(WiFi.softAPIP());  
  server.on("/", indexPage);
  server.on("/wifi", wifiSettingPage);
  server.on("/mqtt", mqttSettingPage);
  server.on("/location", locationSettingPage);
  server.on("/off", settingModeOffPage);
  server.on("/getAvailableNetworks", getAvailableNetworksPage);
  server.onNotFound(notFoundPage);
  server.begin();
}

void settingModeOffPage()
{
  server.send(200, "text/html", "Ok");
  settingModeOff();
}

void indexPage()
{
  String message = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Info</title>\n</head>\n<body>\n<b>Wi-Fi</b><br>\nSSID: ";
  message += ssid;
  message += "<br>\nWi-Fi status: ";
  if (WiFi.status() == WL_CONNECTED)
  {
    message += "Connected"; 
  }
  else
  {
    message += "Not connected";
  }
  message += "<br>\n<br>\n<b>MQTT</b><br>\nMQTT brocker: " + mqtt_server + "<br>\nMQTT status: ";
  if (client.connected())
  {
    message += "Connected"; 
  }
  else
  {
    message += "Not connected";
  }
  message += "<br>\n<br>\nLocation: " + location + "<br>\n<br>\n<a href=\"/wifi\">Wi-Fi setting</a><br>\n<a href=\"/mqtt\">MQTT setting</a><br>\n<a href=\"/location\">Location setting</a><br><br><a href=\"/off\">Setting mode OFF</a><br></body>\n</html>";
  server.send(200, "text/html", message); 
}

void wifiSettingPage()
{
  if (server.method() == HTTP_GET)
  {
    String message = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Wi-Fi setting</title>\n</head>\n<body>\n<p>Available networks</p>\n<ul id=\"net\"></ul>\n<form action=\"/wifi\" method=\"post\">\n<fieldset>\n<legend>Wi-Fi</legend>\n<label>SSID: <input name=\"ssid\" type=\"text\" required></label>\n<br>\n<label>Password: <input name=\"pass\" type=\"password\" required></label>\n<br>\n<button type=\"submit\">Ok</button>\n</fieldset>\n</form>\n<br>\n<a href=\"/\">Info</a>\n</body>\n<script>\nasync function get()\n{\nlet response = await fetch(\"/getAvailableNetworks\");\n\nif (response.ok) { // РµСЃР»Рё HTTP-СЃС‚Р°С‚СѓСЃ РІ РґРёР°РїР°Р·РѕРЅРµ 200-299\nlet text = await response.text();\ndocument.getElementById(\"net\").innerHTML = text;\n} else {\nalert(\"РћС€РёР±РєР° HTTP: \" + response.status);\n}\n}\nget();\n</script>\n</html>";
    server.send(200, "text/html", message);
  }
  else
  {   
    ssid = server.arg("ssid");
    pass = server.arg("pass");
    writeTo(SSID_ADDRESS, ssid);
    writeTo(PASS_ADDRESS, pass);
    connectingToWifi();
    
    server.sendHeader("Location", "/",true);
    server.send(302, "text/plane",""); 
  }
}

void mqttSettingPage()
{
  if (server.method() == HTTP_GET)
  {
    String message = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>MQTT setting</title>\n</head>\n<body>\n<b>Current setting</b><br>\nMQTT brocker: " + mqtt_server + "<br>\nMQTT port: " + mqtt_port + "<br>\nMQTT user: " + mqtt_user + "<br>\nMQTT password: " + mqtt_pass + "<br>\n<br>\n<b>Edit</b>\n<form action=\"\" method=\"post\">\n<p style=\"margin:0; width: 120px; display: inline-block\">MQTT brocker:</p><input name=\"brocker\" type=\"text\" required><br>\n<p style=\"margin:0; width: 120px; display: inline-block\">MQTT port: </p><input name=\"port\" type=\"number\" required><br>\n<p style=\"margin:0; width: 120px; display: inline-block\">MQTT user: </p><input name=\"user\" type=\"text\" required><br>\n<p style=\"margin:0; width: 120px; display: inline-block\">MQTT password: </p><input name=\"password\" type=\"password\" required><br>\n<button type=\"submit\">Ok</button>\n</form>\n<a href=\"/\">Info</a>\n</body>\n</html>";
    server.send(200, "text/html", message);
  }
  else
  {   
    mqtt_server = server.arg("brocker");
    mqtt_port = server.arg("port");
    mqtt_user = server.arg("user");
    mqtt_pass = server.arg("password");
    
    writeTo(MQTT_SERVER_ADDRESS, mqtt_server);
    writeTo(MQTT_PORT_ADDRESS, mqtt_port);
    writeTo(MQTT_USER_ADDRESS, mqtt_user);
    writeTo(MQTT_PASS_ADDRESS, mqtt_pass);

    connectingToMqtt();
    
    server.sendHeader("Location", "/",true);
    server.send(302, "text/plane",""); 
  }
}

void locationSettingPage()
{
  if (server.method() == HTTP_GET)
  {
    String message = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Location setting</title>\n</head>\n<body>\n<b>Current setting</b><br>\nLocation: " + location + "<br>\n<br>\n<b>Edit</b>\n<form action=\"\" method=\"post\">\n<p style=\"margin:0; width: 120px; display: inline-block\">Location: </p><input name=\"location\" type=\"text\" required><br>\n<button type=\"submit\">Ok</button>\n</form>\n<br><a href=\"/\">Info</a> \n</body>\n</html>";
    server.send(200, "text/html", message);
  }
  else
  {   
    location = server.arg("location");
    
    writeTo(LOCATION_ADDRESS  , location);
    
    server.sendHeader("Location", "/",true);
    server.send(302, "text/plane",""); 
  }
}

void getAvailableNetworksPage()
{
  String message = "";
  int number_of_networks = WiFi.scanNetworks();
 
  for(int i =0; i < number_of_networks; i++){
    message += "<li>";
    message += WiFi.SSID(i);
    message += "  ";
    message += WiFi.RSSI(i);
    message += "dBm</li>";
  }
  server.send(200, "text/plain", message);
}

void notFoundPage()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
