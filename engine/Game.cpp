#include "Hunt.h"

void SetupRes()
{
	if (!HARD3D)
		if (OptRes>5) OptRes=5;

	int oldRes = OptRes;
	OptRes = 4;
    if (OptRes==0) { WinW = 320; WinH=240; }
	if (OptRes==1) { WinW = 400; WinH=300; }
	if (OptRes==2) { WinW = 512; WinH=384; }
	if (OptRes==3) { WinW = 640; WinH=480; }
	if (OptRes==4) { WinW = 800; WinH=600; }
	if (OptRes==5) { WinW =1024; WinH=768; }		
	if (OptRes==6) { WinW =1280; WinH=1024; }
	if (OptRes==7) { WinW =1600; WinH=1200; }	
	OptRes = oldRes;
}


float GetLandOH(int x, int y)
{
  return (float)(HMapO[y][x]) * ctHScale;
}


float GetLandOUH(int x, int y)
{
  if (FMap[y][x] & fmReverse)
   return (float)((int)(HMap[y][x+1]+HMap[y+1][x])/2.f)*ctHScale;             
                else
   return (float)((int)(HMap[y][x]+HMap[y+1][x+1])/2.f)*ctHScale;
}



float GetLandUpH(float x, float y)
{ 
	
   int CX = (int)x / 256;
   int CY = (int)y / 256;

   if (!(FMap[CY][CX] & fmWaterA)) return GetLandH(x,y);
   
   return (float)(WaterList[ WMap[CY][CX] ].wlevel * ctHScale);

}


float GetLandH(float x, float y)
{ 
   int CX = (int)x / 256;
   int CY = (int)y / 256;
   
   int dx = (int)x % 256;
   int dy = (int)y % 256; 

   int h1 = HMap[CY][CX];
   int h2 = HMap[CY][CX+1];
   int h3 = HMap[CY+1][CX+1];
   int h4 = HMap[CY+1][CX];


   if (FMap[CY][CX] & fmReverse) {
     if (256-dx>dy) h3 = h2+h4-h1;
               else h1 = h2+h4-h3;
   } else {
     if (dx>dy) h4 = h1+h3-h2;
           else h2 = h1+h3-h4;
   }

   float h = (float)
	   (h1   * (256-dx) + h2 * dx) * (256-dy) +
	   (h4   * (256-dx) + h3 * dx) * dy;

   return  (h / 256.f / 256.f) * ctHScale;	      
}



float GetLandLt(float x, float y)
{ 
   int CX = (int)x / 256;
   int CY = (int)y / 256;
   
   int dx = (int)x % 256;
   int dy = (int)y % 256; 

   int h1 = LMap[CY][CX];
   int h2 = LMap[CY][CX+1];
   int h3 = LMap[CY+1][CX+1];
   int h4 = LMap[CY+1][CX]; 

   float h = (float)
	   (h1   * (256-dx) + h2 * dx) * (256-dy) +
	   (h4   * (256-dx) + h3 * dx) * dy;

   return  (h / 256.f / 256.f);
}



float GetLandLt2(float x, float y)
{ 
   int CX = ((int)x / 512)*2 - CCX;
   int CY = ((int)y / 512)*2 - CCY;
   
   int dx = (int)x % 512;
   int dy = (int)y % 512; 

   int h1 = VMap[CY+128][CX+128].Light;
   int h2 = VMap[CY+128][CX+2+128].Light;
   int h3 = VMap[CY+2+128][CX+2+128].Light;
   int h4 = VMap[CY+2+128][CX+128].Light; 

   float h = (float)
	   (h1   * (512-dx) + h2 * dx) * (512-dy) +
	   (h4   * (512-dx) + h3 * dx) * dy;

   return  (h / 512.f / 512.f);
}



