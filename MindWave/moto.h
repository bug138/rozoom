///////////////////////////////////////////////////////////////////
//      M O T O
///////////////////////////////////////////////////////////////////

int _pin_MotoStep = 10;
int _pin_MotoDir = 9;
int _pin_MotoRst = 8;
int _pin_MotoAlarm = 7;
volatile byte MotoDirection=MOTO_DIR_0;     // направление движения картеки >0 - от 1 к 2, <0 - от 2 к 1, =0 - запрещено движение
volatile byte old_moto_dir = 100;
volatile MOTO_SPEED MotoSpeed;
volatile bool MotoRun=false;                // разрешение движения двигателем
volatile unsigned long MotoCalibrate=0;     // положение каретки при калибровке
volatile unsigned long MotoPos=0;           // текущее положение каретки
volatile unsigned long NewCalibrate=0;      // максимальное положение каретки
volatile unsigned long NewHalfCalibrate=0;  // положение картеки по центру
volatile bool MotoPause = false;
volatile bool DoCalibrate = false;
volatile bool MotoCalibrateOK = false;

///////////////////////////////////////////////////////////////////
void MotoConfig()
{
  pinMode(_pin_MotoStep, OUTPUT);  
  pinMode(_pin_MotoDir,  OUTPUT);  
  pinMode(_pin_MotoRst,  OUTPUT);  
  pinMode(_pin_MotoAlarm, INPUT);  
  digitalWrite(_pin_MotoStep, HIGH);
  digitalWrite(_pin_MotoDir, HIGH);
  digitalWrite(_pin_MotoRst, HIGH);
  
  MotoSpeed.Current=MOTO_START_SPEED;
  MotoSpeed.Target=MOTO_START_SPEED;
  Timer3.initialize(MOTO_MIN_SPEED);
  void MotoISR(void);
  Timer3.attachInterrupt(MotoISR);
}

///////////////////////////////////////////////////////////////////
void MotoSetSpeed()
{
    MotoSpeed.Target = MotoSpeed.SetTo;
    MotoSpeed.Current = MotoSpeed.SetFrom;
    Timer3.initialize(MotoSpeed.Current);
}

///////////////////////////////////////////////////////////////////
void MotoSetPause(bool value)
{
  static bool OldMotoPause = true;
  
  if(value==false)
  {
    if( OldMotoPause != value )
    {
      MotoSpeed.SetFrom = MOTO_START_SPEED;
      MotoSetSpeed();
    }
  }
  OldMotoPause = MotoPause;
  MotoPause = value;
}

///////////////////////////////////////////////////////////////////
static void MotoDirReset(void)
{
  old_moto_dir = 100;
}

///////////////////////////////////////////////////////////////////
void MotoDir(byte moto_dir)
{
  if( old_moto_dir != moto_dir )
  {
    MotoSetPause(true);
    MotoRun=false;
    old_moto_dir = moto_dir;
    MotoDirection = moto_dir;
    DBG("MotoDirection = "+String(MotoDirection+1));
    digitalWrite(_pin_MotoDir, moto_dir);
    delayMicroseconds(5);
    MotoSetPause(false);
    MotoRun=true;
  }
}

///////////////////////////////////////////////////////////////////
static void MotoStep(void)
{
  digitalWrite(_pin_MotoStep, 0);
  delayMicroseconds(5);
  digitalWrite(_pin_MotoStep, 1);
}

