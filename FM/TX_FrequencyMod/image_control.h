/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Servo controll unit
##############################################
*/
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class image_control{
  public:
    uint16_t size = 0;
    File bmpImage;
    image_control();
    File getImage();
    uint16_t getSize();
};
