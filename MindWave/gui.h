// GUI
#ifndef	__gui_h__
#define __gui_h__

/////////////////////////////////////////////////////////////////////////////////////
// Buttons
int pressed_button,
    btn_play,btn_options,btn_sound,btn_poweroff,
    btn_play_normal,btn_play_inverse,
    btn_up,btn_down,
    btn_ok,btn_cancel,
    btn_stop,btn_pause,btn_resume,
    btn_edit_att_threshold,btn_edit_med_threshold,
    btn_time_1,btn_time_3,btn_time_5,btn_time_10,btn_time_15,btn_time_20,btn_time_30,btn_time_40,btn_time_50,btn_time_60,btn_time_70,btn_time_80,btn_time_90,btn_time_99,
    btn_play_att, btn_play_med, btn_play_att_med,
    btn_calibrate,btn_calibrate_full,btn_calibrate_half,btn_calibrate_center;


/****************************************************************/
bool TouchPressed(void)
{
  bool res = myTouch.dataAvailable();
  if(res) {
    HW_Beep(3000,30);
    HW_Beep(6000,30);
  }
  return res;
}

/****************************************************************/
void ClearScreen(void)
{
  myButtons.deleteAllButtons();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myButtons.setTextFont(BigFont);
}


/****************************************************************/
unsigned char ShowConfirm(void)
{
  ClearScreen();
  myGLCD.print("POWER OFF",CENTER,70);
  myGLCD.print("are you sure?",CENTER,100);
  btn_cancel = myButtons.addButton(0,180,150,50,"Cancel");
  btn_ok = myButtons.addButton(  160,180,159,50,"OK");
  myButtons.drawButtons();
  while(true)
  {
      if(TouchPressed())
      {
        pressed_button = myButtons.checkButtons();
        if(pressed_button==btn_ok) return true;
        else if(pressed_button==btn_cancel) return false;
      }
  }
  return 0;
}

/****************************************************************/
void ShowMainMenu(void)
{
  Game.GameSet=GameSet_None;
  ClearScreen();
  btn_play       = myButtons.addButton(0,  0,319, 89, "PLAY");
  btn_options    = myButtons.addButton(0, 90,319, 89, "OPTIONS");

  char str_sound[10]; memset(str_sound,0,10);
  sprintf(str_sound,"SOUND %s",sound?"ON":"OFF");
  btn_sound      = myButtons.addButton(  0,180,159,59, str_sound);
  btn_poweroff   = myButtons.addButton(160,180,159,59, "POWER");
  myButtons.drawButtons();
  CurrentMode = ModeMainMenu;

  print7seg(_P1_,'-',true);
  print7seg(_P2_,'-',true);
}

/****************************************************************/
void ShowOptions(void)
{
  //choice of threshold value.
  ClearScreen();
  btn_edit_att_threshold = myButtons.addButton(0, 0, 319,59, "Attention");
  btn_edit_med_threshold = myButtons.addButton(0, 60,319,59, "Meditation");
           btn_calibrate = myButtons.addButton(0,120,319,59, "Calibrate");
              btn_cancel = myButtons.addButton(0,180,130,59,"<- back");
  myButtons.drawButtons();
}


/****************************************************************/
void ShowWaitHS(void)
{
  ClearScreen();
  btn_ok = myButtons.addButton(0,0,319,99,"Wait HeadSet");
  myButtons.disableButton(btn_ok);
  btn_cancel = myButtons.addButton(0, 190,  319,  49, "STOP");
  myButtons.drawButtons();
}

/****************************************************************/
void ShowGame(void)
{
  ClearScreen();
  btn_pause = myButtons.addButton(0,60, 319,59, "PAUSE");
  btn_resume= myButtons.addButton(0,120,319,59, "RESUME");
  btn_stop  = myButtons.addButton(0,180,319,59, "STOP");
  myButtons.drawButtons();
}

/****************************************************************/
void ShowWaitCalibrate(void)
{
  ClearScreen();
  myGLCD.print("Please wait...", CENTER, 80);
  btn_stop = myButtons.addButton(0,180,319,59, "STOP");
  myButtons.drawButtons();
}

/****************************************************************/
void ShowWinner(void)
{
  ClearScreen();
  myGLCD.print("Winner player",CENTER,50);
  myGLCD.print(String(Game.Winner),CENTER,70);
  btn_ok = myButtons.addButton(0, 150,  319,  89, "OK");
  myButtons.drawButtons();
}