void CalcModelGroundLight(TModel *mptr, float x0, float z0, int FI)
{
	float ca = (float)cos(FI * pi / 2);
	float sa = (float)sin(FI * pi / 2);
	for (int v=0; v<mptr->VCount; v++) {
		float x = mptr->gVertex[v].x * ca + mptr->gVertex[v].z * sa + x0;
		float z = mptr->gVertex[v].z * ca - mptr->gVertex[v].x * sa + z0;
#ifdef _d3d
        mptr->VLight[0][v] = (int)GetLandLt2(x, z) - 128;
#else
        mptr->VLight[0][v] = (float)GetLandLt2(x, z) - 128;
#endif
	}
}


BOOL PointOnBound(float &H, float px, float py, float cx, float cy, float oy, TBound *bound, int angle)
{
	px-=cx;
	py-=cy; 
	
	float ca = (float) cos(angle*pi / 2.f);
	float sa = (float) sin(angle*pi / 2.f);	

	BOOL _on = FALSE;
	H=-10000;

	for (int o=0; o<8; o++) {
		
		if (bound[o].a<0) continue;
		if (bound[o].y2 + oy > PlayerY + 128) continue;        
		
        float a,b;
	    float ccx = bound[o].cx*ca + bound[o].cy*sa;
	    float ccy = bound[o].cy*ca - bound[o].cx*sa;

	    if (angle & 1) {
         a = bound[o].b;
	     b = bound[o].a;
		} else {
	     a = bound[o].a;
	     b = bound[o].b;
		}

	    if ( ( fabs(px - ccx) < a) &&  (fabs(py - ccy) < b) ) 
		{
		      _on=TRUE;			  
			  if (H < bound[o].y2) H = bound[o].y2;			  
		}
	}

	return _on;
}



BOOL PointUnBound(float &H, float px, float py, float cx, float cy, float oy, TBound *bound, int angle)
{
	px-=cx;
	py-=cy; 
	
	float ca = (float) cos(angle*pi / 2.f);
	float sa = (float) sin(angle*pi / 2.f);	

	BOOL _on = FALSE;
	H=+10000;

	for (int o=0; o<8; o++) {
		
		if (bound[o].a<0) continue;
		if (bound[o].y1 + oy < PlayerY + 128) continue;        
		
        float a,b;
	    float ccx = bound[o].cx*ca + bound[o].cy*sa;
	    float ccy = bound[o].cy*ca - bound[o].cx*sa;

	    if (angle & 1) {
         a = bound[o].b;
	     b = bound[o].a;
		} else {
	     a = bound[o].a;
	     b = bound[o].b;
		}

	    if ( ( fabs(px - ccx) < a) &&  (fabs(py - ccy) < b) ) 
		{
		      _on=TRUE;			  
			  if (H > bound[o].y1) H = bound[o].y1;
		}
	}

	return _on;
}





float GetLandCeilH(float CameraX, float CameraZ)
{
  float h;
  
   h = GetLandH(CameraX, CameraZ) + 20480;

   int ccx = (int)CameraX / 256;
   int ccz = (int)CameraZ / 256;

   for (int z=-4; z<=4; z++)
    for (int x=-4; x<=4; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
		
        float CR = (float)MObjects[ob].info.Radius - 1.f;
                
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;

		float LandY = GetLandOH(ccx+x, ccz+z);

		if (!(MObjects[ob].info.flags & ofBOUND)) {
         if (MObjects[ob].info.YLo + LandY > h) continue;
         if (MObjects[ob].info.YLo + LandY < PlayerY+100) continue;         
		}

        float r = CR+1;

		if (MObjects[ob].info.flags & ofBOUND)
		{
			float hh;
			if (PointUnBound(hh, CameraX, CameraZ, ox, oz, LandY, MObjects[ob].bound, ((FMap[ccz+z][ccx+x] >> 2) & 3)  ) )
				if (h > LandY + hh) h = LandY + hh;
		} else {
		 if (MObjects[ob].info.flags & ofCIRCLE)
		   r = (float) sqrt( (ox-CameraX)*(ox-CameraX) + (oz-CameraZ)*(oz-CameraZ) );
		 else
		   r = (float) max( fabs(ox-CameraX) , fabs(oz-CameraZ) );
		 
		 if (r<CR) h = MObjects[ob].info.YLo + LandY;
		}
        
   }
  return h;
}



