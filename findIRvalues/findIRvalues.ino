#include <IRremote.h>

const int RECV_PIN = 0;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  if (irrecv.decode(&results)){
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
}
