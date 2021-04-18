#define fotoSensor A0
#include <dht.h>

dht DHT;

#define DHT11_PIN 7

void setup()
{
  pinMode(fotoSensor,INPUT);
  Serial.begin(9600);
}

void loop()
{
  float lum = analogRead(fotoSensor);
  Serial.println("teste");
  Serial.println(lum);

  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(1000);
}
