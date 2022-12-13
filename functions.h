#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
constexpr uint8_t RST_PIN = D3;    
constexpr uint8_t SS_PIN = D4;    
MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;

//////////////////////////////////////////////////////////HOST PROPERTIES

const char* host = "script.google.com";
const int httpsPort = 443;

//////////////////////////////////////////////////////////

WiFiClientSecure client;
String GAS_ID = "AKfycby2GFG6bdCdLYkYd0audZeIkw9bQvUfqyQOG9dbPPMh7vW4BDg"; //--> spreadsheet script ID

//////////////////////////////////////////////////////////

String tag;
int quantity[2];
int bill[2];
int quantitysum=0;
int costsum=0;
int h=0;

void sendData(String tem, int hum) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  //----------------------------------------Processing data and sending data
  String string_id =  tem;
  String string_val =  String(hum, DEC); 
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_id + "&humidity=" + string_val;
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("ECart Updated Sucessfully");
      break;
    }
  }
  
  Serial.println("==========");
  Serial.println();
}

void getid(){
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    
  Serial.println(tag);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  if(tag=="1154922149"){
    Serial.println("Milk");
    quantity[0]=quantity[0]+1;
    bill[0]=bill[0]+25;
    quantitysum+=1;
    costsum+=25;
    Serial.println(quantity[0]);
    Serial.println(bill[0]);
    h=25;
  }
  if(tag=="161519032"){
    Serial.println("Bread");
    quantity[1]=quantity[1]+1;
    bill[1]=bill[1]+50;
    quantitysum+=1;
    costsum+=50;
    Serial.println(quantity[1]);
    Serial.println(bill[1]);
    h=50;
  }
  Serial.println("Total Cost ");
  Serial.println(costsum);
  Serial.println("Total Quantity ");
  Serial.println(quantitysum);
  String id = "Product ID : " + tag;
  String val = "Product Price : " + String(h);
  Serial.println(id);
  Serial.println(val); 
  sendData(tag, h);
  tag="";
}






void handleRoot()
{server.send(200, "text/html", "<h1>ESP8266 Controller<br>Shopping: /shop</h1>");}

void shop()
{
  getid();
  String page;
  page="<html><head><title>ECart</title><meta http-equiv=\"refresh\" content=\"1\"></head>";
  page+="<style type=\"text/css\">table{border-collapse: collapse;}th {background-color:  #3498db ;color: white;}table,td {border: 4px solid black;font-size: x-large;text-align:center;border-style: groove;border-color: rgb(255,0,0);}</style>";
  page+="<body><center><h1>Smart Cart IoT</h1><br><br><table style=\"width: 1200px;height: 450px;\"><tr><th>ID</th><th>ITEMS</th><th>QUANTITY</th><th>COST</th></tr><tr><td>1154922149</td><td>Milk</td><td>"+String(quantity[0])+"</td><td>"+String(bill[0])+"</td></tr><tr><td>161519032</td><td>Bread</td><td>"+String(quantity[1])+"</td><td>"+String(bill[1])+"</td></tr>";
  page+="<tr><th>Bill</th><th>Grand Total</th><th>"+String(quantitysum)+"</th><th>"+String(costsum)+"</th></tr></table></center></body></html>";
  server.send(200, "text/html", page);
}
