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
#define STABLE 0

//BER Test Mode
#define ber_test 0

//Is it image?
#define send_image 0

//LED OUTPUT DECLARE
#define LED A2

//Frequency Settings
#if STABLE == 0
  #define FREQ00 440
  #define FREQ01 800
  #define FREQ11 1200
  #define FREQ10 1600
#else
  #define FREQ00 1000
  #define FREQ01 1400
  #define FREQ11 1800
  #define FREQ10 2200
#endif
#define FREQ_ABORT 2400
#define FREQ_START 2800


//SYNCBYTE before transmission start
//Minimum 1
//Suggesting 2 ( SOT should be included, Buffer should be flushed at RX )
#define SYNCBYTE 2 //bytes

//Duration of certain Frequency
#define DURATION 1 //CYCLE

//inputString Length
//!!!!!!!!!!!!!!!CAUTION!!!!!!!!!!!!!!!!!!
//IT CONSUMES HUGE MEMORY SPACE
#define LEN 550
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//inputString Setup
const char inputString[LEN] = "A veryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy Message";
const char ber_string[LEN] = "The Catholic University of Korea";
const char image[] PROGMEM = "Qk02GwAAAAAAADYAAAAoAAAAMAAAADAAAAABABgAAAAAAAAbAAAAAAAAAAAAAAAAAAAAAAAApEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt6KIA6KIA6KIA6KIAAPL/6KIA6KIA6KIA6KIA6KIA6KIAAPL/6KIA6KIAAPL/6KIA6KIAAPL/6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////////pEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBzt////JBztJBztJBztJBztJBztJBzt6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIAAPL/6KIAAPL/6KIA6KIATLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjJBztJBztpEmjJBztJBztJBztJBztJBztJBzt////JBztJBztJBztJBztJBztJBzt6KIAAPL/6KIA6KIA6KIA6KIA6KIA6KIAAPL/TLEi6KIATLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBzt////////JBztJBztJBztJBztJBztJBztAPL/APL/6KIAAPL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztpEmjJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztpEmjJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztpEmjJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/////TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztpEmjJBztJBztJBztJBztJBzt////JBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/////TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/////TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/////TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA////////pEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBzt////JBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/////TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBzt////JBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmj////JBztJBztJBztJBztpEmjJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA////6KIAk0CgpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztpEmjJBztAPL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEipEmj6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjJBztpEmjpEmjfXn1JBztJBztJBztJBztJBztJBztJBztJBztJBzt////JBzt////////APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjOTHvJBztpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt////h/j/APL/APL/APL/TLEiAPL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiAPL/TLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIAFQCIpEmj////pEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztpEmjJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiAPL/TLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIAFQCIpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt////APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt////GPP/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEizEg/TLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIAFQCIpEmjpEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjJBztpEmjJBztJBztJBztJBztJBztJBztJBzt////JBztt/v/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjOTHvJBztJBztJBztJBztJBztJBztJBztJBztJBztJ3//JBztJBzt////APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIAFQCIpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt////APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIAKxiTpEmjFQCIZlixpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJ3//JBzt////APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIAoJfOpEmjFQCIpEmjpEmj////pEmj////pEmj////pEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt////t/v/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEizEg/6KIA6KIA6KIA6KIA6KIA6KIA6KIA////FQCIpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBzt////////APL/APL/APL/Xbg3APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjKxiTpEmjpEmjpEmj////pEmj////pEmjpEmjpEmjpEmjJBztpEmjJBztJBztJBztJBztJBztJBztJBztJBzt////////APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi////TLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjFQCIpEmjpEmjpEmjpEmj////pEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBzt////APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiAPL/TLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA////6KIApEmjpEmjpEmjFQCIpEmjpEmj////pEmjpEmjpEmj////pEmjJBztJBztpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztl/n/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmj////////////pEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBzt////////APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBzt////APL/APL/APL/APL/APL/TLEiAPL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmj////pEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBzt////JBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIApEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBzt////JBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBzt////JBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA////pEmjpEmjpEmjpEmj////pEmjpEmj////pEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA6KIA6KIA////////////pEmjpEmjpEmjpEmjpEmj////pEmjpEmjpEmjpEmjpEmjpEmjpEmjJBztJBztJBztJBztJBzt////////JBztJBztJBztJBztJBztJBztAPL/APL/APL/APL/APL/APL/APL/APL/TLEiTLEiTLEiTLEiTLEi6KIA6KIA6KIA6KIA6KIA////////";

//128BYTE BOOLEAN-ARRAY to store binary text data
// bool string_Signal[1024] = {0, }; 
uint8_t string_Signal[550] = {0, };
uint16_t signal_length = 0;
char ber_test_signal = 0x1E;
uint16_t block_size = 0;

//for sending loop
int16_t stringIndex = 0;
uint8_t bitIndex = 0;
bool toggle = 0;
bool image_state = 0;
uint16_t image_index = 0;
byte Grayscale;
// File bmpImage;

//for image
uint16_t size;
uint16_t size_divider = 0;
uint16_t size_divider_n = 0;

// Define various ADC prescaler 
const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//symbol of start of transmission
//NOT USING FOR NOW
bool _symbol_SOT[8] = {1, 0, 0, 0, 0, 1, 1, 0}; // 6