float GetLandQH(float CameraX, float CameraZ)
{
  float h,hh;
  
   h = GetLandH(CameraX, CameraZ);
   hh = GetLandH(CameraX-90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX+90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX-90.f, CameraZ+90.f); if (hh>h) h=hh; 
   hh = GetLandH(CameraX+90.f, CameraZ+90.f); if (hh>h) h=hh;

   hh = GetLandH(CameraX+128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX-128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ+128.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ-128.f); if (hh>h) h=hh;

   int ccx = (int)CameraX / 256;
   int ccz = (int)CameraZ / 256;

   for (int z=-4; z<=4; z++)
    for (int x=-4; x<=4; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
		
        float CR = (float)MObjects[ob].info.Radius - 1.f;
                
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;

		float LandY = GetLandOH(ccx+x, ccz+z);

		if (!(MObjects[ob].info.flags & ofBOUND)) {
         if (MObjects[ob].info.YHi + LandY < h) continue;
         if (MObjects[ob].info.YHi + LandY > PlayerY+128) continue;
         //if (MObjects[ob].info.YLo + LandY > PlayerY+256) continue;
		}

        float r = CR+1;

		if (MObjects[ob].info.flags & ofBOUND)
		{
			float hh;
			if (PointOnBound(hh, CameraX, CameraZ, ox, oz, LandY, MObjects[ob].bound, ((FMap[ccz+z][ccx+x] >> 2) & 3)  ) )
				if (h < LandY + hh) h = LandY + hh;
		} else {
		 if (MObjects[ob].info.flags & ofCIRCLE)
		   r = (float) sqrt( (ox-CameraX)*(ox-CameraX) + (oz-CameraZ)*(oz-CameraZ) );
		 else
		   r = (float) max( fabs(ox-CameraX) , fabs(oz-CameraZ) );
		 
		 if (r<CR) h = MObjects[ob].info.YHi + LandY;
		}
        
   }
  return h;
}


float GetLandHObj(float CameraX, float CameraZ)
{
   float h;   

   h = 0;

   int ccx = (int)CameraX / 256;
   int ccz = (int)CameraZ / 256;

   for (int z=-3; z<=3; z++)
    for (int x=-3; x<=3; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
        float CR = (float)MObjects[ob].info.Radius - 1.f;
        
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;

        if (MObjects[ob].info.YHi + GetLandOH(ccx+x, ccz+z) < h) continue;
        if (MObjects[ob].info.YLo + GetLandOH(ccx+x, ccz+z) > PlayerY+256) continue;
        float r;
		if (MObjects[ob].info.flags & ofCIRCLE) 		
		  r = (float) sqrt( (ox-CameraX)*(ox-CameraX) + (oz-CameraZ)*(oz-CameraZ) );
		else
		  r = (float) max( fabs(ox-CameraX) , fabs(oz-CameraZ) );

        if (r<CR) 
            h = MObjects[ob].info.YHi + GetLandOH(ccx+x, ccz+z);
   }

  return h;
}


float GetLandQHNoObj(float CameraX, float CameraZ)
{
  float h,hh;
  
   h = GetLandH(CameraX, CameraZ);
   hh = GetLandH(CameraX-90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX+90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX-90.f, CameraZ+90.f); if (hh>h) h=hh; 
   hh = GetLandH(CameraX+90.f, CameraZ+90.f); if (hh>h) h=hh;

   hh = GetLandH(CameraX+128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX-128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ+128.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ-128.f); if (hh>h) h=hh;
   
   return h;
}


