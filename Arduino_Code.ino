#include <Adafruit_TCS34725.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Keypad_I2C.h>

#define BTSerial Serial2

const float motorAngle = 1.8;
const float stepSize = 1;
unsigned long rpm = 200;  // Default Speed - min = 100 max = 400
const int NUM_SAMPLES = 10;

const float Cspu = 222;
const float Mspu = 222;
const float Yspu = 222;
const float Kspu = 222;

#define enPin 8
#define cDirPin 5
#define cStepPin 2
#define mDirPin 6
#define mStepPin 3
#define yDirPin 7
#define yStepPin 4
#define kDirPin 13
#define kStepPin 12
#define irSensorPin 49
#define buzPin A8
#define pinLED A9  //color sensor pin
const int redPin = 45;
const int greenPin = 46;
const int bluePin = 44;


#define cTrigPin 31  // Cyan ultrasonic sensor trig pin
#define cEchoPin 30  // Cyan ultrasonic sensor echo pin
#define mTrigPin 33  // Magenta ultrasonic sensor trig pin
#define mEchoPin 32  // Magenta ultrasonic sensor echo pin
#define yTrigPin 34  // Yellow ultrasonic sensor trig pin
#define yEchoPin 35  // Yellow ultrasonic sensor echo pin
#define kTrigPin 39  // Black ultrasonic sensor trig pin
#define kEchoPin 38  // Black ultrasonic sensor echo pin

bool clean = true, pump = false;
unsigned long Red, Green, Blue, Volume;
float Cyan, Magenta, Yellow, Key, Total;
float Csteps, Msteps, Ysteps, Ksteps;

// Define I2C LCD properties
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Change 0x27 to your LCD's I2C address

// Define keypad properties
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 4, 5, 6, 7 };
byte colPins[COLS] = { 0, 1, 2, 3 };
Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, 0x20);  // Set the keypad address to 0x20

// Color sensor part
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_16X);

void setup() {

  BTSerial.begin(9600);
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(enPin, OUTPUT);
  pinMode(cDirPin, OUTPUT);
  pinMode(cStepPin, OUTPUT);
  pinMode(mDirPin, OUTPUT);
  pinMode(mStepPin, OUTPUT);
  pinMode(yDirPin, OUTPUT);
  pinMode(yStepPin, OUTPUT);
  pinMode(kDirPin, OUTPUT);
  pinMode(kStepPin, OUTPUT);
  pinMode(irSensorPin, INPUT);
  pinMode(pinLED, OUTPUT);  //Color Sensor Pin

  pinMode(cTrigPin, OUTPUT);
  pinMode(cEchoPin, INPUT);
  pinMode(mTrigPin, OUTPUT);
  pinMode(mEchoPin, INPUT);
  pinMode(yTrigPin, OUTPUT);
  pinMode(yEchoPin, INPUT);
  pinMode(kTrigPin, OUTPUT);
  pinMode(kEchoPin, INPUT);

  digitalWrite(enPin, HIGH);
  digitalWrite(pinLED, LOW);  // Turn on the sensor's white LED

  keypad.begin();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("Welcome to FIT MIX!");
  delay(2000);
  lcd.clear();
  displayColorChoice();
}
void displayColorChoice() {
  lcd.clear();
  lcd.print("Choose Color Mode:");
  lcd.setCursor(0, 1);
  lcd.print("1. Custom Color");
  lcd.setCursor(0, 2);
  lcd.print("2. Common Colors");
}
void displayInputMethod() {
  lcd.clear();
  lcd.print("Select Input Method:");
  lcd.setCursor(0, 1);
  lcd.print("1.Using Keypad");
  lcd.setCursor(0, 2);
  lcd.print("2.Using Bluetooth");
  lcd.setCursor(0, 3);
  lcd.print("3.Using Color Sensor");
}



void loop() {
  char choice = getColorChoice();

  if (choice == '1') {
    displayInputMethod();
    char input = getInputMethod();
    switch (input) {
      case '1':
        getInputFromKeypad();
        break;
      case '2':
        getInputFromBT();
        break;
      case '3':
        getInputFromColorSensor();
        break;
      default:
        break;
    }
  } else if (choice == '2') {
    navigateCommonColors();
  }
  delay(1000);  // Optional: Add delay to debounce
}

