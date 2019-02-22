//Tony Wang
//GMU Mechanical Engineering Multidisciplinary: Senior Design 2018-2019
//other pertinent info
//sources and credits:....tbd

#include <IRremote.h> //for IR remote control
#include <LiquidCrystal.h> //for using the LCD screen


//PIN DECLARATIONS______________________________________________
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //initializing used pins
const int RECV_PIN = 8; //initializing pin used for IR
const int redPin = 10; //for LED data passthroughs
const int greenPin = 9; //for LED data passthroughs
//for LBSA-1 output control
const int ENABLE1 =11; //Pulse pin on the board
const int DIR1 = 12; //Direction pin on the board
const int PULSE1 = 13; //Enable pin on the board
//for LBSA-2 output control
const int ENABLE2 = 14; //Pulse pin on the board
const int DIR2 = 15; //Direction pin on the board
const int PULSE2 = 16; //Enable pin on the board
// //for LBSA-3 state control
// int ENABLE1STATE = ;
// int DIR1STATE = ;
// int PULSE1STATE = ;

char *lbsaArray[] = {"front","left","rear","right"};

int enableArray[] = {11,14}; //front,left,rear,right
int dirArray[] = {12,15}; //front,left,rear,right
int pulseArray[] = {13,16}; //front,left,rear,right

//GLOBAL VARIABLE DECLARATIONS_________________________________
IRrecv irrecv(RECV_PIN); //initialize the IR receiver object
decode_results results; //results from IR sensors but wtf type is this

unsigned long key_value = 0; //current/previous hex value

int stepsToInch = 4050; //current arbitrary value for calculating inches to steps

float numberOfSteps = 1600; //some arbitrary value/related to Microsteps and LBSAs

int pulseDuration = 200;

// float stepDiff = 0;

// float currentSpringValues[] = [0,0,0,0] //front,left,rear,right
// float newSpringValues[] = [0,0,0,0] //front,left,rear,right

//spring values and activeSpring, and user input activeString
float frontSpringValue; //F where it is
float newFrontSpringValue; //where it wants to go

float rearSpringValue; //B where it is
float newRearSpringValue; //where it wants to go

float leftSpringValue; //L where it is
float newLeftSpringValue; //where it wants to go

float rightSpringValue; //R where it is
float newRightSpringValue; //where it wants to go

char activeSpring = -1; //which spring value to set/change
String activeString; //user inputted wanted value


//IR HEX VALUES TO VARIABLES
const unsigned long zeroButton = 0xFF6897;
const unsigned long oneButton = 0xFF30CF;
const unsigned long twoButton = 0xFF18E7;
const unsigned long threeButton = 0xFF7A85;
const unsigned long fourButton = 0xFF10EF;
const unsigned long fiveButton = 0xFF38C7;
const unsigned long sixButton = 0xFF5AA5;
const unsigned long sevenButton = 0xFF42BD;
const unsigned long eightButton = 0xFF4AB5;
const unsigned long nineButton = 0xFF52AD;
const unsigned long volumeUpButton = 0xFF629D;
const unsigned long volumeDownButton = 0xFFA857;
const unsigned long nextButton = 0xFFC23D;
const unsigned long backButton = 0xFF22DD;
const unsigned long playStopButton = 0xFF02FD;
const unsigned long upButton = 0xFF906F;
const unsigned long downButton = 0xFFE01F;
const unsigned long powerButton = 0xFFA25D;
const unsigned long funcStopButton = 0xFFE21D;
const unsigned long eqDecimalButton = 0xFF9867;
const unsigned long stReptLBSAScrollButton = 0xFFB04F;