void ProcessCommandLine()
{
//  for (int a=0; a<__argc; a++) {
//     LPSTR s = __argv[a];
//     if (strstr(s,"x=")) { PlayerX = (float)atof(&s[2])*256.f; LockLanding = TRUE; }
//     if (strstr(s,"y=")) { PlayerZ = (float)atof(&s[2])*256.f; LockLanding = TRUE; }          
//	 if (strstr(s,"reg=")) TrophyRoom.RegNumber = atoi(&s[4]); 
//     if (strstr(s,"prj=")) strcpy(ProjectName, (s+4)); 
//	 if (strstr(s,"din=")) TargetDino = (atoi(&s[4])*1024);
//	 if (strstr(s,"wep=")) WeaponPres = atoi(&s[4]);	 
//	 if (strstr(s,"dtm=")) OptDayNight  = atoi(&s[4]);
//
//	 if (strstr(s,"-debug"))  DEBUG = TRUE;
//	 if (strstr(s,"-double")) DoubleAmmo = TRUE;
//	 if (strstr(s,"-radar"))  RadarMode = TRUE;
//	 if (strstr(s,"-tranq"))  Tranq = TRUE;
//	 if (strstr(s,"-observ")) ObservMode = TRUE;
//#ifdef _iceage
//     if (strstr(s,"-supply")) Supply = TRUE;
//#endif
//  }
}




void AddWCircle(float x, float z, float scale)
{
   WCircles[WCCount].pos.x = x;
   WCircles[WCCount].pos.z = z;
   WCircles[WCCount].pos.y = GetLandUpH(x, z);
   WCircles[WCCount].FTime = 0;   
   WCircles[WCCount].scale = scale;
   WCCount++;
}


void AddShipTask(int cindex)
{
}



void InitShip(int cindex)
{
}



void HideWeapon()
{
}








void InitGameInfo()
{
	LoadResourcesScript();
}




void InitEngine()
{
    //DEBUG        = TRUE;

	WATERANI     = TRUE;
	NODARKBACK   = TRUE;
    LoDetailSky  = TRUE;
    CORRECTION   = TRUE;
	FOGON        = TRUE;
	FOGENABLE    = TRUE;
    Clouds       = TRUE;   
    SKY          = TRUE;
    GOURAUD      = TRUE;   
    MODELS       = TRUE;
    TIMER        = DEBUG;
    BITMAPP      = FALSE;
    MIPMAP       = TRUE;
    NOCLIP       = FALSE;
    CLIP3D       = TRUE;
	USE_THREADS  = FALSE; //Default to no threading for now while bugs are fixed
	//FADE_IN		 = TRUE; //Fade in
	SelectedDay  = 1; //Select Day by default

    
    SLOW         = FALSE;
	LOWRESTX     = FALSE;
    MORPHP       = TRUE;
    MORPHA       = TRUE;

	_GameState = 0;

	RadarMode    = FALSE;

	fnt_BIG = CreateFont(
        23, 10, 0, 0,
        FW_REGULAR, 0,0,0,
#ifdef __rus
		RUSSIAN_CHARSET,		
#else
        ANSI_CHARSET,
#endif				
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);




    fnt_Small = CreateFont(
        14, 5, 0, 0,
        100, 0,0,0,
#ifdef __rus
		RUSSIAN_CHARSET,		
#else
        ANSI_CHARSET,
#endif		        
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);	


    fnt_Midd  = CreateFont(
        16, 7, 0, 0,
        550, 0,0,0,
#ifdef __rus
		RUSSIAN_CHARSET,		
#else
        ANSI_CHARSET,
#endif		        
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);	


    Heap = HeapCreate( 0, 60000000, 0);
    if( Heap == NULL ) {
      //MessageBox(hwndMain,"Error creating heap.","Error",IDOK);
	  MessageBox(hwndMain,"Error creating heap.","Error", MB_OK | MB_ICONHAND); // alacn
      return; }

    Textures[255] = (TEXTURE*) _HeapAlloc(Heap, 0, sizeof(TEXTURE));

    WaterR = 10;
    WaterG = 38;
    WaterB = 46;
    WaterA = 10;
	TargetDino = 1<<10;
	TargetCall = 10;
	WeaponPres = 1;
    MessageList[0].timeleft = 0;
	MessageListCnt = 0;
	MESSAGELIST_MAXLENGTH = 8;

	InitGameInfo();    
	        
    CreateVideoDIB();
    CreateFadeTab();	
    CreateDivTable();
    InitClips();
   
    TrophyRoom.RegNumber=0;
	
    PlayerX = (ctMapSize / 3) * 256;
	PlayerZ = (ctMapSize / 3) * 256;    

    ProcessCommandLine();
	

    switch (OptDayNight) {
   case 0:
	   SunShadowK = 0.7f;
	   Sun3dPos.x = - 4048;
       Sun3dPos.y = + 2048;
       Sun3dPos.z = - 4048;
	   break;
   case 1:
	   SunShadowK = 0.5f;
	   Sun3dPos.x = - 2048;
       Sun3dPos.y = + 4048;
       Sun3dPos.z = - 2048;
	   break;
   case 2:
	   SunShadowK = -0.7f;
	   Sun3dPos.x = + 3048;
       Sun3dPos.y = + 3048;
       Sun3dPos.z = + 3048;
	   break;
   }

	SelectedPlayer = 99; //99 = defaul trophy data
	LoadTrophy();
	
	ProcessCommandLine();    

	
	
    
    //ctViewR  = 72;
	//ctViewR1 = 28;
	//ctViewRM = 24;
	ctViewR  = 42 + (int)(OptViewR / 8)*2;
	ctViewR1 = 28;
	ctViewRM = 24;
	
    Soft_Persp_K = 1.5f;
    HeadY = 220;

	FogsList[0].fogRGB = 0x000000;
	FogsList[0].YBegin = 0;
	FogsList[0].Transp = 000;
	FogsList[0].FLimit = 000;

	FogsList[127].fogRGB = 0x00504000;
    FogsList[127].Mortal = FALSE;
	FogsList[127].Transp = 460;
	FogsList[127].FLimit = 200;

	FillMemory( FogsMap, sizeof(FogsMap), 0);
	PrintLog("Init Engine: Ok.\n");
}





