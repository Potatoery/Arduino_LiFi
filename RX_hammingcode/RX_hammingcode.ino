/*
##############################################
Author : Jeongcheol, Kim
Company : The Catholic University of Korea
Copyright : MIT License
Description : RX Code for arduino VLC Project
##############################################
*/

//HAMMING CODE APPLIED
//AUTO-CORRECTION 1BIT ERROR

//PARITY FORMAT (HAMMINGCODE) {0, 0, 0, 0, X, 0, 0, 0, X, 0, X, X};
//0 is data, X is parity bit

//Photodiode Input declare
#define LDR_PIN A2

//NOT USING
// #define LEN 50

//CLK length(ms)
#define CLK 5

//Threshold(will be replaced with LPF of input)
#define THRESHOLD 60

//How many bit to be synced
#define SYNC_LENGTH 4

//states to use menchester & fatal error check
bool previous_state = 0;
bool current_state = 0;
bool past_state = 0; //for fatal error check

//parity bit arrays
bool received[12]= {0, };
bool parity[4]= {0, };
bool data[8]= {0, };

//is it receiving
bool state = 0;

//is it synced
bool synced = 0;

//received letter
char ret = 0;

//for receiving loop
int bitIndex = 0;

//for menchester code self clocking
int clk_half = CLK * 0.5;
int clk_quarter = CLK * 0.25;

//how many bit has been synced
int syncCycle = 0;

//received string
String string_buffer = "";

void setup() {
  //MAXIMUM SERIAL SPEED IS RECOMMENDED
  //SLOW SERIAL COMM CAUSES ERROR
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
        delay(clk_quarter);
        previous_state = get_ldr();  
      }
      if(syncCycle > SYNC_LENGTH){
        synced = 1;
      }
    //IS IT REALLY SYNCED======================================================
    }else{
    //WHEN IT SYNCED===========================================================
      if(current_state == 0){
        state = 1;
      }
      Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "RECEIVING BITS || " + string_buffer + "\n");
      delay(clk_half);
      previous_state = get_ldr(); 
      if(state == 1){
        received[11-bitIndex] = current_state;
        bitIndex += 1;
          if(bitIndex == 12){
            parity_check();
              if((ret < 31) | (ret > 127)){
                state = 0;
                ret = 0;
                synced = 0;
                syncCycle = 0;
                Serial.print("Comm ended or Sync failure");
              } else if(ret==4){
                state = 0;
                ret = 0;
                synced = 0;
                syncCycle = 0;
                Serial.print("END OF TRANSMISSION");
              } else {
                string_buffer += ret;
                bitIndex = 0;
                ret = 0;
              }
          }
      }
    //WHEN IT SYNCED===========================================================
    }
  }else{
  // WHEN DECIDING THRESHOLD, TURN UNCOMMENT THIS
  // Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "DELAYING TIMES || " + string_buffer + "\n");
  previous_state = current_state;
  //Arduino ref says under 3~6 microsec delay is not guarenteed
  delayMicroseconds(6);
  }

  
}

bool get_ldr() {
  int voltage = analogRead(LDR_PIN);
  return voltage > THRESHOLD ? true : false;
}

char parity_check(){
  parity[0] = (received[11-2]^received[11-4]^received[11-6]^received[11-8]^received[11-10]);
  parity[1] = (received[11-2]^received[11-5]^received[11-6]^received[11-9]^received[11-10]);
  parity[2] = (received[11-4]^received[11-5]^received[11-6]^received[11-11]);
  parity[3] = (received[11-8]^received[11-9]^received[11-10]^received[11-11]);
  
  if(received[11] == parity[0] && received[10] == parity[1] && received[8] == parity[2] && received[4] == parity[3]){
    return get_char();
  } else {
    data[received[11]*8 + received[10]*4 + received[8]*2 + received[4]*1] = !(data[received[11]*8 + received[10]*4 + received[8]*2 + received[4]*1]);
    return get_char();
  }
}

char get_char(){
  data[7] = received[9]; 
  data[6] = received[7]; 
  data[5] = received[6]; 
  data[4] = received[5]; 
  data[3] = received[3]; 
  data[2] = received[2]; 
  data[1] = received[1];
  data[0] = received[0];
  for(int i = 0; i < 8; i++){
    ret = ret | data[i] << 7-i;
  }
  return ret;
}