bool _symbol_SOT_Control[8] = {1, 0, 0, 0, 1, 1, 1, 1}; // 5

// bool _symbol_Control_Forward[8]; // 8

// bool _symbol_Control_Backward[8]; // 9

// bool _symbol_Control_Rotate_Left[8]; // 10

// bool _symbol_Control_Rotate_Right[8]; // 11

bool _symbol_Control_ChangeSpeed[8]; // 12

bool _symbol_SOT_Image[8] = {1, 0, 0, 0, 0, 1, 1, 1}; //7

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
  // if (!SD.begin(10)) {
  //   Serial.println("initialization failed!");
  //   while (1);
  // }
  // File bmpImage = SD.open("picture.bmp",FILE_READ);
  // size = bmpImage.size();
  //For Debuging=========================================
  if(debug || ber_test){
    Serial.begin(2000000);
    Serial.print("start TRANSMISSION \n");
  }
  if(debug && ber_test){
    Serial.println("Debug & BER can't enabled at the same time");
    while(1);
  }
  if(ber_test){
    buffer_ber_test();
  }else if(send_image){
    buffer_image();
  }else{
    buffer_text();
  }
  //For Debuging=========================================
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
  digitalWriteFast(LED, 1);
  delay(SYNCBYTE);
}

void loop() {
  // if(stringIndex == -1){
  //   _exception_comm_started();
  //   stringIndex += 1;
  // }
  if(bitRead(string_Signal[stringIndex], bitIndex)){
    if(bitRead(string_Signal[stringIndex], bitIndex + 1)){
      write_11();
    }else{
      write_10();
    }
  }else{
    if(bitRead(string_Signal[stringIndex], bitIndex + 1)){
      write_01();
    }else{
      write_00();
    }
  }
  bitIndex += 2;
  if(bitIndex == 8){
    bitIndex = 0;
    stringIndex += 1;
  }
  if(stringIndex >= signal_length - 1) {
      _exception_comm_ended();
      _exception_comm_ended();
      if(send_image){
        buffer_image();
      }
      if(debug){
        Serial.println("512 Byte sent " + String(image_index) + " " + String(size_divider));
      }
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

void buffer_ber_test(){
  for (int i = 0; i < strlen(ber_string) + SYNCBYTE + 1; i++) {
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
    } else if(i == (strlen(ber_string) + SYNCBYTE)) {
      for (int j = 0; j < 8; j++) {
        if(_symbol_EOT[7 - j]){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    } else {
      for (int j = 0; j < 8; j++){
        if((ber_string[i - SYNCBYTE] & (0x01 << j)) != 0){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    }
  }
  signal_length = (strlen(ber_string) + SYNCBYTE + 1);
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
  signal_length = (SYNCBYTE + 2);  
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
  signal_length = (SYNCBYTE + 3);  
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
}

void _exception_comm_ended(){
  for(int i = 0; i < DURATION; i++){
    delayMicroseconds(FREQ_ABORT);
    if(toggle){
      digitalWriteFast(LED, 1);
    }else{
      digitalWriteFast(LED, 0);
    }
    toggle = !toggle;
  }
}

void _exception_comm_started(){
  for(int i = 0; i < DURATION; i++){
    delayMicroseconds(FREQ_START);
    if(toggle){
      digitalWriteFast(LED, 1);
    }else{
      digitalWriteFast(LED, 0);
    }
    toggle = !toggle;
  }
}

void buffer_image(){
  if(!image_state){
    size = strlen(image);
    image_index = 0;
    image_state = 1;
  }

  // bmpImage.seek( );
  if(size > 512){
    size_divider = size / 512 + 1;
    size_divider_n = size % 512;
  }
  
  if(size_divider == image_index + 1){
    block_size = size_divider_n;
    image_state = 0;
  }else{
    block_size = 512;
  }

  for (uint16_t i = 0; i < block_size + SYNCBYTE + 3; i++) {
    if(i == SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
        if(_symbol_SOT_Image[7 - j]){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    } else if(i == SYNCBYTE){
      for (int j = 0; j < 8; j++) {
        if(bitRead(uint8_t(size_divider), 7-j)){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    }else if(i == SYNCBYTE + 1){
      for (int j = 0; j < 8; j++) {
        if(bitRead(uint8_t(image_index), 7-j)){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    } else if(i < SYNCBYTE - 1){
      for (int j = 0; j < 8; j++) {
          bitClear(string_Signal[i], 7-j);
      }
    } else if(i == (512 + SYNCBYTE + 2)){
      for (int j = 0; j < 8; j++) {
        if(_symbol_EOT[7 - j]){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    } else {
      for (int j = 0; j < 8; j++) {
        if(bitRead(pgm_read_byte_near(image + (image_index * 512) + (i - SYNCBYTE - 2)), 7-j)){
          bitSet(string_Signal[i], 7-j);
        }else{
          bitClear(string_Signal[i], 7-j);
        }
      }
    }
  }
  image_index += 1;
  Serial.println(String(block_size + SYNCBYTE + 3) + " " + String(string_Signal[1]) + " " + String(string_Signal[2]) + " " + String(string_Signal[3]) + " " + String(string_Signal[4]) + " " + String(string_Signal[5]));
  signal_length = (block_size + SYNCBYTE + 3);
}
