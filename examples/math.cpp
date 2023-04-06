void setup() 
{
  Serial.begin(9600);
}

static uint32_t i = 0;
void loop() 
{
  Serial.printf("##exp:y:2^n:%.12f\n", pow(2, i)/1000000000000.0);
  Serial.printf("##nlog:y:nlog(n):%.12f\n", i*log(i));
  Serial.printf("##log:y:log(n):%.12f\n", log(i)/100000.0);
  Serial.printf("##log:y:-log(n):%.12f\n", -log(i)/100000.0);
  ++i;
  delay(100);
}
