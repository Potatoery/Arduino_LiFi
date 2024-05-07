/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : TX Code for arduino VLC Project
##############################################
*/

//LED OUTPUT DECLARE
#define LED_PIN A2

//CLK LENGTH(ms)
#define CLK 2

//inputString Length
//!!!!!!!!!!!!!!!CAUTION!!!!!!!!!!!!!!!!!!
//IT CONSUMES HUGE MEMORY SPACE
#define LEN 100
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//SYNCBYTE before transmission start
#define SYNCBYTE 10

//clk boolean
bool clk = 0;

//inputString Setup
const char inputString[LEN] = "hello, its reliable test for very very very very very very very long menchester code";

//128BYTE BOOLEAN-ARRAY to store binary text data
bool string_Signal[1024] = {0, }; 

//sending_value (will updated by xor with clk & data)
bool sending_value = 0;

//for sending loop
int stringIndex = 0;

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
  //FOR BENCHMARKING===========================
  // Serial has been disabled since serial comm causes unknown delay
  // Serial.begin(115200);
  //FOR BENCHMARKING===========================
  pinMode(LED_PIN, OUTPUT);
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
    //FOR BENCHMARKING===========================
    // Serial.print("start TRANSMISSION \n");
    //FOR BENCHMARKING===========================
}

void loop() {
  clk = !clk;
  sending_value = clk ^ string_Signal[stringIndex];
  digitalWrite(LED_PIN, sending_value);
  delayMicroseconds(CLK/2 *1000);
  clk = !clk;
  sending_value = clk ^ string_Signal[stringIndex];
  digitalWrite(LED_PIN, sending_value);
  delayMicroseconds(CLK/2 * 1000);
  stringIndex += 1;
  if(stringIndex == (strlen(inputString) + SYNCBYTE + 1) * 8) {
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
  // digitalWrite(LED_PIN, 0);
}
