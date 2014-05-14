#include <arduino.h>
#include <memorysaver.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EEPROM.h>
#include <UTFT.h>
#include <UTouch.h>
#include <UTouchCD.h>
#include <UTFT_Buttons.h>
#include <TM1637.h>
#include <toneAC.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include "mindwave.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
char strBuf[32];
UTFT myGLCD(ITDB32S,38,39,40,41);
UTouch myTouch(46,45,44,43,42);
UTFT_Buttons  myButtons(&myGLCD,&myTouch);

TMODE CurrentMode = ModeMainMenu;
GAME Game;
bool GoToPowerOff = false;
bool HS_IsReady = false;
bool DoStartGame = false;
bool FirstRun = true;
bool GamePause = false;
bool GameTimeOut = false;

/////////////////////////////////////////////////////////////////////////////////////
// Timer
char EVT_TIME_IN_IDLE_1S=false;
unsigned int time_1s;
unsigned long current_ms,old_ms=0;
ConvertWordToByte WordToByte;

/////////////////////////////////////////////////////////////////////////////////////
// additional 7segment display
TM1637 display1(CLK1,DIO1);
TM1637 display2(CLK2,DIO2);

#include "sound.h"
#include "light.h"
#include "gui.h"
#include "settings.h"
#include "headset.h"
#include "sensor.h"
#include "moto.h"
#include "game.h"

/****************************************************************/
volatile char EVT_MAIN_1S = false;
volatile char EVT_1S = false;
volatile char in_idle=false;
void idle(void)
{
  if(in_idle)
  {
    DBG("Recursive call of idle");
    return;
  }
  in_idle=true;

  static char CurrentHS = 0; 

  // check Headset  
  CheckHeadSet();

  ///////////////////////////////////////////////////////////////
  // time event
  if( (millis()-old_ms) > 999 )
  {
    old_ms = millis();
    EVT_TIME_IN_IDLE_1S=true;
    EVT_1S=true;
  }
  
  // events every second
  if( EVT_TIME_IN_IDLE_1S )
  {
    // HeadSet ----------------------
    if(CurrentMode==ModeWaitHeadSet )
    {
      StartHS(1);
      StartHS(2);
    }
    else if(CurrentMode==ModePlay)
    {
      
    }
    else StartHS(0);
    // ------------------------------
    
    if(Game.CountDown && !GamePause)
    {
      if(Game.CountDownEnable)
      {
        EVENT("&evt:Game.CountDown="+String(Game.CountDown));
        myGLCD.setFont(SevenSegNumFont);
        myGLCD.print(":"+String(Game.CountDown)+":",CENTER,0);
        Game.CountDown--;
        if(Game.CountDown==0) GameTimeOut=true;
      }

      GameCountDownPause++;
      if( GameCountDownPause > GameCountDownPauseMAX )
      {
        Game.CountDownEnable = false;
        MotoSetPause(true);
      }
    }

    EVT_TIME_IN_IDLE_1S = false;
  }

  in_idle=false;
}

