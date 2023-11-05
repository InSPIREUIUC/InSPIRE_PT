

float input_voltage = 0.0;
float TopLeft=0.0;
float TopRight=0.0;
float BottomLeft=0.0;
float BottomRight=0.0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
int analog_value = analogRead(A0);
TopLeft = (analog_value * 100.0) / 1024.0;  
int analog_value2 = analogRead(A1);
TopRight = (analog_value2 * 100.0) / 1024.0;  
int analog_value3 = analogRead(A2);
BottomLeft = (analog_value3 * 100.0) / 1024.0;  
int analog_value4 = analogRead(A3);
BottomRight = (analog_value4 * 100.0) / 1024.0;  
/*
Serial.print("Top Left:");
Serial.println(TopLeft);
Serial.print("Top Right:");
Serial.println(TopRight);
Serial.print("Bottom Left:");
Serial.println(BottomLeft);
Serial.print("Bottom Right:");
Serial.println(BottomRight);
Serial.print("\n");*/

if((TopLeft > TopRight)&&(TopLeft > BottomLeft)&&(TopLeft > BottomRight)){
  Serial.print("Top Left:");
  Serial.println(TopLeft);
}

else if((TopRight > TopLeft)&&(TopRight > BottomLeft)&&(TopRight > BottomRight)){
  Serial.print("Top Right:");
  Serial.println(TopRight);
}

else if((BottomLeft > TopLeft)&&(BottomLeft > TopRight)&&(BottomLeft > BottomRight)){
  Serial.print("Bottom Left:");
  Serial.println(BottomLeft);
}

else{
  Serial.print("Bottom Right:");
  Serial.println(BottomRight);
}

delay(1000);
}





