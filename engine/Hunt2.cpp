#define _MAIN_
#include "Hunt.h"
#include "stdio.h"

//float MouseAtX=0;
//float MouseAtY=0;

#ifdef _soft
BOOL PHONG = FALSE;
BOOL GOUR  = FALSE;
BOOL ENVMAP = FALSE;
#else
BOOL PHONG = TRUE;
BOOL GOUR  = TRUE;
BOOL ENVMAP = TRUE;
#endif

BOOL NeedRVM = TRUE;
float BinocularPower  = 2.5;
float wpshy = 0;
float wpshz = 0;
float wpnb  = 0;
int wpnlight;

void HideWeapon();

char cheatcode[16] = "DEBUGUP";
int  cheati = 0;


void ResetMousePos()
{	
   SetCursorPos(VideoCX, VideoCY);    
}



float CalcFogLevel(Vector3d v)
{  
  if (!FOGON) return 0;
  BOOL vinfog = TRUE;
  int cf;  
  if (!UNDERWATER) {
	  cf = FogsMap[ ((int)(v.z + CameraZ))>>9 ][ ((int)(v.x + CameraX))>>9 ];
      if ((!cf) && CAMERAINFOG) { cf = CameraFogI; vinfog = FALSE; }	  
  } else cf = 127;	        
	  

  if (! (CAMERAINFOG | cf) ) return 0;
  TFogEntity *fptr;
  fptr = &FogsList[cf];
  CurFogColor = fptr->fogRGB;
  
  float d = VectorLength(v);

  v.y+=CameraY;
  
  float fla= -(v.y     - fptr->YBegin*ctHScale) / ctHScale;
  if (!vinfog) if (fla>0) fla=0;

  float flb = -(CameraY - fptr->YBegin*ctHScale) / ctHScale;
  if (!CAMERAINFOG) if (flb>0) flb=0;
  
  if (fla<0 && flb<0) return 0;    
  
  if (fla<0) { d*= flb / (flb-fla); fla = 0; }
  if (flb<0) { d*= fla / (fla-flb); flb = 0; }

  float fl = (fla + flb);

  fl *= (d+(fptr->Transp/2)) / fptr->Transp;
  
  return min(fl, fptr->FLimit);
}



void PreCashGroundModel()
{
   SKYDTime = RealTime>>1;
   int x,y;
   
   int kx = SKYDTime & 255;
   int ky = SKYDTime & 255;
   int SKYDT = SKYDTime>>8;

   VideoCX16 = VideoCX * 16;
   VideoCY16 = VideoCY * 16;
   VideoCXf  = (float)VideoCX;
   VideoCYf  = (float)VideoCY;
   CameraW16 = CameraW * 16;
   CameraH16 = CameraH * 16;

   BOOL FogFound = FALSE;
   NeedWater = FALSE;
   
   MapMinY = 10241024;
   Vector3d rv;


   for (y=-(ctViewR+3); y<(ctViewR+3); y++)
	for (x=-(ctViewR+3); x<(ctViewR+3); x++) {
  					
	  int r = max((max(y,-y)), (max(x,-x)));
	  if ( r>ctViewR1+4 )
	    if ( (x & 1) + (y & 1) > 0) continue; 	  

	  int xx = (CCX + x) & 1023;
	  int yy = (CCY + y) & 1023;      

	  v[0].x = xx*256 - CameraX;
      v[0].z = yy*256 - CameraZ;	  
	  v[0].y = (float)((int)HMap[yy][xx])*ctHScale - CameraY;


//========= water section ===========//

	//if (RunMode) 
	  if ((FMap[yy][xx] & fmWaterA)>0) {
		  rv = v[0];
		  rv.y = WaterList[ WMap[yy][xx] ].wlevel*ctHScale - CameraY; 

		  float wdelta = (float)sin(-pi/2 + RandomMap[yy & 31][xx & 31]/128+RealTime/200.f);          

		  if ( (FMap[yy][xx] & fmWater) && (r < ctViewR1-4)) {
           rv.x+=(float)sin(xx+yy + RealTime/200.f) * 16.f;
           rv.z+=(float)sin(pi/2.f + xx+yy + RealTime/200.f) * 16.f;
		  }

          rv = RotateVector(rv);
		  VMap2[128+y][128+x].v = rv;
	      
		  if (fabs(rv.x) > -rv.z + 1524) {		      
		   VMap2[128+y][128+x].DFlags = 128;		  
		  } else {
			NeedWater = TRUE;
			VMap2[128+y][128+x].Light = 168-(int)(wdelta*24);
			
			float Alpha;
			if (UNDERWATER) {				
				Alpha =	160 - VectorLength(rv)* 160 / 220 / ctViewR;
				if (Alpha<10) Alpha=10;
			} else
            if (r < ctViewR1+2) {		
			 int wi = WMap[yy][xx];
			 Alpha = (float)((WaterList[wi].wlevel - HMap[yy][xx])*2+4)*WaterList[wi].transp;
			 Alpha+=VectorLength(rv) / 256;
			 Alpha+=wdelta*2;
			 if (Alpha<0) Alpha=0;
			 Vector3d va = v[0]; 
			 NormVector(va,1.0f); va.y=-va.y;
			 if (va.y<0) va.y=0;
			 Alpha*=6.f/(va.y+0.1f);
			 if (Alpha>255) Alpha=255.f;
			} else Alpha = 255.f;

			VMap2[128+y][128+x].ALPHA=(int)Alpha;
			VMap2[128+y][128+x].Fog = 0;

			if (rv.z>-256.0) VMap2[128+y][128+x].DFlags=128; else {
#ifdef _soft
	          VMap2[128+y][128+x].scrx = VideoCX - (int)(rv.x / rv.z * CameraW);
	          VMap2[128+y][128+x].scry = VideoCY + (int)(rv.y / rv.z * CameraH);			  
			  			  
			  int DF = 0;
              if (VMap2[128+y][128+x].scrx < 0)     DF+=1;
	          if (VMap2[128+y][128+x].scrx > WinEX) DF+=2;
	          if (VMap2[128+y][128+x].scry < 0)     DF+=4;
	          if (VMap2[128+y][128+x].scry > WinEY) DF+=8;
#endif
#ifdef _3dfx
			  VMap2[128+y][128+x].scrx = VideoCX16 - (int)(rv.x / rv.z * CameraW16);
	          VMap2[128+y][128+x].scry = VideoCY16 + (int)(rv.y / rv.z * CameraH16);			  
			  			  
			  int DF = 0;
              if (VMap2[128+y][128+x].scrx < 0)        DF+=1;
	          if (VMap2[128+y][128+x].scrx > WinEX*16) DF+=2;
	          if (VMap2[128+y][128+x].scry < 0)        DF+=4;
	          if (VMap2[128+y][128+x].scry > WinEY*16) DF+=8;
#endif
#ifdef _d3d
			  VMap2[128+y][128+x].scrx = VideoCXf - (rv.x / rv.z * CameraW);
	          VMap2[128+y][128+x].scry = VideoCYf + (rv.y / rv.z * CameraH);			  
			  			  
			  int DF = 0;
              if (VMap2[128+y][128+x].scrx < 0)        DF+=1;
	          if (VMap2[128+y][128+x].scrx > WinEX)    DF+=2;
	          if (VMap2[128+y][128+x].scry < 0)        DF+=4;
	          if (VMap2[128+y][128+x].scry > WinEY)    DF+=8;
#endif


			  VMap2[128+y][128+x].DFlags = DF;

			} 	              
		  }			  
	  }



#ifdef _soft
#else	  
	  if (r>ctViewR1-20 && r<ctViewR1+8)
		 if ( (x & 1) + (y & 1) > 0)
	  {
	   float y1;
	   float zd = (float)sqrt(v[0].x*v[0].x + v[0].z*v[0].z) / 256.f;
	   float k = (zd - (ctViewR1-8)) / 4.f;
	   if (k<0) k=0;
	   if (k>1) k=1;

	   if ((y & 1)==0) y1 = (float)((int)HMap[yy][xx-1]+HMap[yy][xx+1])*ctHScale/2 - CameraY; else
	   if ((x & 1)==0) y1 = (float)((int)HMap[yy-1][xx]+HMap[yy+1][xx])*ctHScale/2 - CameraY; else
		               y1 = (float)((int)HMap[yy-1][xx-1]+HMap[yy+1][xx+1])*ctHScale/2 - CameraY;

	   v[0].y = ((v[0].y+2) * (1-k) + (y1+8) * k);
	  }
#endif

      rv = RotateVector(v[0]);

	  
      if (fabs(rv.x * FOVK) > -rv.z + 1600) {
		  VMap[128+y][128+x].v = rv;   
		  VMap[128+y][128+x].DFlags = 128;
		  continue;
	  }
      

  	  if (HARD3D) 	
		if (  ((FMap[yy][xx] & fmWater)==0) || UNDERWATER)
		  VMap[128+y][128+x].Fog = CalcFogLevel(v[0]); else
		  VMap[128+y][128+x].Fog = 0;

      VMap[128+y][128+x].ALPHA = 255;

      v[0]=rv;	  

	  if (v[0].z<1024)
	   if (FOGENABLE)
	 	 if (FogsMap[yy>>1][xx>>1]) FogFound = TRUE;      
      	  
	  VMap[128+y][128+x].v = v[0];      
            
      int  DF = 0;
      int  db = 0;

      if (v[0].z<256) {
       if (Clouds) {
	    int shmx = (xx + SKYDT) & 127;
	    int shmy = (yy + SKYDT) & 127;
	   
	    int db1 = SkyMap[shmy * 128 + shmx ];
	    int db2 = SkyMap[shmy * 128 + ((shmx+1) & 127) ];
	    int db3 = SkyMap[((shmy+1) & 127) * 128 + shmx ];
	    int db4 = SkyMap[((shmy+1) & 127) * 128 + ((shmx+1) & 127) ];
	    db = (db1 * (256 - kx) + db2 * kx) * (256-ky) +
	         (db3 * (256 - kx) + db4 * kx) * ky;     
        db>>=17;
	    db = db - 40;
	    if (db<0) db=0;
	    if (db>48) db=48;
	   } 
      
       int clt = LMap[yy][xx];
	   clt= max(64, clt-db);
       VMap[128+y][128+x].Light = clt;	   
      }	  

	  

	  if (v[0].z>-256.0) DF+=128; else { 	   		  	                

#ifdef _soft
	   VMap[128+y][128+x].scrx = VideoCX - (int)(v[0].x / v[0].z * CameraW);
	   VMap[128+y][128+x].scry = VideoCY + (int)(v[0].y / v[0].z * CameraH);

	   if (VMap[128+y][128+x].scrx < 0)        DF+=1;
	   if (VMap[128+y][128+x].scrx > WinEX)    DF+=2;
	   if (VMap[128+y][128+x].scry < 0)        DF+=4;
	   if (VMap[128+y][128+x].scry > WinEY)    DF+=8;
#endif
#ifdef _3dfx
	   VMap[128+y][128+x].scrx = VideoCX16 - (int)(v[0].x / v[0].z * CameraW16);
	   VMap[128+y][128+x].scry = VideoCY16 + (int)(v[0].y / v[0].z * CameraH16);

	   if (VMap[128+y][128+x].scrx < 0)        DF+=1;
	   if (VMap[128+y][128+x].scrx > WinEX*16) DF+=2;
	   if (VMap[128+y][128+x].scry < 0)        DF+=4;
	   if (VMap[128+y][128+x].scry > WinEY*16) DF+=8;
#endif
#ifdef _d3d
	   VMap[128+y][128+x].scrx = VideoCXf - (v[0].x / v[0].z * CameraW);
	   VMap[128+y][128+x].scry = VideoCYf + (v[0].y / v[0].z * CameraH);

	   if (VMap[128+y][128+x].scrx < 0)        DF+=1;
	   if (VMap[128+y][128+x].scrx > WinEX)    DF+=2;
	   if (VMap[128+y][128+x].scry < 0)        DF+=4;
	   if (VMap[128+y][128+x].scry > WinEY)    DF+=8;
#endif       
	  }
	   
      VMap[128+y][128+x].DFlags = DF;
	}

	FOGON = FogFound || UNDERWATER;
}




