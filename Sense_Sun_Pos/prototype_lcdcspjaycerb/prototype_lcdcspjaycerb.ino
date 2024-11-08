#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

unsigned long lastUpdateTime = 0;  // Keep track of the last time the solar position was calculated
float solarAzimuth = 0.0;
float solarElevation = 0.0;

void lcdIn(int x, int y, String message) {
  lcd.setCursor(x,y);
  lcd.print(message);
}

void printDateTime(unsigned long millisec) {
  unsigned long seconds = millisec / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;

  Serial.print(days);
  Serial.print("d ");
  Serial.print(hours % 24);
  Serial.print("h ");
  Serial.print(minutes % 60);
  Serial.print("m ");
  Serial.print(seconds % 60);
  Serial.print("s");
}

unsigned long convertDateTimeToMillis(int year, int inputMonth, int day, int hour, int minute) {
  tmElements_t tm;
  tm.Year = year - 1970;  // Years since 1970
  tm.Month = inputMonth;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = 0;

  time_t t = makeTime(tm);
  return static_cast<unsigned long>(t);
}

float calculateJulianDate(int year, int month, int day) {
  if (month <= 2) {
    year--;
    month += 12;
  }
  int A = year / 100;
  int B = 2 - A + A / 4;
  return static_cast<float>(static_cast<unsigned long>(365.25 * (year + 4716))) + static_cast<float>(static_cast<unsigned long>(30.6001 * (month + 1))) + day - 1524.5;
}

void calculateSolarPosition(unsigned long millisec, float latitude, float longitude) {
  float julianDate = calculateJulianDate(2023, 1, 1) + millisec / 8.64e7;  // 8.64e7 is the number of milliseconds in a day
  float julianCentury = (julianDate - 2451545.0) / 36525.0;
  float meanLongitude = fmod(280.46646 + julianCentury * (36000.76983 + julianCentury * 0.0003032), 360.0);
  float meanAnomaly = fmod(357.52911 + julianCentury * (35999.05029 - 0.0001537 * julianCentury), 360.0);
  float eccentricity = 0.016708634 - julianCentury * (0.000042037 + 0.0001537 * julianCentury);
  float equationOfTime = (1.914602 - julianCentury * (0.004817 + 0.000014 * julianCentury)) * sin(radians(meanAnomaly)) + (0.019993 - 0.000101 * julianCentury) * sin(radians(2.0 * meanAnomaly));

  float solarNoon = 12.0 * 60.0 - (equationOfTime + 4.0 * longitude - 60.0 * (millisec % 86400000) / 3600000);
  if (solarNoon < 0) {
    solarNoon += 1440.0;
  }

  float solarTime = solarNoon + 4.0 * longitude - 60.0 * (millisec % 86400000) / 3600000;
  if (solarTime < 0) {
    solarTime += 1440.0;
  }

  //float solarDeclination = asin(sin(radians(meanAnomaly)) * sin(radians(23.45)));  // old calculation for declination
  double solarDeclination = 23.45 *sin((360.0/365.0)*(julianDate-81));

  solarElevation = asin(sin(radians(latitude)) * sin(solarDeclination) + cos(radians(latitude)) * cos(solarDeclination) * cos(radians(solarTime / 4.0 - 180.0)));

  solarElevation = degrees(solarElevation);

  solarAzimuth = atan2(sin(radians(solarTime / 4.0 - 180.0)),
                       cos(radians(solarTime / 4.0 - 180.0)) * sin(radians(latitude)) - tan(solarDeclination) * cos(radians(latitude)));

  solarAzimuth = degrees(solarAzimuth);
  if (solarAzimuth < 0) {
    solarAzimuth += 360.0;
  }
}

void setup() {

  lcd.init();
  lcd.backlight();



  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("Initialization complete. Ready for input...");
}

