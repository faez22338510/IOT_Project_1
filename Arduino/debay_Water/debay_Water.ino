#define Motor 6
void setup() {
  // put your setup code here, to run once:
  pinMode(Motor,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(Motor, 900);
  delay(12400);
  analogWrite(Motor,0);
  delay(1000000);
}
