#include "AFMotor.h"
#include <Servo.h> 
#include "NewPing.h"
AF_DCMotor M1(1);
AF_DCMotor M2(2);
AF_DCMotor M3(3);
AF_DCMotor M4(4);
Servo servo;
const int servoPin = 10;
const int TRIG = A5;
const int ECHO = A4;
const int MAX_DISTANCE = 100;
NewPing sonar(TRIG, ECHO, MAX_DISTANCE);

int speed = 0;
int state = 10;
int angle = 90;

void setup() {
  M1.setSpeed(speed);
  M2.setSpeed(speed);
  M3.setSpeed(speed);
  M4.setSpeed(speed);
  M1.run(RELEASE);
  M2.run(RELEASE);
  M3.run(RELEASE);
  M4.run(RELEASE);
  servo.attach(servoPin);
  servo.write(angle);
  delay(1000);
  //작동 시작
  M1.run(FORWARD);
  M2.run(FORWARD);
  M3.run(FORWARD);
  M4.run(FORWARD);  
}

void loop() {
  servo.write(angle);
  delay(20);
  int distance = sonar.ping_cm();

  if(distance>0 && distance<20){
      move();
      M1.run(FORWARD);
      M2.run(FORWARD);
      M3.run(FORWARD);
      M4.run(FORWARD);  
  }   
  if (angle == 150) state = -10;    
  else if (angle == 30) state = 10; 
  angle += state;
}
void move(){
      M1.run(BACKWARD);
      M2.run(BACKWARD);
      M3.run(BACKWARD);
      M4.run(BACKWARD); 
     delay(400);

    if(angle>=90){
      M1.run(FORWARD);
      M2.run(FORWARD);
      M3.run(BACKWARD);
      M4.run(BACKWARD); 
      delay(500);   
    }
    
    else{
      M1.run(BACKWARD);
      M2.run(BACKWARD);
      M3.run(FORWARD);
      M4.run(FORWARD); 
      delay(500);   
    }

    angle = 90;
    servo.write(angle);
    delay(100);
}