void navigateCommonColors() {
  const char* colors[] = { "Red", "Green", "Blue", "Yellow", "Pink", "Orange", "Purple", "White", "Black" };
  int index = 0;
  int numColors = sizeof(colors) / sizeof(colors[0]);

  while (true) {
    lcd.clear();
    lcd.print("Common Color:");
    lcd.setCursor(0, 1);
    lcd.print(colors[index]);

    char key = keypad.getKey();
    if (key == '#') {
      setColorByName(colors[index]);
      break;
    } else if (key == '*') {
      index = (index + 1) % numColors;
    }
  }
}

void setColorByName(const char* color) {
  if (strcmp(color, "Red") == 0) {
    Red = 255;
    Green = 0;
    Blue = 0;
  } else if (strcmp(color, "Green") == 0) {
    Red = 0;
    Green = 255;
    Blue = 0;
  } else if (strcmp(color, "Blue") == 0) {
    Red = 0;
    Green = 0;
    Blue = 255;
  } else if (strcmp(color, "Yellow") == 0) {
    Red = 255;
    Green = 255;
    Blue = 0;
  } else if (strcmp(color, "Pink") == 0) {
    Red = 255;
    Green = 192;
    Blue = 203;
  } else if (strcmp(color, "Orange") == 0) {
    Red = 255;
    Green = 165;
    Blue = 0;
  } else if (strcmp(color, "Purple") == 0) {
    Red = 128;
    Green = 0;
    Blue = 128;
  } else if (strcmp(color, "White") == 0) {
    Red = 255;
    Green = 255;
    Blue = 255;
  } else if (strcmp(color, "Black") == 0) {
    Red = 0;
    Green = 0;
    Blue = 0;
  }

  lcd.clear();
  lcd.print("Selected Color:");
  lcd.setCursor(0, 1);
  lcd.print(color);
  delay(2000);  // Display selected color for 2 seconds
  setLightColor();
  lcd.clear();
  lcd.print("Enter Volume (ml):");
  Volume = getNumberFromKeypad();  // Use keypad to get volume
  if (Volume == -2) return;        // Check for reset and return

  if (Volume == -1) {
    lcd.clear();
    lcd.print("Invalid Volume!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("Volume: ");
    lcd.print(Volume);
    lcd.print(" ml");
  }
  getCMYK();
}

void retryMakeColor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bucket not in place");
  lcd.setCursor(0, 2);
  lcd.print("   Press # to Retry");


  while (true) {
    char key = keypad.getKey();
    if (key == '#') {
      makeColor();
      break;
    }
  }
}

void restart() {
  lcd.clear();
  lcd.print("Do you want to make");
  lcd.setCursor(0, 1);
  lcd.print("another color?");
  lcd.setCursor(0, 3);
  lcd.print("Yes(#)        No(*)");

  while (true) {
    char key = keypad.getKey();
    if (key == '#') {
      displayColorChoice();
      break;
    } else if (key == '*') {
      lcd.clear();
      lcd.print("     Thank You!       Happy Painting :) ");
      break;
    }
  }
}


char getInputMethod() {
  char key = keypad.getKey();
  while (!key) {
    key = keypad.getKey();
  }
  return key;
}

char getColorChoice() {
  char key = keypad.getKey();
  while (!key) {
    key = keypad.getKey();
  }
  return key;
}

void setLightColor() {
  analogWrite(redPin, Red);
  analogWrite(greenPin, Green);
  analogWrite(bluePin, Blue);
}

