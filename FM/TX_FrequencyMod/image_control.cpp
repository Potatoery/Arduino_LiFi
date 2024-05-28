/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Servo controll unit
##############################################
*/

#include "image_control.h"
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <SD.h>
  
image_control::image_control(){
  if (!SD.begin(10)) {
      Serial.println("initialization failed!");
      while (1);
    }
  File bmpImage = SD.open("picture.bmp",FILE_READ);
  size = bmpImage.size();
}

File image_control::getImage(){
  return bmpImage;
}

uint16_t image_control::getSize(){
  return size;
}