void ShutDownEngine()
{
   ReleaseDC(hwndMain,hdcMain);   
}



void ProcessSyncro()
{
   RealTime = timeGetTime();
   srand( (unsigned) RealTime );
   if (SLOW) RealTime/=4;
   TimeDt = RealTime - PrevTime;
   if (TimeDt<0) TimeDt = 10;
   if (TimeDt>10000) TimeDt = 10;
   if (TimeDt>1000) TimeDt = 1000;
   PrevTime = RealTime;
   Takt++;
   if (!PAUSE)
	   if (MyHealth) {
		   MyHealth+=TimeDt*4;
		   MyEnergy+=TimeDt*5; //ADP Energy system
	   }
   if (MyHealth>MAX_HEALTH) MyHealth = MAX_HEALTH;   
   if (MyEnergy>MAX_ENERGY) MyEnergy = MAX_ENERGY;  //ADP energy system
}






void AddBloodTrail(TCharacter *cptr)
{
	if (BloodTrail.Count>508) {
	  memcpy(&BloodTrail.Trail[0], &BloodTrail.Trail[1], 510*sizeof(TBloodP));
	  BloodTrail.Count--;
	}
    BloodTrail.Trail[BloodTrail.Count].LTime = 210000;
    BloodTrail.Trail[BloodTrail.Count].pos = cptr->pos;
	BloodTrail.Trail[BloodTrail.Count].pos.x+=siRand(32);
	BloodTrail.Trail[BloodTrail.Count].pos.z+=siRand(32);
	BloodTrail.Trail[BloodTrail.Count].pos.y = 
		GetLandH(BloodTrail.Trail[BloodTrail.Count].pos.x,
		         BloodTrail.Trail[BloodTrail.Count].pos.z)+4;
	BloodTrail.Count++;
}





