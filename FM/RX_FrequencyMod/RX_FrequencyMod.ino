/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : RX Code for arduino VLC Project
Note : Frequency Modulation code
##############################################
*/

#include "motor_control.h"
#include <SPI.h>
#include <SdFat.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define debug 0
#define STABLE 0
#define ber_test 0
#define auto_threshold 1
#define exponential 0.98
#define PD A2
#define SAMPLING_PERIOD 128
#define rev4 0
#define mm_buffer_size 128

#if STABLE == 0
  #define FREQ00 400
  #define FREQ01 800
  #define FREQ11 1200
  #define FREQ10 1600
  #define FREQ_ABORT 2000
#else
  #define FREQ00 1000
  #define FREQ01 1400
  #define FREQ11 1800
  #define FREQ10 2200
  #define FREQ_ABORT 2600
#endif

#define boundary 200

//for find threshold
float suggestion_temp = 0;

//PD threshold
float threshold = 255;

#if rev4 == 0
  const unsigned char PS_16 = (1 << ADPS2); 
  const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
  const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
  const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
#endif

//Address for i2c
char address;
LiquidCrystal_I2C lcd(0x3c,  16, 2);

//pulse width calc
unsigned long current_time;
unsigned long last_time;
int current_period = 0;

//Binarys
bool current_state = 0;
bool previous_state = 0;
uint8_t image_state = 0;
int state = 0;

//PD VALUE
int voltage;

//for IDEA Develop
int8_t bitIndex = 0;
int temp;
uint16_t image_byte_count = 0;
unsigned long time_comm_start;
unsigned long time_comm_ended;

//car control values
bool speed_comm = 0;

//for image
char name[] = "image_0000.jpg";
SdFile bmpImage;
SdFat sd;
uint8_t size_divider = 0;
uint8_t image_index = 0;
uint8_t location = 1;

//Received values
char ret = 0;
uint8_t ber_count = 0;
float ber = 0;
float comm_speed = 0;
const char ber_string[] = "The Catholic University of Korea";
char buffer = 0;
bool control_buffer[24] = {0, };
uint8_t image_buffer[mm_buffer_size] = {0, };
String string_buffer = "";

void setup() {
  Serial.begin(2000000);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  __init__(); 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("hi");
  while (!sd.begin(10)) {
    sd.initErrorHalt();
  }
  sd.remove(name);
  pinMode(PD, INPUT);
  // ADC 5kHz to ~2MHz

  for(address = 1; address < 127; address++ ){
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0){
      if (address<16) 
        Serial.print("0");
      break;
    }
  }

  Serial.println(uint8_t(address));

  #if rev4 == 0
    ADCSRA &= ~PS_128;
    ADCSRA |= PS_16;
  #endif
  //For Debuging=========================================
  if(debug && ber_test){
    Serial.println("Debug & BER can't enabled at the same time");
    while(1);
  }
  //For Debuging=========================================
  Serial.print(String("Started")+"\n");
}

void loop() {
  current_state = get_pd();
  //only starts when state has changed
  if (current_state != previous_state){
    //current time update
    current_time = micros();
    //get current_time - last_time
    current_period = get_period();
    //Check Violation
    if((current_period) > 3000 && state != 0){ Serial.println("result not confident"); }
    //update received values
    get_binary();
    //now update last_time to current_time
    last_time = current_time;
    if(debug){
      Serial.print(String(voltage) + " / " +String(current_period)+" !! "+"\n");
    }
  }
  if(debug){
      Serial.println(String(voltage) + " || Suggesting threshold is, " + String(get_ma()));
      delay(15);
  }
  previous_state = current_state;
  if(auto_threshold){
    threshold = get_ma();
  }
  delayMicroseconds(SAMPLING_PERIOD);
}

bool get_pd() {
  voltage = analogRead(PD);
  return voltage > threshold ? true : false;
}

void get_binary(){
  if((current_period < FREQ10 + boundary)&&(current_period > FREQ10 - boundary)){
      if(debug){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 10"+"\n");
      }
      ret_update(1);
      ret_update(0);
    }
    else if((current_period < FREQ11 + boundary)&&(current_period > FREQ11 - boundary)){
      if(debug){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 11"+"\n");
      }
      ret_update(1);
      ret_update(1);
    }
    else if((current_period < FREQ01 + boundary)&&(current_period > FREQ01 - boundary)){
      if(debug){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 01"+"\n");
      }
      ret_update(0);
      ret_update(1);
    }
    else if((current_period < FREQ00 + boundary)&&(current_period > FREQ00 - boundary)){
      if(debug){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 00"+"\n");
      }
      ret_update(0);
      ret_update(0);
    }else if((current_period < FREQ_ABORT + boundary)&&(current_period > FREQ_ABORT - boundary)){
      if(debug){
        Serial.print(String(voltage) + " / " +String(current_period)+" = ABORT"+"\n");
      }
      _exception_comm_ended();
    }else{
      if(debug){
        Serial.print(String(voltage) + " / " +String(current_period)+" = TIMEOUT"+"\n");
      }
    }
}

int get_period(){
  return (current_time-last_time);
}