void getInputFromKeypad() {
  bool validInput = false;

  while (!validInput) {
    lcd.clear();
    lcd.print("Enter RGB Value");
    lcd.setCursor(0, 1);
    lcd.print("Enter R:");
    Red = getNumberFromKeypad();
    if (Red == -2) continue;  // Check for reset and continue loop

    lcd.setCursor(0, 2);
    lcd.print("Enter G:");
    Green = getNumberFromKeypad();
    if (Green == -2) continue;  // Check for reset and continue loop

    lcd.setCursor(0, 3);
    lcd.print("Enter B:");
    Blue = getNumberFromKeypad();
    if (Blue == -2) continue;  // Check for reset and continue loop

    lcd.clear();
    lcd.print("Enter Volume (ml):");
    Volume = getNumberFromKeypad();
    if (Volume == -2) continue;  // Check for reset and continue loop

    if (Red == -1 || Green == -1 || Blue == -1 || Volume == -1) {
      lcd.clear();
      lcd.print("Invalid Inputs!");
      delay(2000);
    } else {
      validInput = true;
    }
  }

  // Display RGB and volume values
  lcd.clear();
  lcd.print("R: ");
  lcd.print(Red);
  lcd.setCursor(0, 1);
  lcd.print("G: ");
  lcd.print(Green);
  lcd.setCursor(0, 2);
  lcd.print("B: ");
  lcd.print(Blue);
  lcd.setCursor(0, 3);
  lcd.print("Volume: ");
  lcd.print(Volume);
  lcd.print(" ml");
  setLightColor();
  delay(2000);  // Show the values for a brief moment before proceeding
  getCMYK();
}

void getInputFromBT() {
  lcd.clear();
  lcd.print("Waiting for pairing");
  lcd.setCursor(0, 1);
  while (true) {
    if (BTSerial.available() > 0) {
      String command = BTSerial.readStringUntil('\n');

      command.trim();

      // Check the received command and update the LED color accordingly
      if (command.length() == 9) {  // Adjusted to 9 to match the expected length of 3 digits each for R, G, B
        // Extract the RGB values from the command
        Red = command.substring(0, 3).toInt();
        Green = command.substring(3, 6).toInt();
        Blue = command.substring(6).toInt();

        Serial.print("Red: ");
        Serial.println(Red);
        Serial.print("Green: ");
        Serial.println(Green);
        Serial.print("Blue: ");
        Serial.println(Blue);

        lcd.clear();
        lcd.print("R: ");
        lcd.print(Red);
        lcd.print(" G: ");
        lcd.print(Green);
        lcd.print(" B: ");
        lcd.print(Blue);

        // Get volume from user
        lcd.setCursor(0, 2);
        lcd.print("Enter Volume (ml):");
        Volume = getNumberFromKeypad();  // Use keypad to get volume
        if (Volume == -2) continue;      // Check for reset and continue loop

        if (Volume == -1) {
          lcd.clear();
          lcd.print("Invalid Volume!");
          delay(2000);
        } else {
          lcd.clear();
          lcd.print("Volume: ");
          lcd.print(Volume);
          lcd.print(" ml");
          setLightColor();
          getCMYK();
          break;  // Exit the loop after getting valid RGB and volume
        }
      } else {
        Serial.println("Error: Invalid command length");
        lcd.clear();
        lcd.print("Error: Invalid cmd");
        delay(2000);
        lcd.clear();
        lcd.print("Waiting for pairing");
        lcd.setCursor(0, 1);
      }
    } else {
      Serial.println("No data available from BTSerial");
      delay(1000);
    }
  }
}

