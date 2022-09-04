#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
String apiKey = "KBD1JSZTUKCXJ15V"; // Enter your Write API key from ThingSpeak
const char *ssid = "agus";     // replace with your wifi ssid and wpa2 key
const char *pass = "samarinda22";
const char* host = "iot.perumdamtirtakencana.id";
 
#define LED_BUILTIN 16
#define SENSOR  2
 
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;
 
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
 
//WiFiClient client;
 
void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid,pass);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  
  display.clearDisplay();
  display.setCursor(10,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Connecting......");
  display.display();
  delay(10);
  Serial.println("Connecting...");
  while(WiFi.status () !=WL_CONNECTED)
  {
   Serial.print(".");
   delay(500); 
  }

  Serial.println("Connected");
  
  
  
 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);
 
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
 
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}
 
void loop()
{
  int no = 0;
  while( no < 60  ){
        no++; 
        currentMillis = millis();
        if (currentMillis - previousMillis > interval) 
        {
          
          pulse1Sec = pulseCount;
          pulseCount = 0;
          
          flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
          previousMillis = millis();
          
          flowMilliLitres = (flowRate / 60) * 1000;
          flowLitres = (flowRate / 60);
          
          totalMilliLitres += flowMilliLitres;
          totalLitres += flowLitres;
          
         
          Serial.print("Flow rate: ");
          Serial.print(float(flowRate));  // Print the integer part of the variable
          Serial.print("L/min");
          Serial.print("\t");       // Print tab space
       
          display.clearDisplay();
          
          display.setCursor(10,0);  //oled display
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.print("Water Flow Meter");
          
          display.setCursor(0,20);  //oled display
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.print("R:");
          display.print(float(flowRate));
          display.setCursor(100,28);  //oled display
          display.setTextSize(1);
          display.print("L/M");
       
          // Print the cumulative total of litres flowed since starting
          Serial.print("Output Liquid Quantity: ");
          Serial.print(totalMilliLitres);
          Serial.print("mL / ");
          Serial.print(totalLitres);
          Serial.println("L");
       
          display.setCursor(0,45);  //oled display
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.print("V:");
          display.print(totalLitres);
          display.setCursor(100,53);  //oled display
          display.setTextSize(1);
          display.print("L");
          display.display();
        }

         Serial.println(no);
         delay(1000);

  } 
  
  WiFiClient client;
  const int httpPort = 80;
  if(!client.connect(host, httpPort))
  {
    Serial.println("Connection Failed");
    return;
  }

  String Link;
  HTTPClient http;

  Link = "http://" + String(host)+ "/getData.php?wm=" + String(totalLitres) + "&flow=" + String(flowRate);
  http.begin(Link);
  http.GET();

     String respon = http.getString ();
     Serial.println(respon);
     http.end();
  
  delay(1000);
}
