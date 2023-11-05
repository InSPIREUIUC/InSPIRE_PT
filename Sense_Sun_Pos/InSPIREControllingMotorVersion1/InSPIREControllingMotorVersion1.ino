Aurdino code for controlling motors:
#include<Servo.h>
Servo s1;
Servo s2; 
int count = 0;

void setup() {
  s1.attach(9);
  s2.attach(8);
  s1.write(0);
  s2.write(0);

}

void loop() {


delay(500); 

s1.write(count);
s2.write(count); 

count++;

}