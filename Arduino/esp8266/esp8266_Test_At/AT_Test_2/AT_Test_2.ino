#include <SoftwareSerial.h>
#define ledPin 13 
#define rah3 6
SoftwareSerial esp8266(2,3);// 2 ->RX   3->TX
 
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(rah3, OUTPUT);
  digitalWrite(ledPin, LOW);
  // put your setup code here, to run once:
  esp8266.begin(115200);
  /*
   * در صورت جواب ندادن  ، از باد ریت 9600 و یا 57600 استفاده کنید
   *
   * به علت ورژن های مختلفاین ماژول ممکن است بادریت ها متفاوت باشد
   */
  Serial.begin(115200);
}
int value = LOW;
void loop() {
 // esp8266.listen();
  // put your main code here, to run repeatedly:
  String request="";
  if (esp8266.available() > 0)
  {
    while(esp8266.available()){
      char c=esp8266.read();
      request+=c;
    }
    Serial.println(request);
  }
  //-------------------------------------------------------
  //
  //-----------------------------------------------------------
  if(request.indexOf("ON") != -1){
     digitalWrite(ledPin, HIGH);
     digitalWrite(rah3, LOW);
     value = HIGH;
     Serial.println(request.indexOf("ON"));
  }
  else if(request.indexOf("OFF") != -1){
    digitalWrite(ledPin, LOW);
    digitalWrite(rah3, HIGH);
     value = LOW;
     Serial.println(request.indexOf("OFF"));
     Serial.println(request.length());
  }
  //----------------------------------------------------------
  if (Serial.available() > 0)
  {
    delay(1000);
    String command = "";
    while (Serial.available())
    {
      command += (char)Serial.read();
 
    }
    esp8266.println(command);
  }
 
}
