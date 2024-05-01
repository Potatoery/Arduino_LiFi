// Define various ADC prescaler const unsigned char PS_16 = (1 << ADPS2); 
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0); 
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); 
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(500000); 
  pinMode(2, INPUT);
  ADCSRA &= ~PS_128;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A2) + "\n");
  delayMicroseconds(100);
}
