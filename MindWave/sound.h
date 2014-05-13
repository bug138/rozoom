///////////////////////////////////////////////////////////////////
//      S O U N D
///////////////////////////////////////////////////////////////////

#ifndef	__sound_h__
#define __sound_h__

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sound
bool sound = false;

/////////////////////////////////////////////////////////////////////////////////////
// Sound
void HW_Beep( int fr, int time)
{
  if(!sound) return;
  if( fr == 0 ) delay(time);
  else toneAC(fr, 10, time, false);
}

///////////////////////////////////////////////////////////////////////////////////
void Music_Play_Happy_Birthday(){
  if(!sound) return;
	PlayNote(2*MUSIC_NOTE_G,100);
	PlayNote(0,100);
	PlayNote(2*MUSIC_NOTE_G,200);
	PlayNote(2*MUSIC_NOTE_A,400);
	PlayNote(2*MUSIC_NOTE_G,400);
	PlayNote(2*MUSIC_NOTE_C*2,400); 
	PlayNote(2*MUSIC_NOTE_B,600);

	PlayNote(0,200);

	PlayNote(2*MUSIC_NOTE_G,100);
	PlayNote(0,100);
	PlayNote(2*MUSIC_NOTE_G,200);
	PlayNote(2*MUSIC_NOTE_A,400);
	PlayNote(2*MUSIC_NOTE_G,400);
	PlayNote(2*MUSIC_NOTE_D*2,400); 
	PlayNote(2*MUSIC_NOTE_C*2,600);
}

///////////////////////////////////////////////////////////////////////////////////
void Music_Play_Tetris(){
  if(!sound) return;
	PlayNote(2*MUSIC_NOTE_E*2,200);
	PlayNote(2*MUSIC_NOTE_B,100);
	PlayNote(2*MUSIC_NOTE_C*2,100);
	PlayNote(2*MUSIC_NOTE_D*2,200);
	PlayNote(2*MUSIC_NOTE_C*2,100);
	PlayNote(2*MUSIC_NOTE_B,100);
	PlayNote(2*MUSIC_NOTE_A,150);
	delay(50);
	PlayNote(2*MUSIC_NOTE_A,100);
	PlayNote(2*MUSIC_NOTE_C*2,100);
	PlayNote(2*MUSIC_NOTE_E*2,200);
	PlayNote(2*MUSIC_NOTE_D*2,100);	
	PlayNote(2*MUSIC_NOTE_C*2,100);	
	PlayNote(2*MUSIC_NOTE_B,150);	
	delay(50);
	PlayNote(2*MUSIC_NOTE_B,100);	
	PlayNote(2*MUSIC_NOTE_C*2,100);	
	PlayNote(2*MUSIC_NOTE_D*2,200);	
	PlayNote(2*MUSIC_NOTE_E*2,200);
	PlayNote(2*MUSIC_NOTE_C*2,200);
	PlayNote(2*MUSIC_NOTE_A,100);
	delay(100);
	PlayNote(2*MUSIC_NOTE_A,200);

/*
	delay(200);

	PlayNote(2*MUSIC_NOTE_D*2,300);
	PlayNote(2*MUSIC_NOTE_F*2,100);
	PlayNote(2*MUSIC_NOTE_A*2,200);
	PlayNote(2*MUSIC_NOTE_G*2,100);
	PlayNote(2*MUSIC_NOTE_F*2,100);
	PlayNote(2*MUSIC_NOTE_E*2,300);
	PlayNote(2*MUSIC_NOTE_C*2,100);
	PlayNote(2*MUSIC_NOTE_E*2,200);
	PlayNote(2*MUSIC_NOTE_D*2,100);
	PlayNote(2*MUSIC_NOTE_C*2,100);
	PlayNote(2*MUSIC_NOTE_B,150);
	delay(50);
	PlayNote(2*MUSIC_NOTE_B,100);
	PlayNote(2*MUSIC_NOTE_C*2,100);
	PlayNote(2*MUSIC_NOTE_D*2,200);
	PlayNote(2*MUSIC_NOTE_E*2,200);
	PlayNote(2*MUSIC_NOTE_C*2,200);
	PlayNote(2*MUSIC_NOTE_A,100);
	delay(100);
	PlayNote(2*MUSIC_NOTE_A,200);
*/
}

///////////////////////////////////////////////////////////////////////////////////
void Music_Play_JingleBells(){
  if(!sound) return;
	int octave=2;
	PlayNote(octave*MUSIC_NOTE_Gb,180);delay(20);
	PlayNote(octave*MUSIC_NOTE_Gb,180);delay(20);
	PlayNote(octave*MUSIC_NOTE_Gb,380);delay(20);

	PlayNote(octave*MUSIC_NOTE_Gb,180);delay(20);
	PlayNote(octave*MUSIC_NOTE_Gb,180);delay(20);
	PlayNote(octave*MUSIC_NOTE_Gb,380);delay(20);

	PlayNote(octave*MUSIC_NOTE_Gb,180);delay(20);
	PlayNote(octave*MUSIC_NOTE_A,180);delay(20);
	PlayNote(octave*MUSIC_NOTE_D,280);delay(20);
	PlayNote(octave*MUSIC_NOTE_D,100);

	PlayNote(octave*MUSIC_NOTE_Gb,700);delay(100);
}


///////////////////////////////////////////////////////////////////////////////////
void Music_Play_Nokia_Tune(char* nokia){
	
  if(!sound) return;
  
        char* ptr=nokia;
	char tmp[32];

	char got_duration=false;
	int duration=1;
	char sharp=false;
	int freq=0;
	while(*ptr){
		if(*ptr==' '){ptr++;continue;}
		if(got_duration==false){
			int dur=atoi(ptr);
			duration=3200/dur;
                        
                        memset(tmp,0,32);
                        sprintf(tmp,"%d",dur);
			ptr+=strlen(tmp);

			got_duration=true;
			continue;
		}
		if(*ptr=='.'){
			duration=duration*3/2;
			ptr++;
			continue;
		}
		if(*ptr=='#'){
			sharp=true;
			ptr++;
			continue;
		}
		if(*ptr=='-'){
			delay(duration);
			sharp=false;
			got_duration=false;
			ptr++;
			continue;
		}
		//
		if(!sharp){
			switch(*ptr){
				case 'a':freq=MUSIC_NOTE_A;break;
				case 'b':freq=MUSIC_NOTE_B;break;
				case 'c':freq=MUSIC_NOTE_C;break;
				case 'd':freq=MUSIC_NOTE_D;break;
				case 'e':freq=MUSIC_NOTE_E;break;
				case 'f':freq=MUSIC_NOTE_F;break;
				case 'g':freq=MUSIC_NOTE_G;break;
				default: freq=1;break;
			}
		}else{
			switch(*ptr){
				case 'a':freq=MUSIC_NOTE_Bb;break;
				case 'b':freq=MUSIC_NOTE_C;break;
				case 'c':freq=MUSIC_NOTE_Db;break;
				case 'd':freq=MUSIC_NOTE_Eb;break;
				case 'e':freq=MUSIC_NOTE_F;break;
				case 'f':freq=MUSIC_NOTE_Gb;break;
				case 'g':freq=MUSIC_NOTE_Ab;break;
				default: freq=1;break;
			}
		}
		ptr++;
		if(*ptr=='\0') return;
		char octave=atoi(ptr);
		for(int i=0;i<octave;i++) freq=freq*2;
		if(duration<21) duration=20;
		PlayNote(freq,duration-20);
		delay(20);
		sharp=false;
		got_duration=false;
		ptr++;
		continue;
	}
}

#endif /*__sound_h__*/

