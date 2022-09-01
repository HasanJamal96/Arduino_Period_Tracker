int ElapsedTime(uint32_t startTime, uint32_t endTime) {
  uint32_t elapsedTime = startTime - endTime;
  elapsedTime = elapsedTime / 60;
  elapsedTime = elapsedTime / 60;
  elapsedTime = elapsedTime / 24;
  if (elapsedTime > 100)
    return 0;
  return elapsedTime;
}

void findAverageCycle() {
  int val = 0;
  if(LogId < 5)
    val = AveragePeriodDuration;
  int e_val;
  for (uint8_t i = 0; i < LogId - 1; i++) {
    e_val = ElapsedTime(LogCycle[i].EndTime, LogCycle[i].StartTime);
    val += e_val;
  }
  AveragePeriodDuration = val / LogId;

}

void updateDataArray() {
  LogCycle[LogId - 1].StartTime = CurrentCycle.StartTime;
  LogCycle[LogId - 1].EndTime = CurrentCycle.EndTime;
  for (uint8_t i = 0; i < 5; i++)
    LogCycle[LogId - 1].symptoms[i] = CurrentCycle.symptoms[i];
}

void shiftData() {
  for (uint8_t i = 0; i < 4; i++) {
    LogCycle[i] = LogCycle[i + 1];
  }
  updateDataArray();
}

void initScreen() {
  #ifdef DEBUGGING
    Serial.println("[LCD] initializing");
  #endif
  tft.begin();
  
  if (!ts.begin(40)) {
    #ifdef DEBUGGING
      Serial.println("[LCD] initialization failed");
      Serial.println("[Main] Arduino will restart in 3 seconds");
    #endif
      delay(3);
      resetFunc();
    }
  #ifdef DEBUGGING
  else
    Serial.println("[LCD] initialized");
  #endif
  displayMainPage();
}

bool isTouched() {
  if (ts.touched()) {
    p = ts.getPoint();
    p.x = map(p.x, 0, 240, 240, 0);
    p.y = map(p.y, 0, 320, 320, 0);
    return true;
  }
  return false;
}



/////////////////////////////////////  Main Page /////////////////////////////////////////


void displayInfo() {
  if(CurrentPage != 1)
    return;
  tft.setTextSize(2);
  tft.setTextColor(NAVY);
  if (PeriodState == PERIOD_STARTED) {
    tft.fillRect(70, 5, 240, 60, BACKGROUND_COLOR);
    tft.setCursor(70, 20);
    tft.print("Started");
    tft.setCursor(105, 40);
    tft.print(ElapsedTime(DT.unix, CurrentCycle.StartTime));
  }
  else if (PeriodState == PERIOD_ENDED) {
    tft.fillRect(70, 5, 240, 60, BACKGROUND_COLOR);
    tft.setCursor(70, 20);
    //tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
    //tft.setTextColor(NAVY, BACKGROUND_COLOR); //attempt to resolve overlapping info problem
    tft.print("Days until");
    tft.setCursor(105, 40);
    tft.print(ElapsedTime(CurrentCycle.Next, DT.unix));
  }
}

void displayMainPage() {
  #ifdef DEBUGGING
    Serial.println("[LCD] Main page");
  #endif
  CurrentPage = 1;
  tft.fillScreen(BACKGROUND_COLOR);

  uint16_t w = tft.width();
  uint16_t h = tft.height();
  uint16_t btn_height = 30;

  //btn_log.initButton        (&tft, w / 2,     h - (btn_height + (btn_height / 2)), w,   btn_height, WHITE, CYAN, BLACK, "Period log", 2); //outline, fill, text
  //btn_add_symptom.initButton(&tft, w / 4,     h - (btn_height / 2),              w / 2, btn_height, WHITE, CYAN, BLACK, "Add\nsymptoms", 2);
  //btn_about.initButton      (&tft, w - (w / 4), h - (btn_height / 2),              w / 2, btn_height, WHITE, CYAN, BLACK, "About", 2);


  btn_log.initButton        (&tft, w / 2,     h - (btn_height + (btn_height / 2)), w,   btn_height, WHITE, CYAN, BLACK, (char*)"Logs", 2); //outline, fill, text
  btn_add_symptom.initButton(&tft, w / 4,     h - (btn_height / 2),              w / 2, btn_height, WHITE, CYAN, BLACK, (char*)"Symptoms", 2);
  btn_about.initButton      (&tft, w - (w / 4), h - (btn_height / 2),              w / 2, btn_height, WHITE, CYAN, BLACK, (char*)"About", 2);

  h = h / 2;

  btn_start.initButton(&tft, w / 4,    h, w / 3, btn_height, WHITE, CYAN, BLACK, (char*)"Start", 2);
  btn_end.initButton  (&tft, w * 0.75, h, w / 3, btn_height, WHITE, CYAN, BLACK, (char*)"End",   2);

  btn_log.drawButton();
  btn_about.drawButton();
  btn_add_symptom.drawButton();
  btn_end.drawButton();
  btn_start.drawButton();

  displayInfo();
}

