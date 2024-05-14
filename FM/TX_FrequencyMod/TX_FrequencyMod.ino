/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Frequency Modulation code
##############################################
*/

#include "digitalWriteFast.h"
#include "motor_control.h"
#include <SPI.h>
#include <SD.h>

//NEED DEBUG?
#define debug 0
//Is it image?
#define send_image 0

//LED OUTPUT DECLARE
#define LED A2

//Frequency Settings
#define FREQ00 1040
#define FREQ01 1440
#define FREQ11 1840
#define FREQ10 2240

//SYNCBYTE before transmission start
//Minimum 1
//Suggesting 2 ( SOT should be included, Buffer should be flushed at RX )
#define SYNCBYTE 2 //bytes

//Duration of certain Frequency
#define DURATION 1 //CYCLE

//inputString Length
//!!!!!!!!!!!!!!!CAUTION!!!!!!!!!!!!!!!!!!
//IT CONSUMES HUGE MEMORY SPACE
#define LEN 100
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//inputString Setup
const char inputString[LEN] = "hello, its reliable test for very very very very very very very long menchester code";

//128BYTE BOOLEAN-ARRAY to store binary text data
// bool string_Signal[1024] = {0, }; 
uint8_t string_Signal[128] = {0, };
int signal_length = 0;

//for sending loop
int stringIndex = 0;
uint8_t bitIndex = 0;
bool toggle = 0;

//for image
int32_t location = 0;
int32_t dataStartingOffset;
int32_t width;
int32_t height;
int16_t pixelsize;
File bmpImage;

// Define various ADC prescaler 
const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//symbol of start of transmission
//NOT USING FOR NOW
bool _symbol_SOT[8] = {0, 0, 0, 0, 0, 1, 1, 0}; // 6

bool _symbol_SOT_Control[8] = {0, 0, 0, 0, 0, 1, 0, 1}; // 5

// bool _symbol_Control_Forward[8]; // 8

// bool _symbol_Control_Backward[8]; // 9

// bool _symbol_Control_Rotate_Left[8]; // 10

// bool _symbol_Control_Rotate_Right[8]; // 11

bool _symbol_Control_ChangeSpeed[8]; // 12

bool _symbol_SOT_Image[8] = {0, 0, 0, 0, 0, 1, 1, 1}; //7

bool _symbol_EOL_Image[8] = {0, 0, 0, 0, 1, 0, 0, 0}; //8

//symbol of end of transmission
bool _symbol_EOT[8] = {0, 0, 0, 0, 0, 1, 0, 0}; // 4

//FOR BENCHMARKING=============================
// unsigned long _time_started;
// unsigned long _time_ended;
// char buffer[100];
//FOR BENCHMARKING=============================

void setup() {
  pinModeFast(LED, OUTPUT);
  buffer_text();
  //For Debuging=========================================
  if(debug){
    Serial.begin(921600);
    Serial.print("start TRANSMISSION \n");
  }
  //For Debuging=========================================
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
  digitalWriteFast(LED, 1);
  delay(SYNCBYTE);
}

void loop() {
  if(bitRead(string_Signal[stringIndex], bitIndex)){
    if(bitRead(string_Signal[stringIndex], bitIndex + 1)){
      write_11();
      if(debug){
        Serial.println("11");
      }
    }else{
      write_10();
      if(debug){
        Serial.println("10");
      }
    }
  }else{
    if(bitRead(string_Signal[stringIndex], bitIndex + 1)){
      write_01();
      if(debug){
        Serial.println("01");
      }
    }else{
      write_00();
      if(debug){
        Serial.println("00");
      }
    }
  }
  bitIndex += 2;
  if(bitIndex == 8){
    bitIndex = 0;
    stringIndex += 1;
    if(debug){
      Serial.println("1 Byte sent ");
    }
  }
  if(stringIndex >= signal_length) {
      stringIndex = 0;
      //FOR BENCHMARKING===========================
      // _time_ended = millis();
      // Serial.print("\n");
      // Serial.print((strlen(inputString) + SYNCBYTE + 1) * 8);
      // Serial.print("\n");
      // Serial.print(String((_time_ended) - (_time_started)));
      //FOR BENCHMARKING===========================
      // delay(1);
      //FOR BENCHMARKING===========================
      // _time_started = millis();
      //FOR BENCHMARKING===========================
  }
}

