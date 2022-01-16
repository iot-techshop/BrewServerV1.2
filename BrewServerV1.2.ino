

// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#endif

#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org", 3600, 60000);


//#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
#define DS18B20PIN 23
// Setup a oneWire instance to communicate with any OneWire devices

OneWire oneWire(DS18B20PIN);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensor(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

#include "heltec.h"
#define DEMO_DURATION 3000
typedef void (*Demo)(void);
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int ctr = 0;
unsigned long totCnt = 0;
float lastBPM = 0.0;
float BPM = 0.0;
float tempLast = 0.0;
String tmpStr = "";

IPAddress ip;
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 10;
    // the debounce time; increase if the output flickers
unsigned long time_1 = 0;
//#define INTERVAL_MESSAGE1 60000
//#define INTERVAL_MESSAGE1 300000
#define INTERVAL_MESSAGE1 300000// 5 mins&&&&&&&&&&&&&&&&&&

//#define INTERVAL_MESSAGE1 30000// 30 secs

//MQTT SETTINGS
int MQTT = 1;//Set to 1 to send MQTT MSG/Set to 0 for no MQTT

const char* mqtt_server = "iot.yourMqttServer.com";  //Set to your MQTT Server address or url
const char* mqtt_topic = "you/brewing/Brew001"; //Set MQTT Topic
const int buttonPin = 13;    // the number of the bloop detector pin
const int ledPin = 25;      // the number of the LED pin

// Replace with your network credentials
const char* ssid1 = "yourSSID";
const char* password = "yourPassWord";


unsigned long tbloopCount = 0;
int x = 0;
String IPaddress;
unsigned long bloopCount = 0;
String myID;
String myMac;
String dateTime;
String myString = "";
char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
char msg[100];

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readTemp18B20() {
  sensor.requestTemperatures();
  float tempinF = sensor.getTempFByIndex(0);
  tempLast = tempinF;
  Serial.print("Temperature = ");
  Serial.print(tempinF);
  Serial.println("ºF");
  return String(tempinF);
}

String readBloops() {
  tbloopCount = totCnt;
  Serial.print("Bloop Count = ");
  Serial.println(totCnt);
  return String(totCnt);
}

String readltBPM() {
  Serial.print("Bloop Count = ");
  Serial.println(lastBPM);
  return String(lastBPM);
}


String read2hrBPM() {
  Serial.print("Bloop Count = ");
  Serial.println(lastBPM);
  return String(lastBPM);
}


void drawFontFaceDemo() {
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 5, "Total = " + String(totCnt));
  Heltec.display->drawString(0, 25, "BPM = " + String(lastBPM));
  Heltec.display->drawString(0, 45, "Temp F = " + String(tempLast));
 }

void dispIP() {
  IPaddress =  WiFi.localIP().toString();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 5, "IP =     ");
  Heltec.display->drawString(0, 25, IPaddress);
}

void dispDevID() {
  myID = getEspID();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 10, "DEV ID =     ");
  Heltec.display->drawString(0, 35, myID);
}


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  digitalWrite(ledPin, ledState);
  // Serial port for debugging purposes
  Serial.begin(115200);
  sensor.begin();
  bool status;
  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid1, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    x = x + 1;
    if (x > 3) {
      ESP.restart();
    }
    Serial.println("Connecting to WiFi..");
  }

  Heltec.display->clear();
  dispDevID();
  Heltec.display->display();
  delay(4000);
  
  // Print ESP32 Local IP Address
  //  tmpStr = WiFi.localIP();
  Serial.println(WiFi.localIP());
  Heltec.display->clear();
  dispIP();
  Heltec.display->display();
  delay(4000);
  tmpStr = readTemp18B20();
  delay(500);
  tmpStr = readTemp18B20();
  Heltec.display->clear();
  drawFontFaceDemo();
  Heltec.display->display();
timeRefresh();//Gets clean NTP Time
  delay(500);
  timeRefresh();//Gets clean NTP Time
  client.setServer(mqtt_server, 1883);
if (MQTT==1) {
sendMQTTmsg();
}
  
  //// Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html");
  });
  server.on("/ltBPM", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", read2hrBPM().c_str());
  });
  server.on("/2hrBPM", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readltBPM().c_str());
  });
  server.on("/ttlBloops", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBloops().c_str());
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readTemp18B20().c_str());
  });
  server.on("/devID", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", myID.c_str());
  });
  // Start server
  server.begin();
}

void loop() {
  bloopCounter();
}

void bloopCounter() {
  Heltec.display->clear();
  drawFontFaceDemo();
  Heltec.display->display();
  if (millis() >= time_1 + INTERVAL_MESSAGE1) {
    time_1 += INTERVAL_MESSAGE1;
    lastBPM = ctr / 5.0;

if (MQTT==1) {
sendMQTTmsg();
}
      Serial.print(">>>Bloops per Min = ");
    Serial.println(float(ctr) / 5.0);
    ctr = 0;
  }
  // read the state of the bloop sensor into a local variable:
  int reading = digitalRead(buttonPin);
  if (reading == LOW) {
    ctr = ctr + 1;
    totCnt = totCnt + 1;
    //saveTotCnt(totCnt);
    Serial.print("Count= ");
    Serial.println(ctr);
    Serial.print("Total Bloops = ");
    Serial.println(totCnt);
    delay(100);
  }
  // set the LED:
  digitalWrite(ledPin, ledState);
}


String getEspID() {
  myMac = WiFi.macAddress();
  //Creates device ID from ESP01 MAC address
  myID = myMac.substring(0, 2) +  myMac.substring(3, 5) +  myMac.substring(6, 8) +  myMac.substring(9, 11) +  myMac.substring(12, 14) +  myMac.substring(15, 17);
  return myMac.substring(0, 2) +  myMac.substring(3, 5) +  myMac.substring(6, 8) +  myMac.substring(9, 11) +  myMac.substring(12, 14) +  myMac.substring(15, 17);
}

String processor(const String& var) {
  Serial.println(var);
  if (var == "IDDEVICE") {
    return getEspID();
  }
  return String();
}

void timeRefresh() {  
  timeClient.setTimeOffset(-14400);
  timeClient.begin();
  timeClient.update();
  dateTime = timeClient.getFormattedDate();  
  Serial.print("Time Refresh: DateTime=");
  Serial.println(dateTime);
  Serial.println();
}

void sendMQTTmsg(){
String dataString = "";
  String myDateTime = "";
  String myTemp = readTemp18B20();
    dateTime = timeClient.getFormattedDate();
  myDateTime = dateTime.substring(0, 4) + dateTime.substring(5, 7) + dateTime.substring(8, 10) + "_" + dateTime.substring(11, 13) + dateTime.substring(14, 16) + dateTime.substring(17, 19);
   myString = "";  
  myString = "{\"ID\":\"" + myID + "\",\"time\":\"" + myDateTime + "\",\"" + "WortTemp" + "\":\"" + myTemp + "\",\"" + "TotBloops" + "\":\"" + String(totCnt) + "\",\"" + "BPM" + "\":\"" + String(lastBPM) + "\"}";
  myString.toCharArray(msg, myString.length() + 1);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(mqtt_topic, msg);
  espClient.stop();
 }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}


void saveTotCnt(){
  
}
