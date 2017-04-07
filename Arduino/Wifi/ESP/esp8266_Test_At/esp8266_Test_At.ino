#include <SoftwareSerial.h>

#define connectwifi "AT+CWJAP=\"B0i9-aHJm\",\"12345678\""
#define getipaddress "AT+CIFSR"
#define senddata "AT+CIPSEND="
#define connectserver "AT+CIPSTART=\"TCP\",\"192.168.43.168\",2000"

SoftwareSerial esp8266(2,3);// 2 ->RX   3->TX

void setup() {
  // put your setup code here, to run once:
  esp8266.begin(115200);
  /*
   * در صورت جواب ندادن  ، از باد ریت 9600 و یا 57600 استفاده کنید
   *
   * به علت ورژن های مختلفاین ماژول ممکن است بادریت ها متفاوت باشد
   */
  Serial.begin(115200);
}

bool isMessage = true;
String response="";

void loop() {
 // esp8266.listen();
  // put your main code here, to run repeatedly:
  response="";
  if (esp8266.available() > 0)
  {
    while(esp8266.available()){
      char c = esp8266.read();
       Serial.write(c);
    }
    //Serial.println(response);
  }
  if (Serial.available() > 0)
  {
    delay(1000);
    String command = "";
    while (Serial.available())
    {
      command += (char)Serial.read();
    }
    Serial.println(command);
    if(command.substring(0, 1)=="1"){
      Serial.println(connectwifi);
      esp8266.println(connectwifi);
    }
    else if(command.substring(0, 1)=="2"){
      esp8266.println(getipaddress);
    }
    else if(command.substring(0, 1)=="3"){
      String lenght = senddata;
      String data = String(analogRead(A0));
      lenght += data.length();
      //Serial.println(lenght);
      esp8266.println(lenght);
      
      delay(100);
      esp8266.println(data);
    }
    else if(command.substring(0, 1)=="4"){
      esp8266.println(connectserver);
    }
  }

}
