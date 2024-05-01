/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
Note : Frequency Modulation code
##############################################
*/

//LED OUTPUT DECLARE
#define LED A2

//Stand-by before transmission start
#define STANDBY 5 //ms

//inputString Length
//!!!!!!!!!!!!!!!CAUTION!!!!!!!!!!!!!!!!!!
//IT CONSUMES HUGE MEMORY SPACE
#define LEN 100
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//inputString Setup
char inputString[LEN] = "hello, its reliable test for very very very very very very very long menchester code";

//128BYTE BOOLEAN-ARRAY to store binary text data
bool string_Signal[1024] = {0, }; 

//for sending loop
int stringIndex = 0;

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
  pinMode(LED, OUTPUT);
  for (int i = 0; i < strlen(inputString) + STANDBY + 1; i++) {
    if(i < STANDBY){
      for (int j = 0; j < 8; j++) {
          string_Signal[8*i + 7-j] = (1);
      }
    } else if(i == (strlen(inputString) + STANDBY)) {
      for (int j = 0; j < 8; j++) {
          string_Signal[8*i + 7-j] = _symbol_EOT[7 - j];
      }
    } else {
      for (int j = 0; j < 8; j++){
          string_Signal[8*i + 7-j] = ((inputString[i - STANDBY] & (0x01 << j)) != 0);
      }
    }
  }
  //For Debuging=========================================
  Serial.begin(921600);
  Serial.print("start TRANSMISSION \n");
  //For Debuging=========================================
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
  digitalWrite(LED, 1);
  delay(STANDBY);
}

void loop() {
  digitalWrite(LED, 0);
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
  if(stringIndex >= (strlen(inputString) + STANDBY + 1) * 8) {
      stringIndex = 0;
      //FOR BENCHMARKING===========================
      // _time_ended = millis();
      // Serial.print("\n");
      // Serial.print((strlen(inputString) + STANDBY + 1) * 8);
      // Serial.print("\n");
      // Serial.print(String((_time_ended) - (_time_started)));
      //FOR BENCHMARKING===========================
      delay(10);
      //FOR BENCHMARKING===========================
      // _time_started = millis();
      //FOR BENCHMARKING===========================
  }
}

void write_00(){
  for(int i = 0; i < 4; i++){
    digitalWrite(LED, 1);
    delayMicroseconds(350);
    digitalWrite(LED, 0);
    delayMicroseconds(350);
  }
}

void write_01(){
  for(int i = 0; i < 4; i++){
    digitalWrite(LED, 1);
    delayMicroseconds(600);
    digitalWrite(LED, 0);
    delayMicroseconds(600);
  }
}

void write_10(){
  for(int i = 0; i < 4; i++){
    digitalWrite(LED, 1);
    delayMicroseconds(850);
    digitalWrite(LED, 0);
    delayMicroseconds(850);
  }
}

void write_11(){
  for(int i = 0; i < 4; i++){
    digitalWrite(LED, 1);
    delayMicroseconds(1100);
    digitalWrite(LED, 0);
    delayMicroseconds(1100);
  }
}