#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <dht.h>

// init DHT Object
dht DHT;

//starts communication from Arduino to ESP8266 through ports 2 and 3 (TX and RX)
SoftwareSerial esp8266(2, 3);

//LCD Init
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define DEBUG true
#define fotoSensor A0
#define DHT11_PIN 13
#define hyg_pin A2


void setup() {
  Serial.begin(9600);
  // important: ESP8266 by default starts with CIOBAUTRATE as 115200. we had to reconfigure the BAUDRATE to something compatible with Arduino and SoftwareSerial.h, so we tuned it down to 9600.
  esp8266.begin(9600);
  
  // LCD
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows:

  //DHT
  pinMode(DHT11_PIN, INPUT);
  
  //LDR
  pinMode(fotoSensor,INPUT);

  lcd_print("Starting...");

  sendData("AT+RST\r\n", 4000, DEBUG); // reset device
  // Connect to WiFi
  sendData("AT+CWJAP=\"**SSID**\",\"**PASSWORD**\"", 2000, DEBUG); // replace by appropriate stuff
  delay(3000);
  //we send out 2 AT commands to clear out of any possible returns from wifi connectio on step above.
  sendData("AT\r\n", 4000, DEBUG);
  sendData("AT\r\n", 4000, DEBUG);
  sendData("AT+CWMODE=1\r\n", 4000, DEBUG); // sets up as CLIENT mode.
  sendData("AT+CIFSR\r\n", 4000, DEBUG); //lists assigned IP Address
  sendData("AT+CIPMUX=1\r\n", 4000, DEBUG);// configure MUX -- multiple connections, 4 max. to kill all refer to ID 5
  sendData("AT+CIPSERVER=1,80\r\n", 4000, DEBUG);// Start web server on port 80
  sendData("AT+CIPSTO=7200\r\n", 4000, DEBUG);//Specifies Timeout for 7200 secs, which is the maximum time.
  lcd_print("ready");
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
  delay(1000);

  // check if se o ESP8266 is sending data
  if (esp8266.available())
  {
    if (esp8266.find("+IPD,"))
    {
      delay(300);
      int connectionId = esp8266.read() - 48;
      Serial.print("ConnID: ");
      Serial.println(connectionId);

      String fullpage;
      String whitespace="\n";
      String httpresp = "HTTP/1.1 200 OK\n";
//      String httpresp2 = "Content-Type: text/html\n"; // for simple HTML pages

      //this one is for JSON content aka simulating an API.
      String httpresp2 = "Content-Type: text/json\nAccess-Control-Allow-Origin: *\n";

// simple webpage.
//      String webpage = "<!DOCTYPE HTML><html><head><title>test1</title></head><body>Hello From ESP8266!</body></html>\n";
//      String webpage = "{\n'temperatura_do_ambiente': 23.2,\n";
//      webpage += "'humidade_do_solo': 45.2,\n";
//      webpage += "'luminosidade': 200.3,\n";
//      webpage += "'humidade_do_ar': 42.33\n}";


// this is JSON style.
      String webpage = "{\n\"temperatura_do_ambiente\": ";
      webpage += DHT.temperature;
      webpage += ",\n";
      webpage += "\"humidade_do_solo\": ";
      webpage += hyg;
      webpage += ",\n";
      webpage += "\"luminosidade\": ";
      webpage += lum;
      webpage += ",\n";
      webpage += "\"humidade_do_ar\": "; 
      webpage += DHT.humidity;
      webpage += "\n}";

// Assemble payload for the request: Headers, new empty line then the body.
      fullpage += httpresp;
      fullpage += httpresp2;
      fullpage += whitespace;
      fullpage += webpage;

//sends command. first param is the CONNID we are sending info to. Second param is the size in bytes of information we are returning. this requires \r\n
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += fullpage.length();
      cipSend += "\r\n";


// here we send the entire thing: Headers first, then the body. 
// sequence is: 1. Receive GET Req
//              2. respond with AT+CIPSEND=[ConnID],[message size i bytes] \r\n
//              3. send HTTP package -- headers, body and all
//              4. send AT+CIPCLOSE=[ConnID]
//              5. should be all gooooood to go.


// we have packed everything under 'fullpage'-> headers first then an empty \n then body. \n is important here otherwise body content will go on header and body will be empty.
      sendData(cipSend, 2000, DEBUG);
      sendData(fullpage, 2000, DEBUG);


 // finally we close out the connection to the active connection ID
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      Serial.println(closeCommand);
      closeCommand += "\r\n";
 
      sendData(closeCommand, 4000, DEBUG);
    }
  }
}


String sendData(String command, const int timeout, boolean debug)
{
  // Sends AT commands to the module.
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.println("<<<DEBUG>>>");
//    Serial.println(command);
    Serial.println(response);
    lcd_print(response);
    Serial.println(">>>DEBUG<<<");
  }
  return response;
}

void lcd_print(String msg){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg);
}