void setup() 
{
  Serial.begin(9600);
}

static uint32_t i = 0;
void loop() 
{
  Serial.println("test:y in N:a(x):7");
  Serial.println("##default:y in m/s:l(s):2");
  Serial.println("y in m/s:u(k):3");
  Serial.println("z(c):9");
  Serial.println(8);
  ++i;
  delay(1000);
}