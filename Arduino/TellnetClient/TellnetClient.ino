#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 8);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 3);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use port 10002):
EthernetClient client;

void setup() {
  pinMode(7,OUTPUT);
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


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
  int temp;
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    if(c == 'a'){
      if(turn == 0){
        digitalWrite(7,1);
        turn = 1;
        //Serial.print(turn);
      }
      else{
        digitalWrite(7,0);
        turn = 0;
        //Serial.print("OFF");
      }
      temp = analogRead(A0);
      String StringTemp =  String(temp);
      Serial.print(StringTemp);
      char tBuffer[6];
      sprintf(tBuffer,"%u",temp);
      client.write(tBuffer);
      //char inchar = (char)temp;
      //client.print(inchar);
    }
    Serial.print(c);
  }

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (client.connected()) {
      client.print(inChar);
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
    while (true);
  }
}




