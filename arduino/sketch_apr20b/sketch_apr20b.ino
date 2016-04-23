#include<LiquidCrystal.h>

//Ultrasonic module
unsigned long echo = 0;
int ultraSoundSignal = 10; // Ultrasound signal pin
int ultraSoundEcho = 6;    //Ultrasound echo pin
unsigned long ultrasoundValue = 0;
unsigned long tankDepth=10;
String levelString;

//motor driver
int motorPin0=8;
int motorPin1=9;
boolean motorStatus=false;

//system settings
unsigned long minimumDepth = 2;
unsigned long maximumDepth = 6;
int contrast=1;
int updateFrequency=10;
int precision=0;
int displayValue=0;
int previousValue=0;

//Bluetooth interface
String inputString="";
char junk='A';
boolean manual=false;

//manual OVerride
int switchTime=0;
int switchMax=5;
int buttonState=0;
int manualSwitch=0;

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
 pinMode(A1,INPUT);//Manual Override Motor Control
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
  
  if(manual)
  {
    lcd.print("WATER LEVEL MANU"); // prints 16x2 LCD MODULE
  }
  else
  {
    lcd.print("WATER LEVEL AUTO");
  }
  
  x = ping();
  
  if(displayValue-x>precision || x-displayValue > precision )
  {
    displayValue=x;
  }
  
  
  if(!manual)
  {
    if(x>maximumDepth)
    {
      digitalWrite(motorPin0,HIGH);
      digitalWrite(motorPin1,LOW); 
      
      if(!motorStatus)
      {
        Serial.println("Water level low : Switching ON Motor");  
      }  
      motorStatus=true;
    }
    else if(x<=minimumDepth)
    {
      digitalWrite(motorPin0,HIGH);
      digitalWrite(motorPin1,HIGH);
      if(motorStatus)
      {
        Serial.println("Water level reached maximum : Switching OFF Motor");
      } 
      motorStatus=false;
    }
  }
  
  levelString=String(displayValue);
  lcd.setCursor(2,1);           //sets the cursor at row 1 column 2
  
  if(tankDepth-x>0)
  {
    lcd.print(String(tankDepth-x));     // prints HELLO WORLD
  }
  //Bluetooth module interface
  
//  if(Serial.available())
//  {
//  
//      while(Serial.available())
//      {
//        char inChar = (char)Serial.read(); //read the input
//        inputString += inChar;        //make a string of the characters coming on serial
//      }
//      
//      Serial.println(inputString);
//      
//      while (Serial.available() > 0)  
//        { junk = (char)Serial.read() ; }      // clear the serial buffer
//      
//      if(manual)
//      {
//         if(inputString=="a")
//         {
//           motorStatus=false;
//           digitalWrite(motorPin0,HIGH);
//           digitalWrite(motorPin1,HIGH);
//         }  
//         else if(inputString=="b")
//         {
//           motorStatus=true;
//           digitalWrite(motorPin0,HIGH);
//           digitalWrite(motorPin1,LOW);
//         }
//      }
//      
//      if(inputString == "1"){         //in case of 'a' turn the LED on
//            manual=true;
//            digitalWrite(13,LOW);
//      }else if(inputString == "0"){   //incase of 'b' turn the LED off
//            manual=false;
//            digitalWrite(13,HIGH);
//      }
//      
//      inputString = "";
//  }
  
  //Display logic

  if(previousValue!=x)
  {
    Serial.println("Water Level(in inches) : "+String(tankDepth-x));
    previousValue=x;
  }
  
  //Manual Override Logic
  buttonState=digitalRead(A0);
  
  
  if(buttonState==HIGH)
  {
    switchTime++;
    if(switchTime>=switchMax)
    {
      if(!manual)
      {
        digitalWrite(13,LOW);
      }
      else
      {
        digitalWrite(13,HIGH);
      }
    }    
  }
  else
  {
    if(switchTime>=switchMax)
    {
      switchTime=0;
      
      manual=!manual;
      
      if(manual)
      {
        digitalWrite(13,LOW);
      }
      else
      {
        digitalWrite(13,HIGH);
      }
      
//      Serial.println("Manual mode "+String(manual));
      
    }
    switchTime=0;
  }
  
  manualSwitch=digitalRead(A1);
  
  if(manual)
  {
//     Serial.println("manualSwitch " + manualSwitch); 
    
     if(manualSwitch==LOW)
     {
       motorStatus=false;
       digitalWrite(motorPin0,HIGH);
       digitalWrite(motorPin1,HIGH);
     }  
     else if(manualSwitch==HIGH)
     {
       motorStatus=true;
       digitalWrite(motorPin0,HIGH);
       digitalWrite(motorPin1,LOW);
     }
  }  
  
  delay(200); //delay 1/4 seconds.  
}