uint32_t ct;


void readMainPage() {
  bool pressed = isTouched();
  btn_log.press(pressed && btn_log.contains(p.x, p.y));
  btn_add_symptom.press(pressed && btn_add_symptom.contains(p.x, p.y));
  btn_about.press(pressed && btn_about.contains(p.x, p.y));
  btn_end.press(pressed && btn_end.contains(p.x, p.y));
  btn_start.press(pressed && btn_start.contains(p.x, p.y));

  if (PeriodState == PERIOD_ENDED || PeriodState == NONE) {
    if (btn_start.justPressed()) {
      btn_start.drawButton(true);
    }
    else if (btn_start.justReleased()) {
      updateTime();
      PeriodState = PERIOD_STARTED;
      CurrentCycle.StartTime = DT.unix;
      btn_start.drawButton();
      displayInfo();
      EEPROM.write(ADDR_PERIOD_STATE, 1);
      EEPROM.commit();
    }
  }

  if (PeriodState == PERIOD_STARTED) {
    if (btn_end.justPressed()) {
      btn_end.drawButton(true);
    }
    else if (btn_end.justReleased()) {
      updateTime();
      PeriodState = PERIOD_ENDED;
      CurrentCycle.EndTime = DT.unix;
      btn_end.drawButton();
      if (LogId > 4) {
        shiftData();
        saveAllCycles();
      }
      else {
        LogId += 1;
        updateDataArray();
        saveOneCycle(LogId - 1);
        EEPROM.write(ADDR_LOGGED_COUNT, LogId);
      }
      findAverageCycle();
      CurrentCycle.Next = CurrentCycle.EndTime + (AveragePeriodDuration * 24 * 60 * 60);
      EEPROM.write(ADDR_AVERAGE_DAYS, AveragePeriodDuration);
      EEPROM.write(ADDR_PERIOD_STATE, 2);
      EEPROM.commit();
      displayInfo();
      resetSymps();
    }
  }
  if (btn_about.justPressed())
    btn_about.drawButton(true);
  else if (btn_about.justReleased()) {
    displayAboutPage();
    return;
  }
  if(btn_log.justReleased()) {
    displayLogPage();
    return;
  }
  if(btn_add_symptom.justReleased()) {
    displaySymptomsPage();
    return;
  }
  if (millis() - LastInfoUpdate >= 1000) {
    updateTime();
    displayInfo();
    LastInfoUpdate = millis();
  }
}

void resetSymps() {
  for(uint8_t i=0; i<5; i++) {
    CurrentCycle.symptoms[i] = "";
  }
}



/////////////////////////////////////  Symptoms Page /////////////////////////////////////////

uint8_t btn_gap     = 10;
uint8_t btn_height  = 40;
uint8_t btn_width   = 240 - 40;
uint8_t btn_start_x = 240 / 2;
uint8_t btn_start_y = 70;

void displaySymptomsPage() {
  #ifdef DEBUGGING
    Serial.println("[LCD] Symptoms page");
  #endif
  CurrentPage = 2;
  tft.fillScreen(BACKGROUND_COLOR);

  for (uint8_t i = 0; i < 5; i++) {
    if (CurrentCycle.symptoms[i] == "")
      btn_symptoms[i].initButton(&tft, btn_start_x, btn_start_y + ((btn_height * i) + (btn_gap * i)), btn_width, btn_height, WHITE, DESELECTED_COLOR, BLACK, SYMPTOMS_NAMES[i], 1);
    else
      btn_symptoms[i].initButton(&tft, btn_start_x, btn_start_y + ((btn_height * i) + (btn_gap * i)), btn_width, btn_height, WHITE, SELECTED_COLOR,   BLACK, SYMPTOMS_NAMES[i], 1);

    btn_symptoms[i].drawButton();
  }

  btn_back.initButton(&tft, 20, 20, 40, 40, WHITE, CYAN, BLACK, (char*)"<<", 2);
  btn_back.drawButton();
}


