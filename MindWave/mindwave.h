#ifndef	__mindwave_h__
#define __mindwave_h__

#define imagedatatype  unsigned int

#include <TM1637.h>

// Отладочные сообщения и функции
//#define FUNCTION_DEBUG
#define FUNCTION_DEBUG2UART3  // вывод на UART3
//#define FUNCTION_ONLY_ONE_HEADSET
//#define FUNCTION_MOTO_SILENT


#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000


/////////////////////////////////////////////////////////////////////////////////////////////////////////
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

#define imagedatatype  unsigned int

/////////////////////////////////////////////////////////////////////////////////////
// additional 7segment display
#define CLK1 48
#define DIO1 49
#define CLK2 50
#define DIO2 51

/////////////////////////////////
#define	COLOR_WHITE  255,255,255


#ifdef  FUNCTION_DEBUG
  #ifdef FUNCTION_DEBUG2UART3
    #define	DBG(X)    Serial3.println(X)
  #else
    #define	DBG(X)    Serial.println(X)
  #endif
#else
  #define	DBG(X)
#endif

#ifdef FUNCTION_DEBUG2UART3
  #define	EVENT(X)  Serial3.println(X)
#else
  #define	EVENT(X)  Serial.println(X)
#endif

typedef union
{
    uint16_t word;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    } byte;
}ConvertWordToByte;
extern ConvertWordToByte WordToByte;


typedef enum {
	ModeMainMenu,
        ModeSelectPlayMode,
        ModeSelectPlaySet,
        ModeSelectPlayTime,
        ModeWaitHeadSet,
        ModePlay,
        ModeOptions,
        ModeShowResult,
        ModeShowWinner,
        ModeCalibrate
} TMODE;

#define GameModeUnknow  0
#define GameModeInverse 1
#define GameModeNormal  2

#define GameSet_None    0
#define GameSet_Att     1
#define GameSet_Med     2
#define GameSet_AttMed  3

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#define MAX_MEASURE  1
#define MAX_PKT_SIZE 200

#define _P1_  0
#define _P2_  1

typedef struct{
      unsigned char ready;
      unsigned char size;
      unsigned char cnt;
      unsigned char pkt_cnt;
      unsigned char generatedChecksum;
      unsigned char data[MAX_PKT_SIZE];
}PKT;

typedef struct {
      unsigned char ready;
      unsigned char average;
      unsigned char current_pos;
      unsigned char data[MAX_MEASURE];
}DAT;

typedef struct {
  unsigned char ready;
  unsigned char quality;
  PKT  pkt;
  DAT  att;  // attention
  DAT  med;  // meditation
  TM1637 *display; // additional 7segment display
  int8_t displayBuf[4];
}PLAYER;

typedef struct {
  PLAYER Player[2];
  unsigned char GameMode;     // Normal/Inverse
  unsigned char GameSet;      // Att/Med/Att+Med
  unsigned int PlayTime;      // sec
  bool CountDownEnable;
  unsigned int CountDown;     // sec
  unsigned int CountDownOld;  // sec
  unsigned char Winner;       // Winner Player
  unsigned char AttentionThreshold;
  unsigned char MeditationThreshold;
}GAME;
extern GAME Game;


///////////////////////////////////////////////////////////////////
//      M O T O
///////////////////////////////////////////////////////////////////
#define MOTO_DIR_1              0
#define MOTO_DIR_2              1
#define MOTO_DIR_0              0x80
#define MOTO_START_SPEED        1500
#define MOTO_MIN_SPEED          MOTO_START_SPEED
#ifdef FUNCTION_MOTO_SILENT
  #define MOTO_MAX_SPEED          800
  #define MOTO_CALIBRATE_SPEED    800
#else
  #define MOTO_MAX_SPEED          115
  #define MOTO_CALIBRATE_SPEED    115
#endif
#define SPEED_K                 20000

typedef struct {
  unsigned int SetFrom;
  unsigned int SetTo;
  unsigned int Current;
  unsigned int Target;
}MOTO_SPEED;
extern volatile MOTO_SPEED MotoSpeed;