///////////////////////////////////////////////////////////////////
void MotoISR(void)
{
  static char MotoState = LOW;
  if( MotoRun && ((Game.CountDown && !MotoPause) || (DoCalibrate)) )
  {
    MotoStep();

    if(MotoDirection==MOTO_DIR_1) MotoPos--;
    else MotoPos++;

    if(!DoCalibrate && MotoCalibrateOK )
    {
      lboard_SetLight(MotoPos);
    }

    MotoCalibrate++;
  }
  
  if(MotoSpeed.Current>MotoSpeed.Target)
  {
    MotoSpeed.Current--;
    Timer3.initialize(MotoSpeed.Current);
  }

  if(MotoSpeed.Current<MotoSpeed.Target)
  {
    MotoSpeed.Current++;
    Timer3.initialize(MotoSpeed.Current);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MotoDoCalibrate(void)
{
  char WaitSens;
  byte Sens = GetSensState();
  DBG("SENS: "+String(Sens));
    
  MotoDirReset();

  MotoSetPause(true);
  MotoRun=true;
  DoCalibrate = true;

  MotoSpeed.SetFrom=MOTO_MIN_SPEED;
  MotoSpeed.SetTo=MOTO_CALIBRATE_SPEED;
  MotoSetSpeed();
  
  if(Sens==SENS_1)
  {  
    // Move to end
    WaitSens=SENS_2;
    MotoDir(MOTO_DIR_2);
  }
  else if(Sens==SENS_2)
  {
    // Move to start
    WaitSens=SENS_1;
    MotoDir(MOTO_DIR_1);
  }
  else
  {
    // Move to start
    MotoDir(MOTO_DIR_1);
    while( GetSensState() != SENS_1 )
    {
      if( TouchPressed() )
      {
        if(myButtons.checkButtons()==btn_stop)
        {
          MotoRun=false;
          DoCalibrate = false;
          return false;
        }
      }
    }

    MotoSetPause(true);
    delay(100);

    WaitSens=SENS_2;
    MotoDir(MOTO_DIR_2);
  }
  
  // Calc length
  MotoCalibrate=0;
  while( GetSensState() != WaitSens )
  {
    if( TouchPressed() )
    {
      if(myButtons.checkButtons()==btn_stop)
      {
        MotoRun=false;
        DoCalibrate = false;
        return false;
      }
    }
  }
  
  MotoSetPause(true);
  delay(100);

  NewCalibrate = MotoCalibrate;
  NewHalfCalibrate = NewCalibrate/2;

  if(MotoDirection==MOTO_DIR_2)
  {
    MotoPos=NewCalibrate;
    MotoDir(MOTO_DIR_1);
  }
  else
  {
    MotoPos=0;
    MotoDir(MOTO_DIR_2);
  }

  // Move to half pole
  while(1)
  {
    if(MotoDirection==MOTO_DIR_1 && MotoPos <= NewHalfCalibrate ) break;
    if(MotoDirection==MOTO_DIR_2 && MotoPos >= NewHalfCalibrate ) break;
    if( TouchPressed() )
    {
      if(myButtons.checkButtons()==btn_stop)
      {
        MotoRun=false;
        DoCalibrate = false;
        return false;
      }
    }
  }
  
  MotoRun=false;
  DoCalibrate = false;
  MotoCalibrateOK = true;
  
  DBG("NewCalibrate = "+String(NewCalibrate) );
  DBG("NewHalfCalibrate = "+String(NewHalfCalibrate) );
  DBG("MotoPos = "+String(MotoPos) );
  
  lboard_init_sectors(&LBoard,NewCalibrate);
  lboard_SetLight(MotoPos);
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool MotoDoHalfCalibrate(void)
{
  DoCalibrate = true;
  //DBG("-> MotoMoveToCenter <-");
  char WaitSens;
  char Sens = GetSensState();
  DBG("- Sens: "+String(Sens));
  
  MotoDirReset();
  MotoSetPause(true);
  MotoRun=true;

  MotoSpeed.SetFrom=MOTO_MIN_SPEED;
  MotoSpeed.SetTo=MOTO_CALIBRATE_SPEED;
  MotoSetSpeed();

  // Move to start
  MotoDir(MOTO_DIR_1);
  //DBG("- move to start, WaitSens=SENS_1, MOTO_DIR_1");

  while( GetSensState() != SENS_1 )
  {
    if( TouchPressed() )
    {
      if(myButtons.checkButtons()==btn_stop)
      {
        MotoRun=false;
        DoCalibrate = false;
        return false;
      }
    }
  }

  MotoSetPause(true);
  delay(100);
  //DBG("- MOTO_DIR_2, Move to pos: "+String(NewHalfCalibrate));
  MotoPos=0;
  MotoDir(MOTO_DIR_2);

  // Move to half pole
  while(1)
  {
    if(MotoDirection==MOTO_DIR_1 && MotoPos <= NewHalfCalibrate ) break;
    if(MotoDirection==MOTO_DIR_2 && MotoPos >= NewHalfCalibrate ) break;
    if( TouchPressed() )
    {
      if(myButtons.checkButtons()==btn_stop)
      {
        MotoRun=false;
        DoCalibrate = false;
        return false;
      }
    }
  }
  
  MotoRun=false;
  DoCalibrate = false;
  MotoCalibrateOK = true;

  lboard_init_sectors(&LBoard,NewCalibrate);
  lboard_SetLight(MotoPos);

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool MotoMoveToCenter(void)
{
  //DBG("-> MotoMoveToCenter <-");
  char WaitSens;
  char Sens = GetSensState();
  //DBG("- Sens: "+String(Sens));
  
  MotoDirReset();

  MotoSetPause(true);
  MotoRun=true;
  DoCalibrate = true;

  MotoSpeed.SetFrom=MOTO_MIN_SPEED;
  MotoSpeed.SetTo=MOTO_CALIBRATE_SPEED;
  MotoSetSpeed();
  
  if(MotoPos<NewHalfCalibrate)
  {
    WaitSens=SENS_2;
    MotoDir(MOTO_DIR_2);
  }
  else
  {
    WaitSens=SENS_1;
    MotoDir(MOTO_DIR_1);
  }
  
  // Move to half pole
  while(1)
  {
    if(MotoDirection==MOTO_DIR_1 && MotoPos <= NewHalfCalibrate ) break;
    if(MotoDirection==MOTO_DIR_2 && MotoPos >= NewHalfCalibrate ) break;
    if( TouchPressed() )
    {
      if(myButtons.checkButtons()==btn_stop)
      {
        MotoRun=false;
        DoCalibrate = false;
        return false;
      }
    }
  }
  
  MotoRun=false;
  DoCalibrate = false;
  MotoCalibrateOK = true;

  lboard_SetLight(MotoPos);
  
  return true;
}


