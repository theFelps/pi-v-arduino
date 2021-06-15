// Libs
#include <LiquidCrystal.h>
#include <dht.h>

// init DHT Object
dht DHT;

#define fotoSensor A0
#define DHT11_PIN 7
#define hyg_pin A2

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int buttonState = 0;
int num=0;
int res;
void setup() {
  Serial.begin(9600);
  
  // LCD
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows:

  //DHT
  pinMode(DHT11_PIN, INPUT);
  
  //LDR
  pinMode(fotoSensor,INPUT);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Startin' up...");
//  lcd.setCursor(0,1);
//  lcd.print("PI-V");
  delay(5000);  
}

void loop() {
  //room light
  float lum = analogRead(fotoSensor);
  Serial.print("Light = ");
  Serial.println(lum);

  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

  int hyg = analogRead(hyg_pin);
  Serial.print("Soil H = ");
  Serial.println(hyg);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(DHT.temperature);
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.print(DHT.humidity);
  lcd.setCursor(9,0);
  lcd.print("L:");
  lcd.print(lum);
  lcd.setCursor(9,1);
  lcd.print("SH:");
  lcd.print(hyg);
  delay(2000);
}
