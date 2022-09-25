#define BLYNK_TEMPLATE_ID "TMPLZU0YfOUI"
#define BLYNK_DEVICE_NAME "Organic Sprout"
#define BLYNK_AUTH_TOKEN "ITUzEIukQsfTxhqk7ikrXtJhioNmw9zB"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define DHTPIN 32
#define DHTYPE DHT11
#define WaterLevel 34


// Relay pin
#define RelaySlenoid1 25
#define RelaySlenoid2 26
#define RelayPump 27
#define RelayFan 14

DHT dht(DHTPIN,DHTYPE);
BlynkTimer  timer;

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Hardware_Lab@504_2.4GHz";
char pass[] = "hwlab504";

// State all Relay valve
int waterLevelRead;
bool SolenoidValve1 = false;
bool SolenoidValve2 = false;
bool pump = false;
bool Fan = false;




void AllRelay(){
  digitalWrite(RelaySlenoid1, SolenoidValve1);
  digitalWrite(RelaySlenoid2, SolenoidValve2);
  digitalWrite(RelayPump, pump);
  digitalWrite(RelayFan, Fan);
}

BLYNK_WRITE(V7){
  int value = param.asInt();
    SolenoidValve1 = value;
}

BLYNK_WRITE(V8){
  int value = param.asInt();

    SolenoidValve2 = value;

}

BLYNK_WRITE(V10){
  int value = param.asInt();
    pump = value;
}

BLYNK_WRITE(V11){
  int value = param.asInt();
    Fan = value;
}


void setup() {
    Serial.begin(9600);
    delay(1000);
    Blynk.begin(auth,ssid,pass);
    pinMode(RelaySlenoid1, OUTPUT);
    pinMode(RelaySlenoid2, OUTPUT);
    pinMode(RelayPump, OUTPUT);
    pinMode(RelayFan, OUTPUT);
    dht.begin();
    timer.setInterval(1000L,AllRelay);
}

void loop() {
  Blynk.run();
  timer.run();
}