void MakeCall()
{
   if (!TargetDino) return;
   if (UNDERWATER) return;
   if (ObservMode || TrophyMode) return;
   if (CallLockTime) return;
   
   CallLockTime=1024*3;
   
   NextCall+=(RealTime % 2)+1;
   NextCall%=3;
   char fext[4][2] = {"a","b","c"};

   wsprintf(logt,"HUNTDAT\\SOUNDFX\\CALLS\\call%d_%s.wav", TargetCall-9,fext[NextCall]);

   //AddVoicev(fxCall[TargetCall-10][NextCall].length,  
	         //fxCall[TargetCall-10][NextCall].lpData, 256);

//   PlaySound2d(logt);

   float dmin = 512*256;
   int ai = -1;

   for (int c=0; c<ChCount; c++) {
	 TCharacter *cptr = &Characters[c];

	 if (DinoInfo[AI_to_CIndex[TargetCall] ].DangerCall)
		 if (cptr->AI<10) {
			 cptr->State=2;
			 cptr->AfraidTime = (10 + rRand(5)) * 1024; 
		 }

	 if (cptr->AI!=TargetCall) continue;
	 if (cptr->AfraidTime) continue;
	 if (cptr->State) continue;

	 float d = VectorLength(SubVectors(PlayerPos, cptr->pos));
	 if (d < ctViewR * 400) {
	  if (rRand(128) > 32)
	    if (d<dmin) { dmin = d; ai = c; }
	  cptr->tgx = PlayerX + siRand(1800);
	  cptr->tgz = PlayerZ + siRand(1800);
	 }
   }

   if (ai!=-1) {
	   answpos = SubVectors(Characters[ai].pos, PlayerPos);
       answpos.x/=-3.f; answpos.y/=-3.f; answpos.z/=-3.f;
       answpos = SubVectors(PlayerPos, answpos);
	   answtime = 2000 + rRand(2000);	   
	   answcall = TargetCall;
   }
	              
}



DWORD ColorSum(DWORD C1, DWORD C2) 
{
	DWORD R,G,B;
	R = min(255, ((C1>> 0) & 0xFF) + ((C2>> 0) & 0xFF));
	G = min(255, ((C1>> 8) & 0xFF) + ((C2>> 8) & 0xFF));
	B = min(255, ((C1>>16) & 0xFF) + ((C2>>16) & 0xFF));
	return R + (G<<8) + (B<<16);
}


#define partBlood   1
#define partWater   2
#define partGround  3
#define partBubble  4

void AddElements(float x, float y, float z, int etype, int cnt)
{
}


void MakeShot(float ax, float ay, float az,
              float bx, float by, float bz)
{
}


void RemoveCharacter(int index)
{
}


void AnimateShip()
{

  
}





void AnimateSupplyShip()
{   
}



void AnimateAmmoBag()
{
}


void ProcessTrophy()
{
}



void RespawnSnow(int s, BOOL rand)
{
}




void AnimateElements()
{
}


void AnimateProcesses()
{

  if (ExitTime) {
	ExitTime-=TimeDt;
	if (ExitTime<=0) {
		if (FADING <= 0) {
			DoHalt("");
		}
	}
  }
}



void RemoveCurrentTrophy()
{	
}


