#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#include "functions.h"

const char* ssid     = "yuvi";
const char* password = "bunnycamel";

void setup()
{
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.println("");
  Serial.println("ESP8266 Webpage");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/shop", shop);
  
  server.begin();
  Serial.println("HTTP server started");

  SPI.begin(); 
  rfid.PCD_Init(); 
  client.setInsecure();
}


void loop()
{server.handleClient();}
