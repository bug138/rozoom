////////////////////////////////////////////////////////////////////////////////////
// G A M E  Process
////////////////////////////////////////////////////////////////////////////////////

volatile byte GameCountDownPause=0;


inline void StopGameProcess(void)
{
    Game.CountDownEnable=false;
    MotoRun=false;
    LBoard.All.enabled = false;
    Game.CountDown=0;
}

void GameProcess(void)
{
  if(GamePause) return;
  
  bool data_ready=false;
  byte SensState = GetSensState();
  
  // Окончание игры по концу поля
  if(SensState!=SENS_NONE)
  {
    StopGameProcess();
    DBG("\nSensState="+String(SensState));
    Game.Winner=SensState;
    CurrentMode = ModeShowWinner;
    
    if(Game.GameMode==GameModeNormal)
    {
      if(Game.Winner==SENS_1) Game.Winner=SENS_2;
      else Game.Winner=SENS_1;
    }

    EVENT("&evt:game_end, win player "+String(Game.Winner));
    ShowWinner();
    return;
  }
  // время кончилось
  else
  if(GameTimeOut)
  {
    // двигаем каретку до конца поля
    if(MotoPos<NewHalfCalibrate) Game.Winner=SENS_1;
    else Game.Winner=SENS_2;
    MotoDir(Game.Winner-1);
    MotoSpeed.SetTo=MOTO_MAX_SPEED;
    MotoSpeed.SetFrom=MOTO_MIN_SPEED;
    MotoSetSpeed();
    Game.CountDown=1;

    ClearScreen();
    myGLCD.print("Please wait...", CENTER, 80);
    while( GetSensState() != Game.Winner );
    Game.CountDown=0;

    if(Game.GameMode==GameModeNormal)
    {
      if(Game.Winner==SENS_1) Game.Winner=SENS_2;
      else Game.Winner=SENS_1;
    }

    StopGameProcess();
    CurrentMode = ModeShowWinner;
    EVENT("&evt:game_end, win player "+String(Game.Winner));
    ShowWinner();
    return;
  }

  // игра
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
    #ifdef FUNCTION_DEBUG
      myGLCD.setFont(BigFont);
      memset(strBuf,0,32);
      sprintf(strBuf,"Q1:%d  ",Game.Player[_P1_].quality);
      myGLCD.print(strBuf, 0, 70);

      memset(strBuf,0,32);
      sprintf(strBuf,"A1:%d  ",Game.Player[_P1_].att.average);
      myGLCD.print(strBuf, 0, 90);
      
      memset(strBuf,0,32);
      sprintf(strBuf,"M1:%d  ",Game.Player[_P1_].med.average);
      myGLCD.print(strBuf, 0, 110);

      myGLCD.setFont(BigFont);
      memset(strBuf,0,32);
      sprintf(strBuf,"Q2:%d  ",Game.Player[_P2_].quality);
      myGLCD.print(strBuf, 160, 70);
    
      memset(strBuf,0,32);
      sprintf(strBuf,"A2:%d  ",Game.Player[_P2_].att.average);
      myGLCD.print(strBuf, 160, 90);

      memset(strBuf,0,32);
      sprintf(strBuf,"M2:%d  ",Game.Player[_P2_].med.average);
      myGLCD.print(strBuf, 160, 110);
    #endif

    GameCountDownPause = 0;
    Game.CountDownEnable = true;
    Game.Player[_P1_].ready = false;
    Game.Player[_P2_].ready = false;
    Game.Player[_P1_].att.ready = false;
    Game.Player[_P1_].med.ready = false;
    Game.Player[_P2_].att.ready = false;
    Game.Player[_P2_].med.ready = false;
    move_process();
  }
}


/*****************************************************************/
signed int calc_shift(int mode,int a1,int a2,int m1,int m2)
{
  if( mode == GameSet_Att )
  {
    print7seg(_P1_,a1,false);
    print7seg(_P2_,a2,false);
    return (a1-a2);
  }
  else
  if( mode == GameSet_Med )
  {
    print7seg(_P1_,m1,false);
    print7seg(_P2_,m2,false);
    return (m1-m2);
  }
  else
  if( mode == GameSet_AttMed )
  {
    int _r1 = (a1+m1)/2;
    int _r2 = (a2+m2)/2; 
    print7seg(_P1_,(byte)(_r1),false);
    print7seg(_P2_,(byte)(_r2),false);
    return (_r1-_r2);
  }
  return 0;
}


/*****************************************************************/
inline char calc_timer_speed(int game_type, signed int shift, unsigned int *speed, char *dir)
{
	if(shift==0)
	{
		*speed = 0;
		return false;
	}

	*speed = (int)(SPEED_K/abs(shift));

	if( game_type == GameModeNormal )        // Normal
	{
		if( shift<0 ) *dir=MOTO_DIR_1;
		else *dir=MOTO_DIR_2;
	}
	else if( game_type == GameModeInverse )  // Inverse
	{
		if( shift<0 ) *dir=MOTO_DIR_2;
		else *dir=MOTO_DIR_1;
	}
	else				         // Unknow
	{
                *speed = 0;
		return false;
	}

	return true;
}

/*****************************************************************/
inline void move_process(void)
{
  unsigned int speed=0;
  char dir;
  signed int shift = calc_shift(Game.GameSet,
                                Game.Player[_P1_].att.average,
                                Game.Player[_P2_].att.average,
                                Game.Player[_P1_].med.average,
                                Game.Player[_P2_].med.average);
  
  if( calc_timer_speed(Game.GameMode, shift, &speed, &dir) )
  {
    if( speed > MOTO_MIN_SPEED ) speed = MOTO_MIN_SPEED;
    if( speed < MOTO_MAX_SPEED ) speed = MOTO_MAX_SPEED;
    
    MotoSpeed.SetTo=speed;
    MotoSpeed.SetFrom=MotoSpeed.Current;
    MotoSetSpeed();
    MotoDir(dir);
    MotoSetPause(false);
  }
  else
  {
    // error
    MotoSetPause(true);
  }
}




