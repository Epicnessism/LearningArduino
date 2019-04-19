//Tony Wang
//GMU Mechanical Engineering Multidisciplinary: Senior Design 2018-2019
//other pertinent info
//sources and credits:....tbd

#include <IRremote.h> //for IR remote control
#include <LiquidCrystal.h> //for using the LCD screen
#include <Keypad.h> //required for Keypad matrix to be used


//PIN DECLARATIONS______________________________________________
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //initializing used pins
const int RECV_PIN = 10; //initializing pin used for IR
const int redPin = 8; //for LED data passthroughs
const int greenPin = 9; //for LED data passthroughs


//controller pin setup
const byte rows = 4; //four rows
const byte cols = 6; //three columns
char keys[rows][cols] = {
  {'1', '2', '3', 's', 'f', 'b'},
  {'4', '5', '6', 'd', 'l', 'r'},
  {'7', '8', '9'},
  {'.', '0', 'e', 'A', 'B', 'C'}
};

byte rowPins[rows] = {34, 35, 36, 37}; //connect to the row pinouts of the keypad
byte colPins[cols] = {38, 39, 40, 41, 42, 43}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

char *lbsaArray[] = {"front","left","rear","right"};

const int enableArray[] = {22,25,28,31}; //front,left,rear,right
const int dirArray[] = {23,26,29,32}; //front,left,rear,right
const int pulseArray[] = {24,27,30,33}; //front,left,rear,right

//GLOBAL VARIABLE DECLARATIONS_________________________________
IRrecv irrecv(RECV_PIN); //initialize the IR receiver object
decode_results results; //results from IR sensors but wtf type is this

//OPTIMAL SETTINGS:
//PULSEDURATION: 200 MS
//1600 Microsteps
//guesstimated stepstoInche = 4050

unsigned long key_value = 0; //current/previous hex value
int stepsToInch = 4050; //current arbitrary value for calculating inches to steps
float numberOfSteps = 1600; //some arbitrary value/related to Microsteps and LBSAs
int pulseDuration = 200;
float currentSpringValues[] = {0,0,0,0}; //front,left,rear,right
float newSpringValues[] = {0,0,0,0}; //front,left,rear,right
float deltaArray[] = {0,0,0,0}; //make deltaArray a global variable
char activeSpring = -1; //which spring value to set/change
String activeString; //user inputted wanted value

//IR values for black remote in dvd protocol
//function buttons
const unsigned long powerButton = 0x595E13FA; //dvd
const unsigned long menuButton = 0xFBCEF2FA; //dvd
const unsigned long playButton = 0xA887B57F; //dvd
const unsigned long homeButton = 0xAE89EB62; //dvd
const unsigned long enterButton = 0x7404BC5A; //dvd
//number buttons
const unsigned long zeroButton = 0x7885BE56; //dvd
const unsigned long oneButton = 0x4C0F819F; //dvd
const unsigned long twoButton = 0x53A4A1A2; //dvd
const unsigned long threeButton = 0xFF1145E3; //dvd
const unsigned long fourButton = 0x25507BFA; //dvd
const unsigned long fiveButton = 0xA9052DBB; //dvd
const unsigned long sixButton = 0x2B0E02BE; //dvd;
const unsigned long sevenButton = 0xD67AA6FF; //dvd
const unsigned long eightButton = 0xE7F5ED5A; //dvd
const unsigned long nineButton = 0xA772439B; //dvd
const unsigned long decimalButton = 0xF708557B; //dvd

const unsigned long channelBackButton = 0xB507765B; //dvd
const unsigned long undoButton =  0x6D89E7DE;//dvd //bottom left
const unsigned long preset1Button = 0xD5FD2EC3; //dvd
const unsigned long frontButton = 0x9BD466C2; //dvd
const unsigned long rightButton = 0xDFAA9A1F; //dvd
const unsigned long rearButton = 0xFF2F577B; //dvd
const unsigned long leftButton = 0x343DF5DE; //dvd

