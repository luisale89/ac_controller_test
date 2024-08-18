#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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

// global variables.
bool lastOutputState = false;
//-
int tempSensorResolution = 12; // bits.
int tempRequestDelay = 0;
//-
unsigned long lastTempRequest = 0;
unsigned long lastIOUpdate = 0;
const unsigned long IOUpdateInterval = 2L * 1000L; // 2 seconds

enum LOG_LEVEL {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
};

int logger_level = LOG_LEVEL::DEBUG;

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

void check_for_errors_ds18b20(float measurement, char *name) {
  char message_buffer[40];

  if (measurement == -127) {
    sprintf(message_buffer, "Error -127; [%s]", name);
    Serial.println(message_buffer);
  }
  return;
}

void get_temperatures_from_sensors(){
  char word_buffer[40];
  char temp_buffer[8];
  //--
  if (millis() - lastTempRequest >= tempRequestDelay) {
    logger("reading temperature from sensors..", DEBUG);
    float airReturnTemp = air_return_sensor.getTempCByIndex(0);
    float airInyectTemp = air_inyect_sensor.getTempCByIndex(0);
    float evaporatorTemp = evaporator_sensor.getTempCByIndex(0);
    check_for_errors_ds18b20(airReturnTemp, "air return");
    check_for_errors_ds18b20(airInyectTemp, "air inyection");
    check_for_errors_ds18b20(evaporatorTemp, "evaporator temp.");

    Serial.print("AirReturn °C @ ");
    Serial.println(airReturnTemp);
    Serial.print("AirInyect °C @ ");
    Serial.println(airInyectTemp);
    Serial.print("EvaporatorTemp °C @ ");
    Serial.println(evaporatorTemp);
    
    //request temperatures again.
    air_inyect_sensor.requestTemperatures();
    air_return_sensor.requestTemperatures();
    evaporator_sensor.requestTemperatures();

    lastTempRequest = millis();
    logger("Temp. readings Done.", DEBUG);

    return;
  }
}

void update_IO(){
  // io updates.
  char word_buffer[40];
  
  if (millis() - lastIOUpdate >= IOUpdateInterval) {
    //-
    // get datetime

    sprintf(word_buffer, "inputs: Q3[%d] Q2[%d] Q1[%d] NOW[%d]", 
    digitalRead(Q3_INPUT), digitalRead(Q2_INPUT), digitalRead(Q1_INPUT), digitalRead(NOW_CNF));
    logger(word_buffer, DEBUG);
    switch (lastOutputState)
    {
    case true:
      digitalWrite(NETWORK_LED, LOW);
      digitalWrite(COMP_RELAY, LOW);
      digitalWrite(VENT_RELAY, LOW);
      digitalWrite(AUTO_RELAY, LOW);
      lastOutputState = false;
      break;
    
    default:
      digitalWrite(NETWORK_LED, HIGH);
      digitalWrite(COMP_RELAY, HIGH);
      digitalWrite(VENT_RELAY, HIGH);
      digitalWrite(AUTO_RELAY, HIGH);
      lastOutputState = true;
      break;
    }

    lastIOUpdate = millis();
  }

  return;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  logger("setting up DS18B20 sensors.", DEBUG);
  air_inyect_sensor.begin();
  air_return_sensor.begin();
  evaporator_sensor.begin();
  //-- sens. resolution
  air_inyect_sensor.setResolution(tempSensorResolution);
  air_return_sensor.setResolution(tempSensorResolution);
  evaporator_sensor.setResolution(tempSensorResolution);
  tempRequestDelay = 750 / (1 << (12 - tempSensorResolution));
  //-- async mode
  air_inyect_sensor.setWaitForConversion(false);
  air_return_sensor.setWaitForConversion(false);
  evaporator_sensor.setWaitForConversion(false);
  //--
  logger("requesting temp. values from sensors.", DEBUG);
  air_inyect_sensor.requestTemperatures();
  air_return_sensor.requestTemperatures();
  evaporator_sensor.requestTemperatures();
  lastTempRequest = millis();
  logger("DS18B20 settings done", DEBUG);

  //-- pinmodes.
  pinMode(AUTO_RELAY, OUTPUT);
  pinMode(VENT_RELAY, OUTPUT);
  pinMode(COMP_RELAY, OUTPUT);
  pinMode(NETWORK_LED, OUTPUT);
  pinMode(Q3_INPUT, INPUT_PULLUP);
  pinMode(Q2_INPUT, INPUT_PULLUP);
  pinMode(Q1_INPUT, INPUT_PULLUP);
  pinMode(NOW_CNF, INPUT);

  logger("setup done.", INFO);
}

void loop() {
  // put your main code here, to run repeatedly:
  get_temperatures_from_sensors();
  update_IO();
}