void loop() {
  Serial.println("Enter a location (Chicago, Mumbai, New York City, Los Angeles, Montreal, Seoul, Denver): ");
  while (Serial.available() == 0) {
    // Wait for user input
  }

  String userLocation = Serial.readStringUntil('\n');
  Serial.println("You entered: " + userLocation);
  int inputMonth;
  bool locationFound = false;
  for (int i = 0; i < sizeof(locations) / sizeof(locations[0]); ++i) {
    if (userLocation.equalsIgnoreCase(locations[i].name)) {
      while (true) {
        Serial.println("Enter a month (digits, 1-Jan, 2-Feb, ...): ");
        while (Serial.available() == 0) {
          // Wait for user input
        }

        String monthInput = Serial.readStringUntil('\n');
        if (isdigit(monthInput.charAt(0))) {
          inputMonth = monthInput.toInt();
          if (inputMonth >= 1 && inputMonth <= 12) {
            Serial.println("You entered month: " + String(inputMonth));
            break;
          } else {
            Serial.println("Invalid month format. Please enter digits between 1 and 12.");
          }
        } else {
          // Attempt to parse month as a string abbreviation
          monthInput.toLowerCase();
          String months[] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
          for (int i = 0; i < 12; ++i) {
            if (monthInput.equals(months[i])) {
              inputMonth = i + 1;
              Serial.println("You entered month: " + monthInput);
              break;
            }
          }

          if (inputMonth >= 1 && inputMonth <= 12) {
            break;
          } else {
            Serial.println("Invalid month format. Please enter digits between 1 and 12 or use 3-letter abbreviations (e.g., 1 or Jan).");
          }
        }
      }


      Serial.println("Enter a specific time (24-hour clock, e.g., 13:30): ");
      while (Serial.available() == 0) {
        // Wait for user input
      }

      String timeString = Serial.readStringUntil('\n');
      Serial.println("You entered time: " + timeString);

      Serial.println("Enter a year: ");
      while (Serial.available() == 0) {
        // Wait for user input
      }

      int year = Serial.parseInt();
      Serial.println("You entered year: " + String(year));

      int day;
      while (true) {
        Serial.println("Enter a day: ");
        delay(1000);
            while (Serial.available() == 0) {
          // Wait for user input
        }

        day = Serial.parseInt();
        if (day >= 1 && day <= 31) {
          Serial.println("You entered day: " + String(day));
          break;
        } else {
          Serial.println("Invalid day format. Please enter digits between 1 and 31.");
        }
      }

      unsigned long userTime = convertDateTimeToMillis(year, inputMonth, day, 0, 0);

      // Calculate solar position every 5 seconds or change this interval as needed
      if (userTime != 0 && (millis() - lastUpdateTime >= 5000 || lastUpdateTime == 0)) {
        calculateSolarPosition(userTime, locations[i].latitude, locations[i].longitude);
        lastUpdateTime = millis();
      }

      // Print solar position
      Serial.print("Location: ");
      Serial.println(locations[i].name);
      lcd.setCursor(0, 0);
      lcd.print(abbreviatedLocations[i].name);

      Serial.print("Specified Time: ");
      printDateTime(userTime);
      Serial.println();

      Serial.print("Solar Azimuth: ");
      Serial.print(solarAzimuth);

      Serial.print(" degrees, Solar Elevation: ");
      Serial.print(solarElevation);
      Serial.println(" degrees");

      lcd.setCursor(0, 1);
      lcd.print("SA");
      lcd.setCursor(2, 1);
      lcd.print(solarAzimuth);
      lcd.setCursor(9, 1);
      lcd.print("SE");
      lcd.setCursor(11, 1);
      lcd.print(solarElevation);
      
      lcd.setCursor(4,0);
      if (inputMonth < 10){
        lcd.print("0" + String(inputMonth));
      }
      else {
        lcd.print(inputMonth);
      }
     lcdIn(6,0,"/");
     lcd.setCursor(7,0);
      if (day < 10){
        lcd.print("0" + String(day));
      }
      else {
        lcd.print(day);
      }
      lcdIn(9,0,"/");
      lcdIn(10,0,String(year));

      Serial.println();

      locationFound = true;
      break;
    }
  }

  if (!locationFound) {
    Serial.println("Invalid location. Please enter a valid location.");
  }

  delay(1000);
}
