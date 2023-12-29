//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SHORT_LENGTH 300
int outputA = 9;
int outputB = 7;
int button = 8;
int pot = A0;
int cameraLED = 6;

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor
const int deltaMM = 16;
const float stepsPerMil = stepsPerRevolution / (float)deltaMM;
const int stepsPerQuart = stepsPerMil / 4;

//10 coil 1 yellow
//11 coil 2 orange
//12 coil 3 pink
//13 coil 4 blue
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);   //13,11,10,12
Stepper myStepper2(stepsPerRevolution, 13, 11, 12, 10);   //13,11,10,12
const int SPEED = 2;
int stepCount = 0;

float potVal = 0;
float prevPotVal = 0;
int counter = 1;
int aLastState;
int timer = 0;
int butState;

int minPos = 0;
int curPos = 0;
int maxVal = 0;
int minVal = 0;
int menu = 0;

void calibrate();
void printToLCD();
void setup()
{
  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);
  pinMode (button, INPUT);
  pinMode (cameraLED, OUTPUT);

  Serial.begin(9600);

  myStepper.setSpeed(SPEED);
  myStepper2.setSpeed(SPEED);

  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Position: 0");
  aLastState = digitalRead(outputA);
  butState = digitalRead(button);
  prevPotVal = analogRead(pot);
  calibrate();
  printToLCD();
}

void stp(Stepper ss, int numS = 1, int sSize = stepsPerQuart) {
  ss.step(sSize * numS);
  delay(10);
}

void motStep(int stepsPerStep, int motVel = 1) {
  Stepper mot = myStepper;
  mot.setSpeed(motVel);

  for (int i = 0; i < (int)(32/stepsPerStep); i++) {
    
      lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Autotest running");
  lcd.setCursor(0,1);
  potVal = analogRead(pot);
  //float n = (stepsPerStep*i)/4.00;
  lcd.print((float)(8 * (potVal-minVal))/(maxVal-minVal));
    lcd.print("mm/8.00mm");
    stp(mot, stepsPerStep);
    curPos += stepsPerStep*stepsPerQuart;
    digitalWrite(cameraLED, HIGH);
    delay(250);
    digitalWrite(cameraLED, LOW);
    delay(250);
  }
}
void calibrate() {
  maxVal = prevPotVal;
  minVal = prevPotVal;
  lcd.clear();
  lcd.print("Calibrating: ");
  lcd.setCursor(0,1);
  for (int i = 0; i < stepsPerRevolution; i++ ) {
    myStepper.step(1);
    delay(10);
    if (i % (2048/17) == 0) 
      lcd.print(".");
    potVal = analogRead(pot);
    if ((prevPotVal - potVal) > 2 || (prevPotVal - potVal) < -2)  {
      minVal = (potVal < minVal) ? potVal : minVal;
      if (minVal == potVal) {
        minPos = i;
      }
      maxVal = (potVal > maxVal) ? potVal : maxVal;
      prevPotVal = potVal;
     
    }
  }
  lcd.clear();
  lcd.print("0ing motor pos");
  myStepper.step(minPos);
  lcd.setCursor(0,1);
  lcd.print("Complete!");
  delay(500);
  lcd.clear();
  minPos = 0;
  lcd.print("Calibration res:");
  lcd.setCursor(0,1);
  lcd.print("Max:");
  lcd.print(maxVal);
  lcd.print(" Min:");
  lcd.print(minVal);
  while (!digitalRead(button)) {};
  while (digitalRead(button)) {};
}

// next/exit
void longPress() {
  Serial.println("Long Press");
  counter = 0;
  if (menu == 3)
    menu = 0;
  else
    menu++;
}

void resetPos() {
  do {
    potVal = analogRead(pot);
    myStepper.step(1);
  } while(potVal >= minVal);
}
// start/accept
void shortPress() {
  Serial.println("Short Press");
  if (menu == 0) {
    digitalWrite(cameraLED, HIGH);
    delay(500);
    digitalWrite(cameraLED, LOW);
  }
  else if (menu == 2) {
    motStep(counter);
    myStepper.step(stepsPerRevolution - curPos);
    curPos = 0;
  }
  else if (menu == 1) {
    myStepper.step(stepsPerRevolution - curPos);
    curPos = 0;
  }
  else if (menu == 3) {
    calibrate();
  }
}
void inc() {
  Serial.println("inc");
  if (menu == 0) {
    stp(myStepper);
    curPos += stepsPerQuart;
  }
  else if (menu == 1) {
    myStepper.step(1);
    curPos += 1;
}
}
void dec() {
  Serial.println("dec");
  if (menu == 0) {
    stp(myStepper2);
    curPos -= stepsPerQuart;
  }
  else if (menu == 1) {
    myStepper2.step(1);
    curPos -= 1;
  }
}

void printToLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menu: ");
  if (menu == 0) {
    lcd.print("Single stp");
    lcd.setCursor(0, 1);
    lcd.print("Step: ");
    lcd.print( (float)(8 * (potVal-minVal))/(maxVal-minVal));
    lcd.print("MM");
  }
  else if (menu == 2) {
    lcd.print("Auto test");
    lcd.setCursor(0, 1);
    lcd.print("Step Size: ");
    lcd.print((float)(counter * 0.25));
    lcd.print("MM");
  }
  else if (menu == 1)  {
    lcd.print("Tuning stp");
    lcd.setCursor(0, 1);
    float dist = (float)(8 * (potVal-minVal))/(maxVal-minVal);
    lcd.print("Pos(MM): ");
    lcd.print(dist);
  }
  else if (menu == 3) 
    lcd.print("Calibrate");
}


void loop()
{
  int bState = digitalRead(outputB);
  int aState = digitalRead(outputA);
  
  if (aState != aLastState) {
    if (bState == aState) {
      counter += (counter < 32) ? 1: 0;
      inc();
    }
    else {
      counter -= (counter > 1) ? 1: 0;
      dec();
    }
    printToLCD();
  }
  potVal = analogRead(pot);
    if ((prevPotVal - potVal) > 2 || (prevPotVal - potVal) < -2)  {
      prevPotVal = potVal;
    }
  int butState = digitalRead(button);
  if (!digitalRead(button)) {
    Serial.println("Press");
    while (!digitalRead(button) && timer <= SHORT_LENGTH) {
      timer++;
      delay(1);
    }
    Serial.print("Release ");
    Serial.println(timer);
    (timer <= SHORT_LENGTH) ? shortPress() : longPress();
    printToLCD();
    timer = 0;
    while (!digitalRead(button)){};
  }
  aLastState = aState;

}
