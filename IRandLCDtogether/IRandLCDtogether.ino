#include <IRremote.h> //for IR remote control
#include <LiquidCrystal.h> //for using the LCD screen

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //initializing used pins

const int RECV_PIN = 7; //initializing pin used for IR
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

//for LED data passthrough
const int redPin = 10;
const int greenPin = 9;

//possible states for springs
char selectedState;
float frontSpring; //F
float rearSpring; //denoted as B for now
float leftSpring; //L
float rightSpring; //R

char activeSpring; //which spring value to set/change

String activeString; //user inputted desired value

int statusActive; //for scrolling through all states, idk why I made this functionality.

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  //begins the LCD screen, giving input designated size of screen
  lcd.begin(16, 2);

  //pinMode setup, for testing output states
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT); //blinks to confirm successful input value
}

void loop(){
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        
//results.value != key_value && 
        if (results.value != 0XFFFFFFFF) {
          lcd.setCursor(0,0);
          lcd.clear();
        }

        if (results.value == 0xFFE21D && results.value == key_value) {
          statusActive++;
          if (statusActive > 3) {
            statusActive = 0;
          }
        }
        else if (results.value == 0xFFE21D) {
          statusActive = 0;
        }
        
        switch(results.value){
          case 0XFFFFFFFF:
          break;
          
          case 0xFF6897:
          activeString += "0";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
          
          case 0xFF30CF:
          activeString += "1";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
          
          case 0xFF18E7:
          activeString += "2";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
          
          case 0xFF7A85:
          activeString += "3";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;  
          
          case 0xFF10EF:
          activeString += "4";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
                        
          case 0xFF38C7:
          activeString += "5";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
           
          case 0xFF5AA5:
          activeString += "6";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break; 
          
          case 0xFF42BD:
          activeString += "7";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
          
          case 0xFF4AB5:
          activeString += "8";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
          
          case 0xFF52AD:
          activeString += "9";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;
          
          case 0xFF629D:
//          lcd.print("Vol+"); //forward spring
          activeSpring = 'F';
          lcd.print(activeSpring);
          lcd.setCursor(0,1); //second row
          lcd.print(frontSpring);
          break ;
          
          case 0xFFA857:
//          lcd.print("Vol-"); //rear spring
          activeSpring = 'B';
          lcd.print(activeSpring);
          lcd.setCursor(0,1); //second row
          lcd.print(rearSpring);
          break ;
          
          case 0xFFC23D:
//          lcd.print("Next"); //rear spring
          activeSpring = 'R';
          lcd.print(activeSpring);
          lcd.setCursor(0,1); //second row
          lcd.print(rightSpring);
          break ;
          
          case 0xFF22DD:
//          lcd.print("Previous"); //rear spring
          activeSpring = 'L';
          lcd.print(activeSpring);
          lcd.setCursor(0,1); //second row
          lcd.print(leftSpring);
          break ;
          
          case 0xFF02FD:
          lcd.print("Play/Stop"); //execute movements/calculations
          if (digitalRead(redPin) == 1) { //0 if low, 1 if high
            digitalWrite(redPin, LOW); //stop actions
          }
          else {
            digitalWrite(redPin, HIGH); //start actions
          }
          break ;
          
          case 0xFF906F: //linear actuators up
          lcd.print("Up");
          break ;
          
          case 0xFFE01F: //linear actuators down
          lcd.print("Down");
          break ;
          
          case 0xFFA25D:
          lcd.print("Power");
          break ;
          
          case 0xFFE21D: //show all states
//          lcd.print("Func/Stop");
          switch(statusActive){
            case 0:
            lcd.print("Front:");
            lcd.setCursor(0,1);
            lcd.print(frontSpring);
            break;
            case 1:
            lcd.print("Right:");
            lcd.setCursor(0,1);
            lcd.print(rightSpring);
            break;
            case 2:
            lcd.print("Rear:");
            lcd.setCursor(0,1);
            lcd.print(rearSpring);
            break;
            case 3:
            lcd.print("Left:");
            lcd.setCursor(0,1);
            lcd.print(leftSpring);
            break;
            
          }
          
          break ;

          
          case 0xFF9867:
          lcd.print("EQ"); //decimal place... for now
          activeString += ".";
          lcd.setCursor(0,1); //second row
          lcd.print(activeString);
          break;     
          
          case 0xFFB04F: //set the new value for the activeSpring
//          lcd.print("ST/REPT");
          lcd.print(activeSpring); //prints what spring it is writing to
          lcd.setCursor(0,1); //set cursor to second row
          
          boolean completion = true;
          
          switch(activeSpring) {
            case 'F':
            frontSpring = activeString.toFloat();
            lcd.print(frontSpring);
            break;
            
            case 'B':
            rearSpring = activeString.toFloat();
            lcd.print(rearSpring);
            break;

            case 'L':
            leftSpring = activeString.toFloat();
            lcd.print(leftSpring);
            break;

            case 'R':
            rightSpring = activeString.toFloat();
            lcd.print(rightSpring);
            break;

            default:
            completion = false;
            
          } //end switchFindActiveSpring

          if(completion) {
            activeString = "";
            activeSpring = 0;
            
            digitalWrite(greenPin, HIGH); //blink greenLED to confirm set
            delay(500);
            digitalWrite(greenPin, LOW);
          }
          
          break ;
        } //end switchRemote
        if(results.value != 0XFFFFFFFF) {
          key_value = results.value;
        }
        irrecv.resume(); 
  }
}

