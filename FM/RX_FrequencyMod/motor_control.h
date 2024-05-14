/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Servo controll unit
##############################################
*/

void __init__(bool whichcar = 0);
void motor_init();
bool forward(int duration = 0);
bool backward(int duration = 0);
bool rotate(bool LR, int duration = 0);
bool changeSpeed(int speed = 155);