void setup(){
  Serial.begin(9600); //I believe this begins the Serial Monitor?
  irrecv.enableIRIn(); //I think this enables the IR sensor input ability?
  irrecv.blink13(true); //wtf does this do?? //TODO

  lcd.begin(16, 2); //begins the LCD screen, giving input designated size of screen

  //LED pinModes, for visual cues
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  //LBSA1 pinMode declaration
  pinMode(ENABLE1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PULSE1, OUTPUT);

  //LBSA2 pinMode declaration
  pinMode(ENABLE2, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(PULSE2, OUTPUT);

  // //LBSA3 pinMode declaration
  // pinMode(ENABLE3, OUTPUT);
  // pinMode(DIR3, OUTPUT);
  // pinMode(PULSE3, OUTPUT);
}

//takes in cur spring val and new spring val, cur spring, and it's 3 pinNums //TODO ISSUE WITH ROUNDING
float moveSpecificLBSA(float currentSpringValue, float newSpringValue, String currentSpring, int dir, int enable, int pulse) {
  lcd.print("Was:" + String(currentSpringValue) + " To:" + String(newSpringValue));

  //doing math conversion from inches to steps
  Serial.println("new " + currentSpring + " is: " + String(newSpringValue));
  Serial.println("current " + currentSpring + " is: " + String(currentSpringValue));
  numberOfSteps = abs(newSpringValue - currentSpringValue) * stepsToInch;
  Serial.println("number of steps to take is: " + String(numberOfSteps));

  for(int i = 0; i < numberOfSteps; i++) {
    if((newSpringValue - currentSpringValue) < 0) {
      digitalWrite(dir, LOW); //THIS GOES BACKWARDS FOR NOW //TODO GIVE VALUE FOR HIGH
    }
    else {
      digitalWrite(dir, HIGH); //THIS GOES Forward or away from the motor
    }
    digitalWrite(enable, HIGH); //TODO GIVE VALUE FOR HIGH //maybe we  don't need this
    digitalWrite(pulse, HIGH); //activates pulse
    delayMicroseconds(pulseDuration);
    digitalWrite(pulse, LOW); //turns off the pulse
    delayMicroseconds(pulseDuration);
  }
  return newSpringValue;
}

float concurrent_movement_LBSAs() {
  // float deltaArray[] = { newFrontSpringValue - frontSpringValue, newLeftSpringValue - leftSpringValue, newRightSpringValue - rightSpringValue, newRearSpringValue - rearSpringValue};
  //for testing only
  // float deltaArray[] = { newSpringValues[0] - currentSpringValues[0], newSpringValues[1] - currentSpringValues[1]};
  float deltaArray[] = { newFrontSpringValue - frontSpringValue, newLeftSpringValue - leftSpringValue};
  int arraySize = sizeof(deltaArray)/sizeof(float); //TESTING, CHANGE IT TO

  //find distances that need to be traveled
  float deltaMin = deltaArray[0]; //set up the deltaMin first
  // Serial.print("Size of deltaArray: ");
  // Serial.println(sizeof(deltaArray));
  for(int i=1; i < arraySize; i++) {
    if ( deltaArray[i] != 0 ) {
      Serial.println("DeltaArray"); //TESTING
      Serial.println(deltaArray[i]); //TESTING
      deltaMin = min(deltaMin, abs(deltaArray[i]));
      // Serial.println(deltaMin); //TESTING
    }
  }

  //do pulseDuration math
  numberOfSteps = abs(deltaMin) * stepsToInch;


  //determine directions of each LBSA
  for(int i=0; i < arraySize; i++) {
    if(deltaArray < 0) {
      digitalWrite(dirArray[i], LOW);
    }
    else {
      digitalWrite(dirArray[i], HIGH);
    }
    digitalWrite(enableArray[i], HIGH); //idk about this yet
    digitalWrite(pulseArray[i], HIGH);
  }


  Serial.println("Running pulses");
  //run the pulse for this number of steps
  for (int i=0; i < numberOfSteps; i++) {
    delayMicroseconds(pulseDuration);
  }

  //turn off all the pins
  for(int i=0; i < arraySize; i++) {
    digitalWrite(pulseArray[i], LOW);
  }
  Serial.println("Finished running pulses"); //TESTING

  //do distance updation
  for(int i=1; i < arraySize; i++) {
    if ( deltaArray[i] > 0 ) {
      deltaArray[i] = deltaArray[i] - deltaMin;
    }
    else if ( deltaArray[i] < 0) {
      deltaArray[i] = deltaArray[i] + deltaMin;
    }
  }
  Serial.println("new Delta values:");
  for(int i=0; i < arraySize; i++) {
    Serial.println(deltaArray[i]);
  }

  // //as long as there is a non-zero delta, recursive call
  // //can probably put this in the calling function?
  // for(int i=1; i < arraySize; i++) {
  //   if ( deltaArray[i] != 0 ) {
  //     concurrent_movement_LBSAs();
  //     break;
  //   }
  // }

}


void loop(){
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX); //this is for debugging, shows IR input in Serial Monitor

        //results.value != key_value &&
        if (results.value != 0XFFFFFFFF) {  //This resets the lcd screen each time a new button is pressed
          lcd.setCursor(0,0);
          lcd.clear();
        }

        if (results.value == 0xFFE21D && activeSpring == -1) { //if its the first time pressing the funct/stop key
          activeSpring = 0; //0 = Front
        }
        else if (results.value == 0xFFE21D && activeSpring != -1) {
          activeSpring++;
          if (activeSpring > 3) {
            activeSpring = 0;
          }
        }
        //Holding down forward/right lbsa1
        else if (key_value == 0xFFC23D && results.value == 0XFFFFFFFF) { //TODO THIS DOESNT WORK?
          results.value = key_value;
        }
        //Holding down back/left
        else if (key_value == 0xFF22DD && results.value == 0XFFFFFFFF) {
          results.value = key_value;
        }
        //Holding down up/down buttons lbsa2
        else if (key_value == 0xFF906F && results.value == 0XFFFFFFFF) { //TODO THIS DOESNT WORK?
          results.value = key_value;
        }
        //Holding down back/left
        else if (key_value == 0xFFE01F && results.value == 0XFFFFFFFF) {
          results.value = key_value;
        }
        else if (key_value == 0xFF02FD && results.value == 0XFFFFFFFF) {
          results.value = key_value;
        }

		Serial.println(zeroButton);
		if (results.value == zeroButton) {
			Serial.println("results.value is a zero hex value dude...");
		}

		// lcd.setCursor(0,1); //second row //default set to second row, eliminates ~13 lines of code
        switch(results.value){
          case 0XFFFFFFFF: //wtf is this for?? //TODO
          break;

          case zeroButton:
          activeString += "0";
          lcd.print(activeString);
          break;

          case oneButton:
          activeString += "1";
          lcd.print(activeString);
          break;

          case twoButton:
          activeString += "2";
          lcd.print(activeString);
          break;

          case threeButton:
          activeString += "3";
          lcd.print(activeString);
          break;

          case fourButton:
          activeString += "4";
          lcd.print(activeString);
          break;

          case fiveButton:
          activeString += "5";
          lcd.print(activeString);
          break;

          case sixButton:
          activeString += "6";
          lcd.print(activeString);
          break;

          case sevenButton:
          activeString += "7";
          lcd.print(activeString);
          break;

          case eightButton:
          activeString += "8";
          lcd.print(activeString);
          break;

          case nineButton:
          activeString += "9";
          lcd.print(activeString);
          break;

          case volumeUpButton:
          lcd.print("Vol+"); //TODO NEED A DIFF KEY FOR THIS
          //set pulseDuration i to active value
          pulseDuration = activeString.toFloat();
          activeString="";

          break;

          //calculates direction and stepDifference for the front Spring
          //FRONT LBSA1
          case volumeDownButton:
            frontSpringValue = moveSpecificLBSA(frontSpringValue,newFrontSpringValue, "Front", DIR1, ENABLE1, PULSE1);
          break;

		      //Next Button
          //Direction LOW
          //LEFT LBSA2
          case nextButton:
            leftSpringValue = moveSpecificLBSA(leftSpringValue,newLeftSpringValue, "Left", DIR2, ENABLE2, PULSE2);
          break;

          //Previous Button
          case backButton:
          // lcd.print("DOWN: DIR1 LOW");
          break;

          case playStopButton:
          lcd.print("Play/Stop"); //execute movements/calculations
          digitalWrite(redPin, HIGH); //start actions

          concurrent_movement_LBSAs();



          digitalWrite(redPin, LOW); //stop actions

          break;

          case upButton: //linear actuators up
		      // lcd.print("UP: DIR2 LOW");
          break;

          case downButton: //linear actuators down
          // lcd.print("DOWN: DIR2 HIGH");
          break;

	        //NOTE: Power Button
          case powerButton:
            activeString="";
      		  lcd.setCursor(0,0); //first column, first row
      		  lcd.print("Nothing before <");
      		  lcd.setCursor(0,1); //first column, second row
      		  lcd.print(activeString); //does a printcheck, if printed something is wrong
      		  lcd.print("<");
          break;

	        //Func/Stop button
          case funcStopButton:
		        switch(activeSpring){
              case 0:
              lcd.print("Front: ");
              lcd.print(frontSpringValue,6);
              break;
              case 1:
              lcd.print("Left: ");
              lcd.print(leftSpringValue,6);
              break;
              case 2:
              lcd.print("Rear: ");
              lcd.print(rearSpringValue,6);
              break;
              case 3:
              lcd.print("Right: ");
              lcd.print(rightSpringValue,6);
              break;
            }
          break;

          //Note: this is "EQ" button
          case eqDecimalButton:
          activeString += ".";
          lcd.print(activeString);
          break;

	        //Note: this is "ST/REPT" button
          case stReptLBSAScrollButton: //set the new value for the activeSpring
            // lcd.setCursor(0,0); //first column, first row
  		  // lcd.print(activeSpring); //prints what spring it is writing to
            // lcd.setCursor(0,1); //set cursor to second row

            boolean completion = true;
            switch(activeSpring) {
              case 0: //front
              newFrontSpringValue = activeString.toFloat();
              lcd.print("Front set: ");
  			      lcd.print(newFrontSpringValue,6);
              break;

              case 1: //left
              newLeftSpringValue = activeString.toFloat();
              lcd.print("Left set: ");
  			      lcd.print(newLeftSpringValue,6);
              break;

              case 2: //rear
              newRearSpringValue = activeString.toFloat();
              lcd.print("Rear set: ");
  			      lcd.print(newRearSpringValue,6);
              break;

              case 3: //right
              newRightSpringValue = activeString.toFloat();
              lcd.print("Right set: ");
  			      lcd.print(newRightSpringValue,6);
              break;

              default:
              completion = false;
            } //end switchFindActiveSpring

            if(completion) { //error handling?? lmao
              activeString = "";
              activeSpring = -1; //TODO do we want set the activeSpring to "null" after making a set

              digitalWrite(greenPin, HIGH); //blink greenLED to confirm set
              delay(500);
              digitalWrite(greenPin, LOW);
            }
      		  else {
      			digitalWrite(redPin, HIGH); //blinks redLED failure!
                  delay(500);
                  digitalWrite(redPin, LOW);
      		  }
          break;
        } //end switchRemote

        if(results.value != 0XFFFFFFFF) { //TODO: wtf does this do?
          key_value = results.value;
        }
        irrecv.resume();
  }
}
