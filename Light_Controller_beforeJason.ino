#include <SPI.h>
#include <ArduinoHttpClient.h>
#include <WiFi101.h>

#include "arduino_secrets.h"

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

int statusCode = 0;
String response;
int count = 0;

WiFiClient netSocket;            // network socket to server
//const char server[] = "216.165.95.147";  // change server name
const char server[] = "128.122.6.151"; //simon's server
//const char server[] = "216.165.95.164";// jason's server

//String route1 = "/dance";


int toggleFire = 6;
int toggleRainbow = 7;
int toggleLightning = 8;
int patternValue;


int colorWheelPin = A1;
int currentColorWheelValue = 0;
int lastColorWheelValue  = 0;
int useColorWheelValue = 0;

int colorVariable;

int potPin = A2;
int potValue;

int speedValue;

int connectionLED = 0; //this LED indicates whether you are connected to the WIFI or not

void setup() {
  Serial.begin(9600);
  pinMode(toggleFire, INPUT_PULLUP);
  pinMode(toggleRainbow, INPUT_PULLUP);
  pinMode(toggleLightning, INPUT_PULLUP);
  pinMode(colorWheelPin, INPUT);
  pinMode(potPin, INPUT);
  pinMode(connectionLED, OUTPUT);
  // put your setup code here, to run once:

  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);           // print the network name (SSID)
    WiFi.begin(ssid, pass);
    digitalWrite(connectionLED, LOW);// try to connect
    delay(2000);
  }

  printWifiStatus() ;
}

void loop() {
//  Serial.println("I am looping");
  HttpClient http(netSocket, server, 3003);
  //   http.get(route);
  String routePattern = "/pattern";
  String routeSpeed = "/speed";
  String routeColor = "/color";
  String reading = "{\"potentiometer\":\"" + String(speedValue) + "\", \"colorwheel\":\"" + String(colorVariable) + "\", \"switch\":\"" + String(patternValue) + "\"}";    // close the JSON object
  String contentType = "application/json";// Set the content type
  String strangReading = String(reading);

//check pattern
  if (digitalRead(toggleFire) == LOW) {
    //  Serial.println("Fire");
//    patternValue = 1;
    patternValue = "fire";
  }
  else if (digitalRead(toggleRainbow) == LOW) {
    //  Serial.println("rainbow");
//    patternValue = 2;
     patternValue = "rainbow";

  }
  else if (digitalRead(toggleLightning) == LOW) {
    //   Serial.println("lightning");
//    patternValue = 3;
     patternValue = "lightning";
  }
  else {
    patternValue = 0;
  }

   http.beginRequest();                    // start assembling the request
   http.post(routePattern, contentType, strangReading);
   delay(200);
   

//check speed
  potValue = analogRead(potPin);
  if (potValue < 340) {
    speedValue = 0;
  } else if (potValue < 680) {
    speedValue = 1;
  } else {
    speedValue = 2;
  }
    http.beginRequest();                    // start assembling the request
    http.post(routeSpeed, contentType, strangReading);
    delay(200);


//check color
  lastColorWheelValue = analogRead(colorWheelPin);
//  Serial.print(currentColorWheelValue);

  if (currentColorWheelValue != lastColorWheelValue) {
    if (currentColorWheelValue > 1 && currentColorWheelValue < 25) {
      //don't update it
    } else {
      //update it
      useColorWheelValue = currentColorWheelValue;

      if (useColorWheelValue > 25 && useColorWheelValue < 341) {
        colorVariable = 0;
//        Serial.println("red");
      }
      else if (useColorWheelValue > 341 && useColorWheelValue < 680) {
        colorVariable = 1;
        //    Serial.println("blue");
      }
      else if (useColorWheelValue > 681 && useColorWheelValue < 1024) {
        colorVariable = 2;
        //    Serial.println("white");
      }

      http.beginRequest();                    // start assembling the request
      http.post(routeColor, contentType, strangReading);
      delay(200);
    }
  }


  //Serial.println("last:" lastColorWheelValue);
  currentColorWheelValue = lastColorWheelValue;
  delay(100);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // When you connect to wifi turn on light
  digitalWrite(connectionLED, HIGH);
}


