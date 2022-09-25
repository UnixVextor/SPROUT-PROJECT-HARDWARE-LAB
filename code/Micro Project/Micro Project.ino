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

bool isSoakTime = false;
bool isActiveTime = false;
bool isTimeToWater = false;
bool isWaterSprout =  false;
bool isTimeFan = false;

// Active Time
int hh_active = 0;
int mm_active = 0;
int ss_active = 0;
int day_active = 0;
unsigned long lastTime_Active = 0;

// Soak Time
int default_hh_soak = 8;
int default_mm_soak = 0;
int default_ss_soak = 0;
int hh_soak = default_hh_soak;
int mm_soak = default_mm_soak;
int ss_soak = default_ss_soak;
unsigned long lastTime_Soak = 0;

// Wait to Water
int defaul_hh_water = 2;
int defaul_mm_water = 0;
int defaul_ss_water = 0;
int hh_water = defaul_hh_water;
int mm_water = defaul_mm_water;
int ss_water = defaul_ss_water;
unsigned long lastTime_Water = 0;

// Water Sprout
int default_mm_sprout = 10;
int default_ss_sprout = 0;
int mm_sprout = default_mm_sprout;
int ss_sprout = default_ss_sprout;
unsigned long lastTime_Sprout = 0; 

// Time Fan
int default_mm_Fan = 5;
int default_ss_Fan = 0;
int mm_Fan = default_mm_Fan;
int ss_Fan = default_ss_Fan;
unsigned long lastTime_Fan = 0; 

// State all Relay valve
int waterLevelRead;
bool SolenoidValve1 = false;
bool SolenoidValve2 = false;
bool pump = false;
bool Fan = false;
bool clearWater = false;
bool TakeWater = false;
bool flip = true;


void sentDHTSensor(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)){
    Serial.println("Faile to read from DHT Sensor !");
  }
  Blynk.virtualWrite(V2,h);
  Blynk.virtualWrite(V1,t);

}

void ActiveTime(){
  String ss_prefix = "";
  String mm_prefix = "";
  String hh_prefix = "";
  if (isActiveTime && millis() - lastTime_Active >= 1000){
    ss_active++;
  }

  if (ss_active > 59) {
    mm_active++;
    ss_active = 0;
  }
  if (mm_active > 59) 
  {
    hh_active++;
    mm_active = 0;
  }
  if (hh_active > 23){
    day_active++;
    hh_active = 0;
  }
  if (ss_active < 10) ss_prefix = "0";
  if (mm_active < 10) mm_prefix = "0";
  if (hh_active < 10) hh_prefix = "0";

  Blynk.virtualWrite(V3,day_active, " DAY ",hh_prefix,hh_active,":",mm_prefix,mm_active,":",ss_prefix,ss_active);
  if (day_active == 7){
    isActiveTime = false;
    Blynk.virtualWrite(V0,isActiveTime);
  }
}

void Soak_time(){
  String ss_prefix = "";
  String mm_prefix = "";
  String hh_prefix = "";
  if (isActiveTime && isSoakTime && millis() - lastTime_Soak >= 1000 && !clearWater && !TakeWater){
      ss_soak--;
  }
  if (ss_soak < 0){
    mm_soak--;
    ss_soak = 59;
  } 
  if (mm_soak < 0){
    hh_soak--;
    mm_soak = 59;
  }
  if (ss_soak < 10) ss_prefix = "0";
  if (mm_soak < 10) mm_prefix = "0";
  if (hh_soak < 10) hh_prefix = "0";
  
  if (hh_soak <= 0 && mm_soak <= 0 && ss_soak <= 0 and isSoakTime){
    hh_soak = default_hh_soak;
    mm_soak = default_mm_soak;
    ss_soak = default_ss_soak;
    isSoakTime = false;
    SolenoidValve2 = true;
  }
  Blynk.virtualWrite(V5,hh_prefix,hh_soak,":",mm_prefix,mm_soak,":",ss_prefix,ss_soak);
}

