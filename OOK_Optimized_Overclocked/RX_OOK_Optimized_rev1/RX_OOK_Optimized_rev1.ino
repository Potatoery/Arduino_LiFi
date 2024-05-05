/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : RX Code for arduino VLC Project
##############################################
*/

//need DEBUG?
#define debug 1

// Define various ADC prescaler 
// For Fast sampling rate
// kind of ADC overclock
const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//Photodiode Input declare
#define LDR_PIN A2

//NOT USING
// #define LEN 50

//CLK length(ms)
#define CLK 2

//Threshold(will be replaced with LPF of input)
#define THRESHOLD 60

//How many bit to be synced
#define SYNC_LENGTH 4

//for find threshold //FOR DEBUG
float suggestion_temp = 0;

//states to use menchester & fatal error check
bool previous_state = 0;
bool current_state = 0;
bool past_state = 0; //for fatal error check

//ADC level
int voltage = 0;

//is it receiving
bool state = 0;

//is it synced
bool synced = 0;

//received letter
char ret = 0;
char buffer = 0;

//for receiving loop
int bitIndex = 0;

//for menchester code self clocking
int clk_half = CLK * 0.5;
float clk_quarter = CLK * 0.25;

//how many bit has been synced
int syncCycle = 0;

//received string
String string_buffer = "";

void setup() {
  //MAXIMUM SERIAL SPEED IS RECOMMENDED
  //SLOW SERIAL COMM CAUSES ERROR
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
  Serial.begin(921600);
  Serial.println("started");
}

void loop() {
  current_state = get_ldr();
  //For FATAL check detection error=============================
  if (((current_state && previous_state) && past_state) == 1){
    Serial.print("!!!!!!!!!FATAL COMM ERROR DETECTED!!!!!!!!!");
    delay(3000000);
  }
  //For FATAL check detection error=============================

  if(current_state != previous_state){
    //IS IT REALLY SYNCED======================================================
    if(synced == 0){
      Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "SYNCING SYSTEM || " + string_buffer + "\n");
      if(current_state == 1){
        syncCycle += 1;
        delay(clk_half);
        previous_state = get_ldr();
      }else{
        syncCycle = 0;
        //delay for compensate timing incorrection
        delayMicroseconds(int(clk_quarter*1000));
        previous_state = get_ldr();  
      }
      if(syncCycle > SYNC_LENGTH){
        synced = 1;
      }
    //IS IT REALLY SYNCED======================================================
    }else{
    //WHEN IT SYNCED===========================================================
      delayMicroseconds((clk_half + 0.2)*1000);
      previous_state = get_ldr();
      if(state = 0){
        buffer = buffer << 1;
        buffer = buffer | current_state;
        if (buffer == 6) {
          state = 1;
          ret = 0;
        }
      }else{
        ret = ret | current_state << 7-bitIndex;
        bitIndex += 1;
          if(bitIndex == 8){
            if((ret < 31) | (ret > 127)){
              state = 0;
              ret = 0;
              buffer = 0;
              synced = 0;
              syncCycle = 0;
              Serial.print("Comm ended or Sync failure");
            } else if(ret==4){
              state = 0;
              ret = 0;
              buffer = 0;
              synced = 0;
              syncCycle = 0;
              string_buffer = "";
              Serial.print("END OF TRANSMISSION");
            } else {
              string_buffer += ret;
              bitIndex = 0;
              ret = 0;
              Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "RECEIVING BITS || " + string_buffer + "\n");
            }
          }
      }
    //WHEN IT SYNCED===========================================================
    }
  }else{
    if(debug){
      Serial.println(String(voltage) + " || Suggesting threshold is, " + String(get_ma()));
      delay(15);
    }
  previous_state = current_state;
  //Arduino ref says under 3~6 microsec delay is not guarenteed
  delayMicroseconds(6);
  }

  
}

bool get_ldr() {
  voltage = analogRead(LDR_PIN);
  return voltage > THRESHOLD ? true : false;
}

int get_ma(){
  suggestion_temp = suggestion_temp * 0.98;
  suggestion_temp += voltage * 0.02;
  return suggestion_temp;
}