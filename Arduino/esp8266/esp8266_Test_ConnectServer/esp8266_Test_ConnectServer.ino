#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);// 2 ->RX   3->TX

const char* ssid = "faezWLAN";
const char* pw = "faez22338510";
const char* host = "192.168.1.2";
const int port = 2000;

WiFiClient client;

void setup() {
  esp8266.begin(115200);
  Serial.begin(115200);
  WiFi.begin(ssid, pw);
  Serial.println("Connecting to AP");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("."); 
   }
  Serial.println(" connected");
}

void loop() {
  Serial.println("Connecting to host");
  if(!client.connect(host, port)){
    Serial.println("...connection failed!");
    Serial.println("Retrying in 5 seconds...");
    delay(5000);
    return;  
  }
  client.print("A");
  Serial.println("...TCP message fired!");
  delay(2000);
}