void WaitToWater(){
  String ss_prefix = "";
  String mm_prefix = "";
  String hh_prefix = "";
  if (isActiveTime && isTimeToWater && millis() - lastTime_Water >= 1000  && !clearWater && !TakeWater){
    ss_water--;
  }
  if (ss_water < 0){
    mm_water--;
    ss_water = 59;
  } 
  if (mm_water < 0){
    hh_water--;
    mm_water = 59;
  }
  if (ss_water < 10) ss_prefix = "0";
  if (mm_water < 10) mm_prefix = "0";
  if (hh_water < 10) hh_prefix = "0";
  
  Blynk.virtualWrite(V9,hh_prefix,hh_water,":",mm_prefix,mm_water,":",ss_prefix,ss_water);
  if (hh_water <= 0 && mm_water <= 0 && ss_water <= 0 && isTimeToWater){
    hh_water = defaul_hh_water;
    mm_water = default_mm_soak;
    ss_water = defaul_ss_water;
    isWaterSprout = true;
    isTimeToWater = false;
  }

  if (hh_water <= 0) isTimeFan = true;
}


void WaterSprout(){
  String ss_prefix = "";
  String mm_prefix = "";
  if (isActiveTime && isWaterSprout && millis() - lastTime_Sprout >= 1000  && !clearWater && !TakeWater){
    ss_sprout--;
  }
  if (ss_sprout < 0){
    mm_sprout--;
    ss_sprout = 59;
  } 
  if (ss_sprout < 10) ss_prefix = "0";
  if (mm_sprout < 10) mm_prefix = "0";
  
  Blynk.virtualWrite(V6,mm_prefix,mm_sprout,":",ss_prefix,ss_sprout);
  if (mm_sprout <= 0 && ss_sprout <= 0 && isWaterSprout){
    mm_sprout = default_mm_sprout;
    ss_sprout = default_ss_sprout;
    isWaterSprout = false;
    isTimeToWater = true;
  }
}

void TimeFan(){
  String ss_prefix = "";
  String mm_prefix = "";
  if (isActiveTime && isTimeFan && millis() - lastTime_Fan >= 1000  && !clearWater && !TakeWater){
    ss_Fan--;
  }
  if (ss_Fan < 0){
    mm_Fan--;
    ss_Fan = 59;
  } 
  if (ss_Fan < 10) ss_prefix = "0";
  if (mm_Fan < 10) mm_prefix = "0";
  
  Blynk.virtualWrite(V12,mm_prefix,mm_Fan,":",ss_prefix,ss_Fan);
  if (mm_Fan <= 0 && ss_Fan <= 0 && isTimeFan){
    mm_Fan = default_mm_Fan;
    ss_Fan = default_ss_Fan;
    flip = !flip;
  }
}

BLYNK_WRITE(V0){
  int value = param.asInt();
  isActiveTime = value;
  if (value == 0){
    Blynk.virtualWrite(V7,0);
    SolenoidValve1 = false;
    Blynk.virtualWrite(V8,0);
    SolenoidValve2 = false;
    Blynk.virtualWrite(V10,0);
    pump = false;
  }

}

void TurnONSolenoidValve1(){
  if (!TakeWater){
  if(isActiveTime && waterLevelRead < 6  && !SolenoidValve2){
    SolenoidValve1 = true;
    isSoakTime = false;
  }else if (hh_soak > 0 && waterLevelRead > 6 && !SolenoidValve2){
    isSoakTime = true;
    SolenoidValve1 = false;
  }

  if(hh_soak == 0 && mm_soak == 0 && ss_soak == 0){
    SolenoidValve1 = false;
  }
 }

  if (isActiveTime && (day_active != 0 && day_active % 4 == 0) && hh_active == 0 && mm_active < 58 && !SolenoidValve2){
    SolenoidValve1 = true;
    TakeWater = true;
  }else{
    TakeWater = false;
  }

  if(SolenoidValve1 == true){
    Blynk.virtualWrite(V7,1);
  }else{
    Blynk.virtualWrite(V7,0);
  }

  
}