void setup(){
  Serial.begin(9600); //I believe this begins the Serial Monitor?
  irrecv.enableIRIn(); //I think this enables the IR sensor input ability?
  irrecv.blink13(true); //wtf does this do?? //TODO

  lcd.begin(16, 2); //begins the LCD screen, giving input designated size of screen

  //LED pinModes, for visual cues
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  //LBSA1 pinMode declaration
  pinMode(enableArray[0], OUTPUT);
  pinMode(dirArray[0], OUTPUT);
  pinMode(pulseArray[0], OUTPUT);

  //LBSA2 pinMode declaration
  pinMode(enableArray[1], OUTPUT);
  pinMode(dirArray[1], OUTPUT);
  pinMode(pulseArray[1], OUTPUT);

  //LBSA3 pinMode declaration
  pinMode(enableArray[2], OUTPUT);
  pinMode(dirArray[2], OUTPUT);
  pinMode(pulseArray[2], OUTPUT);

  //LBSA4 pinMode declaration
  pinMode(enableArray[3], OUTPUT);
  pinMode(dirArray[3], OUTPUT);
  pinMode(pulseArray[3], OUTPUT);
}

//resets all the lbsa
float reset_LBSAs() {
  // newSpringValues = {0,0,0,0} //front,left,rear,right //sets everything to 0
  int arraySize = sizeof(newSpringValues)/sizeof(float);
  for (int i = 0; i < arraySize; i++) {
    if(newSpringValues[i] != 0) {
      newSpringValues[i] = 0;
    }
  }
  concurrent_movement_LBSAs();
}

float concurrent_movement_LBSAs() {
  computeDeltaArray(); //computes deltaArray differences

  int arraySize = sizeof(deltaArray)/sizeof(float); //TESTING, CHANGE IT TO
  float deltaMin = 15; //set up the deltaMin first //this is some arbitrarily high value

  //for however big deltaArray is, loop through and find the smallest delta
  //set that as the new deltaMin
  for(int i=0; i < arraySize; i++) {
    Serial.print("DeltaArray: "); //TESTING
    Serial.println(deltaArray[i]); //TESTING
    if ( deltaArray[i] != 0 ) {
      deltaMin = min(deltaMin, abs(deltaArray[i]));
      Serial.print("DeltaMin: "); //TESTING
      Serial.println(deltaMin); //TESTING
    }
  }
  if (deltaMin == 15) {
    lcd.print("|");
    return -1; //no movement
  }
  else {
    lcd.print(".");
    //as long as there is a non-zero delta, keep moving
    move_LBSAs(deltaMin, arraySize);
    for(int i = 0; i < arraySize; i++) {
      if ( deltaArray[i] != 0 ) {
        concurrent_movement_LBSAs();
      }
    }
  }
}

void computeDeltaArray() {
  for (int i=0; i < sizeof(deltaArray)/sizeof(float); i++) {
    Serial.print("DArray Before: "); //TESTING
    Serial.println(deltaArray[i]); //TESTING
    deltaArray[i] = newSpringValues[i] - currentSpringValues[i];
    Serial.print("DArray After: "); //TESTING
    Serial.println(deltaArray[i]); //TESTING
  }
}

float move_LBSAs (float deltaMin, int arraySize) {
  //do pulseDuration math
  numberOfSteps = abs(deltaMin) * stepsToInch; //convert deltaMin inches to steps
  Serial.print("Number Of Steps: "); //TESTING
  Serial.println(numberOfSteps); //TESTING

  //run the pulse for this number of steps
  for (int i=0; i < numberOfSteps; i++) {
    for (int i=0; i < arraySize; i++) { //inner forloop activates all the lbsas prior to stepping
      if(deltaArray[i] != 0) { //dont activate lbsa if there is no delta
        if(deltaArray[i] < 0) { //checeks direction lbsa needs to move
          digitalWrite(dirArray[i], LOW); //low is [ ]
        }
        else {
          digitalWrite(dirArray[i], HIGH); //high is [ ]
        }
        digitalWrite(enableArray[i], HIGH); //this magical thing...
        //sets the pulse to high...assumedly it goes fast enough that it doesn't matter
        digitalWrite(pulseArray[i], HIGH);
      }
    }
    delayMicroseconds(pulseDuration); //the pulse
    for (int i=0; i < arraySize; i++) {
      if(deltaArray[i] != 0) {
        // Serial.println("Off");
        digitalWrite(pulseArray[i], LOW); //turns the lbsa off
      }
    }
  }

  //do distance updation
  for(int i=0; i < arraySize; i++) {
    if ( deltaArray[i] > 0 ) {
      currentSpringValues[i] = currentSpringValues[i] + deltaMin;
    }
    else if ( deltaArray[i] < 0) {
      currentSpringValues[i] = currentSpringValues[i] - deltaMin;
    }
    Serial.print("new current Position value: "); //visual feedback //add LCD feedback later
    Serial.println(currentSpringValues[i]);
  }
}