/****************************************************************/
void ShowPlayMode(void)
{
  ClearScreen();
  btn_play_normal = myButtons.addButton(0,0,318,118,  "Play normal");
  btn_play_inverse= myButtons.addButton(0,122,318,117,"Play inverse");
  myButtons.drawButtons();
}

/****************************************************************/
void ShowGameSet(void)
{
  ClearScreen();
  btn_play_att    = myButtons.addButton(0,0,318,78,  "Attention");
  btn_play_med    = myButtons.addButton(0,80,318,78,"Meditation");
  btn_play_att_med = myButtons.addButton(0,160,318,78,"ATT + MED");
  myButtons.drawButtons();
}

/****************************************************************/
void ShowCalibrate(void)
{
  ClearScreen();
  btn_calibrate_full   = myButtons.addButton(0,  0,319,79,"FULL");
  btn_calibrate_half   = myButtons.addButton(0, 80,319,79,"HALF");
  btn_calibrate_center = myButtons.addButton(0,160,319,79,"CENTER");
  myButtons.drawButtons();
}
/****************************************************************/
unsigned int EditPlayTime(unsigned int NewValue)
{
  ClearScreen();
  
  unsigned char dY = 20;

  btn_time_1  = myButtons.addButton(100,0+dY,50,50, "1");
  btn_time_3  = myButtons.addButton(150,0+dY,50,50, "3");
  btn_time_5  = myButtons.addButton(200,0+dY,50,50, "5");
  btn_time_10 = myButtons.addButton(0,50+dY,50,50, "10");
  btn_time_15 = myButtons.addButton(50,50+dY,50,50, "15");
  btn_time_20 = myButtons.addButton(100,50+dY,50,50, "20");
  btn_time_30 = myButtons.addButton(150,50+dY,50,50, "30");
  btn_time_40 = myButtons.addButton(200,50+dY,50,50, "40");
  btn_time_50 = myButtons.addButton(0,100+dY,50,50, "50");
  btn_time_60 = myButtons.addButton(50,100+dY,50,50, "60");
  btn_time_70 = myButtons.addButton(100,100+dY,50,50, "70");
  btn_time_80 = myButtons.addButton(150,100+dY,50,50, "80");
  btn_time_99 = myButtons.addButton(200,100+dY,50,50, "99");
  
  btn_up = myButtons.addButton(  269,0+dY,50,75, "+");
  btn_down = myButtons.addButton(269,75+dY,50,75, "-");
  btn_cancel = myButtons.addButton(0,189,150,50,"Cancel");
  btn_ok = myButtons.addButton(  160,189,159,50,"OK");
  myButtons.drawButtons();

  myGLCD.print("Time to Play (min) ",CENTER,0);
  
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(String(NewValue),4,dY);

  while(true)
  {
    if(TouchPressed())
    {
      pressed_button = myButtons.checkButtons();
      
      if(pressed_button==btn_time_1) NewValue = 1;
      else if(pressed_button==btn_time_3) NewValue = 3;
      else if(pressed_button==btn_time_5) NewValue = 5;
      else if(pressed_button==btn_time_10) NewValue = 10;
      else if(pressed_button==btn_time_15) NewValue = 15;
      else if(pressed_button==btn_time_20) NewValue = 20;
      else if(pressed_button==btn_time_30) NewValue = 30;
      else if(pressed_button==btn_time_40) NewValue = 40;
      else if(pressed_button==btn_time_50) NewValue = 50;
      else if(pressed_button==btn_time_60) NewValue = 60;
      else if(pressed_button==btn_time_70) NewValue = 70;
      else if(pressed_button==btn_time_80) NewValue = 80;
      else if(pressed_button==btn_time_99) NewValue = 99;
      else if(pressed_button==btn_up && NewValue<99) NewValue++;
      else if(pressed_button==btn_down && NewValue>1) NewValue--;
      else if(pressed_button==btn_ok) break;
      else if(pressed_button==btn_cancel) {NewValue=0;break;};
  
      myGLCD.print(String(NewValue)+":",4,dY);
    }
  }
 
  return NewValue;
}