void AddShadowCircle(int x, int y, int R, int D)
{
  if (UNDERWATER) return;
  
  int cx = x / 256;
  int cy = y / 256;
  int cr = 1 + R / 256;
  for (int yy=-cr; yy<=cr; yy++)
   for (int xx=-cr; xx<=cr; xx++) {
     int tx = (cx+xx)*256;
     int ty = (cy+yy)*256;
     int r = (int)sqrt( float(tx-x)*float(tx-x) + float(ty-y)*float(ty-y) );
     if (r>R) continue;
     VMap[cy+yy - CCY + 128][cx+xx - CCX + 128].Light-= D * (R-r) / R;     
	 if (VMap[cy+yy - CCY + 128][cx+xx - CCX + 128].Light < 32)
		 VMap[cy+yy - CCY + 128][cx+xx - CCX + 128].Light = 32;
   }
}





void DrawScene()
{       
   dFacesCount = 0;

   ca = (float)cos(CameraAlpha);
   sa = (float)sin(CameraAlpha);      

   cb = (float)cos(CameraBeta);
   sb = (float)sin(CameraBeta);
   
   CCX = ((int)CameraX / 512) * 2;
   CCY = ((int)CameraZ / 512) * 2;

   PreCashGroundModel();         

#ifdef _soft
   CreateChRenderList();
#endif
   
   RenderSkyPlane();   

   cb = (float)cos(CameraBeta);
   sb = (float)sin(CameraBeta);   
   

   RenderGround();

   RenderModelsList();

   Render3DHardwarePosts();
   
   if (NeedWater) RenderWater();

   RenderElements();
}




void DrawOpticCross( int v)
{
   int sx =  VideoCX + (int)(rVertex[v].x / (-rVertex[v].z) * CameraW);
   int sy =  VideoCY - (int)(rVertex[v].y / (-rVertex[v].z) * CameraH); 

   if (  (fabs(float(VideoCX - sx)) > WinW / 2) ||
	     (fabs(float(VideoCY - sy)) > WinH / 4) ) return;

   Render_Cross(sx, sy,(WinW/12)/16,false);
}



void ScanLifeForms()
{
	int li = -1;
	float dm = (float)(ctViewR+2)*256;
	for (int c=0; c<ChCount; c++) {
		TCharacter *cptr = &Characters[c];
		if (!cptr->Health) continue;
		if (cptr->rpos.z > -512) continue;
		float d = (float)sqrt( cptr->rpos.x*cptr->rpos.x + cptr->rpos.y*cptr->rpos.y + cptr->rpos.z*cptr->rpos.z );
		if (d > ctViewR*256) continue;
        float r = (float)(fabs(cptr->rpos.x) + fabs(cptr->rpos.y)) / d;
		if (r > 0.15) continue;
        if (d<dm) 
		  if (!TraceLook(cptr->pos.x, cptr->pos.y+220, cptr->pos.z,
			             CameraX, CameraY, CameraZ) ) {
		
          dm = d;
		  li = c;
		}

	}

    if (li==-1) return;
	Render_LifeInfo(li);
}


void DrawPostObjects()
{ 
  float b;  
  Hardware_ZBuffer(TRUE);

  //-> Draw Menu Pic...
//  MENU_LOGIN 0
//#define MENU_WARNING 1
//#define MENU_MAIN 2
//#define MENU_OPTIONS 3
//#define MENU_STATS 4
//#define MENU_SELECT 5
//#define MENU_CREDITS 6
  DrawPicture( 0, 0, GameMenus[CURRENT_MENU].Image);
  wsprintf(ActiveInfoText,""); //Clear it. Use memcpy or something better than wsprintf

  switch (CURRENT_MENU) {
	  case MENU_OPTIONS:
		  	  //-> Draw sliders, buttons, and previews
			  Gui_ProcessAll();
			  break;
	  //-> Draw Menu related pictures...
	  case MENU_PREHUNT:
		  {
			  //-> Draw sliders, buttons, and previews
			  Gui_ProcessAll();
			  //-> Previews
			  int ClickedSlot; //HoveringOver
			  int FieldOffset; //Offset by slider bars
			  POINT ms;
			  GetCursorPos(&ms);

			  if (ms.y > 291 && ms.y < 322) {
				  //-> Hovering Over dawn
				  if (ms.x > 8 && ms.x < 69) {
					  strcpy(ActiveInfoText, DawnTxt);
				  }
				  //-> Hovering Over day
				  if (ms.x > 70 && ms.x < 132) {
					  strcpy(ActiveInfoText, DayTxt);
				  }
				  //-> Hovering Over night
				  if (ms.x > 133 && ms.x < 195) {
					  strcpy(ActiveInfoText, NightTxt);
				  }
				  //-> Hovering Over observer
				  if (ms.x > 606 && ms.x < 793) {
					  strcpy(ActiveInfoText, ObservTxt);
				  }
			  }

			  if (ms.x > 15 && ms.x < 180 && ms.y > 382 && ms.y < 545) {
				  //-> Hovering Over map
				  ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
				  FieldOffset = Gui_GetSliderValue("slider_maps")/2; //<- Make it less sensitive
				  if (strlen(MapFile[ClickedSlot+FieldOffset].name) >0 ) {
					DrawPicture(38,73,MapFile[ClickedSlot+FieldOffset].image);
					strcpy(ActiveInfoText,MapFile[ClickedSlot+FieldOffset].desc);
				  }
			  }


			  if (ms.x > 215 && ms.x < 380 && ms.y > 382 && ms.y < 545) {
				  //-> Hovering Over dino
				  ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
				  FieldOffset = Gui_GetSliderValue("slider_dinos")/2; //<- Make it less sensitive
				  if (strlen(DinoInfo[(ClickedSlot+FieldOffset+7)].Name) >0 ) {
					  if (DinoInfo[(ClickedSlot + FieldOffset + 7)].Hide && TrophyRoom.Score < DinoInfo[(ClickedSlot + FieldOffset + 7)].Price) {
						  DrawPicture(38, 73, DinoInfo[(ClickedSlot + FieldOffset + 7)].MenuPicHidden);
					  } else {
						  DrawPicture(38, 73, DinoInfo[(ClickedSlot + FieldOffset + 7)].MenuPic);
						  strcpy(ActiveInfoText, DinoInfo[(ClickedSlot + FieldOffset + 7)].MenuTxt);
						  DinoStatType = 1;
						  DinoStatIndex = (ClickedSlot + FieldOffset + 7);
					  }
				  }

			  }

			  if (ms.x > 410 && ms.x < 574 && ms.y > 382 && ms.y < 545) {
				  //-> Hovering over weapon
				  // -> Play Sound
				  ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
				  FieldOffset = Gui_GetSliderValue("slider_weapons")/2; //<- Make it less sensitive
				  if (strlen(WeapInfo[(ClickedSlot+FieldOffset)].Name) > 0) {
					  DrawPicture(38,73,WeapInfo[(ClickedSlot+FieldOffset)].MenuPic);
					  strcpy(ActiveInfoText,WeapInfo[ClickedSlot+FieldOffset].MenuTxt);
					  DinoStatType = 2;
					  DinoStatIndex = (ClickedSlot + FieldOffset);
					}
			  }
			 
			  if (ms.x > 610 && ms.x < 774 && ms.y > 382 && ms.y < 545) {
				  //-> Hovering over acessory
				  // -> Play Sound
				  ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
				  //FieldOffset = Gui_GetSliderValue("slider_acess")/2; //<- Make it less sensitive
				  if (strlen(AcessInfo[(ClickedSlot)].name) > 0) {
					  DrawPicture(38,73,AcessInfo[(ClickedSlot)].MenuPic);
					  strcpy(ActiveInfoText,AcessInfo[ClickedSlot].MenuTxt);
					}
			  }
			  break;
		  }
  }
 

  if (EXITMODE)
	DrawPicture( (WinW - ExitPic.W) / 2, (WinH - ExitPic.H) / 2, ExitPic);


  if (PAUSE)   
	DrawPicture( (WinW - PausePic.W) / 2, (WinH - PausePic.H) / 2, PausePic);

}





