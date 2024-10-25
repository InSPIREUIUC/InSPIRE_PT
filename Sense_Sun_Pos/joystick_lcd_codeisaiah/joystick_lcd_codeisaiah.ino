#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//////////////////////////////////
//        INITIALIZATION
//////////////////////////////////

//////////////////////////////////
//Servos
//////////////////////////////////
Servo servo1;
Servo servo2;
int servoPin1 = 8;
int servoPin2 = 9;
#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define VRZ_PIN A3

//////////////////////////////////
//Joystick
//////////////////////////////////
int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int zValue = 0; 
int lastInput = 0;

//////////////////////////////////
//LCD
//////////////////////////////////
LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

struct Location {
  const char* name;
  float latitude;
  float longitude;
};

// Location locations[] = {
//   { "Chicago", 41.8781, -87.6298 },
//   { "Mumbai", 19.0760, 72.8777 },
//   { "New York City", 40.7128, -74.0060 },
//   { "Los Angeles", 34.0522, -118.2437 },
//   { "Montreal", 45.5017, -73.5673 },
//   { "Seoul", 37.5665, 126.9780 },
//   { "Denver", 39.7392, -104.9903 },  // Added Denver with its latitude and longitude
// };
Location locations[] = {
  { "CHI", 41.8781, -87.6298 },
  { "MUM", 19.0760, 72.8777 },
  { "NYC", 40.7128, -74.0060 },
  { "LA ", 34.0522, -118.2437 },
  { "MTL", 45.5017, -73.5673 },
  { "ICN", 37.5665, 126.9780 },
  { "DEN", 39.7392, -104.9903 },  // Added Denver with its latitude and longitude
};


String months[] = {"JAN", "FEB", "MAR", "APR", "JUN", "JUL", "AUG", "SEP", "NOV", "DEC"};
int numdays[] = {31, 28, 31, 30, 31, 30, 31, 30, 31, 30, 31, 31};
int states[4] = {0, 0, 0, 0};

int states_size = sizeof(states) / sizeof(states[0]);
int locations_size = sizeof(locations) / sizeof(locations[0]);
int months_size = sizeof(months) / sizeof(months[0]);
int numdays_size = sizeof(numdays) / sizeof(numdays[0]);
int states_limits[4] = {locations_size - 1, months_size - 1, numdays_size - 1, 0};


int state_index = 0;

int threshold = 55;
void lcdIn(int x, int y, String message) {
  lcd.setCursor(x,y);
  lcd.print(message);
}
/////////////////////////////////
void setup() {
  //
  Serial.begin(9600);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  //
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  while (!Serial);
}

void loop() {
  // read analog X, Y, Z values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  zValue = analogRead(VRZ_PIN);
  
  xValue = map(xValue, 0, 1023, 0, 180);
  yValue = map(yValue, 0, 1023, 0, 180);
  if (zValue > 100) {
    zValue = 0;
  } else {
    zValue = 1;
  }

  servo1.write(xValue);
  servo2.write(yValue);
  // print data to Serial Monitor on Arduino IDE
  // Serial.print("x = ");
  // Serial.print(xValue);
  // Serial.print(", y = ");
  // Serial.print(yValue);
  // //Serial.print(", z = ");
  // Serial.println(zValue);
  

  // Start Screen, should say Location
  Serial.print(lastInput);
  Serial.println(state_index);
  lcdIn(0,0,locations[states[0]].name);
  lcdIn(4,0,months[states[1]]);
  lcdIn(7,0,String(numdays[states[2]]));
  //lcdIn(10,0,time[loc_index]);
  // When you move the joystick left and right, move through the states
  // If move left
  if (yValue <= 90 - threshold && lastInput > 90 - threshold) {
    
    // if move too far left wrap around to the end
    lcd.clear();
    if (state_index == 0) {
      state_index = states_size - 1;
    } else {
      state_index--;
    }
    
   
  }
  // If move right
  if (yValue >= 90 + threshold) {
    // if move too far right wrap around to the beginning
    lcd.clear();
    if (state_index == states_size - 1) {
      state_index = 0;
    } else {
      state_index++;
    }
    
  }
  // If move down
  if (xValue >= 90 + threshold) {
    states[state_index] = ((states[state_index] + 1) % states_limits[state_index]);
  }

  // if move up
  if (xValue <= 90 - threshold) {
    if (states[state_index] == 0) {
      states[state_index] = states_limits[state_index];
    } else {
      states[state_index] = ((states[state_index] - 1) % states_limits[state_index]);
    }
    

  }
  // lcd.clear();
  // lcdIn(0,0,states[state_index]);
  // lcdIn(0,1,locations[scroll_index].name);

  delay(100);
}
