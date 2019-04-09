//Tony Wang
//GMU Mechanical Engineering Multidisciplinary: Senior Design 2018-2019
//other pertinent info
//sources and credits:....tbd

#include <Keypad.h> //required for Keypad library to be used
//reference link: https://playground.arduino.cc/Code/Keypad/
//more arduino matrixing:   https://www.arduino.cc/en/Tutorial/RowColumnScanning
//ghosting/masking: http://weirdscience.us/index.php/2017/03/01/ghosting-and-masking/

const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};

byte rowPins[rows] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[cols] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

void setup(){
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();

  if (key != NO_KEY){
    Serial.println(key);
  }
}
