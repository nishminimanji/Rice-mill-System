#include <HX711.h>  // Include the HX711 library
#include <LiquidCrystal_I2C.h>

//LCD Definition
#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4
#define pinDATA     2  

int grainsize=0;

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Pin definitions
const int STPPin1 = 4;    // STEP pin for Motor 1
const int DIRPin1 = 12;     // DIR pin for Motor 1
const int STPPin2 = 11;    // STEP pin for Motor 2
const int DIRPin2 = 13;     // DIR pin for Motor 2
const int STPPin3 = 8;     // STEP pin for Motor 3
const int DIRPin3 = 3;      // DIR pin for Motor 3
const int STPPin4 = 18;    // STEP pin for Motor 4
const int DIRPin4 = 35;     // DIR pin for Motor 4
const int STPPin5 = 15;    // STEP pin for Motor 5
const int DIRPin5 = 16;     // DIR pin for Motor 5
const int buttonPin = A0;  //Pin for the control button
const int ledPin1 = 9;      // LED pin 1
const int ledPin2 = 5;      // LED pin 2
const int ledPin3 = 6;      // LED pin 3
const int ledPin4 = 2;      // LED pin for weight indicator
const int trigPin = 53;     // HC-SR04 Trig pin
const int echoPin = 52;     // HC-SR04 Echo pin
const int tempPin = 51;     // Temperature sensor pin
const int dtPin = 25;       // HX711 Data pin
const int sckPin = 31;      // HX711 Clock pin
const int motorPin1 = 43;   // Motor pin 1
const int motorPin2 = 23;   // Motor pin 2

HX711 scale;  // Create an HX711 object

bool motorRunning1 = false; 
bool motorRunning2 = false;
bool motorRunning3 = false; // Motor 3 status
bool motorRunning4 = false; // Motor 4 status
bool motorRunning5 = false; // Motor 5 status
bool buttonPressed = false;
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 300; // Debounce time for button press

unsigned long motorStartTime = 0; // To keep track of motor run time
bool motorActive = false; // To toggle motor activity
const unsigned long runDuration = 5000; // Motor run time in milliseconds
const unsigned long stopDuration = 10000; // Motor stop time in milliseconds

void setup() {

lcd.init();
//lcd.begin();
lcd.backlight();
lcd.clear();

  pinMode(STPPin1, OUTPUT);
  pinMode(DIRPin1, OUTPUT);
  pinMode(STPPin2, OUTPUT);
  pinMode(DIRPin2, OUTPUT);
  pinMode(STPPin3, OUTPUT);
  pinMode(DIRPin3, OUTPUT);
  pinMode(STPPin4, OUTPUT);
  pinMode(DIRPin4, OUTPUT);
  pinMode(STPPin5, OUTPUT);
  pinMode(DIRPin5, OUTPUT);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);  // LED for weight
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize the HX711
  scale.begin(dtPin, sckPin);
  scale.set_scale(); // Set the scale to the default (use this for calibration)
  scale.tare(); // Reset the scale to 0

  // Set up motor pins
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  delay(500); // Allow time for setup
}

