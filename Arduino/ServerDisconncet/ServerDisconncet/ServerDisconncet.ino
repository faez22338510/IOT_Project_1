#include <SPI.h>
#include <Ethernet.h>

#define Connect 3
#define disConnect 2
#define Error 4

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 8);
IPAddress server(192, 168, 1, 2);

EthernetClient client;

bool isConnect = false;

void tryConncet(){
  Serial.println("connecting...");
  if (client.connect(server, 2000)) {
    Serial.println("connected");
    isConnect = true;
  } 
  else {
    Serial.println("connection failed");
  }
}


void setup() {
  pinMode(Connect,OUTPUT);
  pinMode(disConnect,OUTPUT);
  pinMode(Error,OUTPUT);
  
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  delay(1000);
  Serial.println("connecting...");
//  if (client.connect(server, 2000)) {
//    Serial.println("connected");
//    isConnect = true;
//  } 
//  else {
//    Serial.println("connection failed");
//  }
}

int timer = 400;
int turn=0;
int soil;
int temp;
int humidity;
bool isRecive = true;

void loop() {
  if(isConnect && isRecive){
    digitalWrite(Connect,1);
    digitalWrite(disConnect,0);
  }
  else{
    digitalWrite(disConnect,1);
    digitalWrite(Connect,0);
  }
  int temp;
  if(isConnect){
    if (client.available() > 0) {
      
      delay(100);
      String command = "";
      while (client.available())
      {
        command += (char)client.read();
      }
  
      if(command == "on"){
          digitalWrite(2,1);
          Serial.print("ON");
      }
      else if(command == "off"){
        digitalWrite(2,0);
        //digitalWrite(Motor, 0);
        Serial.print("OFF");
      }
      else if(command == "OK"){
        isRecive = true;
      }
      Serial.print(command);
    }
  }

  if (timer++ == 500) {
    timer = 0;
    soil = analogRead(A0); // Soil moisture
    //humidity = (int)dht.readHumidity(); // Air humidity
    //temp = (int)dht.readTemperature(); // Air Temperature
    if(soil > 290){
        digitalWrite(Error,1);
    }
    else{
        digitalWrite(Error,0);
    }
    if(isConnect == true ){
      if(isRecive){
        char tBuffer[16]= "";
        sprintf(tBuffer,"%u 12 13",soil);
        client.print(tBuffer);
        Serial.println(tBuffer);
        isRecive= false;
      }
    }
    else{
      tryConncet();
    }
  }
  delay(10);
  if(timer%100 == 0)
    Serial.print(timer);
  if(isConnect == true){
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      isConnect = false;
      client.stop();
      isRecive = true;
  //    while (true);
    }
  }
}
