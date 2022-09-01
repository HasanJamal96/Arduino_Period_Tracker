void initRTC() {
  if(!rtc.begin()) {
    isRTC_Working = false;
    #ifdef DEBUGGING
      Serial.println("[RTC] Unable to find");
    #endif
    return;
  }
  else{
    isRTC_Working = true;
    #ifdef DEBUGGING
      Serial.println("[RTC] Initialized");
    #endif
  }
}


void setRTC() {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // this will set rtc time to the time at which code is compiled
  // rtc.adjust(DateTime(YEAR, MONTH, DAY, HOUR , MINUTE, SECOND)); // other way is to manually provide time
  rtc.start();
}


void updateTime() {
  if(isRTC_Working) {
    DateTime nrtc = rtc.now();
    DT.year = nrtc.year();
    DT.month = nrtc.month();
    DT.day = nrtc.day();
    DT.hours = nrtc.hour();
    DT.minutes = nrtc.minute();
    DT.seconds = nrtc.second();
    DT.unix = nrtc.unixtime();
  }
}