/****************************************************************/
void loop()  //                 M A I N
/****************************************************************/
{
  idle();

  // -------------------------------------------------------------------------------------
  if( TouchPressed() )
  { 
    pressed_button = myButtons.checkButtons();
  
    switch(CurrentMode)
    {
      // --- MENU ---
      case ModeMainMenu:{
          if(pressed_button==btn_options)
          {
            ShowOptions();
            CurrentMode = ModeOptions;
          }
          else if(pressed_button==btn_play)
          {
            ShowWaitCalibrate();
            bool res;
            if(FirstRun) res=MotoDoHalfCalibrate();
            else res=MotoMoveToCenter();
            if(res)
            {
              if(FirstRun) FirstRun=false;
              ShowPlayMode();
              CurrentMode=ModeSelectPlayMode;
            }
            else
            {
              ShowMainMenu();
              CurrentMode = ModeMainMenu;
            }
          }
          else if(pressed_button==btn_sound)
          {
            if(sound) sound=false;
            else sound=true;
            char str_sound[10]; memset(str_sound,0,10);
            sprintf(str_sound,"SOUND %s",sound?"ON":"OFF");
            myButtons.relabelButton(btn_sound, str_sound, true);            
            SaveSettings();
          }
          else if(pressed_button==btn_poweroff)
          {
            GoToPowerOff=true;
          }
      }
      break;  
  
      // --- OPTIONS ---
      case ModeOptions:{
          if(pressed_button==btn_edit_att_threshold)
          {
            Game.AttentionThreshold = EditUint(Game.AttentionThreshold, 0, 99, "Attention Threshold");
            SaveSettings();
            CurrentMode = ModeOptions;
            ShowOptions();
          }
          else if(pressed_button==btn_edit_med_threshold)
          {
            Game.MeditationThreshold = EditUint(Game.MeditationThreshold, 0, 99, "Meditation Threshold");
            SaveSettings();
            CurrentMode = ModeOptions;
            ShowOptions();
          }
          else if(pressed_button==btn_calibrate)
          {
            ShowCalibrate();
            CurrentMode=ModeCalibrate;
          }
          else if(pressed_button==btn_cancel)
          {
            ShowMainMenu();
            CurrentMode = ModeMainMenu;
          }
      }
      break;

      // --- Calibrate ---
      case ModeCalibrate:{
          if(pressed_button==btn_calibrate_full)
          {
            ShowWaitCalibrate();
            if(MotoDoCalibrate()) SaveSettings();
            CurrentMode = ModeOptions;
            ShowOptions();
          }
          else if(pressed_button==btn_calibrate_half)
          {
            ShowWaitCalibrate();
            if(MotoDoHalfCalibrate()) SaveSettings();
            CurrentMode = ModeOptions;
            ShowOptions();
          }
          else if(pressed_button==btn_calibrate_center)
          {
            ShowWaitCalibrate();
            if(MotoMoveToCenter()) SaveSettings();
            CurrentMode = ModeOptions;
            ShowOptions();
          }
      }
      break;
        
      // --- SELECT PLAY MODE ---
      case ModeSelectPlayMode:
      {
          if(pressed_button==btn_play_normal)
          {
            DBG("pressed normal");
            Game.GameMode = GameModeNormal;
            CurrentMode = ModeSelectPlaySet;
            ShowGameSet();
          }
          else if(pressed_button==btn_play_inverse)
          {
            DBG("pressed inverse");
            Game.GameMode = GameModeInverse;
            CurrentMode = ModeSelectPlaySet;
            ShowGameSet();
          }
      }
      break;

      // --- SELECT PLAY SET ---
      case ModeSelectPlaySet:
      {
          if(pressed_button==btn_play_att)
          {
            DBG("select att");
            Game.GameSet = GameSet_Att;
            CurrentMode = ModeSelectPlayTime;
          }
          else if(pressed_button==btn_play_med)
          {
            DBG("select med");
            Game.GameSet = GameSet_Med;
            CurrentMode = ModeSelectPlayTime;
          }
          else if(pressed_button==btn_play_att_med)
          {
            DBG("select att+med");
            Game.GameSet = GameSet_AttMed;
            CurrentMode = ModeSelectPlayTime;
          }
      }
      break;

      // --- Wait HeadSET ---
      case ModeWaitHeadSet:
      {
        if(pressed_button==btn_cancel)
        {
          EVENT("&evt:play_cancel");
          CurrentMode = ModeMainMenu;
          ShowMainMenu();
        }
        else if(pressed_button==btn_play && HS_IsReady)
        {
          DoStartGame=true;
        }
      }
      break;
      
      // --- PLAY ---
      case ModePlay:{
        if(pressed_button==btn_stop)
        {
          StopGameProcess();
          EVENT("&evt:play_cancel");
          CurrentMode = ModeMainMenu;
          ShowMainMenu();
        }
        else
        if(pressed_button==btn_pause)
        {
          EVENT("&evt:play_pause");
          GamePause = true;
          MotoRun=false;
        }
        else
        if(pressed_button==btn_resume)
        {
          EVENT("&evt:play_resume");
          GamePause = false;
          MotoSpeed.SetFrom=MOTO_MIN_SPEED;
          MotoSpeed.SetTo=MOTO_MIN_SPEED;
          MotoSetSpeed();
          MotoRun=true;
       }
        
      }
      break;
  
      // --- Show Winner ---
      case ModeShowWinner:{
          if(pressed_button==btn_ok)
          {
            //SendStopGame();
            CurrentMode = ModeMainMenu;
            ShowMainMenu();
          }
        }
        break;
     
      default:
        break;
    }
  }
  // end menu


  ///////////////////////////////////////////////////////////
  // --- SELECT PLAY TIME ---
  if(CurrentMode==ModeSelectPlayTime)
  {
      unsigned int old_Game_PlayTime = Game.PlayTime;
      Game.PlayTime = EditPlayTime(Game.PlayTime/60)*60;
      if((Game.PlayTime!=old_Game_PlayTime)&&(Game.PlayTime>0)) SaveSettings();

      if(Game.PlayTime==0)  // Cancel
      {
        CurrentMode = ModeMainMenu;
        ShowMainMenu();
      }
      else
      {
        memset(Game.Player[_P1_].displayBuf,' ',4);
        memset(Game.Player[_P2_].displayBuf,' ',4);
        Game.Player[_P1_].display->display( Game.Player[_P1_].displayBuf );
        Game.Player[_P2_].display->display( Game.Player[_P2_].displayBuf );
        Game.Player[_P1_].display->point(POINT_ON);
        Game.Player[_P2_].display->point(POINT_ON);

        CurrentMode = ModeWaitHeadSet;
        HS_IsReady = false;
        ShowWaitHS();
      }
  }

  ///////////////////////////////////////////////////////////
  // WaitHeadSet is ready
  if(CurrentMode == ModeWaitHeadSet)
  {
    bool data_ready = false;
    
    if(Game.GameSet==GameSet_Att)
    {
      #ifdef FUNCTION_ONLY_ONE_HEADSET
      if( Game.Player[_P1_].att.ready || Game.Player[_P2_].att.ready ) // 1 player is ready
      #else
      if( Game.Player[_P1_].att.ready && Game.Player[_P2_].att.ready ) // 2 players is ready
      #endif
      data_ready=true;
    }
    else if(Game.GameSet==GameSet_Med)
    {
      #ifdef FUNCTION_ONLY_ONE_HEADSET
      if( Game.Player[_P1_].med.ready || Game.Player[_P2_].med.ready ) // 1 player is ready
      #else
      if( Game.Player[_P1_].med.ready && Game.Player[_P2_].med.ready ) // 2 players is ready
      #endif
      data_ready=true;
    }
    else
    {
      // GameSet_AttMed
      #ifdef FUNCTION_ONLY_ONE_HEADSET
      if( Game.Player[_P1_].att.ready || Game.Player[_P2_].att.ready || Game.Player[_P1_].med.ready || Game.Player[_P2_].med.ready ) // 1 player is ready
      #else
      if( Game.Player[_P1_].att.ready && Game.Player[_P2_].att.ready && Game.Player[_P1_].med.ready && Game.Player[_P2_].med.ready ) // 2 players is ready
      #endif
      data_ready=true;
    }
    
    if(data_ready)
    {
      if(!HS_IsReady)
      {
        myButtons.enableButton(btn_ok);
        myButtons.relabelButton(btn_ok, "  START  ", true);
        print7seg(_P1_,0,false);
        print7seg(_P2_,0,false);
      }
      HS_IsReady = true;
    }
  }

  if(DoStartGame)
  {
      // Start Play Game
      DoStartGame=false;
      GameTimeOut=false;
      GamePause=false;
      init_variable(&Game.Player[_P1_]);
      init_variable(&Game.Player[_P2_]);
      CurrentMode=ModePlay;
      Game.CountDown=Game.PlayTime;
      Game.Winner=0;
      Game.CountDownEnable=true;
      MotoSpeed.SetFrom=MOTO_START_SPEED;
      ShowGame();
      EVENT("&evt:play_start,time="+String(Game.PlayTime)+",mode="+String(Game.GameMode)+",set="+String(Game.GameSet));
      Timer3.initialize(MOTO_MIN_SPEED);
      LBoard.All.enabled = true;
      MotoRun=true;
  }

  ///////////////////////////////////////////////////////////
  //////// G A M E //////////////////////////////////////////
  if(CurrentMode==ModePlay)
  {
    GameProcess();
  }

  ///////////////////////////////////////////////////////////
  if(GoToPowerOff)
  {
    GoToPowerOff=false;
    Music_Play_Tetris();
    if(ShowConfirm())
    {
      ClearScreen();
      myGLCD.print("POWER OFF", CENTER, 110);
      myGLCD.print("please Power Off", CENTER, 220);
      EVENT("&evt:power_off");
      SaveSettings();
      while(1);
    }
    else
    {
      CurrentMode = ModeMainMenu;
      ShowMainMenu();
    }
  }
  
} // end loop()