void getInputFromColorSensor() {
  if (tcs.begin()) {
    lcd.clear();
    lcd.print("TCS34725 found");
    Serial.println("TCS34725 found");
    delay(2000);
    lcd.clear();
    lcd.print("Place your object and Press # to start");

    // Wait for the # key to be pressed
    while (true) {
      char key = keypad.getKey();
      if (key == '#') {
        break;
      }
    }

    lcd.clear();
    lcd.print("Processing...");
    delay(1000);
    uint16_t clear, red, green, blue;

    // Variables to store the sum of readings
    uint32_t sumRed = 0, sumGreen = 0, sumBlue = 0, sumClear = 0;

    // Turn on the LED
    digitalWrite(pinLED, HIGH);
    delay(100);  // Allow some time for the sensor to stabilize

    // Take multiple readings and sum them up
    for (int i = 0; i < NUM_SAMPLES; i++) {
      tcs.getRawData(&red, &green, &blue, &clear);
      sumRed += red;
      sumGreen += green;
      sumBlue += blue;
      sumClear += clear;
      delay(100);  // Small delay between readings
    }

    // Calculate the average of the readings
    red = sumRed / NUM_SAMPLES;
    green = sumGreen / NUM_SAMPLES;
    blue = sumBlue / NUM_SAMPLES;
    clear = sumClear / NUM_SAMPLES;

    // Convert raw data to more readable format
    float r, g, b;
    r = red;
    r /= clear;
    g = green;
    g /= clear;
    b = blue;
    b /= clear;

    // Scale to 256 and apply simple calibration factors (adjust as needed)
    r *= 256 * 0.9;  // Adjust calibration factor for red
    g *= 256 * 1.1;  // Adjust calibration factor for green
    b *= 256 * 1.2;  // Adjust calibration factor for blue

    // Clamp values to 0-255 range
    r = constrain(r, 0, 255);
    g = constrain(g, 0, 255);
    b = constrain(b, 0, 255);

    Serial.print("R: ");
    Serial.print((int)r);
    Serial.print(" G: ");
    Serial.print((int)g);
    Serial.print(" B: ");
    Serial.print((int)b);
    Serial.print(" C: ");
    Serial.println(clear);

    Red = (int)r;
    Green = (int)g;
    Blue = (int)b;

    digitalWrite(pinLED, LOW);  // Turn off the sensor's white LED
    lcd.clear();
    lcd.print("R: ");
    lcd.print(Red);
    lcd.print(" G: ");
    lcd.print(Green);
    lcd.print(" B: ");
    lcd.print(Blue);
    delay(3000);  // Show the color values for a brief moment before proceeding

    // Get volume from user
    lcd.clear();
    lcd.print("Enter Volume (ml):");
    Volume = getNumberFromKeypad();  // Use keypad to get volume
    if (Volume == -2) return;        // Check for reset and return

    if (Volume == -1) {
      lcd.clear();
      lcd.print("Invalid Volume!");
      delay(2000);
    } else {
      lcd.clear();
      lcd.print("Volume: ");
      lcd.print(Volume);
      lcd.print(" ml");
      setLightColor();
      getCMYK();
    }

  } else {
    lcd.clear();
    lcd.print("No TCS34725 found");
    Serial.println("No TCS34725 found ... check your connections");
    while (1)
      ;
  }
}

long getNumberFromKeypad() {
  String input = "";
  char key;

  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key == '#') {  // End of input
        if (input.length() > 0) {
          return input.toInt();
        } else {
          return -1;  // No input entered
        }
      } else if (key == '*') {  // Reset input
        return -2;
      } else {
        input += key;
        lcd.print(key);  // Display entered key
      }
    }
  }
}

void getCMYK() {
  float R = (float)Red / 255.0;
  float G = (float)Green / 255.0;
  float B = (float)Blue / 255.0;

  Key = 1 - max(R, max(G, B));
  Cyan = (1 - R - Key) / (1 - Key);
  Magenta = (1 - G - Key) / (1 - Key);
  Yellow = (1 - B - Key) / (1 - Key);

  if (Key == 1.0) {
    Cyan = 0;
    Magenta = 0;
    Yellow = 0;
  }

  Cyan = Cyan * 100;
  Magenta = Magenta * 100;
  Yellow = Yellow * 100;
  Key = Key * 100;


  Total = Cyan + Magenta + Yellow + Key;
  Cyan = (Cyan / Total) * 100;
  Magenta = (Magenta / Total) * 100;
  Yellow = (Yellow / Total) * 100;
  Key = (Key / Total) * 100;
  Csteps = 1790 + (Cyan / 100) * Volume * Cspu;
  Msteps = 2400 + (Magenta / 100) * Volume * Mspu;
  Ysteps = 2080 + (Yellow / 100) * Volume * Yspu;
  Ksteps = 2630 + (Key / 100) * Volume * Kspu;

  lcd.clear();
  lcd.print("C: ");
  lcd.print(Cyan, 1);
  lcd.print("% M: ");
  lcd.print(Magenta, 1);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Y: ");
  lcd.print(Yellow, 1);
  lcd.print("% K: ");
  lcd.print(Key, 1);
  lcd.print("%");

  BTSerial.print("C: ");
  BTSerial.print(Cyan, 1);
  BTSerial.print("% M: ");
  BTSerial.print(Magenta, 1);
  BTSerial.print("%");
  BTSerial.print(" Y: ");
  BTSerial.print(Yellow, 1);
  BTSerial.print("% K: ");
  BTSerial.print(Key, 1);
  BTSerial.println("%");

  Serial.print("C: ");
  Serial.print(Cyan, 1);
  Serial.print("% M: ");
  Serial.print(Magenta, 1);
  Serial.print("%");
  Serial.print(" Y: ");
  Serial.print(Yellow, 1);
  Serial.print("% K: ");
  Serial.print(Key, 1);
  Serial.println("%");

  delay(2000);  // Allow user to see the CMYK values before proceeding
  makeColor();
}

