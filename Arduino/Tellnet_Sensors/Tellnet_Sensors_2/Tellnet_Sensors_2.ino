#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"

#define Motor 6
#define DHTPIN 5


#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 8);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 2);

EthernetClient client;
int timer = 1400;


void setup() {
  pinMode(13,OUTPUT);
  pinMode(Motor,OUTPUT);
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
   dht.begin();
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 2000)) {
    Serial.println("connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}


int turn=0;

void loop() {
  int soil;
  int temp;
  int humidity;
//-------------------------------------------------------------
  if (timer++ == 2000) {
    timer = 0;
    Serial.println();
    
    soil = analogRead(A0); // Soil moisture
    humidity = (int)dht.readHumidity(); // Air humidity
    temp = (int)dht.readTemperature(); // Air Temperature
    
    char tBuffer[16];
    sprintf(tBuffer,"%u %u %u",temp,soil,humidity);
    client.write(tBuffer);
    Serial.println(tBuffer);
    
  }
//-------------------------------------------------------------
  if (client.available() > 0) {
    
    String command = "";
    while (client.available())
    {
      command += (char)client.read();
    }

    if(command == "on"){
        digitalWrite(13,1);
        turn = 1;
        digitalWrite(Motor, 1);
        //delay(1000);
        Serial.print("ON");
    }
    else if(command == "off"){
      digitalWrite(13,0);
      turn = 0;
      digitalWrite(Motor, 0);
      Serial.print("OFF");
    }
    Serial.print(command);
  }
//-------------------------------------------------------------
  delay(10);
  if(timer%100 == 0)
    Serial.print(timer/100);
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
    while (true);
  }
}




