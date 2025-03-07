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
#include <ItemToggle.h>


//////////////////////////////////
//        INITIALIZATION
//////////////////////////////////
#define LCD_ROWS 2
#define LCD_COLS 16
#define VRX_PIN A0 // Arduino pin connected to VRX pin
#define VRY_PIN A1 // Arduino pin connected to VRY pin
#define VRZ_PIN A3
//////////////////////////////////
//Joystick
//////////////////////////////////
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
int mid = 90;
bool isJoystick = 0;
//////////////////////////////////
struct Location {
  const char* name;
  float latitude;
  float longitude;
};

Location locations[] = {
  { "CHI", 41.8781, -87.6298 },
  { "MUM", 19.0760, 72.8777 },
  { "NYC", 40.7128, -74.0060 },
  { "LA ", 34.0522, -118.2437 },
  { "MTL", 45.5017, -73.5673 },
  { "ICN", 37.5665, 126.9780 },
  { "DEN", 39.7392, -104.9903 },  // Added Denver with its latitude and longitude
};

String months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
int numdays[] = {31, 28, 31, 30, 31, 30, 31, 30, 31, 30, 31, 31};
//////////////////////////////////
//////////////////////////////////
//LCD
//////////////////////////////////
extern MenuScreen* locTimeScreen;
void toggleJoystickMode(bool isOn) {
  isJoystick = isOn;
}

MENU_SCREEN(mainScreen, mainItems,
    ITEM_TOGGLE("Joystick", toggleJoystickMode),
    ITEM_BASIC("SunSense"),
    ITEM_SUBMENU("Input Loc/Time", locTimeScreen));

MENU_SCREEN(locTimeScreen, locTimeItems,
    ITEM_WIDGET(
    "LOC",
    [](const uint8_t loc) { Serial.println(loc); },
    WIDGET_LIST(locations, sizeof(locations) / sizeof(locations[0]), 0, " In %s", 0, true)),
    ITEM_WIDGET(
    "TIME",
    [](int hour, int minute, int month, int day, int year) { Serial.println(hour); Serial.println(minute); Serial.println(month); Serial.println(day); Serial.println(year);},
    WIDGET_RANGE(0, 1, 0, 23, "%02d", 0, false),
    WIDGET_RANGE(0, 1, 0, 59, ":%02d", 0, false),
    
    WIDGET_RANGE(1, 1, 1, 12, " %02d", 0, true),
    WIDGET_RANGE(1, 1, 1, 31, "/%02d", 0, true),
    WIDGET_RANGE(2021, 1, 2020, 2050, "/%04d", 0, true)),
    ITEM_BACK("BACK"));

LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, LCD_COLS, LCD_ROWS);
LcdMenu menu(renderer);


int threshold = 75;
void lcdIn(int x, int y, String message) {
  lcd.setCursor(x,y);
  lcd.print(message);
}

void CheckInputLeft() {
    // Check for holding control 
  while(yValue <= 90 - threshold && currentMillis - startMillis >= MillisDelayTime) {
    //    If started, start timer, input down input
    menu.process(LEFT);
    if (currentMillis - startMillis >= MillisDelayTime) {  
      startMillis = currentMillis;
    }
    //    When timer hits .5 s restart the while loop
  }
}
void CheckInputRight() {
  while(yValue >= 90 + threshold && currentMillis - startMillis >= MillisDelayTime) {
    //    If started, start timer, input down input
    menu.process(RIGHT);
    if (currentMillis - startMillis >= MillisDelayTime) {  
      startMillis = currentMillis;
    }
    //    When timer hits .5 s restart the while loop
  }
}
void CheckInputUp() {
    // Check for holding control 
  while(xValue >= 90 + threshold && currentMillis - startMillis >= MillisDelayTime) {
    //    If started, start timer, input down input
    menu.process(UP);
    if (currentMillis - startMillis >= MillisDelayTime) {  
      startMillis = currentMillis;
    }
    //    When timer hits .5 s restart the while loop
  }
}
void CheckInputDown() {
  // Check for holding control 
  while(xValue <= 90 - threshold && currentMillis - startMillis >= MillisDelayTime) {
    //    If started, start timer, input down input
    menu.process(DOWN);
    if (currentMillis - startMillis >= MillisDelayTime) {  
      startMillis = currentMillis;
    }
    //    When timer hits .5 s restart the while loop
  }
  
}
bool CheckInputPress() {
  while(zValue < 100 && currentMillis - startMillis >= MillisDelayTime) {
    //    If started, start timer, input down input
    menu.process(ENTER);
    if (currentMillis - startMillis >= MillisDelayTime) {  
      startMillis = currentMillis;
    }
    //    When timer hits .5 s restart the while lo
    
  }
}
void CheckJoyRotation() {
   if (yValue < mid)
 {
 // reverse rotationsl
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

/////////////////////////////////
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  menu.setScreen(mainScreen);
  startMillis = millis();
  currentMillis = millis(); 
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
}

void loop() {
  // Serial.println(currentMillis - startMillis);
  // read analog X, Y, Z values
  currentMillis = millis(); 
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  zValue = analogRead(VRZ_PIN);
  xValue = map(xValue, 0, 1023, 0, 180);
  yValue = map(yValue, 0, 1023, 0, 180);
  CheckInputPress();
  if(isJoystick) {
  CheckJoyRotation();
  } else {
    CheckInputLeft();
    CheckInputRight();
    CheckInputUp();
    CheckInputDown();
  }
  


  
  Serial.println(isJoystick);
}
