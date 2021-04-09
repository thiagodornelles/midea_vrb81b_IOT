#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include "midea_controller.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "";
const char *password = "";
ESP8266WebServer server(8080);

unsigned long key_value = 0;
const uint16_t kRecvPin = D1;
const int kSendPin = D4;

IRrecv irrecv(kRecvPin);
IRsend irsend(kSendPin);

decode_results results;

void setup()
{
  Serial.begin(9600);

  while (!Serial) // Wait for the serial connection to be establised.
      delay(50);  

  irrecv.enableIRIn(); // Start the receiver
  irsend.begin();
  pinMode(D4, OUTPUT);

  WiFi.hostname("JEREDY");
  WiFi.begin(ssid, password);
  
  //Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  

  server.on("/", handle_on_connect);
  server.on("/power", handle_power);
  server.on("/forwards", handle_forwards);
  server.on("/backwards", handle_backwards);
  server.on("/left", handle_left);
  server.on("/right", handle_right);
  server.on("/charge", handle_charge);
  server.on("/program", handle_program);
  server.on("/vacuum", handle_vacuum);

  server.onNotFound(handle_not_found);

  server.begin();
  Serial.println("HTTP server started");
  
}

void handle_on_connect()
{
  server.send(200, "text/html", "<h2>BEM VINDO AO JEREDY TURBO PLUS!</h2> <br> <br> GET COMMANDS: <br> power <br> forwards <br> backwards <br> left <br> right <br> charge <br> program <br> vacuum");
}

void handle_power()
{
  irsend.sendRaw(POWER_ON, sizeof(POWER_ON), 38);
  server.send(200, "text/html", "POWER");
}

void handle_forwards()
{
  irsend.sendRaw(FORWARDS, sizeof(FORWARDS), 38);
  server.send(200, "text/html", "FORWARDS");
}

void handle_backwards()
{
  irsend.sendRaw(BACKWARDS, sizeof(BACKWARDS), 38);
  server.send(200, "text/html", "BACKWARDS");
}

void handle_left()
{
  irsend.sendRaw(LEFT, sizeof(LEFT), 38);
  server.send(200, "text/html", "LEFT");
}

void handle_right()
{
  irsend.sendRaw(RIGHT, sizeof(RIGHT), 38);
  server.send(200, "text/html", "RIGHT");
}

void handle_charge(){
  irsend.sendRaw(CHARGE, sizeof(CHARGE), 38);
  server.send(200, "text/html", "CHARGE");
}

void handle_program(){
  irsend.sendRaw(CHANGE_PROGRAM, sizeof(CHANGE_PROGRAM), 38);
  server.send(200, "text/html", "CHANGE PROGRAM");
}

void handle_vacuum(){
  irsend.sendRaw(VACUUM_POWER, sizeof(VACUUM_POWER), 38);
  server.send(200, "text/html", "VACCUM POWER");
}

void handle_not_found()
{
  server.send(404, "text/plain", "WOOPS, no donut for you.");
}

void loop()
{
  server.handleClient();
  if (irrecv.decode(&results))
  {
    serialPrintUint64(results.value, HEX);
    Serial.println();
    irrecv.resume(); // Receive the next value
  }  

  //IN THE CASE OF DISCONNECTION
  while (WiFi.status() != WL_CONNECTED) {  
    WiFi.begin(ssid, password);  
    while (WiFi.status() != WL_CONNECTED) {  
        delay(1000);  
        Serial.print(".");
    }      
    Serial.println();
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
  }
}
