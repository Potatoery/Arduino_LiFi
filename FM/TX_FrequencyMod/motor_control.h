/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Servo controll unit
##############################################
*/

void __init__(bool whichcar);
void motor_init();
bool forward(int duration);
bool backward(int duration);
bool rotate(bool LR, int duration);
bool changeSpeed(int speed);