void loop() {
  // Measure distance using HC-SR04
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2.0) * 0.0344; // Convert to centimeters

  // Control LED and Motor 2 based on distance
  if (distance >= 100 && distance <= 400) {
    digitalWrite(ledPin1, HIGH);
    motorRunning2 = true;
  } else if (distance < 100) {
    digitalWrite(ledPin1, LOW);
    motorRunning2 = false;
  }

  // Check button state and toggle Motor 1
  if (digitalRead(buttonPin) == LOW && (millis() - lastButtonPress) > debounceDelay) {
    lastButtonPress = millis(); // Update the last button press time
    buttonPressed = !buttonPressed; // Toggle button state
    motorRunning1 = buttonPressed; // Set motorRunning1 based on button state
    if (buttonPressed) {
      // Turn on the remaining LED when button is pressed
      digitalWrite(ledPin3, HIGH);
      digitalWrite(ledPin2, HIGH);
    } else {
      // Turn off the remaining LED when button is released
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin2, LOW);
    }
    if(grainsize<10){  //If Grain size is less than 10mm
    lcd.setCursor(0,1);
    lcd.print(" Grain Type OK");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(" Processing");
    delay(10000);
    
  }
  if(grainsize>10){//If Grain size is greater than 10mm
    lcd.setCursor(0,1);
    lcd.print("Grain Type Invalid");
    delay(1500);
  }
  lcd.clear();

  }

  // Control motor 1
  if (motorRunning1) {
    digitalWrite(DIRPin1, HIGH); 
    digitalWrite(STPPin1, HIGH);
    delayMicroseconds(2000); // Adjust delay for desired speed
    digitalWrite(STPPin1, LOW);
    delayMicroseconds(2000); // Adjust delay for desired speed
  }

  // Control motor 2
  if (motorRunning2) {
    digitalWrite(DIRPin2, HIGH); 
    digitalWrite(STPPin2, HIGH);
    delayMicroseconds(2000); // Adjust delay for desired speed
    digitalWrite(STPPin2, LOW);
    delayMicroseconds(2000); // Adjust delay for desired speed
  }

  // Measure temperature using NTC sensor
  int tempReading = analogRead(tempPin);
  float voltage = tempReading * (5.0 / 1023.0); // Convert analog reading to voltage
  float temperatureC = (voltage - 0.5) * 100.0; // Convert voltage to temperature in Celsius

  // Control motor 3 based on temperature
  if (temperatureC < 10.0) {
    motorRunning3 = true;
  } else {
    motorRunning3 = false;
  }

  // Control motor 3
  if (motorRunning3) {
    digitalWrite(DIRPin3, HIGH);
    digitalWrite(STPPin3, HIGH);
    delayMicroseconds(2000); // Adjust delay for desired speed
    digitalWrite(STPPin3, LOW);
    delayMicroseconds(2000); // Adjust delay for desired speed
  }

  // Measure weight using HX711
  float weight = scale.get_units(10); // Average over 10 readings
  
  // Control LED based on weight
  if (weight >= 1.0) {
    digitalWrite(ledPin4, HIGH); // Turn on LED if weight is 1kg or more
  } else {
    digitalWrite(ledPin4, LOW);  // Turn off LED if weight is less than 1kg
  }

  // Check if the valve LED (ledPin4) is on
  if (digitalRead(ledPin4) == HIGH) {
    unsigned long currentTime = millis();
    
    // Control motor 4
    if (motorActive) {
      if (currentTime - motorStartTime < runDuration) {
        // Run the motor 4
        digitalWrite(STPPin4, HIGH);
        digitalWrite(DIRPin4, HIGH);
      } else if (currentTime - motorStartTime < runDuration + stopDuration) {
        // Stop the motor 4
        digitalWrite(STPPin4, LOW);
        digitalWrite(DIRPin4, LOW);
      } else {
        // Update motorStartTime for next cycle
        motorStartTime = currentTime;
      }
      
      // Run the motor 5
      if (currentTime - motorStartTime < runDuration) {
        digitalWrite(STPPin5, HIGH);
        digitalWrite(DIRPin5, HIGH);
      } else if (currentTime - motorStartTime < runDuration + stopDuration) {
        digitalWrite(STPPin5, LOW);
        digitalWrite(DIRPin5, LOW);
      } else {
        // Update motorStartTime for next cycle
        motorStartTime = currentTime;
      }
      
      // Run the motor 1
      if (currentTime - motorStartTime < runDuration) {
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin2, HIGH);
      } else if (currentTime - motorStartTime < runDuration + stopDuration) {
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
      } else {
        // Update motorStartTime for next cycle
        motorStartTime = currentTime;
      }
      
      // Run the motor 3
      if (currentTime - motorStartTime < runDuration) {
        digitalWrite(STPPin3, HIGH);
        digitalWrite(DIRPin3, HIGH);
      } else if (currentTime - motorStartTime < runDuration + stopDuration) {
        digitalWrite(STPPin3, LOW);
        digitalWrite(DIRPin3, LOW);
      } else {
        // Update motorStartTime for next cycle
        motorStartTime = currentTime;
      }
      
    } else {
      // Turn off all motors if not active
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(STPPin3, LOW);
      digitalWrite(DIRPin3, LOW);
      digitalWrite(STPPin4, LOW);
      digitalWrite(DIRPin4, LOW);
      digitalWrite(STPPin5, LOW);
      digitalWrite(DIRPin5, LOW);
    }
    
    // Toggle motor activity
    motorActive = !motorActive;
  } else {
    // Turn off all motors if LED is off
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(STPPin3, LOW);
    digitalWrite(DIRPin3, LOW);
    digitalWrite(STPPin4, LOW);
    digitalWrite(DIRPin4, LOW);
    digitalWrite(STPPin5, LOW);
    digitalWrite(DIRPin5, LOW);
  }

  delay(100); // Small delay to ensure smooth operation
}
