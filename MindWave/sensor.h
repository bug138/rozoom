/////////////////////////////////////////////////////////////////////////////////////
//   Sensor 1&2
/////////////////////////////////////////////////////////////////////////////////////

// pin's
int _pin_Sens1 = 21;
int _pin_Sens2 = 20;


void SensConfigPins()
{
  pinMode(_pin_Sens1, INPUT);
  pinMode(_pin_Sens2, INPUT);
}

inline char GetSensState(void)
{
  if(digitalRead(_pin_Sens1)==0)
  {
    return SENS_1;
  }
  else if(digitalRead(_pin_Sens2)==0)
  {
    return SENS_2;
  }

  return SENS_NONE;
}