void SwitchMode(LPSTR lps, BOOL& b)
{
  b = !b;
  char buf[200];
  if (b) wsprintf(buf,"%s is ON", lps);
    else wsprintf(buf,"%s is OFF", lps);
  //MessageBeep(0xFFFFFFFF);
  AddMessage(buf);
}


void ChangeViewR(int d1, int d2, int d3)
{
  char buf[200];
  ctViewR +=d1;
  ctViewR1+=d2;
  ctViewRM+=d3;
  if (ctViewR<20) ctViewR = 20;
  if (ctViewR>122) ctViewR = 122;

  if (ctViewR1 < 12) ctViewR1=12;
  if (ctViewR1 > ctViewR-10) ctViewR1=ctViewR-10;
  if (ctViewRM <  4) ctViewRM = 4;
  if (ctViewRM > 60) ctViewRM = 60;
  
  wsprintf(buf,"ViewR = %d (%d + %d) BMP at %d", ctViewR, ctViewR1, ctViewR-ctViewR1, ctViewRM);
  //MessageBeep(0xFFFFFFFF);
  AddMessage(buf);
  
}


void ChangeCall()
{
	if (!TargetDino) return;
	if (ChCallTime)
	for (int t=0; t<32; t++) {
		TargetCall++;
		if (TargetCall>32) TargetCall=10;
		if (TargetDino & (1<<TargetCall)) break;
	}
	//wsprintf(logt,"Call: %s", DinoInfo[ AI_to_CIndex[TargetCall] ].Name);
	//AddMessage(logt);
	//CallLockTime+= 1024;
	ChCallTime = 2048;
}


void CallSupply()
{
	//if (!Supply) return;
    //if (SupplyUsed) return;
    //SupplyUsed = TRUE;
	if (TrophyRoom.Score < 102) {
		AddMessage("Request denied by base. Check credit balance");
		return;
	}
	if (SupplyShip.State) {
		AddMessage("Request denied by base");
		return;
	}
	AddMessage("Supplies Requested... Stand By");
    AddVoicev(SShipModel.SoundFX[1].length,
		      SShipModel.SoundFX[1].lpData, 256);
    
    SupplyShip.pos.x = PlayerX - (ctViewR+4)*256; if (SupplyShip.pos.x < 256) SupplyShip.pos.x = PlayerX + (ctViewR+4)*256; 
	SupplyShip.pos.z = PlayerZ - (ctViewR+4)*256; if (SupplyShip.pos.z < 256) SupplyShip.pos.z = PlayerZ + (ctViewR+4)*256; 	
	SupplyShip.pos.y = GetLandUpH(SupplyShip.pos.x, SupplyShip.pos.z)  + 2048;

	SupplyShip.tgpos.x = PlayerX;
	SupplyShip.tgpos.z = PlayerZ;
    SupplyShip.tgpos.y = GetLandUpH(SupplyShip.tgpos.x, SupplyShip.tgpos.z) + 2048;
	SupplyShip.State = 1;

	SupplyShip.retpos = SupplyShip.pos;
	//->Charge the player for the ammo...
	TrophyRoom.Score -= 2;
}


void ToggleBinocular()
{
  if (Weapon.state) return;
  if (UNDERWATER) return;
  if (!MyHealth) return;
  BINMODE = !BINMODE;  
  MapMode = FALSE;
}


void ToggleRunMode()
{
	RunMode = !RunMode;
    if (RunMode) 
        AddMessage("Run mode is ON");
	else AddMessage("Run mode is OFF");
}

void EnableSprint() {
	// == Added 4.13.09 by Adelphospro == //
	// Enables Sprint Mode feature after some 
	// preliminary tests are passed, such as energy and health level check
	if (!SWIM && !CrouchMode && MyEnergy >= (MAX_ENERGY*.15) && (VSpeed != 0 || SSpeed != 0) && !YSpeed) {
		SprintMode = TRUE;
		Weapon.ironsights = 99;
	}
}

void DisableSprint() {
	// == Added 4.14.09 by Adelphospro == //
	// Disables sprint and sets animation codes
	SprintMode = FALSE;
    // -> set to 1 (going rest animation)
	Weapon.ironsights = 1;
}

void ToggleCrouchMode()
{
	CrouchMode = !CrouchMode;
	if (CrouchMode) AddMessage("Crouch mode is ON");
	else AddMessage("Crouch mode is OFF");
}


void ToggleMapMode()
{
	if (!MyHealth) return;
	if (BINMODE) return;
	if (Weapon.state || SprintMode) return;
	MapMode = !MapMode;	
}




void ShowShifts()
{
	sprintf(logt, "Y=%3.4f  Z=%3.4f  A=%3.4f", wpshy/2, wpshz/2, wpnb*180/3.1415);
	AddMessage(logt);
}

//void TypedText_clear() {
//	TypingMode = FALSE;
//	memset(TypedText, 0, sizeof(TypedText));
//	TypedTextLength = 0;
//}

void GetKeyChar(int ID, char* stringholder)
{
	HKL keybLayout = GetKeyboardLayout(NULL);
	BYTE kbdState [256];
	GetKeyboardState(kbdState);
	int resULT = ToUnicodeEx(ID,NULL,kbdState,(LPWSTR)stringholder,sizeof(stringholder),NULL,keybLayout);
}

LONG APIENTRY MainWndProc( HWND hWnd, UINT message, UINT wParam, LONG lParam)
{  

	BOOL A = (GetActiveWindow() == hWnd);	
	bool JustExitInit = false;

	if (A!=blActive) {
		blActive = A;	   	   

		// alacn
		if (blActive)
#ifdef __high_priority_process
			SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#else
			SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
		else
			SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS);	   

		if (!blActive) {		  
			ShutDown3DHardware();		  
			NeedRVM = TRUE;
		}

		if (blActive) {           
			Audio_Restore();		   		   
			NeedRVM = TRUE;
		}

	}
	TypingJustExit = false;
	if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) { //<- Modified to catch system keys too
		if (DEBUG) {
			//-> Display entered item...
			wsprintf(logt,"KeyChar: %s, KeyCode: %d",KeysName[int(wParam)],int(wParam));
			AddMessage(logt);
		}
		if (TypingMode && !EXITMODE && !AKeySelected) {
			if (Weapon.state)
				HideWeapon();
			//Process Typed Word
			switch ((int)wParam) {
				case 192: // -> Little key next to 1
				case VK_ESCAPE: 
					//== Clear all text ==
					//AddVoicev(TypeSound[0].length, 
					  //TypeSound[0].lpData, 128);
					if (CURRENT_MENU != 0 ) {
						//PlaySound2d("HUNTDAT\\SOUNDFX\\type.wav");
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
						TypingJustExit = true;
						Console_Clear();
					} else {
						//Display ExitMenu
						EXITMODE = TRUE;
						JustExitInit = true;
					}
					break;
				case VK_BACK: 
					//== Backspace ==
					AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					//PlaySound2d("HUNTDAT\\SOUNDFX\\type.wav");
					if (TypedTextLength == 0) break;
					TypedText[TypedTextLength-1] = 0;
					TypedTextLength--;
					break;
				case VK_RETURN: 
					//== Process Typed Text ==
					//-> First process 'default button' for a given menu
					switch (CURRENT_MENU) {
						case MENU_LOGIN:
							ProcessButton1();
							break;
						default:
							//-> Now process debug commands for menus without default button and typing enabled (All but login)
							Console_ProcessInput(NULL);
							break;
					}
					break;
				default: //Update Text
					{
					HKL keybLayout = GetKeyboardLayout(NULL);
					BYTE kbdState [256];
					GetKeyboardState(kbdState);
					char keyBuff[4];
					int resULT = ToUnicodeEx(wParam,NULL,kbdState,(LPWSTR)keyBuff,sizeof(keyBuff),NULL,keybLayout);
					if ( int(wParam) > 32 && (int)wParam < 126 || (int)wParam == 189 ) { //NOTE: Dont allow white spaces for name
						// -> Is a valid key...
						wsprintf(logt,"%s",keyBuff);

						TypedText[TypedTextLength] = logt[0];
						TypedTextLength++;
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					} else if (DEBUG) {
						wsprintf(logt,"TypingMode->KeyCode Entered: %d",wParam);
						AddMessage(logt);
					}
					break;
					}
			}
			
		} else if (!AKeySelected) {
		if ((int)wParam == KeyMap.fkBinoc) ToggleBinocular();
		if ((int)wParam == KeyMap.fkCCall) ChangeCall();

		//if ((int)wParam == KeyMap.fkSupply) CallSupply();

		if ((int)wParam == KeyMap.fkRun  ) ToggleRunMode();
		if ((int)wParam == KeyMap.fkCrouch) ToggleCrouchMode();

		} else if (AKeySelected) {
			//-> Update the key
			 *PtrSelectedKey = (int)wParam; //The area of memory being pointed to by Prt = the value
			 AKeySelected = false;
			 AddVoicev(TypeSound[1].length,TypeSound[1].lpData, 256);
		}
	}


	switch (message) {        	    
		case WM_CREATE: return 0;                        


		case WM_KEYDOWN: {
			BOOL CTRL = (GetKeyState(VK_SHIFT) & 0x8000);
			if (!TypingMode || EXITMODE) {
				switch( (int)wParam ) {

		case 192: //Little key next to 1
			if (!TypingMode && !TypingJustExit) {
				TypingMode = TRUE;
				//PlaySound2d("HUNTDAT\\SOUNDFX\\type.wav");
				AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
			}
			break;


		case 'N':
			if (EXITMODE) EXITMODE = FALSE;
			break;

		case VK_ESCAPE:	
			if (!JustExitInit) {
				if (TypingJustExit == true) {
					TypingJustExit = false;
				} else {		
					if (PAUSE) PAUSE = FALSE; 
					else EXITMODE = !EXITMODE;
					if (ExitTime) EXITMODE = FALSE;
					//ResetMousePos(); 
				}
			}
			break;

		case 'Y':
		case VK_RETURN: 
			if (EXITMODE ) {
				if (MyHealth) {
					ExitTime = 1500;
					FADING = 64;
				} else {
					ExitTime = 1;
				}
				EXITMODE = FALSE;
			}
			break;

		case VK_F12: SaveScreenShot();              break;

				}   // switch  
			}

			break;     }                                

		case WM_DESTROY:       
			PostQuitMessage(0);			
			break;
			/*
			case WM_PAINT:
			case WM_ERASEBKGND:
			case WM_NCPAINT   : break;
			*/
			/*case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC  hdc =  BeginPaint(hWnd, &ps );         		  
			EndPaint(hWnd, &ps);        		  		  
			return 0; 
			} */
		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}




