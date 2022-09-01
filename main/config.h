#define DEBUGGING         // comment this line to disable DEBUG messages
#define FLASH_DEBUG 0
#define BAUDRATE    115200


const uint8_t MAGIC_NUMBER = 100; // change this number between 100-200 to reset EEPROM or delete old data and reset RTC

// Touch screens globals

#define CAPACITIVE_SCREEN  // screen with capacitive touch

#define ORIENTATION 0   // 0-> potrait, 1-> landscape

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

#define TFT_DC 9
#define TFT_CS 10

uint8_t CurrentPage = 1;


// Color definitions
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xFC18

#define BACKGROUND_COLOR  WHITE
#define SELECTED_COLOR    YELLOW
#define DESELECTED_COLOR  GREEN


char *SYMPTOMS_NAMES[5] = {"Abdominal cramps", "Bloating", "Fatigue", "Tenderness", "Headache"};
const char *AboutText[] = {"Here you can", "add periods", "track predictions", "add symptoms", "and learn about", "your reproductive", "health"};
const uint8_t Lines = 7;

uint32_t LastInfoUpdate;

typedef enum {
  NONE,
  PERIOD_STARTED,
  PERIOD_ENDED,
}period_state_t;


uint8_t AveragePeriodDuration = 28;
uint8_t LogId = 0;
typedef struct {
  uint32_t StartTime; //4
  uint32_t EndTime;   //4
  uint32_t Next;      // do not need to save as it can be calculated based on AveragePeriodDuration and StartTime
  String symptoms[5] = {""};
}cycle_t;

typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint32_t unix;
}date_time_t;

const int ADDR_MAGIC_NUMBER = 0;
const int ADDR_PERIOD_STATE = 1;
const int ADDR_AVERAGE_DAYS = 2;
const int ADDR_LOGGED_COUNT = 3;
const int ADDR_START_TIME[5]  = {5,  9,  13, 17, 21};
const int ADDR_END_TIME[5]    = {25, 29, 33, 37, 41};
const int ADDR_SYMPTOMS[5][5] = {45, 61, 77, 93, 109,
                                 125, 141, 157, 173, 189,
                                 205, 221, 237, 253, 269,
                                 285, 301, 317, 333, 349,
                                 365, 381, 397, 413, 429
                                };


bool isRTC_Working = false;