/****************************************************************/
unsigned int EditUint(unsigned int in_variable, unsigned int min_value, unsigned int max_value, char* caption)
{
  unsigned int NewValue = in_variable;
  if(NewValue<min_value) NewValue=min_value;
  if(NewValue>max_value) NewValue=max_value;

  ClearScreen();

  unsigned char dY = 20;
  btn_time_10 = myButtons.addButton(0,0+dY,50,50, "10");
  btn_time_90 = myButtons.addButton(200,0+dY,50,50, "90");
  btn_time_20 = myButtons.addButton(0,50+dY,50,50, "20");
  btn_time_80 = myButtons.addButton(200,50+dY,50,50, "80");
  btn_time_30 = myButtons.addButton(0,100+dY,50,50, "30");
  btn_time_40 = myButtons.addButton(50,100+dY,50,50, "40");
  btn_time_50 = myButtons.addButton(100,100+dY,50,50, "50");
  btn_time_60 = myButtons.addButton(150,100+dY,50,50, "60");
  btn_time_70 = myButtons.addButton(200,100+dY,50,50, "70");
  btn_up = myButtons.addButton(  269,20,50,75, "+");
  btn_down = myButtons.addButton(269,95,50,75, "-");
  btn_cancel = myButtons.addButton(0,189,150,50,"Cancel");
  btn_ok = myButtons.addButton(  160,189,159,50,"OK");
  myButtons.drawButtons();

  myGLCD.print(caption,CENTER,0);
 
  unsigned short value_X = 100;
  unsigned short value_Y = 55;
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(String(NewValue), value_X, value_Y);
  
  while(true)
  {
    if(TouchPressed())
    {
      pressed_button = myButtons.checkButtons();
      if(pressed_button==btn_up && NewValue<max_value) NewValue++;
      else if(pressed_button==btn_down && NewValue>min_value) NewValue--;
      else if(pressed_button==btn_ok) break;
      else if(pressed_button==btn_cancel)
      {
        NewValue=in_variable;
        break;
      }
      else if(pressed_button==btn_time_10) NewValue = 10;
      else if(pressed_button==btn_time_20) NewValue = 20;
      else if(pressed_button==btn_time_30) NewValue = 30;
      else if(pressed_button==btn_time_40) NewValue = 40;
      else if(pressed_button==btn_time_50) NewValue = 50;
      else if(pressed_button==btn_time_60) NewValue = 60;
      else if(pressed_button==btn_time_70) NewValue = 70;
      else if(pressed_button==btn_time_80) NewValue = 80;
      else if(pressed_button==btn_time_90) NewValue = 90;
      
      myGLCD.print(String(NewValue)+":", value_X, value_Y);
    }    
  }
  
  return NewValue;
}


byte bin2bcd(byte bin)
{
  div_t _bcd = div(bin,10);
  return (_bcd.quot<<4)+_bcd.rem;
}

void print7seg(char PlayerNum, byte Number, bool Dot)
{
  if(Dot) Game.Player[PlayerNum].display->point(POINT_ON);
  else Game.Player[PlayerNum].display->point(POINT_OFF);

  if(Number>99) Number=99;

  if(Number==' ' ||
     Number=='~' ||
     Number=='-' ||
     Number=='_' )
     {
        memset(Game.Player[PlayerNum].displayBuf,0,4);
        Game.Player[PlayerNum].displayBuf[3] = ' ';
        Game.Player[PlayerNum].displayBuf[2] = Number;
        Game.Player[PlayerNum].displayBuf[1] = Number;
        Game.Player[PlayerNum].displayBuf[0] = ' ';
        Game.Player[PlayerNum].display->display( Game.Player[PlayerNum].displayBuf );
     }
     else
     {
        byte bcdNumber = bin2bcd(Number);
        if(Dot) Game.Player[PlayerNum].display->point(POINT_ON);
        else Game.Player[PlayerNum].display->point(POINT_OFF);
      
        memset(Game.Player[PlayerNum].displayBuf,0,4);
        Game.Player[PlayerNum].displayBuf[3] = ' ';
        Game.Player[PlayerNum].displayBuf[2] = 0x0F & bcdNumber;
        Game.Player[PlayerNum].displayBuf[1] = 0x0F & (bcdNumber>>4);
        Game.Player[PlayerNum].displayBuf[0] = ' ';
        Game.Player[PlayerNum].display->display( Game.Player[PlayerNum].displayBuf );
     }

     if(Game.GameSet==GameSet_Att) Game.Player[PlayerNum].display->display(0,'W');
     else if(Game.GameSet==GameSet_Med) Game.Player[PlayerNum].display->display(0,'Q');
     else if(Game.GameSet==GameSet_AttMed) Game.Player[PlayerNum].display->display(0,'E');
     else Game.Player[PlayerNum].display->display(0,' ');

     

     
}

#endif /*__gui_h__*/


