void readAllConfiguration() {
  AveragePeriodDuration = EEPROM.read(ADDR_AVERAGE_DAYS);
  LogId = EEPROM.read(ADDR_LOGGED_COUNT);
  if(LogId > 100)
    LogId = 0;
  if(AveragePeriodDuration > 100)
    AveragePeriodDuration = 28;
  readLogs();
}

bool checkResetEEPROM() {
  uint8_t number = EEPROM.read(ADDR_MAGIC_NUMBER);
  if(number != MAGIC_NUMBER) {
    for(uint8_t i=0; i<5; i++) {
      saveLong(ADDR_START_TIME[i], 0);
      saveLong(ADDR_END_TIME[i], 0);
      for(uint8_t x=0; x<5; x++)
        saveString(ADDR_SYMPTOMS[i][x], "");
    }
    EEPROM.write(ADDR_PERIOD_STATE, 0);
    EEPROM.write(ADDR_AVERAGE_DAYS, 28);
    EEPROM.write(ADDR_MAGIC_NUMBER, MAGIC_NUMBER);
    EEPROM.commit();
    return true;
  }
  return false;
}

void readLogs() {
  if(LogId == 0)
    return;
  for(uint8_t i=0; i<5; i++) {
    LogCycle[i].StartTime  = readLong(ADDR_START_TIME[i]);
    LogCycle[i].EndTime    = readLong(ADDR_END_TIME[i]);
    for(uint8_t x=0; x<5; x++)
      LogCycle[i].symptoms[x] = readString(ADDR_SYMPTOMS[i][x]);
  }
}


void saveString(int addr, const String &strToWrite) {
  byte len = strToWrite.length();
  for (int i = 0; i < len; i++){
    EEPROM.update(addr + i, strToWrite[i]);
  }
  EEPROM.commit();
}

String readString(int addr) {
  char read_data[16];
  for (int i = 0; i < 16; i++)
    read_data[i] = EEPROM.read(addr + i);
  return String(read_data);
}


uint32_t readLong(int addr) {
  long Byte4 = EEPROM.read(addr);
  long Byte3 = EEPROM.read(addr + 1);
  long Byte2 = EEPROM.read(addr + 2);
  long Byte1 = EEPROM.read(addr + 3);
  
  return ((Byte4 << 0) & 0xFF) + ((Byte3 << 8) & 0xFFFF) + ((Byte2 << 16) & 0xFFFFFF) + ((Byte1 << 24) & 0xFFFFFFFF);
}


void saveLong(int addr, uint32_t val) {
  byte Byte4 = ((val >> 0)  & 0xFF);
  byte Byte3 = ((val >> 8)  & 0xFF);
  byte Byte2 = ((val >> 16) & 0xFF);
  byte Byte1 = ((val >> 24) & 0xFF);
  
  EEPROM.write(addr,     Byte4);
  EEPROM.write(addr + 1, Byte3);
  EEPROM.write(addr + 2, Byte2);
  EEPROM.write(addr + 3, Byte1);
  EEPROM.commit();
}

void saveOneCycle(uint8_t i) {
  saveLong(ADDR_START_TIME[i], LogCycle[i].StartTime);
  saveLong(ADDR_END_TIME[i], LogCycle[i].EndTime);
  for(uint8_t x=0; x<5; x++)
    saveString(ADDR_SYMPTOMS[i][x], LogCycle[i].symptoms[x]);
  EEPROM.commit();
}


void saveAllCycles() {
  for(uint8_t i=0; i<5; i++) {
    saveLong(ADDR_START_TIME[i], LogCycle[i].StartTime);
    saveLong(ADDR_END_TIME[i], LogCycle[i].EndTime);
    for(uint8_t x=0; x<5; x++)
      saveString(ADDR_SYMPTOMS[i][x], LogCycle[i].symptoms[x]);
  }
  EEPROM.commit();
}