void LoadTrophy()
{
    int pr = SelectedPlayer;
	if (SelectedPlayer < 0) return;
	FillMemory(&TrophyRoom, sizeof(TrophyRoom), 0);
	TrophyRoom.RegNumber = pr;
	bool resetScore = false;
	DWORD l;
	char fname[128];
	int rn = TrophyRoom.RegNumber;
	if (pr != 99)
		wsprintf(fname, "trophy0%d.sav", TrophyRoom.RegNumber);
	else {
		wsprintf(fname, "trophyNEW.sav");
		resetScore = true;
	}
	HANDLE hfile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile==INVALID_HANDLE_VALUE) {
		PrintLog("===> Error loading trophy!\n");
		return;
	}
	ReadFile(hfile, &TrophyRoom, sizeof(TrophyRoom), &l, NULL);

	ReadFile(hfile, &OptAgres, 4, &l, NULL);
	ReadFile(hfile, &OptDens , 4, &l, NULL);
	ReadFile(hfile, &OptSens , 4, &l, NULL);

	ReadFile(hfile, &OptRes, 4, &l, NULL);
	ReadFile(hfile, &FOGENABLE, 4, &l, NULL);
	ReadFile(hfile, &OptText , 4, &l, NULL);
	ReadFile(hfile, &OptViewR, 4, &l, NULL);
	ReadFile(hfile, &SHADOWS3D, 4, &l, NULL);
	ReadFile(hfile, &OptMsSens, 4, &l, NULL);
	ReadFile(hfile, &OptBrightness, 4, &l, NULL);


	ReadFile(hfile, &KeyMap, sizeof(KeyMap), &l, NULL);
	ReadFile(hfile, &REVERSEMS, 4, &l, NULL);

	ReadFile(hfile, &ScentMode, 4, &l, NULL);
	ReadFile(hfile, &CamoMode, 4, &l, NULL);
	ReadFile(hfile, &RadarMode, 4, &l, NULL);
	ReadFile(hfile, &Tranq    , 4, &l, NULL);
	ReadFile(hfile, &OPT_ALPHA_COLORKEY, 4, &l, NULL);

	ReadFile(hfile, &OptSys  , 4, &l, NULL);
	ReadFile(hfile, &OptSound , 4, &l, NULL);
	ReadFile(hfile, &OptRender, 4, &l, NULL);
	

	SetupRes();

	CloseHandle(hfile);	 
	TrophyRoom.RegNumber = rn;
	if (resetScore) TrophyRoom.Score = startScore;
	PrintLog("Trophy Loaded.\n");
//	TrophyRoom.Score = 299;
}




void SaveTrophy()
{
	DWORD l;
	char fname[128];
	TrophyRoom.RegNumber = SelectedPlayer;
	wsprintf(fname, "trophy0%d.sav", TrophyRoom.RegNumber);

	int r = TrophyRoom.Rank;
	TrophyRoom.Rank = 0;
	if (TrophyRoom.Score >= 100) TrophyRoom.Rank = 1;
	if (TrophyRoom.Score >= 300) TrophyRoom.Rank = 2;

    
    HANDLE hfile = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		PrintLog("==>> Error saving trophy!\n");
		return;
	}
	WriteFile(hfile, &TrophyRoom, sizeof(TrophyRoom), &l, NULL);

    WriteFile(hfile, &OptAgres, 4, &l, NULL);
	WriteFile(hfile, &OptDens , 4, &l, NULL);
	WriteFile(hfile, &OptSens , 4, &l, NULL);

	WriteFile(hfile, &OptRes, 4, &l, NULL);
	WriteFile(hfile, &FOGENABLE, 4, &l, NULL);
	WriteFile(hfile, &OptText , 4, &l, NULL);
	WriteFile(hfile, &OptViewR, 4, &l, NULL);
	WriteFile(hfile, &SHADOWS3D, 4, &l, NULL);
	WriteFile(hfile, &OptMsSens, 4, &l, NULL);
	WriteFile(hfile, &OptBrightness, 4, &l, NULL);

	WriteFile(hfile, &KeyMap, sizeof(KeyMap), &l, NULL);
	WriteFile(hfile, &REVERSEMS, 4, &l, NULL);	

	WriteFile(hfile, &ScentMode, 4, &l, NULL);
	WriteFile(hfile, &CamoMode , 4, &l, NULL);
	WriteFile(hfile, &RadarMode, 4, &l, NULL);
	WriteFile(hfile, &Tranq    , 4, &l, NULL);
	WriteFile(hfile, &OPT_ALPHA_COLORKEY, 4, &l, NULL);

	WriteFile(hfile, &OptSys   , 4, &l, NULL);
	WriteFile(hfile, &OptSound , 4, &l, NULL);
	WriteFile(hfile, &OptRender, 4, &l, NULL);
	CloseHandle(hfile);
	PrintLog("Trophy Saved.\n");
}