BOOL CreateMainWindow()
{
	PrintLog("Creating main window...");
    WNDCLASS wc;
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)MainWndProc; 
    wc.cbClsExtra = 0;                  
    wc.cbWndExtra = 0;                  
    wc.hInstance = (HINSTANCE)hInst;
    wc.hIcon = wc.hIcon = (HICON)LoadIcon((HINSTANCE)hInst,"ACTION");
    wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = NULL;
    //wc.lpfnWndProc  = NULL;
	wc.lpszClassName = "HuntRenderWindow";
    if (!RegisterClass(&wc)) return FALSE;
    
    hwndMain = CreateWindow(
        "HuntRenderWindow","Carnivores CCE Menu",
  		WS_VISIBLE |  WS_POPUP,
		0, 0, 0, 0, NULL,  NULL, (HINSTANCE)hInst, NULL );

	if (hwndMain)
	  PrintLog("Ok.\n");

    return TRUE;
}




void ProcessShootThreaded(void*) {
	// == Adelphos pro == //
	// -> 4.16.09
	// -> Threaded ProcessShoot of moving bullet
	for (int s=0; s<=WeapInfo[CurrentWeapon].TraceC; s++) {
		float rA = 0;
		float rB = 0;

		rA = siRand(128) * 0.00010f * (2.f - WeapInfo[CurrentWeapon].Prec);
		rB = siRand(128) * 0.00010f * (2.f - WeapInfo[CurrentWeapon].Prec);


		float ca = (float)cos(PlayerAlpha + wpnDAlpha + rA);
		float sa = (float)sin(PlayerAlpha + wpnDAlpha + rA);
		float cb = (float)cos(PlayerBeta + wpnDBeta + rB);
		float sb = (float)sin(PlayerBeta + wpnDBeta + rB);

		nv.x=sa;
		nv.y=0; 
		nv.z=-ca;

		nv.x*=cb;
		nv.y=-sb;
		nv.z*=cb;

		MakeShot(PlayerX, PlayerY+HeadY, PlayerZ,
			PlayerX+nv.x * 256*ctViewR, 
			PlayerY+nv.y * 256*ctViewR + HeadY, 
			PlayerZ+nv.z * 256*ctViewR);
	}
}

void ProcessButton1() {
	//==== Button 1 was clicked ====//
	switch (CURRENT_MENU) {
		case MENU_LOGIN:
			//-> Login was clicked. Process Login
			// -> Loop through users to see if you can find one with the same name to use....
			SelectedPlayer = -1;
			for (int fp = 0; fp < TOTAL_PLAYERS; fp++) {
				if ( !strcmp(PlayerFile[fp].TrophyRoom.PlayerName,TypedText) ) {
					//-> We found it!
					SelectedPlayer = fp;
				}
			}
			// -> Create a new user if need be
			if (SelectedPlayer == -1 && (int)strlen(TypedText) > 0 && (int)strlen(TypedText) < 128) {
				//-> Create new user prompt warning menu...
				wsprintf(CREATE_USER_NAME,"");
				strcpy(CREATE_USER_NAME,TypedText);
				Console_Clear();
				TypingMode = FALSE;
				CURRENT_MENU = MENU_WARNINGUSER;
			} else if (SelectedPlayer != -1){
				//-> Do login and go to login menu
				LoadTrophy();
				InitGUIitems(); //Init GUI objects
				Console_Clear();
				TypingMode = FALSE;
				CURRENT_MENU = MENU_MAIN;
			}
			break;
		case MENU_PREHUNT:
			//-> Dawn was selected
			SelectedDay = 0;
			break;
		case MENU_WARNINGUSER:
			//-> Accept button was pressed
			// -> Create a new user trophy file (Save the existing one) and login
			SelectedPlayer = 99;
			LoadTrophy();
			wsprintf(TrophyRoom.PlayerName,"");
			strcpy(TrophyRoom.PlayerName,CREATE_USER_NAME);
			SelectedPlayer = TOTAL_PLAYERS;
			SaveTrophy();
			//-> Reload total users and data...
			LoadUserList();
			//-> Go to main menu
			CURRENT_MENU = MENU_MAIN;
			break;
		case MENU_DELETEUSER:
			//-> Delete button was pressed
			// -> Delete the user
			wsprintf(logt,"trophy0%d.sav",SelectedPlayer);
			remove(logt);
			//-> Reload total users and data...
			LoadUserList();
			//-> Return to login
			CURRENT_MENU = MENU_LOGIN;
			SelectedPlayer = 0;
			break;
		case MENU_MAIN:
			//-> Goto Hunt was pressed
			CURRENT_MENU = MENU_PREHUNT;
			break;
		case MENU_QUIT:
			//->Yes to Quit Game was pressed
			SaveTrophy();
			ExitTime = 1500;
			FADING = 64;
			break;
	}
}

void ProcessButton2() {
	//==== Button 2 was clicked ====//
	switch (CURRENT_MENU) {
		case MENU_PREHUNT:
			//-> Day was selected
			SelectedDay = 1;
			break;
		case MENU_LOGIN:
			//-> Delete was clicked. Process Delete User
			SelectedPlayer = -1;
			for (int fp = 0; fp < TOTAL_PLAYERS; fp++) {
				if ( !strcmp(PlayerFile[fp].TrophyRoom.PlayerName,TypedText) ) {
					//-> We found it!
					SelectedPlayer = fp;
				}
			}
			if (SelectedPlayer != -1) {
				Console_Clear();
				TypingMode = FALSE;
				CURRENT_MENU = MENU_DELETEUSER;
			} //-> Otherwise, no value user was selected for delete
			break;
		case MENU_DELETEUSER:
		case MENU_WARNINGUSER:
			//-> Cancel was pressed. Return to login
			wsprintf(CREATE_USER_NAME,"");
			CURRENT_MENU = MENU_LOGIN;
			break;
		case MENU_MAIN:
			//-> Options was pressed
			CURRENT_MENU = MENU_OPTIONS;
			break;
		case MENU_QUIT:
			//-> No to quit game was pressed
			CURRENT_MENU = MENU_MAIN;
			break;
	}
}

void ProcessButton6() {
	//==== Button 6 was pressed ====//
	switch (CURRENT_MENU) {
		case MENU_MAIN:
			//-> Stats was pressed
			CURRENT_MENU = MENU_STATS;
			break;
		case MENU_PREHUNT:
			//-> Observer Mode was pressed
			ObservMode = !ObservMode;
			break;
	}
}



