/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Servo controll unit
##############################################
*/

#include "motor_control.h"
#include "AFMotor.h"
#include <Arduino.h>

AF_DCMotor motor_1(1);  // 모터 1 객체 생성 (앞바퀴 왼쪽)
AF_DCMotor motor_2(2);  // 모터 2 객체 생성 (앞바퀴 오른쪽)
AF_DCMotor motor_3(3);  // 모터 3 객체 생성 (뒷바퀴 왼쪽)
AF_DCMotor motor_4(4);  // 모터 4 객체 생성 (뒷바퀴 오른쪽)

void __init__(){
  motor_1.setSpeed(191); 
  motor_2.setSpeed(191); 
  motor_3.setSpeed(191); 
  motor_4.setSpeed(191); 
}

void forward() {
  Serial.println("Moving forward.");
  motor_1.run(FORWARD);
  motor_2.run(FORWARD);
  motor_3.run(FORWARD);
  motor_4.run(FORWARD);
}

void backward() {
  Serial.println("Moving backward.");
  motor_1.run(BACKWARD);
  motor_2.run(BACKWARD);
  motor_3.run(BACKWARD);
  motor_4.run(BACKWARD);
}


void stop() {
  Serial.println("Stopping motors.");
  motor_1.run(RELEASE);  
  motor_2.run(RELEASE);  
  motor_3.run(RELEASE);  
  motor_4.run(RELEASE); 
}