#define LDR_PIN A2
#define LEN 50
#define CLK 5
#define THRESHOLD 60
#define SYNC_LENGTH 4

bool previous_state = 0;
bool current_state = 0;
bool past_state = 0; //for debugging
bool state = 0;
bool synced = 0;
char ret = 0;

int bitIndex = 0;
int clk_half = CLK * 0.5;
int syncCycle = 0;
String string_buffer = "";

void setup() {
  // pinMode(LED_PIN, OUTPUT);
  Serial.begin(921600);
  Serial.println("started");
}

void loop() {
  current_state = get_ldr();
  //For check detection error
  if (((current_state && previous_state) && past_state) == 1){
    Serial.print("!!!!!!!!!FATAL COMM ERROR DETECTED!!!!!!!!!");
    delay(3000000);
  }

  if(current_state != previous_state){
    //IS IT REALLY SYNCED
    if(synced == 0){
      Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "SYNCING SYSTEM || " + string_buffer + "\n");
      if(current_state == 1){
        syncCycle += 1;
        delay(clk_half);
        previous_state = get_ldr();
      }else{
        syncCycle = 0;
        delay(3);
        previous_state = get_ldr();  
      }
      if(syncCycle > SYNC_LENGTH){
        synced = 1;
      }
    }else{
      if(current_state == 0){
        state = 1;
      }
      Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "RECEIVING BITS || " + string_buffer + "\n");
      delay(clk_half); //0.6 for 160 CLK
      // past_state = previous_state;
      previous_state = get_ldr(); 
      if(state == 1){
        // Serial.print("#");
        ret = ret | current_state << 7-bitIndex;
        bitIndex += 1;
          if(bitIndex == 8){
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
              // state = 0;
              string_buffer += ret;
              bitIndex = 0;
              ret = 0;
            }
          }
      }
    }
  }else{
  // Serial.print(String(analogRead(LDR_PIN)) + " " + String(current_state) + String(previous_state) + " || " + state + " " + "DELAYING TIMES || " + string_buffer + "\n");
  previous_state = current_state;  
  delayMicroseconds(6);
  }

  
}

bool get_ldr() {
  int voltage = analogRead(LDR_PIN);
  // Serial.print(String(voltage) + " || \n"); //for debug
  return voltage > THRESHOLD ? true : false;
}