void LaunchCarnProcess() {
	//=====================//
	//-> Set logt to the full commandline before calling this
	STARTUPINFO info={sizeof(info)};
	PROCESS_INFORMATION processInfo;
	DWORD cl;
	cl = DDSCL_NORMAL|DDSCL_FULLSCREEN; //<- For menu mode

	HRESULT hres = lpDD->SetCooperativeLevel( hwndMain, cl);
	if( hres != DD_OK )  {
		wsprintf(logt, "Launch:SetCooperativeLevel Normal Error: %Xh\n", hres);
		PrintLog(logt);
		DoHalt("");
	}
	if (CreateProcess(NULL, logt, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
	{
		//-> Wait for game to close....
		::WaitForSingleObject(processInfo.hProcess, INFINITE);
		//->Done
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		//-> Reload Trophy
		LoadTrophy();
		//-> Restore menu
		cl = DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN; //<- For menu mode
		hres = lpDD->SetCooperativeLevel( hwndMain, cl);
		if( hres != DD_OK )  {
			wsprintf(logt, "LaunchDone:SetCooperativeLevel Exclusive Error: %Xh\n", hres);
			PrintLog(logt);
			DoHalt("");
		}
		NeedRVM = TRUE;
	} else {
		PrintLog("Failed to launch\n");
		AddMessage("FAIL");
	}

}


void ProcessButton7() {
	//==== Button 7 was pressed ====//
	switch (CURRENT_MENU) {
	case MENU_PREHUNT:
	{
		//-> Back was pressed
		CURRENT_MENU = MENU_MAIN;
		break;
	}
	case MENU_MAIN:
	{
		//survival
		char Renderer[128];
		//-> Get Renderer...
		switch (OptRender) {
		case 0:
			strcpy(Renderer, "v_soft");
			break;
		case 1:
			strcpy(Renderer, "v_3dfx");
			break;
		case 2:
			strcpy(Renderer, "v_d3d");
			break;
			//case 3:
			//	strcpy(Renderer,"v_d3dsoft");
			//	break;
		}
		wsprintf(logt, "%s.ren prj=huntdat\\areas\\area%d reg=%d dtm=%d wep=%d din=0 -survival",
			Renderer,
			survivalArea,
			TrophyRoom.RegNumber,
			survivalDTM,
			1 << (survivalWeapon - 1));
			
		//MessageBox(hwndMain, logt, "test", MB_OK | MB_ICONHAND); // alacn
		LaunchCarnProcess();
		break;
	}
	}
}

void ProcessButton3() {
	//==== Button 3 was clicked ====//
	switch (CURRENT_MENU) {
		case MENU_PREHUNT:
			//-> Night was selected
			SelectedDay = 2;
			break;
		case MENU_MAIN:
			{
				//-> Trophy
				char Renderer[128];
				//-> Get Renderer...
				switch (OptRender) {
					case 0:
						strcpy(Renderer,"v_soft");
						break;
					case 1:
						strcpy(Renderer,"v_3dfx");
						break;
					case 2:
						strcpy(Renderer,"v_d3d");
						break;
					//case 3:
					//	strcpy(Renderer,"v_d3dsoft");
					//	break;
				}
				wsprintf(logt,"%s.ren prj=huntdat\\areas\\trophy reg=%d dtm=1",Renderer,TrophyRoom.RegNumber);
				LaunchCarnProcess();
				break;
			}
	}

}
void ProcessButton4() {
	//==== Button 4 was clicked ====//
	switch (CURRENT_MENU) {
		case MENU_OPTIONS:
			//-> Back was pressed. Return to main menu
			// -> Save Settings....
			SaveTrophy();
			// -> Go Back
			CURRENT_MENU = MENU_MAIN;
			break;
		case MENU_MAIN:
			//->Credits was pressed...
			CURRENT_MENU = MENU_CREDITS;
			break;
	}

}

void ProcessButton5() {
	//==== Button 5 was clicked ====//
	switch (CURRENT_MENU) {
		case MENU_MAIN:
			//-> Quit was pressed
			CURRENT_MENU = MENU_QUIT;
			break;
	}

}

void ProcessButton8() {
	//======== Button 8 was clicked ====//
	switch (CURRENT_MENU) {
		case MENU_PREHUNT:
			{
			//-> Hunt was pressed...
				int DinoCode = 0;
				int WeaponCode = 0;
				//int DayTime = SelectedDay; //0 = dawn, 1 = day, 2 = night
				char Acces[128];
				char Renderer[128];
				//-> Get Renderer...
				switch (OptRender) {
					case 0:
						strcpy(Renderer,"v_soft");
						break;
					case 1:
						strcpy(Renderer,"v_3dfx");
						break;
					case 2:
						strcpy(Renderer,"v_d3d");
						break;
					//case 3:
					//	strcpy(Renderer,"v_d3dsoft");
					//	break;
				}
				//-> Get Day Time...
					//SelectedDay = SelectedDay; //<- Here to get your attention

				//-> Build list of selected dinos.. For some reason, the code is adding '20' to the value....
					for (int i = 0; i < TotalC; i++) {
						if (DinoInfo[i].Selected)
							DinoCode += DinoInfo[i].Code;
					}
				//-> Build list of selected weapons...
					for (int i = 0; i < TotalW; i++) {
						if (WeapInfo[i].Selected) {
							WeaponCode = WeaponCode+WeapInfo[i].Code;
						}
					}
			    //-> Get Accessories.... and Get Observer Mode...

					bool radarTemp, camoTemp, scentTemp, doubleAmmoTemp,
						tranqTemp, supplyTemp, sonarTemp, dogTemp,
						binoTemp, binTextTemp;
					radarTemp = radarDefault;
					camoTemp = camoDefault;
					scentTemp = scentDefault;
					doubleAmmoTemp = doubleAmmoDefault;
					tranqTemp = tranqDefault;
					supplyTemp = supplyDefault;
					sonarTemp = sonarDefault;
					dogTemp = dogDefault;
					binoTemp = binoDefault;
					binTextTemp = binTextDefault;
					bool mapviewTemp = mapviewDefault;
					bool callboxTemp = callboxDefault;

					float scoreMultiplier = 1;

					for (int i = 0; i < TotalA; i++) {
						if (AcessInfo[i].Selected) {
							scoreMultiplier *= AcessInfo[i].scoreMod;
							if (AcessInfo[i].radar) radarTemp = TRUE;
							if (AcessInfo[i].camo) camoTemp = TRUE;
							if (AcessInfo[i].scent) scentTemp = TRUE;
							if (AcessInfo[i].doubleAmmo) doubleAmmoTemp = TRUE;
							if (AcessInfo[i].tranq) tranqTemp = TRUE;
							if (AcessInfo[i].supply) supplyTemp = TRUE;
							if (AcessInfo[i].sonar) sonarTemp = TRUE;
							if (AcessInfo[i].dog) dogTemp = TRUE;
							if (AcessInfo[i].bino) binoTemp = TRUE;
							if (AcessInfo[i].binText) binTextTemp = TRUE;
							if (AcessInfo[i].mapview) mapviewTemp = TRUE;
							if (AcessInfo[i].callbox) callboxTemp = TRUE;
						}
					}

					int scoreMultiplierI = scoreMultiplier * 10000;

					if (radarTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-radar");
						else
							strcpy(Acces, "-radar");
					}
					if (camoTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-camo");
						else
							strcpy(Acces, "-camo");
					}
					if (scentTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-scent");
						else
							strcpy(Acces, "-scent");
					}
					if (doubleAmmoTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-double");
						else
							strcpy(Acces,  "-double");
					}
					if (tranqTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-tranq");
						else
							strcpy(Acces, "-tranq");
					}
					if (supplyTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-supply");
						else
							strcpy(Acces, "-supply");
					}
					if (sonarTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-sonar");
						else
							strcpy(Acces, "-sona ");
					}
					if (dogTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-huntdog");
						else
							strcpy(Acces, "-huntdog");
					}
					if (binoTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-binoc");
						else
							strcpy(Acces, "-binoc");
					}
					if (binTextTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-bintext");
						else
							strcpy(Acces, "-bintext");
					}
					if (mapviewTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-map");
						else
							strcpy(Acces, "-map");
					}
					if (callboxTemp) {
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces, "%s %s", Acces, "-callbox");
						else
							strcpy(Acces, "-callbox");
					}

					if (ObservMode)
						if (int(strlen(Acces)) > 3)
							wsprintf(Acces,"%s %s",Acces,"-observ");
						else
							strcpy(Acces,"-observ");

			    //-> Verify that it is OK to launch....
				if (SelectedMap == -1 || SelectedMap >= TotalM || DinoCode == 0 || WeaponCode == 0) return;
				//-> LAUNCH!!
				wsprintf(logt,"%s.ren prj=%s reg=%d din=%d wep=%d dtm=%d scr=%d %s",Renderer,MapFile[SelectedMap].mapfile,TrophyRoom.RegNumber,DinoCode,WeaponCode,SelectedDay,scoreMultiplierI,Acces);
				PrintLog(logt);
				LaunchCarnProcess();
			break;
			}
		case MENU_MAIN:
			{
				//MULTIPLAYER
			}
	}
}

int ProcessShoot()
{
	//->first check if the computer is waiting for key input...
	if (AKeySelected) {
			//-> Update the key
			 *PtrSelectedKey = 1; //The area of memory being pointed to by Prt = the value
			 AKeySelected = false;
			 AddVoicev(TypeSound[1].length,TypeSound[1].lpData, 256);
		return 0;
	}
	POINT ms;
	GetCursorPos(&ms);
	int ButtonID = int(GameMenus[CURRENT_MENU].Hotspots[int(ms.y/2)][int(ms.x/2)]);
	if (DEBUG) {
		//-> Print Co-ords
		wsprintf(logt,"X: %d Y: %d ButtonID: %d",ms.x,ms.y,ButtonID);
		AddMessage(logt);
	}

	if (ButtonID != 0) {
		//PlaySound2d("HUNTDAT\\SOUNDFX\\MENUGO.wav");
		AddVoicev  (MENUGO.length, MENUGO.lpData, 256);
		switch (ButtonID) {
			case 1:
				ProcessButton1();
				break;
			case 2:
				ProcessButton2();
				break;
			case 3:
				ProcessButton3();
				break;
			case 4:
				ProcessButton4();
				break;
			case 5:
				ProcessButton5();
				break;
			case 6:
				ProcessButton6();
				break;
			case 7:
				ProcessButton7();
				break;
			case 8:
				ProcessButton8();
				break;
		}
	} else {
		//-> Menu related commands (did user click a map, dinosaur, weapon, etc?
		switch (CURRENT_MENU) {
			case MENU_OPTIONS:
				{
					int newvalue;
					//-> Process Key or Slider clicks
					// -> Process Sliders...
					if (MouseAtX > 207 && MouseAtX < 330 && MouseAtY < 114 && MouseAtY > 104) {
						//-> User clicked agresivity slide...
						//SliderXStart = 205
						newvalue = (MouseAtX - 205)*(255/123); // = VALUE*(maxvalue/x_width_of_slider)
						//^ Value MUST be in the range of 0 to 255
						Gui_SetSliderValue("slider_agressive",newvalue);
						OptAgres = newvalue;
					}
					if (MouseAtX > 207 && MouseAtX < 330 && MouseAtY < 114+(22*1) && MouseAtY > 103+(22*1)) {
						//-> User clicked dense slide...
						//SliderXStart = 205
						newvalue = (MouseAtX - 205)*2;
						Gui_SetSliderValue("slider_dense",newvalue);
						OptDens = newvalue;
					}
					if (MouseAtX > 207 && MouseAtX < 330 && MouseAtY < 114+(22*2) && MouseAtY > 103+(22*2)) {
						//-> User clicked sense slide...
						//SliderXStart = 205
						newvalue = (MouseAtX - 205)*2;
						Gui_SetSliderValue("slider_sense",newvalue);
						OptSens = newvalue;
					}
					if (MouseAtX > 207 && MouseAtX < 330 && MouseAtY < 114+(22*3) && MouseAtY > 103+(22*3)) {
						//-> User clicked slider_vrange slide...
						//SliderXStart = 205
						newvalue = (MouseAtX - 205)*2;
						Gui_SetSliderValue("slider_vrange",newvalue);
						OptViewR = newvalue;
					}
					if (MouseAtX > 207 && MouseAtX < 330 && MouseAtY < 502 && MouseAtY > 491) {
						//-> User clicked slider_brightness slide...
						//SliderXStart = 205
						newvalue = (MouseAtX - 205)*2;
						Gui_SetSliderValue("slider_bright",newvalue);
						OptBrightness = newvalue;
					}
					if (MouseAtX > 618 && MouseAtX < 618+123 && MouseAtY < 71 + 21 + (22 * 18) && MouseAtY > 71 + (22 * 18)) {
						//-> User clicked slider_mouse slide...
						//SliderXStart = 205
						newvalue = (MouseAtX - 618)*2;
						Gui_SetSliderValue("slider_mouse",newvalue);
						OptMsSens = newvalue;
					}

					//-> Process Buttons/Keys...
						//-> GameOptions
					if (MouseAtY > 191 && MouseAtY < 208 && MouseAtX > 61 && MouseAtX < 275) {
						//-> Measurement clicked
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						if (OptSys == 0)
							OptSys = 1;
						else
							OptSys = 0;
					}
					   //-> Keys

					if (MouseAtY > 72 && MouseAtY < 91 && MouseAtX > 518 && MouseAtX < 668) {
						//Forward key...
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkForward; //PrtSelectedKey = Addressof keymap.forward
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 93 && MouseAtY < 111 && MouseAtX > 518 && MouseAtX < 668) {
						//Backward key...
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkBackward; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 115 && MouseAtY < 133 && MouseAtX > 518 && MouseAtX < 668) {
						//Fire key
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkFire; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 138 && MouseAtY < 155 && MouseAtX > 518 && MouseAtX < 668) {
						//Hide weapon
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkShow; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 161 && MouseAtY < 177 && MouseAtX > 518 && MouseAtX < 668) {
						//Stepleft
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkSLeft; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 181 && MouseAtY < 200 && MouseAtX > 518 && MouseAtX < 668) {
						//atepright
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkSRight; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 205 && MouseAtY < 222 && MouseAtX > 518 && MouseAtX < 668) {
						//Jump
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkJump; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 228 && MouseAtY < 241 && MouseAtX > 518 && MouseAtX < 668) {
						//Runmode
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkRun; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 246 && MouseAtY < 264 && MouseAtX > 518 && MouseAtX < 668) {
						//crouch
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkCrouch; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 270 && MouseAtY < 287 && MouseAtX > 518 && MouseAtX < 668) {
						//call
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkCall; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 291 && MouseAtY < 309 && MouseAtX > 518 && MouseAtX < 668) {
						//ccall
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkCCall; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 312 && MouseAtY < 333 && MouseAtX > 518 && MouseAtX < 668) {
						//binocs
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkBinoc; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}



					if (MouseAtY > 71+(22*12) && MouseAtY < 71+21+(22*12) && MouseAtX > 518 && MouseAtX < 668) {
						//Reload
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkUp; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 71+(22*13) && MouseAtY < 71+21+(22*13) && MouseAtX > 518 && MouseAtX < 668) {
						//Resupply
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkDown; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 71+(22*14) && MouseAtY < 71+21+(22*14) && MouseAtX > 518 && MouseAtX < 668) {
						//Hold Breath
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkLeft; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length,TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 71 + (22 * 15) && MouseAtY < 71 + 21 + (22 * 15) && MouseAtX > 518 && MouseAtX < 668) {
						//Firing Mode
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkRight; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length, TypeSound[0].lpData, 256);
					}
					if (MouseAtY > 71 + (22 * 16) && MouseAtY < 71 + 21 + (22 * 16) && MouseAtX > 518 && MouseAtX < 668) {
						//Rack
						AKeySelected = true;
						PtrSelectedKey = &KeyMap.fkStrafe; //PrtSelectedKey = Addressof keymap.key
						AddVoicev(TypeSound[0].length, TypeSound[0].lpData, 256);
					}


					if (MouseAtY > 71 + (22 * 17) && MouseAtY < 71 + 21 + (22 * 17) && MouseAtX > 518 && MouseAtX < 668) {
						//reverse mouse
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						if (REVERSEMS == 0)
							REVERSEMS = 1;
						else
							REVERSEMS = 0;
					}
					//-> Video and sound settings...
					if (MouseAtY > 353 && MouseAtY < 372 && MouseAtX > 61 && MouseAtX < 275) {
						//-> Audio settings
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						OptSound++;
						if (OptSound > 3)
							OptSound = 0;
					}
					if (MouseAtY > 375 && MouseAtY < 395 && MouseAtX > 61 && MouseAtX < 275) {
						//-> Video
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						OptRender++;
						if (OptRender > 2)
							OptRender = 0;
					}
					if (MouseAtY > 398 && MouseAtY < 417 && MouseAtX > 61 && MouseAtX < 275) {
						//-> res
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						OptRes++;
						if (OptRes > 7)
							OptRes = 0;
					}
					if (MouseAtY > 418 && MouseAtY < 439 && MouseAtX > 61 && MouseAtX < 275) {
						//-> shadows
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						SHADOWS3D++;
						if (SHADOWS3D > 1)
							SHADOWS3D = 0;
					}
					if (MouseAtY > 441 && MouseAtY < 462 && MouseAtX > 61 && MouseAtX < 275) {
						//-> fog
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						FOGENABLE++;
						if (FOGENABLE > 1)
							FOGENABLE = 0;
					}
					if (MouseAtY > 464 && MouseAtY < 484 && MouseAtX > 61 && MouseAtX < 275) {
						//-> texture
						AddVoicev  (MENUGO.length,MENUGO.lpData,255);
						OptText++;
						if (OptText > 2)
							OptText = 0;
					}
				}
			case MENU_PREHUNT:
				{
					//-> Toggle clicked dino,map,weapon, or accessory
					int ClickedSlot; //Selected Slot
					int FieldOffset; //Offset by slider bars

					if (MouseAtX > 15 && MouseAtX < 15+163 && MouseAtY > 382 && MouseAtY < 545) {
						//-> User clicked a map, process it:
						// -> Play Sound
						ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
						FieldOffset = Gui_GetSliderValue("slider_maps")/2; //<- Make it less sensitive
						if ((ClickedSlot+FieldOffset) < TotalM) {
							//-> Toggle selected
							MapFile[ClickedSlot+FieldOffset].selected = !MapFile[ClickedSlot+FieldOffset].selected;
							//-> Check price...
							if (MapFile[ClickedSlot+FieldOffset].selected && TrophyRoom.Score - (SpentCredits+MapFile[ClickedSlot+FieldOffset].points) < 0) {
								//-> Don't allow it...
								MapFile[ClickedSlot+FieldOffset].selected = false;
							} else {
								//-> OK the player can afford it OR the player is unselecting it.. Process the selection
								if (MapFile[ClickedSlot+FieldOffset].selected) {
									//-> Clear old one
									if (MapFile[SelectedMap].selected) {
										MapFile[SelectedMap].selected = false;
										SpentCredits -= MapFile[SelectedMap].points;
									}
									SelectedMap = ClickedSlot+FieldOffset;
									SpentCredits += MapFile[ClickedSlot+FieldOffset].points;
								} else {
									//-> No map selected because the user pressed the same one again
									//if (!MapFile[SelectedMap].selected) {
									//-> If no map is currently selected
									SelectedMap = -1;
									SpentCredits -= MapFile[ClickedSlot+FieldOffset].points;
									//}
								}
								AddVoicev  (MENUGO.length,MENUGO.lpData,255);
							}
						}
					}

					if (MouseAtX > 215 && MouseAtX < 380 && MouseAtY > 382 && MouseAtY < 545) {
						//-> User clicked a dinosaur, process it:
						// -> Play Sound
						ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
						FieldOffset = Gui_GetSliderValue("slider_dinos")/2; //<- Make it less sensitive
						if ((ClickedSlot+FieldOffset)+7 < TotalC) {
							DinoInfo[(ClickedSlot+FieldOffset)+7].Selected = !DinoInfo[(ClickedSlot+FieldOffset)+7].Selected;
							//-> Check price...
							if (DinoInfo[ClickedSlot+FieldOffset+7].Selected && TrophyRoom.Score - (SpentCredits+DinoInfo[(ClickedSlot+FieldOffset)+7].Price) < 0) {
								//-> Don't allow it...
								DinoInfo[ClickedSlot+FieldOffset+7].Selected = false;
							} else {
								AddVoicev  (MENUGO.length,MENUGO.lpData,255);
								if (DinoInfo[(ClickedSlot+FieldOffset)+7].Selected)
									SpentCredits += DinoInfo[(ClickedSlot+FieldOffset)+7].Price;
								else
									SpentCredits -= DinoInfo[(ClickedSlot+FieldOffset)+7].Price;
							}
						}
					}

					if (MouseAtX > 410 && MouseAtX < 574 && MouseAtY > 382 && MouseAtY < 545) {
						//-> User clicked a weapon, process it:
						// -> Play Sound
						ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
						FieldOffset = Gui_GetSliderValue("slider_weapons")/2; //<- Make it less sensitive
						if ((ClickedSlot+FieldOffset) < TotalW) {
							WeapInfo[(ClickedSlot+FieldOffset)].Selected = !WeapInfo[(ClickedSlot+FieldOffset)].Selected;
							//-> Check price...
							if (WeapInfo[(ClickedSlot+FieldOffset)].Selected && TrophyRoom.Score - (SpentCredits+WeapInfo[(ClickedSlot+FieldOffset)].Price) < 0) {
								//-> Don't allow it...
								WeapInfo[(ClickedSlot+FieldOffset)].Selected = false;
							} else {
								AddVoicev  (MENUGO.length,MENUGO.lpData,255);
								if (WeapInfo[(ClickedSlot+FieldOffset)].Selected)
									SpentCredits += WeapInfo[(ClickedSlot+FieldOffset)].Price;
								else
									SpentCredits -= WeapInfo[(ClickedSlot+FieldOffset)].Price;
							}
						}
					}

					if (MouseAtX > 610 && MouseAtX < 774 && MouseAtY > 382 && MouseAtY < 545) {
						//-> User clicked an acessory, process it:
						// -> Play Sound
						ClickedSlot = int( (ms.y - 382)/16); //-> SlotID = (ClickedY - MinimimY)/size_per_slot. See fonts in game.cpp for font sizes. This is using small
						//FieldOffset = Gui_GetSliderValue("slider_weapons")/2; //<- Make it less sensitive
						if ((ClickedSlot) < TotalA) {
							AcessInfo[(ClickedSlot)].Selected = !AcessInfo[(ClickedSlot)].Selected;
							//-> Check price...
							if (AcessInfo[(ClickedSlot)].Selected && TrophyRoom.Score - (SpentCredits+AcessInfo[(ClickedSlot)].price) < 0) {
								//-> Don't allow it...
								AcessInfo[(ClickedSlot)].Selected = false;
							} else {
								AddVoicev  (MENUGO.length,MENUGO.lpData,255);
								if (AcessInfo[(ClickedSlot)].Selected)
									SpentCredits += AcessInfo[(ClickedSlot)].price;
								else
									SpentCredits -= AcessInfo[(ClickedSlot)].price;
							}
						}
					}
					break;
				}
			case MENU_LOGIN: 
				{
					//-> Find username clicked and update TypedText to match value....
					int ClickedUserID = int( (ms.y - 365)/20); //-> SlotID = (ClickedY - MinimimY)/size_per_slot
					if (ClickedUserID < TOTAL_PLAYERS && ms.y >= 365) {
						//-> Existing user was clicked
						if (SelectedPlayer == ClickedUserID) {
							//-> Process Double Click
							AddVoicev  (MENUGO.length, MENUGO.lpData, 256);
							ProcessButton1();
							return 0;
						}
						SelectedPlayer = ClickedUserID;
						Console_Clear();
						strcpy(TypedText,PlayerFile[ClickedUserID].TrophyRoom.PlayerName);
						TypedTextLength = strlen(TypedText);
						//PlaySound2d("HUNTDAT\\SOUNDFX\\MENUGO.wav");
						AddVoicev  (MENUGO.length, MENUGO.lpData, 256);
					} 
					break;
				}
			case MENU_STATS:
				//-> Exit this menu when anyplace is clicked
					CURRENT_MENU = MENU_MAIN;
					AddVoicev  (MENUGO.length, MENUGO.lpData, 256);
				break;
			case MENU_CREDITS:
				//-> Exit this menu
					CURRENT_MENU = MENU_MAIN;
					AddVoicev  (MENUGO.length, MENUGO.lpData, 256);
				break;
		}
	}
	return 0; //0 = cold area
}


