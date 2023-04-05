#include <limits>
#include <cmath>

void setup() 
{
  Serial.begin(9600);
}

void Exp(uint32_t i)
{
  Serial.print("2^n:");
  Serial.println(std::pow<double>(2, i)/1000000000000.0, 12);
}

void nLog(uint32_t i)
{
  Serial.print("nlog(n):");
  Serial.println(i*std::log(i), 10);
}

void N(uint32_t n)
{
  Serial.print("N:");
  Serial.println(n);
}

void Log(uint32_t i)
{
  Serial.print("log(n):");
  Serial.println(std::log(i), 10);
}

void MinusLog(uint32_t i)
{
  Serial.print("-log(n):");
  Serial.println(-std::log(i), 10);
}

void loop() 
{
  for(uint32_t i = 0; i < std::numeric_limits<uint32_t>::max(); ++i)
  {
    //Exp(i);
    //nLog(i);
    Log(i);
    //N(i);
    delay(1000);
  }
  delay(1000);
}