void buffer_text(){
  for (int i = 0; i < strlen(inputString) + SYNCBYTE + 1; i++) {
    if(i == SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
        if(_symbol_SOT[7 - j]){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    } else if(i < SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
          bitClear(string_Signal[i], 7-j);
      }
    } else if(i == (strlen(inputString) + SYNCBYTE)) {
      for (int j = 0; j < 8; j++) {
        if(_symbol_EOT[7 - j]){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    } else {
      for (int j = 0; j < 8; j++){
        if((inputString[i - SYNCBYTE] & (0x01 << j)) != 0){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    }
  }
  signal_length = (strlen(inputString) + SYNCBYTE + 1);
}

void buffer_control(char control_signal, char is_it_speed = 0){
  if(is_it_speed == 0){
    for (int i = 0; i < 1 + SYNCBYTE + 1; i++) {
      if(i == SYNCBYTE - 1){
        for (int j = 0; j < 8; j++) {
          if(_symbol_SOT_Control[7 - j]){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
      }
      else if(i < SYNCBYTE - 1){
        for (int j = 0; j < 8; j++) {
          bitClear(string_Signal[i], 7-j);
        }
      } else if(i == (1 + SYNCBYTE)) {
        for (int j = 0; j < 8; j++) {
          if(_symbol_EOT[7 - j]){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
      } else {
        for (int j = 0; j < 8; j++){
          if((control_signal & (0x01 << j)) != 0){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
      }
    }
  signal_length = (SYNCBYTE + 2) * 8;  
  }else{
    for (int i = 0; i < 2 + SYNCBYTE + 1; i++) {
      if(i == SYNCBYTE - 1){
        for (int j = 0; j < 8; j++) {
          if(_symbol_SOT_Control[7 - j]){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
      }
      else if(i < SYNCBYTE - 1){
        for (int j = 0; j < 8; j++) {
          bitClear(string_Signal[i], 7-j);
        }
      } else if(i == (1 + SYNCBYTE)) {
        for (int j = 0; j < 8; j++) {
          if(_symbol_EOT[7 - j]){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
      } else {
        for (int j = 0; j < 8; j++){
          if((control_signal & (0x01 << j)) != 0){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
        i += 1;
        for (int j = 0; j < 8; j++){
          if((is_it_speed & (0x01 << j)) != 0){
            bitSet(string_Signal[i], 7-j);
          }else{
            bitClear(string_Signal[i], 7-j);
          }
        }
      }
    }
  signal_length = (SYNCBYTE + 3) * 8;  
  }
}

void write_00(){
  for(int i = 0; i < DURATION; i++){
    delayMicroseconds(FREQ00);
    if(toggle){
      digitalWriteFast(LED, 1);
    }else{
      digitalWriteFast(LED, 0);
    }
    toggle = !toggle;
  }
  if(debug){
    Serial.print(String(FREQ00) + "\n");
  }
}

void write_01(){
  for(int i = 0; i < DURATION; i++){
    delayMicroseconds(FREQ01);
    if(toggle){
      digitalWriteFast(LED, 1);
    }else{
      digitalWriteFast(LED, 0);
    }
    toggle = !toggle;
  }
  if(debug){
    Serial.print(String(FREQ01) + "\n");
  }
}

void write_10(){
  for(int i = 0; i < DURATION; i++){
    delayMicroseconds(FREQ10);
    if(toggle){
      digitalWriteFast(LED, 1);
    }else{
      digitalWriteFast(LED, 0);
    }
    toggle = !toggle;
  }
  if(debug){
    Serial.print(String(FREQ10) + "\n");
  }
}

void write_11(){
  for(int i = 0; i < DURATION; i++){
    delayMicroseconds(FREQ11);
    if(toggle){
      digitalWriteFast(LED, 1);
    }else{
      digitalWriteFast(LED, 0);
    }
    toggle = !toggle;
  }
  if(debug){
    Serial.print(String(FREQ11) + "\n");
  }
}

void buffer_image(){
  if(location != 0){
    bmpImage = SD.open("image.bmp" ,FILE_READ);
    dataStartingOffset = readNbytesInt(&bmpImage, 0x0A, 4);
    width = readNbytesInt(&bmpImage, 0x12, 4);
    height = readNbytesInt(&bmpImage, 0x16, 4);
    pixelsize = readNbytesInt(&bmpImage, 0x1C, 2);
  }

  if(pixelsize != 24){
    Serial.println("Exception has occured || 24bpp");
    while(1);
  }

  uint8_t B, G, R;
  uint8_t Grayscale; 

  for (int32_t i = 0; i < width + SYNCBYTE + 1; i++) {
    if(i == SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
        if(_symbol_SOT[7 - j]){
          bitSet(string_Signal[8*i], 7-j);
        }else{
          bitClear(string_Signal[8*i], 7-j);
        }
      }
    }
    else if(i < SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
          bitClear(string_Signal[8*i], 7-j);
      }
    } else if(i == (width + SYNCBYTE)){
      if(location == height - 1){
        for (int j = 0; j < 8; j++) {
          if(_symbol_EOT[7 - j]){
            bitSet(string_Signal[8*i], 7-j);
          }else{
            bitClear(string_Signal[8*i], 7-j);
          }
          bmpImage.close();
          location = 0;
        }
      }else{
        for (int j = 0; j < 8; j++) {
          if(_symbol_EOL_Image[7 - j]){
            bitSet(string_Signal[8*i], 7-j);
          }else{
            bitClear(string_Signal[8*i], 7-j);
          }
        }
      }
    } else {
      B = bmpImage.read();
      G = bmpImage.read();
      R = bmpImage.read();
      Grayscale = B / 10 + G / 2 + R / 3;
      if((Grayscale == 4) || (Grayscale == 8)){
        Grayscale += 1;
      }
      for (int j = 0; j < 8; j++) {
        if(bitRead(Grayscale, 7-j)){
          bitSet(string_Signal[8*i], 7-j);
        }else{
          bitClear(string_Signal[8*i], 7-j);
        }
      }
    }
  }
  location += 1;
}

int32_t readNbytesInt(File *p_file, int position, byte nBytes){
  //can't exceed 4bytes
  //assumes 24bpp BMP
    if (nBytes > 4)
        return 0;
    p_file->seek(position);

    int32_t weight = 1;
    int32_t result = 0;
    for (; nBytes; nBytes--){
        result += weight * p_file->read();
        weight <<= 8;
    }
    return result;
}
