//Tony Wang
//Senior Design 2018-2019
//testing basic step motor controller
//one revolution forward and back

//6400 steps should = 1 revolution??? under what power settings?

int PULSE=13; //Pulse pin on the board
int DIR=8; //Direction pin on the board
int ENABLE=6; //Enable pin on the board

//define all pins as output pins
//no inputs in this system
void setup() {
  Serial.begin(9600);
  pinMode (PULSE, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENABLE, OUTPUT);
}

void loop() {
  
  for (int i=0; i<10; i++){ //Forward 6400 steps
    digitalWrite(DIR,HIGH); //DIR LOW = FORWARD
    
    digitalWrite(ENABLE,HIGH);
    digitalWrite(PULSE,HIGH);
    delayMicroseconds(100);
    digitalWrite(PULSE,LOW);
    delayMicroseconds(100);
    
  }
  
//  for (int i=0; i<100; i++){ //Backward 6400 steps
//    digitalWrite(DIR,HIGH); //DIR HIGH = BACKWARD
//    
//    digitalWrite(ENABLE,HIGH);
//    digitalWrite(PULSE,HIGH);
//    delayMicroseconds(50);
//    digitalWrite(PULSE,LOW);
//    delayMicroseconds(50);
//  }
}
