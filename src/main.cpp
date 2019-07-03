#include <Arduino.h>
#include <wire.h>
#include <DHT12.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h"
#include "ntpnet.h"
#include "debug.h"


#define Myssid "xxx"
#define Mypwd "***"

DHT12 DHT;
ESP8266WebServer server(80);
Ntpnet ntpnet;
Page page;

unsigned int period = 1000;
unsigned long time_now = 0;

void handleRoot() {
 String s = page.writeout();
 //printd(s.c_str());
 server.send(200, "text/html", s);
}

void setup() {
  Wire.begin(14, 12);
  Serial.begin(115200);

  WiFi.hostname("humidity");
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(Myssid, Mypwd);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);

  ntpnet.udpStart();

  server.on("/", handleRoot);      
 
  server.begin();

  Serial.println("HTTP server started");

  Serial.println(__FILE__);
  Serial.print("DHT12 LIBRARY VERSION: ");
  Serial.println(DHT12_VERSION);
  Serial.println();

  page.humidityNow = &DHT.humidity;
  page.temperatureNow =&DHT.temperature;

  Serial.println("Type,\tStatus,\tHumidity (%),\tTemperature (C)");
  ntpnet.lastNTPResponse = millis();

  debug_setup();
}



void loop() {
  
  ntpnet.ntpInLoop();

  if(millis() - time_now > period){
    time_now = millis();

    int status = DHT.read();
    switch (status)
    {
    case DHT12_OK:
      Serial.print("OK,\t");
      break;
    case DHT12_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHT12_ERROR_CONNECT:
      Serial.print("Connect error,\t");
      break;
    case DHT12_MISSING_BYTES:
      Serial.print("Missing bytes,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
    }
    
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t");
    Serial.println(DHT.temperature, 1);
    
    page.save(ntpnet.giveTime());
    debug_handle();
  }

  server.handleClient();

}