/****************************************************************/
void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  Serial3.print("\r\nSerial 3\r\n");
  init_variable(&Game.Player[_P1_]);
  init_variable(&Game.Player[_P2_]);

  lboard_init_pin();

  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);

  myButtons.setTextFont(BigFont);
  old_ms=current_ms=millis();
  
  // 7seg display config
  Game.Player[_P1_].display = &display1;
  Game.Player[_P2_].display = &display2;
  
  Game.Player[_P1_].display->set(BRIGHTEST);
  Game.Player[_P1_].display->init();
  
  Game.Player[_P2_].display->set(BRIGHTEST);
  Game.Player[_P2_].display->init();

  print7seg(_P1_,'-',true);
  print7seg(_P2_,'-',true);
  
  // MOTO cfg
  MotoConfig();

  // LightAllBoard
  LBoard.All.enabled = false;
  LBoard.All.freq=500000;
  Timer1.initialize(LBoard.All.freq); // set a timer 1sec in microseconds
  Timer1.attachInterrupt(IsrLightAllBoard); // attach the service routine here

  // Sens cfg
  SensConfigPins();
  
  LoadSettings();
  ShowMainMenu();

  EVENT("&evt:power_on,v1.1");
  
  HW_Beep(3000,100);
  
  Game.GameSet=GameSet_None;
  Game.CountDownOld=0;
}

/*
* ------------------------------------------------------------------------------------------------------------------------------------------------
*/
void init_variable(PLAYER *var)
{
  for(byte i=0; i<MAX_MEASURE; i++)
  {
    var->att.data[i]=0;
    var->med.data[i]=0;
  }
  
  var->att.current_pos = 0;
  var->med.current_pos = 0;
  var->pkt.cnt=0;
  var->ready=false;
}



