//#include <SoftwareSerial.h>
#include <Wire.h>
#include "DHT.h"

#define Reset "AT+RST\r\n"
#define changemode "AT+CWMODE=1\r\n"
#define setip "AT+CIPAP=\"192.168.1.1\"\r\n"
#define multiconnect "AT+CIPMUX=1\r\n"
#define creatserver "AT+CIPSERVER=1,80\r\n"
#define connectwifi "AT+CWJAP=\"Irancell-Z6000\",\"faez22338510\""
//#define connectwifi "AT+CWJAP=\"Innovation Center 2\",\"CreateFuture1495\""
#define connecting "AT+CWJAP="
#define getipaddress "AT+CIFSR"
#define senddata "AT+CIPSEND="
#define connectserver "AT+CIPSTART=\"TCP\",\"192.168.1.80\",2000"
#define Motor 3
#define WifiDisConnect 4
#define ServerDisConnect 5
#define DHTPIN 6
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//SoftwareSerial Serial1(2,3);// 2 ->RX   3->TX

DHT dht(DHTPIN, DHTTYPE);

bool Auto = true;
bool ChangeSSID = false;
int WaterPumpWater = 30;//time for pump do nothing after last watering
bool Watering = false; // do watering
bool Motor_on = false;//Motor Statues
bool isMessage = false;
bool getSonsors = false;
bool notif = false;
bool alreadyConnect = false; // if server is connected(CLOSED / ALREADY CONNECTED)
bool WifiConnect = false; // if Wifi is Coonected (no ip & WIFI DISCONNECT / WIFI CONNECTED & WIFI GOT IP) 
int turn = 0;
int soil = 100;
int temp = 0;
int humidity = 0;
int connectionId;
int timer = 1400; //timer for every 20 sec send data to server
int Motor_time_on = 300;//time for watering (100 mlitr)
String request = "";//request of server
String ssid="";
String pass="";
float valf = 0; // Sun Sensor Data

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
  pinMode(Motor,OUTPUT);//3
  pinMode(WifiDisConnect, OUTPUT);//4
  pinMode(ServerDisConnect, OUTPUT);//5  
  Wire.begin();
  BH1750_Init(BH1750_address);
  dht.begin();
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(115200);
  delay(200);
  //Serial1.print(connectwifi);
  //delay(1500);
  Serial2.print(Reset);
  delay(2000);
  Serial2.print(changemode);
  Serial1.print("AT+CWMODE=1\r\n");
  delay(1500);
  Serial2.print(setip);
  delay(1500);
  Serial2.print(multiconnect);
  delay(1500);
  Serial2.print(creatserver);
  Serial2.print(getipaddress);
  delay(1500);
}
  
