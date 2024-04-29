#define LED_PIN A2
#define CLK 5
#define LEN 500
#define SYNCBYTE 5

char inputString[LEN] = "hello, its reliable test for very very very very very very very long menchester code";
bool string_Signal[1000] = {0, };
bool sending_value = 0;
bool _symbol_EOT[8] = {0, 0, 0, 0, 0, 1, 0, 0};
int stringIndex = 0;
char buffer[100]; //for debug
bool clk = 0;

//FOR BENCHMARKING
unsigned long _time_started;
unsigned long _time_ended;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
    for (int i = 0; i < strlen(inputString) + SYNCBYTE + 1; i++) {
      if(i < SYNCBYTE){
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
    // sprintf(buffer, "%b",(string_Signal));
    // Serial.print(buffer + "\n");
    Serial.print("start TRANSMISSION \n");
}

void loop() {
  // put your main code here, to run repeatedly:
  clk = !clk;
  sending_value = clk ^ string_Signal[stringIndex];
  digitalWrite(LED_PIN, sending_value);
  sprintf(buffer, "%d",(sending_value));
  printf(buffer);
  delay(CLK/2);
  clk = !clk;
  sending_value = clk ^ string_Signal[stringIndex];
  digitalWrite(LED_PIN, sending_value);
  sprintf(buffer, "%d",(sending_value));
  printf(buffer);
  delay(CLK/2);
  stringIndex += 1;
  // Serial.print("transmitting \n");
  if(stringIndex == (strlen(inputString) + SYNCBYTE + 1) * 8) {
      stringIndex = 0;
      _time_ended = millis();
      Serial.print("\n");
      Serial.print((strlen(inputString) + SYNCBYTE + 1) * 8);
      Serial.print("\n");
      Serial.print(String((_time_ended) - (_time_started)));
      delay(10);
      _time_started = millis();
  }
  digitalWrite(LED_PIN, 0);
}