void ProcessSlide()
{
  if (NOCLIP || UNDERWATER) return;
	float ch = GetLandQHNoObj(PlayerX, PlayerZ);
	float mh = ch;
	float chh;
	int   sd = 0;

	chh=GetLandQHNoObj(PlayerX - 16, PlayerZ); if (chh<mh) { mh = chh; sd = 1; }
	chh=GetLandQHNoObj(PlayerX + 16, PlayerZ); if (chh<mh) { mh = chh; sd = 2; }
	chh=GetLandQHNoObj(PlayerX, PlayerZ - 16); if (chh<mh) { mh = chh; sd = 3; }
	chh=GetLandQHNoObj(PlayerX, PlayerZ + 16); if (chh<mh) { mh = chh; sd = 4; }

	chh=GetLandQHNoObj(PlayerX - 12, PlayerZ - 12); if (chh<mh) { mh = chh; sd = 5; }
	chh=GetLandQHNoObj(PlayerX + 12, PlayerZ - 12); if (chh<mh) { mh = chh; sd = 6; }
	chh=GetLandQHNoObj(PlayerX - 12, PlayerZ + 12); if (chh<mh) { mh = chh; sd = 7; }
	chh=GetLandQHNoObj(PlayerX + 12, PlayerZ + 12); if (chh<mh) { mh = chh; sd = 8; }

	if (!NOCLIP)
		if (mh<ch-16) {
			float delta = (ch-mh) / 4;
			if (sd == 1) { PlayerX -= delta; }
			if (sd == 2) { PlayerX += delta; }
			if (sd == 3) { PlayerZ -= delta; }
			if (sd == 4) { PlayerZ += delta; }

			delta*=0.7f;
			if (sd == 5) { PlayerX -= delta; PlayerZ -= delta; }
			if (sd == 6) { PlayerX += delta; PlayerZ -= delta; }
			if (sd == 7) { PlayerX -= delta; PlayerZ += delta; }
			if (sd == 8) { PlayerX += delta; PlayerZ += delta; }  
			// AddMessage(char(sd));
			//wsprintf(t,"%d", sd);
			// AddMessage(LPSTR(sd));
		}    
}