void readSymptomsPage() {
  bool pressed = isTouched();
  for (uint8_t i = 0; i < 5; i++) {
    btn_symptoms[i].press(pressed && btn_symptoms[i].contains(p.x, p.y));

    if (btn_symptoms[i].justReleased()) {
      if (CurrentCycle.symptoms[i] == "") {
        CurrentCycle.symptoms[i] = SYMPTOMS_NAMES[i];
        btn_symptoms[i].initButton(&tft, btn_start_x, btn_start_y + ((btn_height * i) + (btn_gap * i)), btn_width, btn_height, WHITE, SELECTED_COLOR, BLACK, SYMPTOMS_NAMES[i], 1);
      }
      else {
        CurrentCycle.symptoms[i] = "";
        btn_symptoms[i].initButton(&tft, btn_start_x, btn_start_y + ((btn_height * i) + (btn_gap * i)), btn_width, btn_height, WHITE, DESELECTED_COLOR, BLACK, SYMPTOMS_NAMES[i], 1);
      }
    }
    btn_symptoms[i].drawButton();
  }
  btn_back.press(pressed && btn_back.contains(p.x, p.y));
  if (btn_back.justPressed())
    btn_back.drawButton(true);
  else if (btn_back.justReleased()) {
    displayMainPage();
    return;
  }
}



/////////////////////////////////////  About Page /////////////////////////////////////////

void displayAboutPage() {
  #ifdef DEBUGGING
    Serial.println("[LCD] About page");
  #endif
  CurrentPage = 3;
  tft.fillScreen(BACKGROUND_COLOR);
  btn_back.initButton(&tft, 20, 20, 40, 40, WHITE, CYAN, BLACK, (char*)"<<", 2);
  btn_back.drawButton();

  tft.setTextSize(2);
  tft.setTextWrap(true);

  uint16_t start_ps = tft.height() * 0.15;

  for(uint8_t i=0; i<Lines; i++) {
    tft.setCursor(10, start_ps + (20*i));
    tft.print(AboutText[i]);
  }
}


void readAboutPage() {
  bool pressed = isTouched();
  btn_back.press(pressed && btn_back.contains(p.x, p.y));
  if (btn_back.justPressed())
    btn_back.drawButton(true);
  else if (btn_back.justReleased()) {
    displayMainPage();
    return;
  }
}


uint8_t LogValue = 0;
/////////////////////////////////////  Log Page /////////////////////////////////////////
void displayLogPage() {
  CurrentPage = 4;
  tft.fillScreen(BACKGROUND_COLOR);
  tft.drawRect(10, 30, tft.width() - 20, tft.height() - 60, PURPLE);

  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.setCursor(110, 320 - 25);
  tft.print(LogValue);

  if(LogValue <= (LogId-1)) {
    tft.setCursor(20, 40);
    tft.print("Cycle: ");
    tft.print(ElapsedTime(LogCycle[LogValue].EndTime, LogCycle[LogValue].StartTime));
  
    tft.setCursor(20, 60);
    tft.print("Start: ");
    setTime(LogCycle[LogValue].StartTime);
    tft.print(month());
    tft.print("/");
    tft.print(day());
    tft.print("/");
    tft.print(year());
  
    tft.setCursor(20, 80);
    tft.print("End: ");
    setTime(LogCycle[LogValue].EndTime);
    tft.print(month());
    tft.print("/");
    tft.print(day());
    tft.print("/");
    tft.print(year());
  
    tft.setCursor(20, 100);
    tft.print("Symptoms:");
    uint16_t x = 0;
    for (uint8_t i = 0; i < 5; i++) {
      if (LogCycle[LogValue].symptoms[i] != "") {
        x += 20;
        tft.setCursor(30, 100 + x);
        tft.print(LogCycle[LogValue].symptoms[i]);
      }
    }
  }

  btn_next.initButton(&tft, tft.width() - 30, tft.height() - 15, 60, 30, WHITE, DARKGREY, BLACK, (char*)">", 1);
  btn_prev.initButton(&tft, 30,             tft.height() - 15, 60, 30, WHITE, DARKGREY, BLACK, (char*)"<", 1);
  btn_back.initButton(&tft, 20, 15, 40, 10, WHITE, CYAN, BLACK, (char*)"<<", 1);

  btn_next.drawButton();
  btn_prev.drawButton();
  btn_back.drawButton();

}


void readLogPage() {
  bool pressed = isTouched();
  btn_next.press(pressed && btn_next.contains(p.x, p.y));
  btn_prev.press(pressed && btn_prev.contains(p.x, p.y));
  btn_back.press(pressed && btn_back.contains(p.x, p.y));

  if (btn_next.justReleased()) {
    LogValue += 1;
    if (LogValue > 4 || LogValue > LogId)
      LogValue = 0;
    displayLogPage();
    return;
  }

  if (btn_prev.justReleased()) {
    LogValue -= 1;
    if (LogValue > 100)
      LogValue = 4;
    displayLogPage();
    return;
  }

  if (btn_back.justReleased()) {
    LogValue = 0;
    displayMainPage();
    return;
  }
}



/////////////////////////////////////  Main function /////////////////////////////////////////

void updateScreen() {
  if (CurrentPage == 1)
    readMainPage();
  else if (CurrentPage == 2)
    readSymptomsPage();
  else if (CurrentPage == 3)
    readAboutPage();
  else if (CurrentPage == 4)
    readLogPage();
}
