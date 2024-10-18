#include <Servo.h>
#include <math.h>

// Servo object for rotation
// Servo servoRot;

 // Photoresistor analog input pins
int tiltU = A0; // Photoresistor for tilt (up)
int tiltD = A3; // Photoresistor for tilt (down)
// int rotE = A1;  // Photoresistor for rotation (east)
// int rotW = A2;  // Photoresistor for rotation (west)

// Motor control pins for the tilt mechanism
int RPWM_Output = 9; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output = 10; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
int REN = 7;
int LEN = 8;

// Core variables
int tiltNet = 0; // Difference in light detected for tilt control
// int rotNet = 0;  // Difference in light detected for rotation control
// int rotAngle = 0; // Current angle for rotation servo
// int sensitivity = 250; // Sensitivity threshold

void setup() {
  // Attach the rotation servo to pin 3
  
  
  // Set motor control pins as outputs
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
  pinMode(REN, OUTPUT);
  pinMode(LEN, OUTPUT);
  
  digitalWrite(REN, HIGH); // Enable the driver
  digitalWrite(LEN, HIGH); // Enable the driver
  
  // servoRot.attach(3);
  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Prioritize tilt adjustment first
  tiltNet = analogRead(tiltU) - analogRead(tiltD);
  if (tiltNet > 150) {
    analogWrite(LPWM_Output, 0);
    analogWrite(RPWM_Output, 128);
    delay(100);
    analogWrite(RPWM_Output, 0);
  } else if (tiltNet < -100) {
    analogWrite(RPWM_Output, 0);
    analogWrite(LPWM_Output, 128);
    delay(100);
    analogWrite(LPWM_Output, 0);
  }

  // Then handle rotation adjustment
  // rotNet = analogRead(rotW) - analogRead(rotE);
  // if (rotNet > 100 && rotAngle < 100) {
  //   rotAngle += 5;
  // } else if (rotNet < -100 && rotAngle > 0) {
  //   rotAngle -= 5;
  // }
  // // delay(300);
  // servoRot.write(rotAngle);

  // Debugging output
  Serial.print("tiltNet: ");
  Serial.println(tiltNet);
  // Serial.print("rotNet: ");
  // Serial.println(rotNet);
  // Serial.print("rotAngle:");
  // Serial.println(rotAngle);
  Serial.print("tiltU:");
  Serial.println(analogRead(tiltU));
  Serial.print("tiltD:");
  Serial.println(analogRead(tiltD));
  // Serial.print("rotE:");
  // Serial.println(analogRead(rotE));
  // Serial.print("rotW:");
  // Serial.println(analogRead(rotW));
  Serial.println();

  delay(500);
}



// double scale(double angle) {
//   return angle * 130 / 180;
// }