void cleanPump() {
  lcd.print("Cleaning Pumps");
  digitalWrite(enPin, LOW);
  Serial.print("Cleaning Pumps...");
  cStepperRotate(-3000, rpm);
  Serial.print("...");
  mStepperRotate(-3000, rpm);
  Serial.print("...");
  yStepperRotate(-3000, rpm);
  Serial.print("...");
  kStepperRotate(-3000, rpm);
  Serial.println("...Done!");
  digitalWrite(enPin, HIGH);
}

bool checkPaintLevel() {
  float cDistance = measureDistance(cTrigPin, cEchoPin);
  float mDistance = measureDistance(mTrigPin, mEchoPin);
  float yDistance = measureDistance(yTrigPin, yEchoPin);
  float kDistance = measureDistance(kTrigPin, kEchoPin);

  Serial.print("Cyan Level: ");
  Serial.println(cDistance);
  Serial.print("Magenta Level: ");
  Serial.println(mDistance);
  Serial.print("Yellow Level: ");
  Serial.println(yDistance);
  Serial.print("Black Level: ");
  Serial.println(kDistance);

  bool isCyanEnough = cDistance < 10;  // Example threshold
  bool isMagentaEnough = mDistance < 10;
  bool isYellowEnough = yDistance < 10;
  bool isBlackEnough = kDistance < 10;
  lcd.clear();
  if (!isCyanEnough) {
    lcd.setCursor(0, 0);
    lcd.print("Cyan Low");
  }
  // if (!isMagentaEnough) lcd.setCursor(0, 1); lcd.print("Magenta Low");
  if (!isYellowEnough) {
    lcd.setCursor(0, 2);
    lcd.print("Yellow Low");
  }
  if (!isBlackEnough) {
    lcd.setCursor(0, 3);
    lcd.print("Black Low");
  }

  delay(2000);  // Display the message for 2 seconds

  return /*isCyanEnough && isMagentaEnough && */ isYellowEnough && isBlackEnough;
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void makeColor() {

  if (digitalRead(irSensorPin) == LOW && checkPaintLevel()) {

    digitalWrite(enPin, LOW);
    if (Csteps > 1790) {  // Ensure actual volume for Cyan is greater than zero
      lcd.clear();
      lcd.print("Pumping Cyan with ");
      lcd.setCursor(0, 3);
      lcd.print(Csteps);  // Display only the actual steps for the volume
      lcd.print(" Steps");
      cStepperRotate(Csteps, rpm);
      lcd.clear();
      lcd.print("    .....Done!");
      setBuzz(500);
      delay(2000);
    }

    if (Msteps > 2400) {  // Ensure actual volume for Magenta is greater than zero
      lcd.clear();
      lcd.print("Pumping Magenta with ");
      lcd.setCursor(0, 3);
      lcd.print(Msteps);  // Display only the actual steps for the volume
      lcd.print(" Steps");
      mStepperRotate(Msteps, rpm);
      lcd.clear();
      lcd.print("    .....Done!");
      setBuzz(500);
      delay(2000);
    }

    if (Ysteps > 2080) {  // Ensure actual volume for Yellow is greater than zero
      lcd.clear();
      lcd.print("Pumping Yellow with ");
      lcd.setCursor(0, 3);
      lcd.print(Ysteps);  // Display only the actual steps for the volume
      lcd.print(" Steps");
      yStepperRotate(Ysteps, rpm);
      lcd.clear();
      lcd.print("    .....Done!");
      setBuzz(500);
      delay(2000);
    }

    if (Ksteps > 2630) {  // Ensure actual volume for Key (Black) is greater than zero
      lcd.clear();
      lcd.print("Pumping Black with ");
      lcd.setCursor(0, 3);
      lcd.print(Ksteps);  // Display only the actual steps for the volume
      lcd.print(" Steps");
      kStepperRotate(Ksteps, rpm);
      lcd.clear();
      lcd.print("    .....Done!");
      setBuzz(500);
      delay(2000);
    }


    lcd.clear();
    lcd.print(" ");
    lcd.print("All colors pumped!");
    lcd.print(" ");
    delay(1800);
    setBuzz(500);
    delay(100);
    setBuzz(500);
    delay(100);
    setBuzz(500);
    delay(100);
    setBuzz(500);
    delay(2000);
    cleanPump();
    setBuzz(500);
    delay(100);
    setBuzz(500);
    delay(100);
    setBuzz(500);
    delay(100);
    setBuzz(500);
    lcd.clear();
    restart();
  } else if (checkPaintLevel() == false) {
    lcd.clear();
    lcd.print("Error:");
    lcd.setCursor(0, 1);
    lcd.print("Low Paint Levels! (press # to try again)");
    retryMakeColor();


  }

  else {

    lcd.clear();
    lcd.print("Error:");
    lcd.setCursor(0, 1);
    lcd.print("Bucket not in place!");
    retryMakeColor();
    //Serial.print("Bucket not in place!");

    delay(2000);
  }
}

void setBuzz(int time) {
  analogWrite(buzPin, 255);
  delay(time);
  analogWrite(buzPin, 0);
  delay(50);
}

void cStepperRotate(float steps, float rpm) {
  float stepsPerRotation = (360.00 / motorAngle) / stepSize;
  unsigned long stepPeriodmicroSec = ((60.0000 / (rpm * stepsPerRotation)) * 1E6 / 2.0000) - 5;

  if (steps > 0) {
    digitalWrite(cDirPin, HIGH);
  } else {
    digitalWrite(cDirPin, LOW);
  }

  if (steps > 0) {
    float totalSteps = steps;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(cStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(cStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
    }
  } else {
    float totalSteps = steps * -1;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(cStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(cStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
    }
  }
}

void mStepperRotate(float steps, float rpm) {
  float stepsPerRotation = (360.00 / motorAngle) / stepSize;
  unsigned long stepPeriodmicroSec = ((60.0000 / (rpm * stepsPerRotation)) * 1E6 / 2.0000) - 5;

  if (steps > 0) {
    digitalWrite(mDirPin, HIGH);
  } else {
    digitalWrite(mDirPin, LOW);
  }

  if (steps > 0) {
    float totalSteps = steps;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(mStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(mStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
    }
  } else {
    float totalSteps = steps * -1;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(mStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(mStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
    }
  }
}

void yStepperRotate(float steps, float rpm) {
  float stepsPerRotation = (360.00 / motorAngle) / stepSize;
  unsigned long stepPeriodmicroSec = ((60.0000 / (rpm * stepsPerRotation)) * 1E6 / 2.0000) - 5;

  if (steps > 0) {
    digitalWrite(yDirPin, LOW);
  } else {
    digitalWrite(yDirPin, HIGH);
  }

  if (steps > 0) {
    float totalSteps = steps;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(yStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(yStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
    }
  } else {
    float totalSteps = steps * -1;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(yStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(yStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
    }
  }
}

void kStepperRotate(float steps, float rpm) {
  float stepsPerRotation = (360.00 / motorAngle) / stepSize;
  unsigned long stepPeriodmicroSec = ((60.0000 / (rpm * stepsPerRotation)) * 1E6 / 2.0000) - 5;

  if (steps > 0) {
    digitalWrite(kDirPin, LOW);
  } else {
    digitalWrite(kDirPin, HIGH);
  }

  if (steps > 0) {
    float totalSteps = steps;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(kStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(kStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
    }
  } else {
    float totalSteps = steps * -1;
    for (unsigned long i = 0; i < totalSteps; i++) {
      digitalWrite(kStepPin, LOW);
      delayMicroseconds(stepPeriodmicroSec);
      digitalWrite(kStepPin, HIGH);
      delayMicroseconds(stepPeriodmicroSec);
    }
  }
}