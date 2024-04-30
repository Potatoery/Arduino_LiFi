//HAMMING CODE APPLIED
//AUTO-CORRECTION 1BIT ERROR

//PARITY FORMAT (HAMMINGCODE) {0, 0, 0, 0, X, 0, 0, 0, X, 0, X, X};
//0 is data, X is parity bit

//LED OUTPUT DECLARE
#define LED_PIN A2

//CLK LENGTH(ms)
#define CLK 5

//inputString Length
//!!!!!!!!!!!!!!!CAUTION!!!!!!!!!!!!!!!!!!
//IT CONSUMES HUGE MEMORY SPACE
#define LEN 100
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//SYNCBYTE before transmission start
#define SYNCBYTE 5

//clk boolean
bool clk = 0;

//inputString Setup
char inputString[LEN] = "hello, its reliable test for very very very very very very very long menchester code";

//128BYTE BOOLEAN-ARRAY to store binary text data
bool string_Signal[1024] = {0, }; 

//sending_value (will updated by xor with clk & data)
bool sending_value = 0;

//for sending loop
int stringIndex = 0;

//for perity loop
int8_t k = 0;

//symbol of start of transmission
//NOT USING FOR NOW
//====================={0, 0, 0, 0, X, 0, 0, 0, X, 0, X, X};
bool _symbol_SOT[12] = {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1};

//symbol of end of transmission
//====================={0, 0, 0, 0, X, 0, 0, 0, X, 0, X, X};
bool _symbol_EOT[12] = {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0};

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
      if(i < SYNCBYTE){
        for (int j = 0; j < 12; j++) {
            string_Signal[12*i + 11-j] = (1);
        }
      } else if(i == (strlen(inputString) + SYNCBYTE)) {
        for (int j = 0; j < 12; j++) {
            string_Signal[12*i + 11-j] = _symbol_EOT[11 - j];
        }
      } else {
        for (int j = 0; j < 12; j++){
          if(j == 0 || j == 1 || j == 3 || j == 7){
            k += 1;
            continue;
          }else{
            string_Signal[12*i + 11-j] = ((inputString[i - SYNCBYTE] & (0x001 << j - k)) != 0);
          }
        }
        string_Signal[12*i + 11] = (string_Signal[12*i + 11-2]^string_Signal[12*i + 11-4]^string_Signal[12*i + 11-6]^string_Signal[12*i + 11-8]^string_Signal[12*i + 11-10]);
        string_Signal[12*i + 11-1] = (string_Signal[12*i + 11-2]^string_Signal[12*i + 11-5]^string_Signal[12*i + 11-6]^string_Signal[12*i + 11-9]^string_Signal[12*i + 11-10]);
        string_Signal[12*i + 11-3] = (string_Signal[12*i + 11-4]^string_Signal[12*i + 11-5]^string_Signal[12*i + 11-6]^string_Signal[12*i + 11-11]);
        string_Signal[12*i + 11-7] = (string_Signal[12*i + 11-8]^string_Signal[12*i + 11-9]^string_Signal[12*i + 11-10]^string_Signal[12*i + 11-11]);
        k = 0;
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
  delay(CLK/2);
  clk = !clk;
  sending_value = clk ^ string_Signal[stringIndex];
  digitalWrite(LED_PIN, sending_value);
  delay(CLK/2);
  stringIndex += 1;
  if(stringIndex == (strlen(inputString) + SYNCBYTE + 1) * 12) {
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
