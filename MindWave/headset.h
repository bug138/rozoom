///////////////////////////////////////////////////////////////////
//      H E A D S E T
///////////////////////////////////////////////////////////////////

void StartHS(char number)
{
  if( number == 1 )
  {
    // HeadSet 1
    Serial1.write(0xC0);
    Serial1.write(0xE0);
    Serial1.write(0x8B);
  }
  else
  if( number == 2)
  {
    // HeadSet 2
    Serial2.write(0xC0);
    Serial2.write(0xD7);
    Serial2.write(0x88);
  }
  else
  {
    // Stop HS
    Serial1.write(0xC1);
    Serial2.write(0xC1);
  }
}

/*
* ------------------------------------------------
*/
void calc_average(DAT *var)
{
  if(MAX_MEASURE>1)
  {
    word summ = 0;
    for(byte i=0; i<MAX_MEASURE; i++) summ += var->data[i];
    var->average = (byte)(summ/MAX_MEASURE);  
    var->current_pos++;
    if(var->current_pos>=MAX_MEASURE) var->current_pos=0;
  }
  else
  {
    var->average = var->data[0];
  }

}


/*
* ------------------------------------------------
*/
char ParseData(PLAYER *hs)
{
  char res=false;

  for(int i=0; i<hs->pkt.size; i++)
  {
    switch (hs->pkt.data[i])
    {
      case 0x02:  // Quality 0..100 -> normal signal
        i++;
        hs->quality = hs->pkt.data[i];
        if(hs->quality<=100) hs->ready=true;
        else hs->ready=false;
        res=true;
        break;

      case 0x04:  // Attention
        i++;
        if(hs->pkt.data[i]>=Game.AttentionThreshold && hs->pkt.data[i]<=100)
        {
          if(hs->pkt.data[i]<Game.AttentionThreshold) hs->att.data[hs->att.current_pos] = 0;
          else hs->att.data[hs->att.current_pos] = hs->pkt.data[i];
          calc_average(&hs->att);
          hs->att.ready=true;
        }
        else
        {
          hs->att.average=0;
          hs->att.ready=false;
        }
        res=true;
        break;
      
      case 0x05:  // Meditation
        i++;
        if(hs->pkt.data[i]>=Game.MeditationThreshold && hs->pkt.data[i]<=100)
        {
          if(hs->pkt.data[i]<Game.MeditationThreshold) hs->med.data[hs->med.current_pos] = 0;
          else hs->med.data[hs->med.current_pos] = hs->pkt.data[i];
          calc_average(&hs->med);
          hs->med.ready=true;
        }
        else
        {
          hs->med.average=0;
          hs->med.ready=false;
        }
        res=true;
        break;

        case 0x16:
        break;

      case 0x80:
        i = i + 3;
        break;
        
      case 0x83:
        i = i + 25;
        break;
        
      default:
        break;
    }
  }
  hs->pkt.ready = false;  
  return res;
}

/*
* ------------------------------------------------------------------------------------------------------------------------------------------------
*/
void CheckHeadSet(void)
{
  if(Game.Player[_P1_].pkt.ready)
  {
    bool res = ParseData(&Game.Player[_P1_]);
    #ifdef FUNCTION_DEBUG
    if(res)
    {
        DBG("Q1:"+String(Game.Player[_P1_].quality));
        if(Game.Player[_P1_].att.ready)
        {
          DBG("A1:"+String(Game.Player[_P1_].att.average));
        }
        
        if(Game.Player[_P1_].med.ready)
        {
          DBG("M1:"+String(Game.Player[_P1_].med.average));
        }
    }
    #endif
  }

  if(Game.Player[_P2_].pkt.ready)
  {
    bool res = ParseData(&Game.Player[_P2_]);
    #ifdef FUNCTION_DEBUG
    if(res)
    {
      DBG("Q2:"+String(Game.Player[_P2_].quality));
        if(Game.Player[_P2_].att.ready)
        {
          DBG("A2:"+String(Game.Player[_P2_].att.average));
        }
        if(Game.Player[_P2_].med.ready)
        {
          DBG("M2:"+String(Game.Player[_P2_].med.average));
        }
    }
    #endif
  }
}

/*
* ------------------------------------------------
*/
void serialEventParser(PLAYER *hs, HardwareSerial *uart)
{
  if(!(hs->pkt.ready))
  {
    byte ch = uart->read();
    if( (hs->pkt.cnt==0 || hs->pkt.cnt==1) && ch==0xAA ) hs->pkt.cnt++;
    else if(hs->pkt.cnt==2)
    {
      if(ch < 0xAA)
      {
        hs->pkt.size = ch;
        hs->pkt.pkt_cnt = 0;
        hs->pkt.cnt++;
        hs->pkt.generatedChecksum = 0;
      }
      else hs->pkt.cnt=0; 
    }
    else if(hs->pkt.cnt>2 && hs->pkt.pkt_cnt<hs->pkt.size)
    {
      hs->pkt.data[hs->pkt.pkt_cnt] = ch;
      hs->pkt.generatedChecksum += ch;
      hs->pkt.pkt_cnt++;
      hs->pkt.cnt++;
    }
    else if(hs->pkt.pkt_cnt >= hs->pkt.size)
    {
      hs->pkt.generatedChecksum = 255 - hs->pkt.generatedChecksum;
      if(ch != hs->pkt.generatedChecksum) hs->pkt.cnt=0;  // error
      else
      {        
        hs->pkt.ready = true;
        hs->pkt.cnt=0;
      }
    }
    else hs->pkt.cnt=0;
  }
}

/*
*  READ DATA from HeadSet 1
*/
void serialEvent1() {
  serialEventParser(&Game.Player[_P1_],&Serial1);
}

/*
*  READ DATA from HeadSet 2
*/
void serialEvent2() {
  serialEventParser(&Game.Player[_P2_],&Serial2);
}