void ProcessHover() {
	int ButtonID = int(GameMenus[CURRENT_MENU].Hotspots[int(MouseAtY/2)][int(MouseAtX/2)]);
	if (ButtonID != 0) {
		//-> If not the same button, then play the sound
		if (BUTTON_HOVER_ID != ButtonID)
			AddVoicev  (MENUMOV.length, MENUMOV.lpData, 256);
		//-> Process Hover...
		BUTTON_HOVER_ID = ButtonID;
	} else {
		BUTTON_HOVER_ID = 0;
	}
}

void ProcessPlayerMovement()
{
   //if (TypingMode) return;
	//-> Used by menu code to catch mouse movement and clicking

   POINT ms;
   
    GetCursorPos(&ms);
	if (ms.y != MouseAtY || ms.x != MouseAtX) {
		//== Mouse Moved ==
		//-> Get Move direction
		if (KeyboardState [KeyMap.fkFire] & 128) {
			if (ms.y < MouseAtY) {
				Mouse_SlideDir = 1; //Went up
			}
			if (ms.y > MouseAtY) {
				Mouse_SlideDir = -1;//Went down
			}
			//Fixed slider 'feel'
			SetCursorPos(MouseAtX, MouseAtY); 
			ms.y = MouseAtY;
			ms.x = MouseAtX;
		} else {
			Mouse_SlideDir = 0;
		}
		//-> Process Hover
		MouseAtY = ms.y;
		MouseAtX = ms.x;
		ProcessHover();
	}

	//-> Left-Click (Note semi-auto, so one click doesn't process a bunch of times)
	if (KeyboardState [1] & 128) {
		if (!Weapon.TPA) {
			int clicked_buttonID = ProcessShoot();
			Weapon.TPA = 1;
		}
	} else {
		Weapon.TPA = 0;
	}
   
	//-> Right-click
	if (KeyboardState [2] & 128) {
		//->first check if the computer is waiting for key input...
		if (AKeySelected) {
			//-> Update the key
			*PtrSelectedKey = 2; //The area of memory being pointed to by Prt = the value
			AKeySelected = false;
			AddVoicev(TypeSound[1].length,TypeSound[1].lpData, 256);
		}
	}

//===========================================================      
}


