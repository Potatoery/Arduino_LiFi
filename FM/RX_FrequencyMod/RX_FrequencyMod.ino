/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : RX Code for arduino VLC Project
Note : Frequency Modulation code
##############################################
*/

//need DEBUG?
#define debug 0

//Photodiode
#define PD A2

//PD THRESHOLD
#define THRESHOLD 55

//Sampling Period
#define SAMPLING_PERIOD 10 //us

//Frequency Settings
#define FREQ00 750
#define FREQ01 1250
#define FREQ11 1750
#define FREQ10 2250

//for find threshold //FOR DEBUG
float suggestion_temp = 0;

//Frequency boundaries
int boundary = 250;

// Define various ADC prescaler 
const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//pulse width calc
unsigned long current_time;
unsigned long last_time;
int current_period = 0;

//Binarys
bool current_state = 0;
bool previous_state = 0;
bool state = 0;

//PD VALUE
int voltage;

//for IDEA Develop
int8_t bitIndex = 0;
int temp;

//Received values
char ret = 0;
char buffer = 0;
String string_buffer = "";

void setup() {
  Serial.begin(921600); 
  pinMode(PD, INPUT);
  //ADC 5kHz to ~2MHz
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
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
    //update received values
    get_binary();
    //now update last_time to current_time
    last_time = current_time;
    if(debug){
      Serial.print(String(voltage) + " / " +String(current_period)+" !! "+"\n");
    }
  }
  // if(debug){
  //     Serial.println(String(voltage) + " || Suggesting threshold is, " + String(get_ma()));
  //     delay(15);
  // }
  //For sampling
  previous_state = current_state;
  delayMicroseconds(SAMPLING_PERIOD);
}

bool get_pd() {
  voltage = analogRead(PD);
  return voltage > THRESHOLD ? true : false;
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
  if(state = 0){
    buffer = buffer << 1;
    buffer = buffer | temp;
    if (buffer == 6) {
      state = 1;
      ret = 0;
    }
  }else{
    ret = ret | temp << 7-bitIndex;
    bitIndex += 1;
    if(bitIndex == 8){
      if(ret == 4){
        ret = 0;
        state = 0;
        buffer = 0;
        Serial.println("END OF TRANSMISSION");
      } else if((ret < 31) | (ret > 127)){
        ret = 0;
        state = 0;
        buffer = 0;
        Serial.println("Comm ended or Wrong bytes");
      } else {
        Serial.print(String(voltage) + " / " +String(current_period)+" = "+ string_buffer +"\n");
        string_buffer += ret;
        bitIndex = 0;
        ret = 0;
      }
    }
  }
}

int get_ma(){
  suggestion_temp = suggestion_temp * 0.98;
  suggestion_temp += voltage * 0.02;
  return suggestion_temp;
}