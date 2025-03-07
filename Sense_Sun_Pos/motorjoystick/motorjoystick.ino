
#include <LcdMenu.h>
#include <MenuScreen.h>
#include <ItemBack.h>
#include <ItemWidget.h>
#include <widget/WidgetBool.h>
#include <widget/WidgetList.h>
#include <widget/WidgetRange.h>
#include <ItemSubMenu.h>
#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include<stdio.h>
#include<stdlib.h>
//==========================================================================
// Connection to the BTS7960 board:
// BTS7960 Pin 1 (RPWM) to Arduino pin 5(PWM)
// BTS7960 Pin 2 (LPWM) to Arduino pin 6(PWM)
// BTS7960 Pin 3 (R_EN), 4 (L_EN), 7 (VCC) to Arduino 5V pin
// BTS7960 Pin 8 (GND) to Arduino GND
// BTS7960 Pin 5 (R_IS) and 6 (L_IS) not connected
//
#define VRX_PIN A0 // Arduino pin connected to VRX pin
#define VRY_PIN A1 // Arduino pin connected to VRY pin
#define VRZ_PIN A3
int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int zValue = 0; 
int last_input = 0;
int current_input = 0;
unsigned long startMillis;
unsigned long currentMillis;
unsigned long MillisDelayTime = 500;

int SENSOR_PIN = 0; // center pin of the potentiometer
int RPWM_Output = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
void setup()
{
 pinMode(RPWM_Output, OUTPUT);
 pinMode(LPWM_Output, OUTPUT);
}
void loop()

{
    // Serial.println(currentMillis - startMillis);
  // read analog X, Y, Z values
  currentMillis = millis(); 
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  zValue = analogRead(VRZ_PIN);
  xValue = map(xValue, 0, 1023, 0, 180);
  yValue = map(yValue, 0, 1023, 0, 180);

  // CheckInputLeft();
  // CheckInputRight();
 // sensor value is in the range 0 to 1023
 // the lower half of it we use for reverse rotation; the upper half for forward
  // rotation
  int mid = 90;
  
 if (yValue < mid)
 {
 // reverse rotation
 int reversePWM = -(yValue - mid - 1) / 2;
 analogWrite(LPWM_Output, 0);
 analogWrite(RPWM_Output, reversePWM);
 }
 else
 {
 // forward rotation
 int forwardPWM = (yValue - mid) / 2;
 analogWrite(LPWM_Output, forwardPWM);
 analogWrite(RPWM_Output, 0);
 }
}