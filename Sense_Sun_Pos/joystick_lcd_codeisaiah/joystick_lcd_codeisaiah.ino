#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//
#include <Servo.h>

Servo servo1;
Servo servo2;
int servoPin1 = 8;
int servoPin2 = 9;

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define VRZ_PIN A3

//

int i = 0;
int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int zValue = 0; 

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


struct Location {
  const char* name;
  float latitude;
  float longitude;
};

Location locations[] = {
  { "Chicago", 41.8781, -87.6298 },
  { "Mumbai", 19.0760, 72.8777 },
  { "New York City", 40.7128, -74.0060 },
  { "Los Angeles", 34.0522, -118.2437 },
  { "Montreal", 45.5017, -73.5673 },
  { "Seoul", 37.5665, 126.9780 },
  { "Denver", 39.7392, -104.9903 },  // Added Denver with its latitude and longitude
};

Location abbreviatedLocations[] = {
  { "CHI", 41.8781, -87.6298 },
  { "MUM", 19.0760, 72.8777 },
  { "NYC", 40.7128, -74.0060 },
  { "LA", 34.0522, -118.2437 },
  { "MTL", 45.5017, -73.5673 },
  { "ICN", 37.5665, 126.9780 },
  { "DEN", 39.7392, -104.9903 },
};

void lcdIn(int x, int y, String message) {
  lcd.setCursor(x,y);
  lcd.print(message);
}

void setup() {
  //
  Serial.begin(9600) ;
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  //

  lcd.init();
  lcd.backlight();



  Serial.begin(9600);
  while (!Serial);
}

void loop() {

   // read analog X and Y analog values
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
  Serial.print("x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.print(yValue);
  Serial.print(", z = ");
  Serial.println(zValue);
  
  if (yValue <= 45) {
    i++;
  }
  lcd.clear();
  lcdIn(0,0,locations[i].name);

  delay(1000);
}