void ProcessDemoMovement()
{  
  BINMODE = FALSE;
  
  PAUSE = FALSE;
  MapMode = FALSE;

  if (DemoPoint.DemoTime>6*1000)
	if (!PAUSE) {       
	   EXITMODE = TRUE; 
	   ResetMousePos(); 
	  }

  if (DemoPoint.DemoTime>12*1000) { 
	//ResetMousePos();
    //DemoPoint.DemoTime = 0;
	//LoadTrophy();
	DoHalt("");
	return;  }

  VSpeed = 0.f;

  DemoPoint.pos = Characters[DemoPoint.CIndex].pos;
#ifdef _iceage // alacn
  if (Characters[DemoPoint.CIndex].AI == AI_MAMM) DemoPoint.pos.y+=512;
#else
  if (Characters[DemoPoint.CIndex].AI==AI_TREX) DemoPoint.pos.y+=512;
#endif
  DemoPoint.pos.y+=256;  

  Vector3d nv = SubVectors(DemoPoint.pos,  CameraPos);
  Vector3d pp = DemoPoint.pos;
  pp.y = CameraPos.y;
  float l = VectorLength( SubVectors(pp,  CameraPos) );
  float base = 824;
#ifdef _iceage // alacn
  if (Characters[DemoPoint.CIndex].AI == AI_MAMM) base+=512;
#else
  if (Characters[DemoPoint.CIndex].AI==AI_TREX) base=1424;
#endif
  
  if (DemoPoint.DemoTime==1)
   if (l < base) DemoPoint.DemoTime = 2; 
  NormVector(nv, 1.0f);
  
  if (DemoPoint.DemoTime == 1) {
   DeltaFunc(CameraX, DemoPoint.pos.x, (float)fabs(nv.x) * TimeDt * 3.f);  
   DeltaFunc(CameraZ, DemoPoint.pos.z, (float)fabs(nv.z) * TimeDt * 3.f);  
  } else {
   DemoPoint.DemoTime+=TimeDt;
   CameraAlpha+=TimeDt / 1224.f;
   ca = (float)cos(CameraAlpha);
   sa = (float)sin(CameraAlpha);         
   //float k = (base - l) / 350.f;
   DeltaFunc(CameraX, DemoPoint.pos.x  - sa * base, (float)TimeDt );
   DeltaFunc(CameraZ, DemoPoint.pos.z  + ca * base, (float)TimeDt );
  }

  float b = FindVectorAlpha( (float)
			  sqrt ( (DemoPoint.pos.x - CameraX)*(DemoPoint.pos.x - CameraX) +
			         (DemoPoint.pos.z - CameraZ)*(DemoPoint.pos.z - CameraZ) ),
			  DemoPoint.pos.y - CameraY - 400.f);
  if (b>pi) b = b - 2*pi;
  DeltaFunc(CameraBeta, -b , TimeDt / 4000.f);



  float h = GetLandQH(CameraX, CameraZ);    
  DeltaFunc(CameraY, h+128, TimeDt / 8.f);
  if (CameraY < h + 80) CameraY = h + 80;    
}




void ProcessControls()
{      
   int _KeyFlags = KeyFlags;
   KeyFlags = 0;
   GetKeyboardState(KeyboardState);   
   //if (KeyboardState [VK_LSHIFT] & 128)

   //if ( DemoPoint.DemoTime) ProcessDemoMovement();
   if (!DemoPoint.DemoTime) ProcessPlayerMovement();      

return;    
}















void ProcessGame()
{
	// -> Check if Max FPS is set, and if so then process it...
	ShowCursor(TRUE); 
	if (MAX_FPS) {
		// -> Only process if the current FPS is larger than it should be...
		if (ENGINE_FPS > MAX_FPS) {
			Sleep(1000/MAX_FPS);
		}
	}
	if (RestartMode && FADING == 0) {
		ShutDown3DHardware();
		AudioStop();
		NeedRVM = TRUE;
		FADE_IN		 = TRUE;
	}

    if (!_GameState) {				
		PrintLog("Entered game\n");		        
		ReInitGame();						
        //while (ShowCursor(FALSE)>=0);        
	}

    _GameState = 1;

    if (NeedRVM) {		
		SetWindowPos(hwndMain, HWND_TOP, 0,0,0,0,  SWP_SHOWWINDOW);
		SetFocus(hwndMain);							    
		Activate3DHardware();		
		NeedRVM = FALSE;
	}
	
    ProcessSyncro();

	//if (!PAUSE || !MyHealth) {
   ProcessControls();
	//	AudioSetCameraPos(CameraX, CameraY, CameraZ, CameraAlpha, CameraBeta);
	//	// -> Run AI thread
	//	if (USE_THREADS) {
	//		if (!AI_THREAD_ACTIVE) {
	//			AI_THREAD_ACTIVE = TRUE;
	//			_beginthread( AnimateCharacters,0,NULL);
	//		}
	//	} else {
	//		AnimateCharacters(NULL);
	//	}
	AnimateProcesses();
	//}
		    
   /* if (DEBUG || ObservMode || TrophyMode) 
		if (MyHealth) MyHealth = MAX_HEALTH;
	if (DEBUG) ShotsLeft[CurrentWeapon] = WeapInfo[CurrentWeapon].Shots;
    */
	//DrawScene();  
	/*
	if (!TrophyMode)
     if (MapMode) DrawHMap();*/
	if (CURRENT_MENU == MENU_MAIN && !MENU_MUSICPLAYING) {
		//-> Play background music
		//PlaySound2dLoop("HUNTDAT\\SOUNDFX\\MENUAMB.WAV");
		SetAmbient(MENUAMB.length, MENUAMB.lpData, 256);
		MENU_MUSICPLAYING = TRUE;
	}

	switch (CURRENT_MENU) {
		//-> Preform per-menu related tasks
		case MENU_LOGIN:
				if (!TypingMode)
					TypingMode = TRUE;
			break;
	}

    DrawPostObjects(); 
	
    ShowControlElements();
	        
    ShowVideo();    
}



int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			       LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg;	
	
	hInst = hInstance;    
	CreateLog();
	   
	CreateMainWindow();            
	
	Init3DHardware();
	InitEngine();    	
	InitAudioSystem(hwndMain, hlog, OptSound);	
//	InitSoundEngine(); //Init secondary sound engine

	StartLoading();    
	PrintLoad("Loading...");
    
	PrintLog("== Loading resources ==\n");
    hcArrow = LoadCursor(NULL, IDC_ARROW);


	PrintLog("Loading common resources:");
	PrintLoad("Loading common resources...");

	if (OptDayNight==2)
	  LoadModelEx(SunModel,    "HUNTDAT\\MOON.3DF");
	else
	  LoadModelEx(SunModel,    "HUNTDAT\\SUN2.3DF");
	LoadModelEx(CompasModel, "HUNTDAT\\COMPAS.3DF");
	LoadModelEx(Binocular,   "HUNTDAT\\BINOCUL.3DF");

	LoadCharacterInfo(WCircleModel , "HUNTDAT\\WCIRCLE2.CAR"); 	
	LoadCharacterInfo(ShipModel, "HUNTDAT\\ship2a.car"); 
	LoadCharacterInfo(WindModel, "HUNTDAT\\WIND.CAR");

    LoadCharacterInfo(SShipModel, "HUNTDAT\\sship1.car"); 
    LoadCharacterInfo(AmmoModel, "HUNTDAT\\bag1.car"); 

	LoadWav("HUNTDAT\\SOUNDFX\\a_underw.wav",  fxUnderwater);

	LoadWav("HUNTDAT\\SOUNDFX\\STEPS\\hwalk1.wav",  fxStep[0]);
	LoadWav("HUNTDAT\\SOUNDFX\\STEPS\\hwalk2.wav",  fxStep[1]);
	LoadWav("HUNTDAT\\SOUNDFX\\STEPS\\hwalk3.wav",  fxStep[2]);

	LoadWav("HUNTDAT\\SOUNDFX\\STEPS\\footw1.wav",  fxStepW[0]);
	LoadWav("HUNTDAT\\SOUNDFX\\STEPS\\footw2.wav",  fxStepW[1]);
	LoadWav("HUNTDAT\\SOUNDFX\\STEPS\\footw3.wav",  fxStepW[2]);

	LoadWav("HUNTDAT\\SOUNDFX\\hum_die1.wav",  fxScream[0]);
	LoadWav("HUNTDAT\\SOUNDFX\\hum_die2.wav",  fxScream[1]);
	LoadWav("HUNTDAT\\SOUNDFX\\hum_die3.wav",  fxScream[2]);
	LoadWav("HUNTDAT\\SOUNDFX\\hum_die4.wav",  fxScream[3]);		

	//== Adelphospro == //
	// -> These sounds are now played by 2ndarry sound engine
	//LoadWav("HUNTDAT\\SOUNDFX\\type.wav",  TypeSound[0]); //Typing...
	//LoadWav("HUNTDAT\\SOUNDFX\\typego.wav",  TypeSound[1]); //Click enter
	
	LoadPictureTGA(PausePic,   "HUNTDAT\\MENU\\pause.tga");       conv_pic(PausePic);
	LoadPictureTGA(ExitPic,    "HUNTDAT\\MENU\\exit.tga");        conv_pic(ExitPic);
	LoadPictureTGA(TrophyExit, "HUNTDAT\\MENU\\trophy_e.tga");    conv_pic(TrophyExit);		
	LoadPictureTGA(MapPic,     "HUNTDAT\\MENU\\mapframe.tga");    conv_pic(MapPic);
		
	LoadPictureTGA(TFX_ENVMAP,    "HUNTDAT\\FX\\envmap.tga");   ApplyAlphaFlags(TFX_ENVMAP.lpImage, TFX_ENVMAP.W*TFX_ENVMAP.W);
	LoadPictureTGA(TFX_SPECULAR,  "HUNTDAT\\FX\\specular.tga"); ApplyAlphaFlags(TFX_SPECULAR.lpImage, TFX_SPECULAR.W*TFX_SPECULAR.W);



	
	PrintLog(" Done.\n");
    
	PrintLoad("Loading area...");
	LoadResources();

	PrintLoad("Starting game...");
	PrintLog("Loading area: Done.\n");	
	
	EndLoading();	

	ProcessSyncro();	
	blActive = TRUE;

    PrintLog("Entering messages loop.\n");
    for( ; ; )
   	  if( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) ) {
        if (msg.message == WM_QUIT)  break;
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	  } else {
        if (QUITMODE==1) {
			ShutDown3DHardware();
            Audio_Shutdown();
//			ShutdownSoundEngine();
			DestroyWindow(hwndMain);
			QUITMODE=2;
		}
		if (!QUITMODE)
        if (blActive) ProcessGame();                  
                 else Sleep(100); 
	  }

     
     
     ShutDownEngine();    
     ShowCursor(TRUE);   
	 PrintLog("Game normal shutdown.\n");
	 
	 CloseLog();
     return msg.wParam;	 
}
