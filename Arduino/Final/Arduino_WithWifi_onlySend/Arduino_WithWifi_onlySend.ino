#include <SoftwareSerial.h>
#include <Wire.h>
#include "DHT.h"

#define connectwifi "AT+CWJAP=\"B0i9-aHJm\",\"12345678\""
#define getipaddress "AT+CIFSR"
#define senddata "AT+CIPSEND="
#define connectserver "AT+CIPSTART=\"TCP\",\"192.168.43.168\",2000"
#define Motor 6
#define DHTPIN 5
#define DHTTYPE DHT22   // DHT 22  (AM2302)

SoftwareSerial esp8266(2,3);// 2 ->RX   3->TX

DHT dht(DHTPIN, DHTTYPE);

  bool Auto = true;
  int WaterPumpWater = 30;//time for pump do nothing after last watering
  bool Watering = false; // do watering
  int turn=0;
  int soil=100;
  int temp=0;
  int humidity=0;
  int timer = 1400; //timer for every 20 sec send data to server
  int Motor_on = false;//Motor Statues
  int Motor_time_on = 300;//time for watering (100 mlitr)
  bool isMessage = true;
  String response="";//response of server
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
//--------------------------------------------------------

void setup() {
//--------------------------------------------------------
// PinModes(Pins we need to do Work like turn on/off Motor)
//--------------------------------------------------------
  pinMode(Motor,OUTPUT);//6
  esp8266.begin(115200);
  Wire.begin();
  BH1750_Init(BH1750_address);
  dht.begin();
  Serial.begin(9600);
  delay(200);
}
  
void loop() {
  
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
    
    String command = "";
    while (esp8266.available())
    {
      command += (char)esp8266.read();
    }

    if(command == "on"){
        digitalWrite(13,1);
        Motor_on = true;
        turn = 1;
        digitalWrite(Motor, 1);
        //delay(1000);
        Serial.print("ON");
    }
    else if(command == "off"){
      digitalWrite(13,0);
      Motor_on = false;
      Motor_time_on = 300;
      turn = 0;
      digitalWrite(Motor, 0);
      Serial.print("OFF");
    }
    else if(command == "auto"){
      Auto = true;
    }
    Serial.println(command);
  }
//-------------------------------------------------------------
  delay(10);
  if(timer%100 == 0)
    Serial.print(timer/100);

}




