#include <SoftwareSerial.h>

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
    if(c==':'){
      isMessage=true;
    }
    if(isMessage){
      response+= c;
      Serial.write(c);
    }
  }
  Serial.println(response);
  if(response==":OK"){
    Serial.println("YEEESs");
  }
  if (Serial.available() > 0)
  {
    delay(1000);
    String command = "";
    while (Serial.available())
    {
      command += (char)Serial.read();
    }
    //Serial.println("MEEE:"+command.substring(0, 5));
    if(command.substring(0, 11)=="AT+CIPSEND="){
      isMessage=false;
    }
    esp8266.println(command);
  }

}
