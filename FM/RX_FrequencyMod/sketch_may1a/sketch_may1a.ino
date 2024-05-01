#define LDR A2
#define THRESHOLD 50
#define SAMPLING_PERIOD 6 //us

// Define various ADC prescaler 
const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//Frequency_calc
unsigned long current_time;
unsigned long last_time;
int current_period = 0;

//Binarys
bool current_state = 0;
bool previous_state = 0;
bool first_10 = 1;
bool first_11 = 1;
bool first_01 = 1;
bool first_00 = 1;

//LDR VALUE
int voltage;

//for IDEA Develop
int8_t duration = 0;
int temp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(921600); 
  pinMode(LDR, INPUT);
  ADCSRA &= ~PS_128;
  ADCSRA |= PS_16;
}

void loop() {
  current_state = get_ldr();
  if (current_state != previous_state){
    current_time = micros();
    current_period = get_period();
    if((current_period < 1250)&&(current_period > 1000)){
      if(!first_11 || !first_01 || !first_00){
        duration = 0;
        first_11 = 1;
        first_01 = 1;
        first_00 = 1;
      }
      first_10 = 0;
      duration += 1;
      if(duration > 5){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 10"+"\n");
        duration = 0;
        first_10 = 1;
      }
    }
    else if((current_period < 1000)&&(current_period > 750)){
      if(!first_10 || !first_01 || !first_00){
        duration = 0;
        first_10 = 1;
        first_01 = 1;
        first_00 = 1;
      }
      first_11 = 0;
      duration += 1;
      if(duration > 5){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 11"+"\n");
        duration = 0;
        first_11 = 1;
      }
    }
    else if((current_period < 750)&&(current_period > 500)){
      if(!first_11 || !first_10 || !first_00){
        duration = 0;
        first_11 = 1;
        first_10 = 1;
        first_00 = 1;
      }
      first_01 = 0;
      duration += 1;
      if(duration > 5){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 01"+"\n");
        duration = 0;
        first_01 = 1;
      }
    }
    else if((current_period < 500)&&(current_period > 250)){
      if(!first_11 || !first_10 || !first_01){
        duration = 0;
        first_11 = 1;
        first_10 = 1;
        first_01 = 1;
      }
      first_00 = 0;
      duration += 1;
      if(duration > 5){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 00"+"\n");
        duration = 0;
        first_00 = 1;
      }
    }
    last_time = current_time;
  }
  // Serial.print(String(voltage) + " / " +String(current_period)+" !! "+"\n");
  previous_state = current_state;
  delayMicroseconds(SAMPLING_PERIOD);
}

bool get_ldr() {
  voltage = analogRead(LDR);
  return voltage > THRESHOLD ? true : false;
}

int get_period(){
  return (current_time-last_time);
}