void ret_update(bool temp){
  //Serial.print(String(voltage) + " / " +String(current_period)+" "+ string_buffer +"\n");
  if(state == 0){
    buffer = buffer << 1;
    buffer = buffer | temp;
    if (uint8_t(buffer) == 134) {
      state = 1;
      ret = 0;
      buffer = 0;
      Serial.println("START OF TEXT TRANSMISSION");
      time_comm_start = micros();
    }else if ((uint8_t(buffer) == 135) && (!ber_test)) {
      state = 3;
      ret = 0;
      buffer = 0;
      Serial.println("START OF IMAGE TRANSMISSION");
      time_comm_start = micros();
    }else if (uint8_t(buffer) == 143 && (!ber_test)) {
      state = 2;
      ret = 0;
      buffer = 0;
      Serial.println("START OF CONTROL TRANSMISSION");
      time_comm_start = micros();
    }
  } else if(state == 1){
    //TEXT TRANSMISSION
    ret = ret | temp << 7-bitIndex;
    bitIndex += 1;
    if(bitIndex == 8){
      if((ret < 31) | (ret > 127)){
        if(ret == 0xFF){
                ret = 0;
        }else{
          _exception_comm_failed();
          if(ber_test){ 
            Serial.println("BER was " + String(ber/strlen(ber_string)));
            ber_count = 0; 
            ber = 0;
          }
        }
      } else {
        if(ber_test){
          ber += calculate_byte_ber();
          ber_count += 1;
        }
        string_buffer += ret;
        bitIndex = 0;
        ret = 0;
      }
    }
  }else if (state == 2){
    ret = ret | temp << 7-bitIndex;
    bitIndex += 1;
    if(bitIndex == 8){
      if(speed_comm){
        //changeSpeed(int(ret));
        speed_comm = 0;
        ret = 0;
        bitIndex = 0;
      }else{
        if(ret == 8){
          forward();
          Serial.println("Remote_control : forward");
          ret = 0;
          bitIndex = 0;
        }else if(ret == 9){
          backward();
          Serial.println("Remote_control : backward");
          ret = 0;
          bitIndex = 0;
        }else if(ret == 10){
          //rotate(0);
          Serial.println("Remote_control : rotate");
          ret = 0;
          bitIndex = 0;
        }else if(ret == 11){
          //rotate(1);
          Serial.println("Remote_control : rotate");
          ret = 0;
          bitIndex = 0;
        }else if(ret == 12){
          Serial.println("Remote_control : speed");
          state = 2;
          ret = 0;
          bitIndex = 0;
          // speed_comm = 1;
        } else {
          if(ret == 0xFF){
            ret = 0;
            bitIndex = 0;
          }else{
            _exception_comm_failed();
          }
        }
      }
    }
  }else if (state == 3){
    ret = ret | temp << 7-bitIndex;
    bitIndex += 1;
    if(bitIndex == 8){
      if(image_state == 0){
        size_divider = uint8_t(ret);
        image_state = 1;
        bitIndex = 0;
        ret = 0;
      }else if(image_state == 1){
        image_index = uint8_t(ret);
        image_state = 2;
        bitIndex = 0;
        ret = 0;
      }else{
        if(image_index == 1 && location == 1 && !bool(bmpImage)){
          bmpImage.open(name, O_CREAT | O_EXCL | O_WRITE);
          Serial.println("Start Writing");
        }
        if(bmpImage){
          image_buffer[image_byte_count] = ret;
          image_byte_count++;
        }
        bitIndex = 0;
        ret = 0;
      }
    }
  }
}

int get_ma(){
  suggestion_temp = suggestion_temp * exponential;
  suggestion_temp += voltage * (1-exponential);
  return suggestion_temp;
}

float calculate_byte_ber(){
  uint8_t temp = 0;
  for(int i = 0; i < 8; i++){
    if(bitRead(ret, i) == bitRead(ber_string[ber_count], i)){
      temp += 1;
    }
  }
  return (float)temp / 8.0;
}

void _exception_comm_failed(){
  ret = 0;
  state = 0;
  bitIndex = 0;
  string_buffer = "";
  Serial.println("Comm ended or Sync failure, ret was " + String(int(ret)));
}

void _exception_comm_ended(){
  time_comm_ended = micros();
  if(state == 1){
    Serial.print(String(current_period) + " || " + String(string_buffer) + "\n");
    if(ber_test){ 
      comm_speed = string_buffer.length() * 8.0 / (time_comm_ended - time_comm_start) * 1000 * 1000;
      Serial.print("Speed : " + String(comm_speed));
      ber = (1.0 - ber/strlen(ber_string));
      Serial.println(" BER : " + String(ber));
      ber_count = 0; 
      ber = 0;
    }
  }else if(state == 3){
    Serial.println(String(uint8_t(size_divider)) + " " + String(uint8_t(location)) + " " + String(uint8_t(image_index)) + " " + String(image_byte_count));
  }
  ret = 0;
  state = 0;
  bitIndex = 0;
  string_buffer = "";
  if(image_state == 2){
    if((size_divider < location + 1) && (size_divider == image_index)){
      bmpImage.write(image_buffer, image_byte_count);
      bmpImage.close();
      size_divider = 0;
      location = 0;
      Serial.println("Check SD Card");
      while(1);
    }else if (location != image_index){
      Serial.println("removed");
      bmpImage.close();
      sd.remove(name);
      location = 1;
    }else{
      if(!bmpImage){
        bmpImage.open(name, O_CREAT | O_EXCL | O_WRITE);
        Serial.println("Opened");
      }
      bmpImage.write(image_buffer, image_byte_count);
      bmpImage.sync();
      location += 1;
    }
    image_state = 0;
  }
  image_byte_count = 0;
  Serial.println("END OF TRANSMISSION");
}