extern int _pin_MotoStep;
extern int _pin_MotoDir;
extern int _pin_MotoRst;
extern int _pin_MotoAlarm;
extern volatile byte MotoDirection;
extern volatile byte old_moto_dir;
extern volatile bool MotoRun;
extern volatile unsigned long MotoCalibrate;
extern volatile unsigned long MotoPos;
extern volatile unsigned long NewCalibrate;
extern volatile unsigned long NewHalfCalibrate;
extern volatile bool MotoPause;
extern volatile bool DoCalibrate;
extern volatile bool MotoCalibrateOK;



///////////////////////////////////////////////////////////////////
//      SENSOR'S
#define SENS_NONE  0
#define SENS_1     1
#define SENS_2     2
extern int _pin_Sens1;
extern int _pin_Sens2;


/////////////////////////////////////////////////////////////////////////////////////
// Sound

extern bool sound_mute;

#define PlayNote  HW_Beep
#define MUSIC_NOTE_C	262//261,63
#define MUSIC_NOTE_Db	277//277,18
#define MUSIC_NOTE_D	294//293,67
#define MUSIC_NOTE_Eb 	311//311,13
#define MUSIC_NOTE_E	330//329,63
#define MUSIC_NOTE_F	349//349,22
#define MUSIC_NOTE_Gb	370//369,99
#define MUSIC_NOTE_G	392//391,99
#define MUSIC_NOTE_Ab	415//415,30
#define MUSIC_NOTE_A	440//440,00
#define MUSIC_NOTE_Bb	466//466,16
#define MUSIC_NOTE_B	494//493.88

///////////////////////////////////////////////////////////////////////////////////
// Buttons
extern int pressed_button,
    btn_play,btn_options,btn_sound,btn_poweroff,
    btn_play_normal,btn_play_inverse,
    btn_up,btn_down,
    btn_ok,btn_cancel,
    btn_stop,btn_pause,btn_resume,
    btn_edit_att_threshold,btn_edit_med_threshold,
    btn_time_1,btn_time_3,btn_time_5,btn_time_10,btn_time_15,btn_time_20,btn_time_30,btn_time_40,btn_time_50,btn_time_60,btn_time_70,btn_time_80,btn_time_90,btn_time_99,
    btn_play_att, btn_play_med, btn_play_att_med,
    btn_calibrate,btn_calibrate_full,btn_calibrate_half,btn_calibrate_center;


extern volatile byte GameCountDownPause;
#define GameCountDownPauseMAX  2    // через сколько секунд при пропадании данных от игрока вставать на паузу

inline void StopGameProcess(void);
inline signed int calc_shift(int mode,int a1,int a2,int m1,int m2);
inline char calc_timer_speed(int game_type, signed int shift, unsigned int *speed, char *dir);
inline void move_process(void);
void print7seg(char PlayerNum, byte Number, bool Dot);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_SECTORS  7
typedef struct {
  byte idx;                            // текущий сектор
  int pin[MAX_SECTORS];              // 
  unsigned long limit[MAX_SECTORS-1];  // границы секторов
}LIGHT_SECTOR;

typedef struct {
  bool enabled;
  int pin;        // нога отвечает за подсветку всей игровой поверхности
  int freq;       // частота мигания подсветки
}LIGHT_ALL_BOARD;

typedef struct {
  LIGHT_SECTOR     Sector;
  LIGHT_ALL_BOARD  All;
}LIGHT_BOARD;
extern LIGHT_BOARD LBoard;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LIGHT_ALL_BOARD_DIR_DOWN    0
#define LIGHT_ALL_BOARD_DIR_UP      1

extern volatile int FadeStep;

void lboard_init_pin(void);
void lboard_init_sectors(LIGHT_BOARD *lbrd, unsigned long brd_max);
void lboard_SetLight(unsigned long position);
void lboard_TurnOn(byte idx);
void IsrLightAllBoard(void);


#endif /*__mindwave_h__*/