void TurnONSolenoidValve2(){
  if(!clearWater){
  if(SolenoidValve2 && isActiveTime && waterLevelRead > 1 && !SolenoidValve1){
    SolenoidValve2 = true;
    isTimeToWater = false;
  }else if (!SolenoidValve1 || !isActiveTime){
    SolenoidValve2 = false;
  }
  if(isActiveTime && !SolenoidValve2 && !SolenoidValve1 && !isSoakTime && !isWaterSprout){
    isTimeToWater = true;
  }else{
    isTimeToWater = false;
  }
}

  if (isActiveTime && ( day_active != 0 && day_active % 4 == 0) && hh_active == 0 && mm_active < 55){
    SolenoidValve2 = true;
    SolenoidValve1 = false;
    clearWater = true;
  }else{
    clearWater = false;
  }

  if(SolenoidValve2){
    Blynk.virtualWrite(V8,1);
  }else{
    Blynk.virtualWrite(V8,0);
  }

}

void PUMP(){
  if(isWaterSprout && isActiveTime){
    pump = true;
  }else{
    pump = false;
  }
  Blynk.virtualWrite(V10,pump);
}

void TurnOnFan(){
  if( isTimeFan && flip && isActiveTime){
    Fan = true;
  }else{
    Fan = false;
  }
  Blynk.virtualWrite(V11,Fan);
}


void waterLevelSensor(){
  waterLevelRead = analogRead(WaterLevel);
  waterLevelRead = map(waterLevelRead, 1, 1023, 0, 10);
  Blynk.virtualWrite(V4,waterLevelRead);
}

void AllRelay(){
  digitalWrite(RelaySlenoid1, SolenoidValve1);
  digitalWrite(RelaySlenoid2, SolenoidValve2);
  digitalWrite(RelayPump, pump);
  digitalWrite(RelayFan, Fan);
}

// BLYNK_WRITE(V7){
//   int value = param.asInt();
//   if(!isActiveTime){
//     SolenoidValve1 = value;
//   }
// }

// BLYNK_WRITE(V8){
//   int value = param.asInt();
//   if(!isActiveTime){
//     SolenoidValve2 = value;
//   }

// }

// BLYNK_WRITE(V10){
//   int value = param.asInt();
//   if(!isActiveTime){
//     pump = value;
//   }
// }

// BLYNK_WRITE(V11){
//   int value = param.asInt();
//   if(!isActiveTime){
//     Fan = value;
//   }
// }


void setup() {
    Serial.begin(9600);
    delay(1000);
    Blynk.begin(auth,ssid,pass);
    pinMode(RelaySlenoid1, OUTPUT);
    pinMode(RelaySlenoid2, OUTPUT);
    pinMode(RelayPump, OUTPUT);
    pinMode(RelayFan, OUTPUT);
    dht.begin();
    timer.setInterval(100L,sentDHTSensor);
    timer.setInterval(1000L,ActiveTime);
    if (!isTimeToWater) timer.setInterval(100L,Soak_time);
    timer.setInterval(1000L,WaitToWater);
    timer.setInterval(1000L,WaterSprout);
    timer.setInterval(1000L,PUMP);
    timer.setInterval(100L, waterLevelSensor);
    timer.setInterval(1000L,TurnONSolenoidValve1);
    timer.setInterval(1000L,TurnONSolenoidValve2);
    timer.setInterval(1000L,PUMP);
    timer.setInterval(1000L,AllRelay);
    timer.setInterval(1000L,TurnOnFan);
    timer.setInterval(1000L,TimeFan);
}

void loop() {
  Blynk.run();
  timer.run();
}
