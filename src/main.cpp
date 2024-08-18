#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

enum LOG_LEVEL {
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4
};

LOG_LEVEL logger_level = DEBUG;

//pinout
#define AUTO_RELAY 18
#define VENT_RELAY 17
#define COMP_RELAY 16
#define NETWORK_LED 2
#define Q3_INPUT 14
#define Q2_INPUT 27
#define Q1_INPUT 26
#define RETURN_TEMP 25
#define INYECT_TEMP 33
#define EVAPOR_TEMP 32
#define NOW_CNF 35

// oneWire and DallasTemperature
OneWire ow_return_temp(RETURN_TEMP);
OneWire ow_inyect_temp(INYECT_TEMP);
OneWire ow_evapor_temp(EVAPOR_TEMP);

DallasTemperature air_return_sensor(&ow_return_temp);
DallasTemperature air_inyect_sensor(&ow_inyect_temp);
DallasTemperature evaporator_sensor(&ow_evapor_temp);


// put function declarations here:

//logger function.
void logger(char *message, LOG_LEVEL msg_level){
  unsigned long now = millis();
  switch (msg_level)
  {
  case DEBUG:
    /* code */
    if (logger_level <= DEBUG) {
      Serial.print("- [D] @ ");
      Serial.print(now);
      Serial.print(" > msg: ");
      Serial.println(message);
      Serial.flush();
    }
    break;

  case INFO:
    if (logger_level <= INFO) {
      Serial.print("- [I] @ ");
      Serial.print(now);
      Serial.print(" > msg: ");
      Serial.println(message);
      Serial.flush();
    }
    break;

  case WARNING:
    if (logger_level <= WARNING) {
      Serial.print("- [W] @ ");
      Serial.print(now);
      Serial.print(" > msg: ");
      Serial.println(message);
      Serial.flush();
    }
    break;

  case ERROR:
    if (logger_level <= ERROR) {
      Serial.print("- [E] @ ");
      Serial.print(now);
      Serial.print(" > msg: ");
      Serial.println(message);
      Serial.flush();
    }
    break;
  
  default:
    Serial.println("not implemented..");
    Serial.flush();
    break;
  }
  return;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(AUTO_RELAY, OUTPUT);
  pinMode(VENT_RELAY, OUTPUT);
  pinMode(COMP_RELAY, OUTPUT);
  pinMode(NETWORK_LED, OUTPUT);
  pinMode(Q3_INPUT, INPUT);
  pinMode(Q2_INPUT, INPUT);
  pinMode(Q1_INPUT, INPUT);
  pinMode(NOW_CNF, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
}