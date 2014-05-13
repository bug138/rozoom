///////////////////////////////////////////////////////////////////
//      L I G H T
///////////////////////////////////////////////////////////////////

// define pin for sector's
#define  LB_PIN0    2
#define  LB_PIN1    3
#define  LB_PIN2    4
#define  LB_PIN3    5
#define  LB_PIN4    6
#define  LB_PIN5    47
#define  LB_PIN6    8
#define  LB_PIN_ALL 13

///////////////////////////////////////////////////////////////////
LIGHT_BOARD LBoard;
volatile int FadeStep = 1;
volatile int FadeFrq=1000;

///////////////////////////////////////////////////////////////////
void lboard_init_pin(void)
{
  LBoard.Sector.pin[0]=LB_PIN0;
  LBoard.Sector.pin[1]=LB_PIN1;
  LBoard.Sector.pin[2]=LB_PIN2;
  LBoard.Sector.pin[3]=LB_PIN3;
  LBoard.Sector.pin[4]=LB_PIN4;
  LBoard.Sector.pin[5]=LB_PIN5;
  LBoard.Sector.pin[6]=LB_PIN6;
  LBoard.All.pin=LB_PIN_ALL;

  for(byte i=0; i<MAX_SECTORS; i++)
  {
    pinMode(LBoard.Sector.pin[i], OUTPUT);
    digitalWrite(LBoard.Sector.pin[i], HIGH);
  }
}

///////////////////////////////////////////////////////////////////
void lboard_init_sectors(LIGHT_BOARD *lbrd, unsigned long brd_max)
{
  unsigned long OneSector = brd_max / MAX_SECTORS;
  unsigned long Limit = OneSector;
  
  DBG("One Sector = "+String(OneSector));
  
  lbrd->Sector.limit[0] = 0;
  for( byte i=0; i<MAX_SECTORS-1; i++)
  {
    lbrd->Sector.limit[i] = Limit;
    DBG("limit["+String(i)+"] = "+String(lbrd->Sector.limit[i]));
    Limit += OneSector;
  }
}

///////////////////////////////////////////////////////////////////
void lboard_SetLight(unsigned long position)
{
  static byte old_idx = 100;

  if( position < LBoard.Sector.limit[0] )
  {
    LBoard.Sector.idx=0;
    LBoard.All.freq=2000;
  }
  else 
  if( position < LBoard.Sector.limit[1] )
  {
    LBoard.Sector.idx=1;
    LBoard.All.freq=3000;
  }
  else 
  if( position < LBoard.Sector.limit[2] )
  {
    LBoard.Sector.idx=2;
    LBoard.All.freq=6000;
  }
  else 
  if( position < LBoard.Sector.limit[3] )
  {
    LBoard.Sector.idx=3;
    LBoard.All.freq=9000;
  }
  else 
  if( position < LBoard.Sector.limit[4] )
  {
    LBoard.Sector.idx=4;
    LBoard.All.freq=6000;
  }
  else 
  if( position < LBoard.Sector.limit[5] )
  {
    LBoard.Sector.idx=5;
    LBoard.All.freq=3000;
  }
  else
  {
    LBoard.Sector.idx=6;
    LBoard.All.freq=2000;
  }

  if( old_idx != LBoard.Sector.idx )
  {
    // нашли сектор, включаем/переключаем подсветку
    lboard_TurnOn(LBoard.Sector.idx);
    Timer1.initialize(LBoard.All.freq); // set a timer 1 sec in microseconds
  }
}

///////////////////////////////////////////////////////////////////
void lboard_TurnOn(byte idx)
{
  // All light OFF
  for(byte i=0; i<MAX_SECTORS; i++) digitalWrite(LBoard.Sector.pin[i], HIGH);
  // Turn On
  digitalWrite(LBoard.Sector.pin[idx], LOW);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void IsrLightAllBoard(void)
{
  static int fadeValue = 0;
  static int fadeDir = LIGHT_ALL_BOARD_DIR_UP;

  if( LBoard.All.enabled )
  {
    if( fadeDir == LIGHT_ALL_BOARD_DIR_UP )
    {
      if( fadeValue > 254 )
      {
        fadeDir = LIGHT_ALL_BOARD_DIR_DOWN;
        fadeValue -= FadeStep;
      }
      else fadeValue += FadeStep;
    }
    else
    {
      if( fadeValue < 1 )
      {
        fadeDir = LIGHT_ALL_BOARD_DIR_UP;
        fadeValue += FadeStep;
      }
      else fadeValue -= FadeStep;
    }
  }
  else
  {
    fadeValue = 255;
  }

  analogWrite(LBoard.All.pin, fadeValue);
}

