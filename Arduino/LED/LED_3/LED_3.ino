void setup() {
  // put your setup code here, to run once:
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(9,1);
  delay(500);
  digitalWrite(9,0);
  digitalWrite(10,1);
  delay(500);
  digitalWrite(10,0);
  digitalWrite(11,1);
  delay(500);
  digitalWrite(11,0);  
  for(int i=0;i<=1;i++){
    for(int j=0;j<=1;j++){
      for(int l=0;l<=1;l++){
        if(i==1){
          digitalWrite(9,1);
        }
        else{
          digitalWrite(9,0);
        }
        if(j==1){
          digitalWrite(10,1);
        }
        else{
          digitalWrite(10,0);
        }
        if(l==1){
          digitalWrite(11,1);
        }
        else{
          digitalWrite(11,0);
        }
        delay(1000);
      }
    }
  }
}