void loop(){
  //keypad control method
  char key = keypad.getKey();
  if (key != NO_KEY){
    Serial.println(key);
  }
  //check if the same as the last key, lcd.clear()
  switch(key) {
    case '1':
      activeString += "1";
      lcd.print(activeString);
      break;
    case '2':
      activeString += "2";
      lcd.print(activeString);
      break;
    case 'e':
      digitalWrite(redPin, HIGH); //start actions
      concurrent_movement_LBSAs();
      digitalWrite(redPin, LOW); //stop actions
      break;
  }

  //IR controller method
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX); //this is for debugging, shows IR input in Serial Monitor

        //results.value != key_value &&
        if (results.value != 0XFFFFFFFF) {  //This resets the lcd screen each time a new button is pressed
          lcd.setCursor(0,0);
          lcd.clear();
        }

        if (results.value == menuButton && activeSpring == -1) { //if its the first time pressing the funct/stop key
          activeSpring = 0; //0 = Front
        }
        else if (results.value == menuButton && activeSpring != -1) {
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

          //Previous Button
          case undoButton:
            activeString = "";
            lcd.print("Cleared.");
          break;

          // case playStopButton:
          case playButton:
          digitalWrite(redPin, HIGH); //start actions
          concurrent_movement_LBSAs();
          digitalWrite(redPin, LOW); //stop actions
          break;

	        //NOTE: Power Button
          case powerButton: //ie set to zero before turning off
            reset_LBSAs();
          break;

          case preset1Button:
          break;

          // case preset2Button:
          // break;
          // case preset3Button:
          // break;

          case frontButton:
            currentSpringValues[0] = activeString.toFloat();
            lcd.print("Front set: ");
            lcd.print(currentSpringValues[0],10);
            activeString = "";
          break;

          case leftButton:
            currentSpringValues[1] = activeString.toFloat();
            lcd.print("Left set: ");
            lcd.print(currentSpringValues[1],10);
            activeString = "";
          break;

          case rearButton:
            currentSpringValues[2] = activeString.toFloat();
            lcd.print("Rear set: ");
            lcd.print(currentSpringValues[2],10);
            activeString = "";
          break;

          case rightButton:
            currentSpringValues[3] = activeString.toFloat();
            lcd.print("Right set: ");
            lcd.print(currentSpringValues[3],10);
            activeString = "";
          break;

	        //Func/Stop button
          case menuButton:
          // case funcStopButton:
		        switch(activeSpring){
              case 0:
              lcd.print("Front: ");
              lcd.print(currentSpringValues[0],6);
              lcd.setCursor(0,1);
              lcd.print("Target: ");
              lcd.print(newSpringValues[0],6);
              break;
              case 1:
              lcd.print("Left: ");
              lcd.print(currentSpringValues[1],6);
              lcd.setCursor(0,1);
              lcd.print("Target: ");
              lcd.print(newSpringValues[1],6);
              break;
              case 2:
              lcd.print("Rear: ");
              lcd.print(currentSpringValues[2],6);
              lcd.setCursor(0,1);
              lcd.print("Target: ");
              lcd.print(newSpringValues[2],6);
              break;
              case 3:
              lcd.print("Right: ");
              lcd.print(currentSpringValues[3],6);
              lcd.setCursor(0,1);
              lcd.print("Target: ");
              lcd.print(newSpringValues[3],6);
              break;
            }
          break;

          //Note: this is "EQ" button
          case decimalButton:
          activeString += ".";
          lcd.print(activeString);
          break;

	        //Note: this is "ST/REPT" button
          // case stReptLBSAScrollButton: //set the new value for the activeSpring
          case enterButton:
            // lcd.setCursor(0,0); //first column, first row
  		      // lcd.print(activeSpring); //prints what spring it is writing to
            // lcd.setCursor(0,1); //set cursor to second row

            boolean completion = true;
            switch(activeSpring) {
              case 0: //front
              newSpringValues[0] = activeString.toFloat();
              lcd.print("Front set: ");
  			      lcd.print(newSpringValues[0],6);
              break;

              case 1: //left
              newSpringValues[1] = activeString.toFloat();
              lcd.print("Left set: ");
  			      lcd.print(newSpringValues[1],6);
              break;

              case 2: //rear
              newSpringValues[2] = activeString.toFloat();
              lcd.print("Rear set: ");
  			      lcd.print(newSpringValues[2],6);
              break;

              case 3: //right
              newSpringValues[3] = activeString.toFloat();
              lcd.print("Right set: ");
  			      lcd.print(newSpringValues[3],6);
              break;

              default:
              Serial.println("Failed to assign, Completion: False");
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