void loop() {
    if (Auto == true) {
    if (Watering == false) {
      if (soil > 600) {
        digitalWrite(Motor, 1);
        Motor_on = true;
        Watering = true;
        isMessage = true;
        request = "stat";
      }
    }
    else {
      if (timer == 1800) {
        if (WaterPumpWater-- == 0) {
          Watering = false;
          WaterPumpWater = 30;
        }
      }
    }
    if (Motor_on == true) {
      if (Motor_time_on-- == 0) {
        digitalWrite(Motor, 0);
        Motor_on = false;
        digitalWrite(10,0);
        Motor_time_on = 300;
      }
    }
  }
   //-------------------------------------------------------------
  //Change SSID
  //-------------------------------------------------------------
  if(ChangeSSID){
    String ConnectCommand="";
    ConnectCommand += connecting;
    ConnectCommand += "\"" + ssid + "\",\"" + pass + "\"\r\n";
    Serial.println("MMMMMMMMMMMMMMMMM");
    ConnectCommand.replace("%20", " ");
    Serial.println(ConnectCommand);
    Serial.println("MMMMMMMMMMMMMMMMM");
    Serial1.print(ConnectCommand);
    ChangeSSID = false;
  }
//-------------------------------------------------------------
//Check Connection 
//-------------------------------------------------------------  
if (timer == 2500) {
    if(alreadyConnect == false){
      Serial1.println(connectserver);
        if(isMessage){
          Serial1.println(connectserver);
          delay(200);
        }
    }
  }
//-------------------------------------------------------------
// Send Sensors Data Every 20 Second to Server
//-------------------------------------------------------------
  if (timer++ == 3000) {
    timer = 0;
    Serial.println();
    
    humidity = (int)dht.readHumidity(); // Air humidity
    delay(20);
    temp = (int)dht.readTemperature(); // Air Temperature
    delay(20);
    if(temp > 40 || temp < 10 ){
      notif = true;
    }
    else{
      notif = false;
    }
    //temp = 100;
    //humidity = 20;
    soil = analogRead(A0);
      String tBuffer = "{";
      tBuffer += "\"t\":"+(String)temp + ",";
      tBuffer += "\"so\":"+(String)soil + ",";
      tBuffer += "\"h\":"+(String)humidity + ",";
      //tBuffer += (String)valf + " ";
      if (Motor_on) {
        tBuffer += "\"M\":\"ON\",";
      }
      else {
        tBuffer += "\"M\":\"OFF\",";
      }
     tBuffer += "\"n\":"+(String)notif+","; 
     tBuffer += "\"se\":"+"1234,"; 
     tBuffer += "\"pId\":"+"1,"; 
     tBuffer += "}";
    Serial.println(tBuffer);
    timer = 0;
    //------------------------------------------------------------
//Sunlight
//---------------------------------------------------------
   // if(BH1750_Read(BH1750_address)==2){
      
   //   valf=((buff[0]<<8)|buff[1])/1.2;
   //   valf =(int)valf; 
   // }
//--------------------------------------------------------------
  }
    
   if(isMessage){
    Serial.println(request);
    if ((request.indexOf("st")!=-1 || request.indexOf("ta")!=-1 || request.indexOf("at")!=-1) 
    || ((request.indexOf("mOn")!=-1 || request.indexOf("On")) && Motor_on == false) 
    || ((request.indexOf("Of")!=-1 || request.indexOf("ff")) && Motor_on == true)) {
      
       if((request.indexOf("mOn")!=-1 || request.indexOf("On")!=-1) && Motor_on == false){
          digitalWrite(13,1);
          Motor_on = true;
          Watering = true;
          digitalWrite(Motor, 1);
          WaterPumpWater = 30;
          Serial.print("ON");
      }
      else if((request.indexOf("Of")!=-1 || request.indexOf("ff")!=-1) && Motor_on == true){
        digitalWrite(13,0);
          Motor_on = false;
          //Watering = true;
          digitalWrite(Motor, 0);
          //WaterPumpWater = 30;
          Motor_time_on = 3000;
          Serial.print("OFF");
      }
      Serial.println("*********hamidreza faez!!!!!");
      soil = analogRead(A0); // Soil moisture
      humidity = (int)dht.readHumidity(); // Air humidity
      temp = (int)dht.readTemperature(); // Air Temperature
      //temp = 100;
      //humidity = 20;
      String tBuffer = "{";
      tBuffer += "\"t\":"+(String)temp + ",";
      tBuffer += "\"so\":"+(String)soil + ",";
      tBuffer += "\"h\":"+(String)humidity + ",";
      //tBuffer += (String)valf + " ";
      if (Motor_on) {
        tBuffer += "\"M\":\"ON\",";
      }
      else {
        tBuffer += "\"M\":\"OFF\",";
      }
     tBuffer += "\"n\":"+(String)notif+",";  
     tBuffer += "\"se\":"+"1234,"; 
     tBuffer += "\"pId\":"+"1,"; 
     tBuffer += "}";
      String lenght = senddata;
      lenght += tBuffer.length();

      Serial1.println(lenght);
      delay(100);
      Serial1.println(tBuffer);
    }
    
    isMessage = false;
    request = "";
  }
//-------------------------------------------------------------
// Recive Order From Server
//-------------------------------------------------------------
   if (Serial1.available() > 0) {
     String command = "";
     Serial.print("Wifi1(Connected to home Wifi): ");
     while (Serial1.available())
     {
       char c = Serial1.read();
       command += (char)c;
     }
     if(command.indexOf("+IPD,") != -1)
     {
      if(command.indexOf("Teln") != -1 || command.indexOf("serv") != -1){
        alreadyConnect = true;
      }
       isMessage = true;
       delay(300);
       request = command;
     }
    else if(command.indexOf("GOT") != -1 ||command.indexOf("FAIL") != -1){
      if(command.indexOf("GOT") != -1){
        Serial.println("1111111111111111111111111111");
        WifiConnect = true;
      }
       isMessage = true;
       delay(300);
       request = command;
    }
    else if(command.indexOf("no ip") != -1 || command.indexOf("DISC") != -1){
      WifiConnect = false;
      alreadyConnect = false;
    }
    else if(command.indexOf("CLOSED") != -1){
      alreadyConnect = false;
    }
    else if(command.indexOf("ALRE") != -1 || command.indexOf("ADY") != -1){
      alreadyConnect = true;
      WifiConnect = true;
    }
    else if(command.indexOf("GOT") != -1  || alreadyConnect){
      Serial.println("2222222222222222222222222");
      WifiConnect = true;
    }
    if(!WifiConnect){
      digitalWrite(WifiDisConnect,1);
      digitalWrite(ServerDisConnect,0);
    }
    else if(!alreadyConnect){
      digitalWrite(WifiDisConnect,0);
      digitalWrite(ServerDisConnect,1);
    }
    else{
      digitalWrite(WifiDisConnect,0);
      digitalWrite(ServerDisConnect,0);
    }
    Serial.println(command);
  }
  //--------------------------------------------------------------
  if (Serial2.available() > 0) {
    if(Serial2.find("+IPD,")){
      String com = "";
      Serial.print("Wifi2(Mobile Connect Them): ");
      delay(300);
      connectionId = Serial2.read()-48;
      Serial.print("Connection ID : ");
      Serial.println(connectionId);
      while (Serial2.available())
      {
        char c = Serial2.read();
        com += (char)c;
      }
    //sample request 192.168.1.1/ssidfaezIrancell-Z6000passfaez22338510
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
     
//     String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command 
//     closeCommand+=connectionId; // append connection id
//     closeCommand+="\r\n";
//     Serial2.println(closeCommand);
    // delay(2000);
     //sendData(closeCommand,3000,DEBUG);
      digitalWrite(13, !digitalRead(13));
      Serial.println(com);
    }
    else{
      String com = "";
      while (Serial2.available())
      {
        char c = Serial2.read();
        com += (char)c;
      }
      Serial.println(com);
    }
  }
    //-------------------------------------------------------------
  if (Serial.available() > 0)
  {
    delay(1000);
    String command = "";
    while (Serial.available())
    {
      command += (char)Serial.read();
 
    }
    command.replace("%20", " ");
    Serial1.println(command);
  }
//-------------------------------------------------------------
  delay(10);
  if(timer%100 == 0)
    Serial.print(timer/100);

}
