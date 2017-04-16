/*
  Arduino Webserver using ESP8266
  Displays temperature in a webpage

  Arduino Mega has three Serial communication ports,this code works well with 
  Arduino Mega.For UNO users,use Softserial library to connect ESP8266 with
  Arduino UNO 
  If you're unsure about your Arduino model or its pin cofigurations,please check
  the documentation at http://www.arduino.cc
 
  modified August 2016
  By Joyson Chinta and Gladson Kujur
 */

#define DEBUG true
 
void setup()
{
  Serial.begin(115200);    ///////For Serial monitor 
  //Serial1.begin(115200); ///////ESP Baud rate
  pinMode(11,OUTPUT);    /////used if connecting a LED to pin 11
  digitalWrite(11,LOW);
  pinMode(9,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}
 
int connectionId;
void loop()
{
  if(Serial.available())
  {
    /////////////////////Recieving from web browser to toggle led
    if(Serial.find("+IPD,"))
    {
     delay(300);
     connectionId = Serial.read()-48;
     if(Serial.find("pin="))
     { 
     int pinNumber = (Serial.read()-48)*10; 
     pinNumber += (Serial.read()-48); 
     digitalWrite(pinNumber, !digitalRead(pinNumber));
     }

     String add1="<h4>DONE</h4>";
     espsend(add1);
     
     String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
  }
}
 
  //////////////////////////////sends data from ESP to webpage///////////////////////////
 
 void espsend(String d)
         {
             String cipSend = " AT+CIPSEND=";
             cipSend += connectionId; 
             cipSend += ",";
             cipSend +=d.length();
             cipSend +="\r\n";
             sendData(cipSend,1000,DEBUG);
             sendData(d,1000,DEBUG); 
         }
//////////////gets the data from esp and displays in serial monitor///////////////////////         
String sendData(String command, const int timeout, boolean debug)
            {
                String response = "";
                Serial.print(command);
                long int time = millis();
                while( (time+timeout) > millis())
                {
                   while(Serial.available())
                      {
                         char c = Serial.read(); // read the next character.
                         response+=c;
                      }  
                }
                return response;
            }
