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
#include <SD.h>
#include <SPI.h>

//NEED DEBUG?
bool debug = 0;

//LED OUTPUT DECLARE
#define LED A2

//Frequency Settings
#define FREQ00 250
#define FREQ01 750
#define FREQ11 1250
#define FREQ10 1750

//SYNCBYTE before transmission start
//Minimum 2 ( SOT should be included, Buffer should be flushed at RX )
#define SYNCBYTE 2 //bytes

//Duration of certain Frequency
#define DURATION 1 //CYCLE

//inputString Length
//!!!!!!!!!!!!!!!CAUTION!!!!!!!!!!!!!!!!!!
//IT CONSUMES HUGE MEMORY SPACE
#define LEN 100
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//inputString Setup
const char PROGMEM inputString[LEN] = "hello, its reliable test for very very very very very very very long menchester code";

//128BYTE BOOLEAN-ARRAY to store binary text data
bool string_Signal[1024] = {0, }; 

//Photo Tests
uint8_t bmp[128][128] = {{0, }, };

//for sending loop
int stringIndex = 0;
bool toggle = 0;

// Define various ADC prescaler 
const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//symbol of start of transmission
//NOT USING FOR NOW
const bool PROGMEM _symbol_SOT[8] = {0, 0, 0, 0, 0, 1, 1, 0};

//symbol of end of transmission
const bool PROGMEM _symbol_EOT[8] = {0, 0, 0, 0, 0, 1, 0, 0};

//FOR BENCHMARKING=============================
// unsigned long _time_started;
// unsigned long _time_ended;
// char buffer[100];
//FOR BENCHMARKING=============================

void setup() {
  pinModeFast(LED, OUTPUT);
  for (int i = 0; i < strlen(inputString) + SYNCBYTE + 1; i++) {
    if(i == SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
          string_Signal[8*i + 7-j] = _symbol_SOT[7 - j];
      }
    }
    else if(i < SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
          string_Signal[8*i + 7-j] = (1);
      }
    } else if(i == (strlen(inputString) + SYNCBYTE)) {
      for (int j = 0; j < 8; j++) {
          string_Signal[8*i + 7-j] = _symbol_EOT[7 - j];
      }
    } else {
      for (int j = 0; j < 8; j++){
          string_Signal[8*i + 7-j] = ((inputString[i - SYNCBYTE] & (0x01 << j)) != 0);
      }
    }
  }
  //For Debuging=========================================
  if(debug){
    Serial.begin(921600);
    Serial.print("start TRANSMISSION \n");
  }
  //For Debuging=========================================
  if(!SD.begin(53)){
    Serial.print("Check SD Card.. failed.");
    while(1);
  }
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
  File bmpImage = SD.open("image.bmp", FILE_READ);
  int32_t dataOffset = readNbytesInt(&bmpImage, 0x0A, 4);
  int32_t width = readNbytesInt(&bmpImage, 0x12, 4); //should be 128 do not exceed it
  int32_t height = readNbytesInt(&bmpImage, 0x16, 4); //should be 128 do not exceed it
  int pixelsize = readNbytesInt(&bmpImage, 0x1C, 2);
  if(width != 128){
    Serial.print("Check width");
    while(1);
  }
  if(height != 128){
    Serial.print("Check height");
    while(1);
  }
  if(pixelsize != 8){
    Serial.print("Check pixelsize");
    while(1);
  }
  bmpImage.seek(dataOffset);

  byte data;

  for(int i = 0; i < height; i ++) {
    for (int j = 0; j < width; j ++) {
      bmp[height-1-i][j] = bmpImage.read();
    }
  }
  bmpImage.close();
  digitalWriteFast(LED, 1);
  delay(SYNCBYTE*8);
}

void loop() {
  if(string_Signal[stringIndex]){
    if(string_Signal[stringIndex+1]){
      write_11();
    }else{
      write_10();
    }
  }else{
    if(string_Signal[stringIndex+1]){
      write_01();
    }else{
      write_00();
    }
  }
  stringIndex += 2;
  if(stringIndex >= (strlen(inputString) + SYNCBYTE + 1) * 8) {
      stringIndex = 0;
      //FOR BENCHMARKING===========================
      // _time_ended = millis();
      // Serial.print("\n");
      // Serial.print((strlen(inputString) + SYNCBYTE + 1) * 8);
      // Serial.print("\n");
      // Serial.print(String((_time_ended) - (_time_started)));
      //FOR BENCHMARKING===========================
      delay(10);
      //FOR BENCHMARKING===========================
      // _time_started = millis();
      //FOR BENCHMARKING===========================
  }
}

int32_t readNbytesInt(File *filename, int position, byte nBytes) {
    if (nBytes > 4)
        //int32_t can't exceed 4bytes
        return 0;

    filename->seek(position);

    int32_t weight = 0;
    int32_t result = 0;

    for (; nBytes; nBytes--)
    {
        result += weight ^ filename->read();
        weight <<= 8;
    }
    return result;
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
