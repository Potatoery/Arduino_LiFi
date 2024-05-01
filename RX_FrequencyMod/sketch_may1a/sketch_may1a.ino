#define LDR A2
#define THRESHOLD 45
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
    if((current_period < 225)&&(current_period > 25)){
      duration += 1;
      if(duration > 5){
        Serial.print(String(voltage) + " / " +String(current_period)+" = 0"+"\n");
        duration = 0;
      }
    }
  }
  previous_state = current_state;
  last_time = current_time;
  delayMicroseconds(SAMPLING_PERIOD);
}

bool get_ldr() {
  voltage = analogRead(LDR);
  return voltage > THRESHOLD ? true : false;
}

int get_period(){
  return (current_time-last_time);
}