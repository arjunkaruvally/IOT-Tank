#include<LiquidCrystal.h>

//Ultrasonic module
unsigned long echo = 0;
int ultraSoundSignal = 10; // Ultrasound signal pin
int ultraSoundEcho = 6;
unsigned long ultrasoundValue = 0;
unsigned long tankDepth=7;
String levelString;

//motor driver
int motorPin0=8;
int motorPin1=9;

//system settings
unsigned long maximumDepth = 4;
int contrast=1;
int updateFrequency=10;
int precision=0;
int displayValue=0;

int updateVariable=0;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // sets the interfacing pins

void setup()
{
 
 lcd.begin(16, 2);  // initializes the 16x2 LCD
 pinMode(ultraSoundSignal,OUTPUT);
 pinMode(ultraSoundEcho,INPUT);
 pinMode(motorPin0,OUTPUT);
 pinMode(motorPin1,OUTPUT);
 pinMode(A0,INPUT);//Manual Override
 Serial.begin(9600);
 displayValue=ping();
}

unsigned long ping()
{ 
  pinMode(ultraSoundSignal, OUTPUT); // Switch signalpin to output
  digitalWrite(ultraSoundSignal, LOW); // Send low pulse 
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(ultraSoundSignal, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(ultraSoundSignal, LOW); // Holdoff
  pinMode(ultraSoundSignal, INPUT); // Switch signalpin to input
  digitalWrite(ultraSoundSignal, HIGH); // Turn on pullup resistor
  // please note that pulseIn has a 1sec timeout, which may
  // not be desirable. Depending on your sensor specs, you
  // can likely bound the time like this -- marcmerlin
  // echo = pulseIn(ultraSoundSignal, HIGH, 38000)
  echo = pulseIn(ultraSoundEcho, HIGH); //Listen for echo
  ultrasoundValue = (echo / 58.138) * .39; //convert to CM then to inches
  return ultrasoundValue;
}

void loop()
{
  int x=0;
  lcd.clear();  
  lcd.setCursor(0,0);           //sets the cursor at row 0 column 0
  lcd.print("WATER LEVEL"); // prints 16x2 LCD MODULE
    
  x = ping();
  
  if(displayValue-x>precision || x-displayValue > precision )
  {
    displayValue=x;
  }
  
  Serial.println(x);
  
  if(x>maximumDepth)
  {
    digitalWrite(motorPin0,HIGH);
    digitalWrite(motorPin1,LOW); 
  }
  else
  {
    digitalWrite(motorPin0,HIGH);
    digitalWrite(motorPin1,HIGH);
  }
     
  levelString=String(displayValue);
  lcd.setCursor(2,1);           //sets the cursor at row 1 column 2
  lcd.print(levelString);     // prints HELLO WORLD
 
  delay(250); //delay 1/4 seconds.  
}
