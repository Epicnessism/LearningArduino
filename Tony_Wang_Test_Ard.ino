void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(3, OUTPUT); //led on 3
  pinMode(5, OUTPUT); //led on 5
  pinMode(6, OUTPUT); //trigg on 6
  pinMode(9, INPUT); //echo on 9
  pinMode(10, OUTPUT); //speaker on 10
}

void loop() {
  // For alternating lights
//  Serial.print("Hello World");
//  delay(1000);
//  Serial.print("dfadf");
//
//  digitalWrite(3,HIGH);
//  delay(500);
//  digitalWrite(5,HIGH);
//  delay(500);
//  digitalWrite(3,LOW);
//  delay(500);
//  digitalWrite(5,LOW);

//  //For ultrasonic sensor
//  unsigned long distance;
//  digitalWrite(6,LOW); 
//  delayMicroseconds(2);
//  digitalWrite(6,HIGH);
//  delayMicroseconds(10);
//  digitalWrite(6,LOW);
//  distance = pulseIn(9,HIGH);
//  distance = distance/58;
//  distance = distance*.393701l;
//  Serial.println(distance);
//  delay(500);
//  //return(distance);

////  speaker stuff
//  tone(10,2000);
//  delay(100);
//  tone(10,3000);
//  delay(100);
//  tone(10,2000);
//  delay(500);

//new ultrasonic + speaker
  unsigned long distance;
  digitalWrite(6,LOW); 
  delayMicroseconds(2);
  digitalWrite(6,HIGH);
  delayMicroseconds(10);
  digitalWrite(6,LOW);
  distance = pulseIn(9,HIGH);
  distance = distance/58;
  distance = distance*.393701l;
  Serial.println(distance);
  delay(500);
  if(distance > 2 && distance <= 10) {
    tone(10,1000,100);
  }
  else if(distance <= 2) {
    tone(10,3500,100);
    delay(50);
  }
}


