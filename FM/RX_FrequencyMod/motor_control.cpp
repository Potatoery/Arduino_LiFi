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

#define IN1 4
#define IN2 6
#define IN3 7
#define IN4 8

#define ENA 3
#define ENB 5

int RightMotor_Enable = 5;
int LeftMotor_Enable = 6;
int RightMotor_1 = 8;
int RightMotor_2 = 9;
int LeftMotor_1 = 10;
int LeftMotor_2 = 11;
int LeftMotor_Speed = 153;
int RightMotor_Speed = 153;
bool car_var = 0;

// AF_DCMotor M1(1);
// AF_DCMotor M2(2);
// AF_DCMotor M3(3);
// AF_DCMotor M4(4);

void __init__(){
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}

void forward() {
  Serial.println("Moving forward.");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  Serial.println("Moving backward.");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  Serial.println("Turning left.");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  Serial.println("Turning right.");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stop() {
  Serial.println("Stopping motors.");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}