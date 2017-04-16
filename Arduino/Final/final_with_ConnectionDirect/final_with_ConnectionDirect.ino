#include <SoftwareSerial.h>
#include <Wire.h>
#include "DHT.h"

#define Reset "AT+RST\r\n"
#define changemode "AT+CWMODE=3\r\n"
#define setip "AT+CIPAP=\"192.168.1.1\"\r\n"
#define multiconnect "AT+CIPMUX=1\r\n"
#define creatserver "AT+CIPSERVER=1,8888\r\n"
#define connectwifi "AT+CWJAP="
#define getipaddress "AT+CIFSR\r\n"
#define senddata "AT+CIPSEND="
#define connectserver "AT+CIPSTART=\"TCP\",\"192.168.1.173\",2000\r\n"
#define Motor 6
#define DHTPIN 5
#define DHTTYPE DHT22   // DHT 22  (AM2302)

SoftwareSerial esp8266(2,3);// 2 ->RX   3->TX

DHT dht(DHTPIN, DHTTYPE);

  bool Auto = true;
  bool isMessage = true;
  bool Watering = false; // do watering
  bool ChangeSSID = false;
  int WaterPumpWater = 30;//time for pump do nothing after last watering
  int turn=0;
  int soil=100;
  int temp=0;
  int humidity=0;
  int timer = 1400; //timer for every 20 sec send data to server
  int Motor_on = false;//Motor Statues
  int Motor_time_on = 300;//time for watering (100 mlitr)
  int connectionId;
  String response="";//response of server
  String ssid="";
  String pass="";
  float valf=0; // Sun Sensor Data

  //------------------------------------------------
  //light Sensor(Sun Sensor)
  //------------------------------------------------
  int BH1750_address = 0x23; // i2c Addresse
  byte buff[2];

  void BH1750_Init(int address){
  
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1 [lux] resolution
  Wire.endTransmission();
}

byte BH1750_Read(int address){
  
  byte i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read(); 
    i++;
  }
  Wire.endTransmission();  
  return i;
}


void setup() {
//--------------------------------------------------------
// PinModes(Pins we need to do Work like turn on/off Motor)
//--------------------------------------------------------
  pinMode(Motor,OUTPUT);//6
  esp8266.begin(115200);
  Wire.begin();
  BH1750_Init(BH1750_address);
  dht.begin();
  Serial.begin(115200);
  delay(200);
  esp8266.print(Reset);
  delay(2000);
  esp8266.print(changemode);
  delay(1500);
  esp8266.print(setip);
  delay(1500);
  esp8266.print(multiconnect);
  delay(1500);
  esp8266.print(creatserver);
  delay(1500);
}
  
void loop() {
  if(ChangeSSID){
    String ConnectCommand="";
    ConnectCommand += connectwifi;
    ConnectCommand += "\"" + ssid + "\",\"" + pass + "\"\r\n";
    esp8266.print(ConnectCommand);
    ChangeSSID = false;
  }
  if(Auto == true){
    if(Watering == false){
      if(soil > 500){
         digitalWrite(Motor, 1);
         Motor_on = true;
         Watering = true;
      }
    }
    else {
      if (timer == 1900){
        if(WaterPumpWater-- == 0){
          Watering = false;
          WaterPumpWater = 30;
        }
      }
    }
    if(Motor_on == true){
      if(Motor_time_on-- == 0){
        digitalWrite(Motor, 0);
        Motor_on = false;
        Motor_time_on = 300;
      }
    }
  }
//-------------------------------------------------------------
//Check Connection 
//-------------------------------------------------------------  
  if(timer == 1800){ 
    esp8266.println(connectserver);
  }
//-------------------------------------------------------------
// Send Sensors Data Every 20 Second to Server
//-------------------------------------------------------------
  if (timer++ == 2000) {
    timer = 0;
    Serial.println();
    
    soil = analogRead(A0); // Soil moisture
    humidity = (int)dht.readHumidity(); // Air humidity
    temp = (int)dht.readTemperature(); // Air Temperature
//------------------------------------------------------------
//Sunlight
//---------------------------------------------------------
    if(BH1750_Read(BH1750_address)==2){
      
      valf=((buff[0]<<8)|buff[1])/1.2;
      valf =(int)valf; 
    }
//--------------------------------------------------------------
    
    String tBuffer="";
    tBuffer += (String)temp +" ";
    tBuffer += (String)soil +" ";
    tBuffer += (String)humidity +" ";
    tBuffer += (String)valf +" ";
    if(Motor_on){
      tBuffer += "ON";
    }
    else{
      tBuffer += "OFF";
    }
    String lenght = senddata;
    lenght += tBuffer.length();

    esp8266.println(lenght);
    delay(100);
    esp8266.println(tBuffer);
    
  }
//-------------------------------------------------------------
// Recive Order From Server
//-------------------------------------------------------------
    if (esp8266.available() > 0) {
    if(esp8266.find("+IPD,"))
    {
      String com = "";
      delay(300);
      connectionId = Serial.read()-48;
      while (esp8266.available())
      {
        char c = esp8266.read();
        com += (char)c;
      }
      
    if(com.indexOf("pin=on") != -1)
     { 
     // add1="Light is on";
     int pinNumber = 11; 
     digitalWrite(pinNumber, 1);
     }
    if(com.indexOf("pin=off") != -1)
     { 
      //add1="Light is off";
      int pinNumber = 11;
      digitalWrite(pinNumber, 0);
     }
    if(com.indexOf("ssid") != -1){
      int sid = com.lastIndexOf("ssid");
      int pas = com.lastIndexOf("pass");
      int http = com.indexOf("HTTP");
      int len = com.length();
      Serial.print("sid ===");
      Serial.println(String(sid));
      Serial.print("password ===" );
      Serial.println(pas);
      Serial.print("length ===");
      Serial.println(len);
      Serial.print("HTTP ===");
      Serial.println(http);
      ssid=com.substring(sid+4, pas);
    pass= com.substring(pas+4, http-1);
      Serial.println(com.substring(sid+4, pas));
      Serial.println(com.substring(pas+4, http-1));
    ChangeSSID = true;
    }
     
     String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     esp8266.println(closeCommand);
     delay(2000);
     //sendData(closeCommand,3000,DEBUG);
      digitalWrite(13, !digitalRead(13));
      Serial.println(com);
    }
  }
//-------------------------------------------------------------
  delay(10);
  if(timer%100 == 0)
    Serial.print(timer/100);

}




