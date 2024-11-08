#include <Servo.h>
#include <math.h>

// Motor control pins for the tilt mechanism
int RPWM_Output = 9;  // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output = 10; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
int REN = 7;
int LEN = 8;

void setup() {
  // Set motor control pins as outputs
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
  pinMode(REN, OUTPUT);
  pinMode(LEN, OUTPUT);
  
  digitalWrite(REN, HIGH); // Enable the driver
  digitalWrite(LEN, HIGH); // Enable the driver
  
  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Constantly turn the motor in one direction
  analogWrite(LPWM_Output, 0);  // Stop the left PWM
  analogWrite(RPWM_Output, 128); // Run the right PWM at half speed
  
  // Debugging output
  Serial.println("Motor turning...");
  
  delay(1000); // Keep the motor running
}
