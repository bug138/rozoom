///////////////////////////////////////////////////////////////////
//      setting's
///////////////////////////////////////////////////////////////////

#ifndef	__settings_h__
#define __settings_h__

typedef struct{
  byte Marker;
  unsigned char AttentionThreshold;
  unsigned char MeditationThreshold;
  unsigned int PlayTime;
  unsigned long MotoCalibrate;
  unsigned long MotoPos;
  unsigned long NewCalibrate;
  unsigned long NewHalfCalibrate;
  bool sound;
}TSettings;
TSettings Settings;


/****************************************************************/
void SaveSettings(void)
{
  DBG("\r\nSave settings:");
  Settings.Marker = 0xA5;
  Settings.AttentionThreshold = Game.AttentionThreshold;
  Settings.MeditationThreshold = Game.MeditationThreshold;
  Settings.PlayTime = Game.PlayTime;
  Settings.MotoCalibrate = MotoCalibrate;
  Settings.MotoPos = MotoPos;
  Settings.NewCalibrate = NewCalibrate;
  Settings.NewHalfCalibrate = NewHalfCalibrate;
  Settings.sound = sound;

  byte *ptr = (byte*)&Settings;   
  for( int i=0; i<sizeof(Settings); i++) EEPROM.write(i,*(ptr+i));
  
  DBG("\r\nLoad Settings:");
  DBG("Game.AttentionThreshold = "+String(Game.AttentionThreshold));
  DBG("Game.MeditationThreshold = "+String(Game.MeditationThreshold));
  DBG("Game.PlayTime = "+String(Game.PlayTime));
  DBG("MotoCalibrate = "+String(MotoCalibrate));
  DBG("MotoPos = "+String(MotoPos));
  DBG("NewCalibrate = "+String(NewCalibrate));
  DBG("NewHalfCalibrate = "+String(NewHalfCalibrate));
  DBG("sound = "+String(sound));
}


/****************************************************************/
void LoadSettings(void)
{
  byte *ptr = (byte*)&Settings;   
  for( int i=0; i<sizeof(Settings); i++) *(ptr+i)=EEPROM.read(i);
  
  if(Settings.Marker==0xA5)
  {
    Game.AttentionThreshold=Settings.AttentionThreshold;
    Game.MeditationThreshold=Settings.MeditationThreshold;
    Game.PlayTime=Settings.PlayTime;
    MotoCalibrate=Settings.MotoCalibrate;
    MotoPos=Settings.MotoPos;
    NewCalibrate=Settings.NewCalibrate;
    NewHalfCalibrate=Settings.NewHalfCalibrate;
    sound=Settings.sound;
  }
  else
  {
    // eeprom чистая, устанавливаем по умолчанию
    DBG("Set default settings");
    Game.AttentionThreshold  = 10;
    Game.MeditationThreshold = 10;
    SaveSettings();
    LoadSettings();
  }
  
  DBG("\r\nLoad Settings:");
  DBG("Game.AttentionThreshold = "+String(Game.AttentionThreshold));
  DBG("Game.MeditationThreshold = "+String(Game.MeditationThreshold));
  DBG("Game.PlayTime = "+String(Game.PlayTime));
  DBG("MotoCalibrate = "+String(MotoCalibrate));
  DBG("MotoPos = "+String(MotoPos));
  DBG("NewCalibrate = "+String(NewCalibrate));
  DBG("NewHalfCalibrate = "+String(NewHalfCalibrate));
  DBG("sound = "+String(sound));
}


#endif /*__settings_h__*/


