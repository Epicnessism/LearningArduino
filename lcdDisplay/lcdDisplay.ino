#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2);
//  lcd.print("Hello Dave, I have been expecting you.");
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("hello, world!");
  delay(750);
  lcd.clear();
  delay(600);
  lcd.setCursor(1, 1);
  lcd.print("hello, world!");
  delay(750);
  lcd.clear();
  delay(400);
}

