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

//NEED DEBUG?
bool debug = 0;

//LED OUTPUT DECLARE
#define LED A2

//Frequency Settings
#define FREQ00 750
#define FREQ01 1250
#define FREQ11 1750
#define FREQ10 2250

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
bool _symbol_SOT[8] = {0, 0, 0, 0, 0, 1, 1, 0};

//symbol of end of transmission
bool _symbol_EOT[8] = {0, 0, 0, 0, 0, 1, 0, 0};

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
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
  digitalWriteFast(LED, 1);
  delay(SYNCBYTE);
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
      delay(100);
      //FOR BENCHMARKING===========================
      // _time_started = millis();
      //FOR BENCHMARKING===========================
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
