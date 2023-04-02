#include <SoftwareSerial.h>

SoftwareSerial SerialBt(2, 3);

// ultrasonic pins setup
int trigPin = A0;    
int echoPin = A1;
int rO=25;
int lO=10;
// declares duration variable to store decimal result of how long the ultrasonic
// signal travels between robot and a hand (in millisecond)
float duration;
// declares distance variable to store the decimal result of the distance between
// the robot and the hand
float distance;

// motors pins setup
// right motor setup
// in3 pin on h-bridge to pin 8
int in3 = 8;
// in4 pin on h-bridge to pin 9
int in4 = 9;
// enb pin on h-bridge to pin 5
int enb = 5;

//left motor setup
// in1 pin on h-bridge to pin 6
int in1 = 6;
// in2 pin on h-bridge to pin 7
int in2 = 7;
// ena pin on h-bridge to pin 10
int ena = 10;

// infrareed sensors pins setup
// left sensor uses pin 13
int LEFT = 4; 
// right sensor uses pin 12
int RIGHT =12;

// honk
int make_sound = 0;
int honk = 11;

// prepares components before running
void setup() {
  // sets data transfer rate to print out to serial monitor
  Serial.begin(9600);

  // sets left motor pins as output to send the power to motor
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // sets speed control pin (ena) of left motor as output
  pinMode(ena, OUTPUT);

  // sets right motor pins as output to send power to the motor
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // sets speed control pin (enb) of right motor as output
  pinMode(enb, OUTPUT);

  // set up infrared sensors to receive input
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);

  // set up ultrasonic sensor
  // configures the trigger pin to output mode to send signal
  pinMode(trigPin, OUTPUT);
  // configures the echo pin to input mode to receive signal
  pinMode(echoPin, INPUT);

  pinMode(honk, OUTPUT);
  
  //Serial Begin for the bluetooth
  SerialBt.begin(9600);
  pinMode(13, OUTPUT);

  digitalWrite(honk, LOW);
}
bool ultrasonicProtocol=false;
bool controllerProtocol=false;
float myTime=0;
void loop() {
  // Collects bluetooth update
    char charRead=' ';
    int btAval=SerialBt.available();
    if (btAval>0) {
        charRead=SerialBt.read();
        Serial.write(charRead);
     }

    if (Serial.available()>0) {
        SerialBt.write(Serial.read());
    }
    if (charRead=='N'){
        digitalWrite(13, HIGH);
    }
    if(charRead=='O'){
        digitalWrite(13, LOW);
    }
    if(charRead=='S'){
      ultrasonicProtocol=false;
      controllerProtocol=false;
    }
    if(charRead=='C'||controllerProtocol==true){
      delay(50);
      controllerProtocol=true;
      if(charRead=='F'){
        // sets speed of left motor as 155 (0 to max 255)
        analogWrite(ena, 155);
        // sends in1 as LOW and in2 as HIGH to move forward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        
        // sets speed of right motor as 155 (0 to max 255)
        analogWrite(enb, 160);
        // sends in3 as LOW and in4 as HIGH to move forward
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
      
      }
      else if(charRead=='B'){
        // sets speed of left motor as 155 (0 to max 255)
        analogWrite(ena, 155);
        // sends in1 as LOW and in2 as HIGH to move forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        
        // sets speed of right motor as 155 (0 to max 255)
        analogWrite(enb, 175);
        // sends in3 as LOW and in4 as HIGH to move forward
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      
      }else if(charRead=='R') {
        // sets speed of left motor as 155 (0 to max 255)
        analogWrite(ena, 155);
        // sends in1 as LOW and in2 as HIGH to move forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        
        // sets speed of right motor as 100 (0 to max 255)
        analogWrite(enb, 100);
        // sends in3 as HIGH and in4 as LOW to move backward
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
    
      }else if(charRead=='L') {
        // sets speed of left motor as 100 (0 to max 255)
        analogWrite(ena, 100);
        // sends in3 as HIGH and in4 as LOW to move backward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        
        // sets speed of right motor as 155 (0 to max 255)
        analogWrite(enb, 155);
        // sends in3 as LOW and in4 as HIGH to move forward
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      }else if (charRead=='T'){
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
      }
    }
    if(charRead=='U'||ultrasonicProtocol==true){
       delay(50);
       ultrasonicProtocol=true;
      // ultrasonic sensor sends signal as HIGH
      digitalWrite(trigPin, HIGH);
      // sends ultrasonic signal as HIGH for 10 milliseconds
      delayMicroseconds(10);
      // stops sending ultrasonic signal
      digitalWrite(trigPin, LOW);
      
      // measure duration of the signal travels from robot to echo pin
      // result is stored in duration variable
      duration = pulseIn(echoPin, HIGH);
      
      // calculate the distance between the robot and the hand (in cm)
      distance = 0.017 * duration;// Speed of sound wave, 0.034m/millisecond divided by 2
      
      // print the distance between the robot and the hand to the serial monitor
      Serial.print("distance");
      Serial.println(distance);
      
      // gets data from right infrared sensor to know if it detects a hand and store data in Right_value
      // if Right_value is 1, sensor detects nothing. 0 means sensor detects a hand
      int Right_Value = digitalRead(RIGHT);
      // gets data from left infrared sensor to know if it detects a hand and store data in Left_value
      // if Left_value is 1, sensor detects nothing. 0 means sensor detects a hand
      int Left_Value = digitalRead(LEFT);
      
      // print the detecting result of right sensor to serial monitor
      Serial.print("RIGHT");
      Serial.println(Right_Value);
      // print the detecting result of left sensor to serial monitor
      Serial.print("LEFT");
      Serial.println(Left_Value);
      
    
      // if a hand is in range from 10 - 30 cm, and both infrared sensors don't detect anything, moves foward
      if((Right_Value==1) && (distance>=10 && distance<=30) && (Left_Value==1)){
        Serial.println("forward");
        // sets speed of left motor as 155 (0 to max 255)
        analogWrite(ena, 155+lO);
        // sends in1 as LOW and in2 as HIGH to move forward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        
        // sets speed of right motor as 155 (0 to max 255)
        analogWrite(enb, 155+rO);
        // sends in3 as LOW and in4 as HIGH to move forward
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
      
      }else if( (Left_Value==0)&&(distance<=8)&&(Right_Value==0)){
        Serial.println("backward");
    
        make_sound = 1;
        if(myTime==0){
          myTime=millis();
          digitalWrite(honk, HIGH);
        }
        // sets speed of left motor as 155 (0 to max 255)
        analogWrite(ena, 155);
        // sends in1 as LOW and in2 as HIGH to move forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        
        // sets speed of right motor as 155 (0 to max 255)
        analogWrite(enb, 155);
        // sends in3 as LOW and in4 as HIGH to move forward
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      
      }
      // if the hand is detected by right infrared sensor only, turns right till both sensors detect the hand
      else if((Right_Value==0) && (Left_Value==1)) {
        Serial.println("right");
        // sets speed of left motor as 155 (0 to max 255)
        analogWrite(ena, 155+lO);
        // sends in1 as LOW and in2 as HIGH to move forward
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        
        // sets speed of right motor as 100 (0 to max 255)
        analogWrite(enb, 100+rO);
        // sends in3 as HIGH and in4 as LOW to move backward
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
    
      }
      // if the hand is detected by left infrared sensor only, turns left till both sensors detect the hand
      else if((Right_Value==1)&&(Left_Value==0)) {
        Serial.println("left");
        // sets speed of left motor as 100 (0 to max 255)
        analogWrite(ena, 100+lO);
        // sends in3 as HIGH and in4 as LOW to move backward
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        
        // sets speed of right motor as 155 (0 to max 255)
        analogWrite(enb, 155+rO);
        // sends in3 as LOW and in4 as HIGH to move forward
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      }
      // if the hand is detected by both nfrared sensors, then the hand is close and will stop moving
      else if((Right_Value==1)&&(Left_Value==1)) {
        Serial.println("stop");
        // sends low to bout in1 and in2 of left motor to stop the motor
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        
        // sends low to bout in3 and in4 of right motor to stop the motor
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
      }
      
      else if((Right_Value==0)&&(Left_Value==0)) {
        Serial.println("stop");
        // sends low to bout in1 and in2 of left motor to stop the motor
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        
        // sends low to bout in3 and in4 of right motor to stop the motor
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
      }
      

    }

    if((millis()-myTime)>=250){
        digitalWrite(honk, LOW);
    }
    if((millis()-myTime)>1000){
        myTime=0;
    }
}
