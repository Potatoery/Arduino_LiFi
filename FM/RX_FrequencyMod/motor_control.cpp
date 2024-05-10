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
#include <Arduino.h>

int RightMotor_Enable = 5;
int LeftMotor_Enable = 6;
int RightMotor_1 = 8;
int RightMotor_2 = 9;
int LeftMotor_1 = 10;
int LeftMotor_2 = 11;
int LeftMotor_Speed = 153;
int RightMotor_Speed = 153;

void __init__(){
  pinMode(RightMotor_Enable, OUTPUT);    
  pinMode(LeftMotor_Enable, OUTPUT);
  pinMode(RightMotor_1, OUTPUT);
  pinMode(RightMotor_2, OUTPUT);
  pinMode(LeftMotor_1, OUTPUT);
  pinMode(LeftMotor_2, OUTPUT);
}

void motor_init();

bool forward(int duration = 0){
  digitalWrite(RightMotor_1, HIGH);
  digitalWrite(RightMotor_2, LOW);
  digitalWrite(LeftMotor_1, HIGH);
  digitalWrite(LeftMotor_2, LOW);
  analogWrite(RightMotor_Enable, LeftMotor_Speed);
  analogWrite(LeftMotor_Enable, RightMotor_Speed);
}

bool backward(int duration = 0){
  digitalWrite(RightMotor_1, LOW);
  digitalWrite(RightMotor_2, HIGH);
  digitalWrite(LeftMotor_1, LOW);
  digitalWrite(LeftMotor_2, HIGH);
  analogWrite(RightMotor_Enable, LeftMotor_Speed);
  analogWrite(LeftMotor_Enable, RightMotor_Speed);
}

bool rotate(bool LR, int duration = 0){
  digitalWrite(RightMotor_1, LR);
  digitalWrite(RightMotor_2, !LR);
  digitalWrite(LeftMotor_1, !LR);
  digitalWrite(LeftMotor_2, LR);
  analogWrite(RightMotor_Enable, LeftMotor_Speed);
  analogWrite(LeftMotor_Enable, RightMotor_Speed);
}

bool changeSpeed(int speed){
  int LeftMotor_Speed = speed;
  int RightMotor_Speed = speed;
}
