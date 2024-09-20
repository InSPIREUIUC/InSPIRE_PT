/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-joystick
 */

// Servo code
#include <Servo.h>
Servo servo1;
Servo servo2;
int servoPin1 = 8;
int servoPin2 = 9;
//


#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis

void setup() {
  Serial.begin(9600) ;
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
}

void loop() {
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  servo1.write(xValue/5.683);
  servo2.write(yValue/5.683);
  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(xValue/5.683);
  Serial.print(", y = ");
  Serial.println(yValue/5.683);
  map()
  delay(100);
}
