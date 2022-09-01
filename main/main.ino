#include <SPI.h>
#include <Wire.h>
#include "config.h"
#include "RTClib.h"
#include <TimeLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_FT6206.h>
#include <Adafruit_ILI9341.h>
#include <FlashStorage_SAMD.h>

Adafruit_FT6206 ts = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

Adafruit_GFX_Button btn_about, btn_log, btn_add_symptom, btn_back, btn_start, btn_end, btn_delete, btn_next, btn_prev;
Adafruit_GFX_Button btn_symptoms[5];

TS_Point p = ts.getPoint();

RTC_PCF8523 rtc;


date_time_t     DT;
period_state_t  PeriodState = NONE;
cycle_t         CurrentCycle;
cycle_t         LastCycle;
cycle_t         LogCycle[5];



void(* resetFunc) (void) = 0;

void setup() {
  #ifdef DEBUGGING
    Serial.begin(BAUDRATE);
    Serial.println("[Main] Setup started");
  #endif

  readAllConfiguration();
  updateDataArray();
  if(checkResetEEPROM()){
    Serial.println("Rest eeprom");
    setRTC();
  }

  uint8_t ps = EEPROM.read(ADDR_PERIOD_STATE);
  if(ps == 1){
    PeriodState = PERIOD_ENDED;
    CurrentCycle.Next = CurrentCycle.EndTime + (AveragePeriodDuration * 24*60*60);
  }
  else if(ps == 2)
    PeriodState = PERIOD_STARTED;
  else
    PeriodState = NONE;
  
  initScreen();
  
  #ifdef DEBUGGING
    Serial.println("[Main] Setup complete");
  #endif
}


void loop() {
  updateScreen();
}
