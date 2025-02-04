#ifdef _soft

#include "Hunt.h"
#include "stdio.h"
/*
typedef struct _tmpoint {
    int x, y, tx, ty;
} tmpoint;
*/


typedef struct _CharListItem {
    int CType, Index;
} TCharListItem;

typedef struct _CharListLine {
    int ICount;
    TCharListItem Items[256];
} TCharListLine;

TCharListLine ChRenderList[128];


Vector2di ORList[2][2048];
int ORLCount[2];

int rmlistselector;

void RenderSupplyShip();
void RenderAmmoBag();
void RenderBMPModel2     (TBMPModel*, float, float, float, int);

//================================================================//
int xa; int xb; int xa16; int xb16;
int v1, v2, v3, 
    l1, l2, l3,
    x1, Y1, tx1, ty1, lt1, zdepth1,
	x2, y2, tx2, ty2, lt2, zdepth2,
	x3, y3, tx3, ty3, lt3, zdepth3;
    
	
    	
int dx1,  dx2,  dx3, 
    dtx1, dty1, dlt1, 
    dtx2, dty2, dlt2, 
    dtx3, dty3, dlt3,
    tyb, tya, txa, txb, lta, ltb,
    ddtx1, ddty1, ddtx2, ddty2, ddtx3, ddty3;

int ctdy, ctdx, cdlt;
int _sp;
float k;
BOOL LockWater;
int OpacityMode;

void STTextOut(int x, int y, LPSTR t, int color)
{
	SetTextColor(hdcCMain, 0x00000000);
    TextOut(hdcCMain, x+1, y+1, t, strlen(t));
	SetTextColor(hdcCMain, color);
    TextOut(hdcCMain, x, y, t, strlen(t));
}



void ShowControlElements()
{
  
  HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcCMain, hbmpVideoBuf);

  HFONT   hfntOld = (HFONT)  SelectObject(hdcCMain, fnt_Midd); //<- Default Size
    
  char buf[128];  
  
  SetBkMode(hdcCMain, TRANSPARENT);
   
  if (TIMER) {
   //Calc FPS...
   ENGINE_FPS = 1000/TimeDt;
   wsprintf(buf,"FPS: %d", ENGINE_FPS);       
   STTextOut(WinEX-70, 10, buf, RGB(128,255,128)); 
   wsprintf(buf,"polys: %d", dFacesCount);   
   STTextOut(WinEX-90, 24, buf, RGB(128,255,128));    //Orig Color: 0x0020A0A0
  }
  //-> Process Delayed Messages

  //-> Now, clear message list of any expired messages
  if (MessageList[0].timeleft)
	  if (RealTime>MessageList[0].timeleft)
		   DropFirstMessage(); //Drop the first message and resort the array

  //-> Now, draw visible messages
  int lastYMSG = 10;
  if (Weapon.state)
	  lastYMSG = 30;
  for (int msgID = 0; msgID < MessageListCnt; msgID++) {
	  if (MessageList[msgID].timeleft) {
		  //if ((int)timeGetTime()<MessageList[msgID].delayed) continue; //<- Means we have a delayed message. Wait to display it
		  if ((int)timeGetTime()>MessageList[msgID].timeleft) MessageList[msgID].timeleft = 0;   
		  STTextOut(10, lastYMSG, MessageList[msgID].mtext, RGB(128,255,128));
		  lastYMSG += 14;
	  }  
  }

  // -> Display Typed Text...
  if (TypingMode && !EXITMODE) {
	  char blink[4];
	  BlinkTime += TimeDt;
	  if (BlinkTime > 1000)
		  BlinkTime = 0;
	  		  if (BlinkTime <= 500)
			strcpy(blink,"_");
		  else
			  strcpy(blink,"");
	  if (DEBUG_ASKFORPASS) {
		  // -> Convert all characters to astrics
		  char astCover[128];
		  memset(astCover,0,128);
		  memset (astCover,'*',int(strlen(TypedText)));
		  if (int(strlen(TypedText)) > 0)
			wsprintf(logt,"-> %s%s",astCover,blink);
		  else
			  wsprintf(logt,"-> %s",blink);
	  } else {  
		wsprintf(logt,"-> %s%s",TypedText,blink);
	  }
	  STTextOut(306,324, logt, regOnFontC);//RGB(128,255,128)
  }

  if (DEBUG_ASKFORPASS) {
	  wsprintf(logt,"Enter Password:");
	  STTextOut((WinW/2)-(GetTextW(hdcMain, TypedText)/2), WinH/2 - 14, logt, RGB(180,0,0));
	  if (!TypingMode)
		  TypingMode = TRUE;
  }

  //-> Draw Text based on current menu...
  int x,y,startAt;
  switch (CURRENT_MENU) {
	  case MENU_OPTIONS:
		  {
			  //-> Draw option text and values
			  //-> Value RGB color: 165,182,181
				hfntOld = (HFONT)SelectObject(hdcCMain, fnt_BIG);
			  //-> Game
							//wsprintf(logt,"%s",KeysName[KeyMap.fkForward-70]);
				STTextOut(190-GetTextW(hdcCMain,"Aggresivity"), 100, "Aggresivity", opNameFontC);
				STTextOut(190-GetTextW(hdcCMain,"Density"), 122, "Density", opNameFontC);
				STTextOut(190-GetTextW(hdcCMain,"Sensitivity"), 144, "Sensitivity", opNameFontC);
				STTextOut(190-GetTextW(hdcCMain,"View Range"), 166, "View Range", opNameFontC);
				STTextOut(190-GetTextW(hdcCMain,"Measurement"), 188, "Measurement", opNameFontC);
				switch (OptSys) {
					case 0: //Metric
							STTextOut(205, 188, "Metric", opValueFontC);
						break;
					case 1: //US
							STTextOut(205, 188, "US", opValueFontC);
						break;
				}
				//->System
				wsprintf(logt,"Audio Driver:");
				STTextOut(190-GetTextW(hdcCMain,logt), 353, logt, opNameFontC);
				switch (OptSound) {
					case 0: //Soft
							STTextOut(205, 353+(22*0), "Software Stereo", opValueFontC);
						break;
					case 1: //directsound
							STTextOut(205, 353+(22*0), "DirectSound 3D", opValueFontC);
						break;
					case 2: //aureal
							STTextOut(205, 353+(22*0), "Aureal A3D", opValueFontC);
						break;
					case 3: //Creative EAX
							STTextOut(205, 353+(22*0), "Creative EAX", opValueFontC);
						break;
				}
				wsprintf(logt,"Video Driver:");
				STTextOut(190-GetTextW(hdcCMain,logt), 353+(22*1), logt, opNameFontC);
				switch (OptRender) {
					case 0: //Soft
							STTextOut(205, 353+(22*1), "Software", opValueFontC);
						break;
					case 1: //3dfx
							STTextOut(205, 353+(22*1), "3DFX Glide", opValueFontC);
						break;
					case 2: //d3d
							STTextOut(205, 353+(22*1), "Direct3D", opValueFontC);
						break;
					//case 3: //d3d software
					//		STTextOut(205, 353+(22*1), "Direct3D Software", opValueFontC);
					//	break;
				}
				wsprintf(logt,"Resolution:");
				STTextOut(190-GetTextW(hdcCMain,logt), 353+(22*2), logt, opNameFontC);
				switch (OptRes) {
					case 0: //320x240
							STTextOut(205, 353+(22*2), "320x240", opValueFontC);
						break;
					case 1: //400x300
							STTextOut(205, 353+(22*2), "400x300", opValueFontC);
						break;
					case 2: //512x384
							STTextOut(205, 353+(22*2), "512x384", opValueFontC);
						break;
					case 3: //640x480
							STTextOut(205, 353+(22*2), "640x480", opValueFontC);
						break;
					case 4: //800x600
							STTextOut(205, 353+(22*2), "800x600", opValueFontC);
						break;
					case 5: //1024x768
							STTextOut(205, 353+(22*2), "1024x768", opValueFontC);
						break;
					case 6: //1280x1024
							STTextOut(205, 353+(22*2), "1280x1024", opValueFontC);
						break;
					case 7: //1600x1200
							STTextOut(205, 353+(22*2), "1600x1200", opValueFontC);
						break;
				}
				STTextOut(190-GetTextW(hdcCMain,"3D Shadows:"), 353+(22*3), "3D Shadows:", opNameFontC);
					if (SHADOWS3D)
						STTextOut(205, 353+(22*3), "On", opValueFontC);
					else
						STTextOut(205, 353+(22*3), "Off", opValueFontC);
					STTextOut(190-GetTextW(hdcCMain,"Fog:"), 353+(22*4), "Fog:", opNameFontC);
					if (FOGENABLE)
						STTextOut(205, 353+(22*4), "On", opValueFontC);
					else
						STTextOut(205, 353+(22*4), "Off", opValueFontC);
					STTextOut(190-GetTextW(hdcCMain,"Textures:"), 353+(22*5), "Textures:", opNameFontC);
				switch (OptText) {
					case 0: //low
							STTextOut(205, 353+(22*5), "Low", opValueFontC);
						break;
					case 1: //high
							STTextOut(205, 353+(22*5), "High", opValueFontC);
						break;
					case 2: //auto
							STTextOut(205, 353+(22*5), "Auto", opValueFontC);
						break;
				}
					STTextOut(190-GetTextW(hdcCMain,"Brightness:"), 353+(22*6), "Brightness:", opNameFontC);
				//->Keys: start: X:600 Y:71: KeysName[KeyMap.fkForward-70]
				STTextOut(600-GetTextW(hdcCMain,"Forward"), 71+(22*0), "Forward", opNameFontC);
				 if (PtrSelectedKey != &KeyMap.fkForward || !AKeySelected)
					STTextOut(617, 71+(22*0), KeysName[KeyMap.fkForward], opValueFontC);
				 else
					STTextOut(617, 71+(22*0), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Backward"), 71+(22*1), "Backward", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkBackward || !AKeySelected)
					STTextOut(617, 71+(22*1), KeysName[KeyMap.fkBackward], opValueFontC);
				else
					STTextOut(617, 71+(22*1), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Fire"), 71+(22*2), "Fire", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkFire || !AKeySelected)
					STTextOut(617, 71+(22*2), KeysName[KeyMap.fkFire], opValueFontC);
				else
					STTextOut(617, 71+(22*2), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Hide Weapon"), 71+(22*3), "Hide Weapon", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkShow || !AKeySelected)
					STTextOut(617, 71+(22*3), KeysName[KeyMap.fkShow], opValueFontC);
				else
					STTextOut(617, 71+(22*3), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Step Left"), 71+(22*4), "Step Left", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkSLeft || !AKeySelected)
					STTextOut(617, 71+(22*4), KeysName[KeyMap.fkSLeft], opValueFontC);
				else
					STTextOut(617, 71+(22*4), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Step Right"), 71+(22*5), "Step Right", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkSRight || !AKeySelected)
					STTextOut(617, 71+(22*5), KeysName[KeyMap.fkSRight], opValueFontC);
				else
					STTextOut(617, 71+(22*5), "<...>", opValueFontC);
				STTextOut(600-GetTextW(hdcCMain,"Jump"), 71+(22*6), "Jump", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkJump || !AKeySelected)
					STTextOut(617, 71+(22*6), KeysName[KeyMap.fkJump], opValueFontC);
				else
					STTextOut(617, 71+(22*6), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Run"), 71+(22*7), "Run", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkRun || !AKeySelected)
					STTextOut(617, 71+(22*7), KeysName[KeyMap.fkRun], opValueFontC);
				else
					STTextOut(617, 71+(22*7), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Crouch"), 71+(22*8), "Crouch", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkCrouch || !AKeySelected)
					STTextOut(617, 71+(22*8), KeysName[KeyMap.fkCrouch], opValueFontC);
				else
					STTextOut(617, 71+(22*8), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Call"), 71+(22*9), "Call", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkCall || !AKeySelected)
					STTextOut(617, 71+(22*9), KeysName[KeyMap.fkCall], opValueFontC);
				else
					STTextOut(617, 71+(22*9), "<...>", opValueFontC);
				STTextOut(600-GetTextW(hdcCMain,"Change Call"), 71+(22*10), "Change Call", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkCCall || !AKeySelected)
					STTextOut(617, 71+(22*10), KeysName[KeyMap.fkCCall], opValueFontC);
				else
					STTextOut(617, 71+(22*10), "<...>", opValueFontC);
				STTextOut(600-GetTextW(hdcCMain,"Binoculars"), 71+(22*11), "Binoculars", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkBinoc || !AKeySelected)
					STTextOut(617, 71+(22*11), KeysName[KeyMap.fkBinoc], opValueFontC);
				else
					STTextOut(617, 71+(22*11), "<...>", opValueFontC);

				STTextOut(600 - GetTextW(hdcCMain, "Reload"), 71 + (22 * 12), "Reload", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkUp || !AKeySelected)
					STTextOut(617, 71 + (22 * 12), KeysName[KeyMap.fkUp], opValueFontC);
				else
					STTextOut(617, 71 + (22 * 12), "<...>", opValueFontC);

				STTextOut(600 - GetTextW(hdcCMain, "Resupply"), 71 + (22 * 13), "Resupply", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkDown || !AKeySelected)
					STTextOut(617, 71 + (22 * 13), KeysName[KeyMap.fkDown], opValueFontC);
				else
					STTextOut(617, 71 + (22 * 13), "<...>", opValueFontC);

				STTextOut(600 - GetTextW(hdcCMain, "Hold Breath"), 71 + (22 * 14), "Hold Breath", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkLeft || !AKeySelected)
					STTextOut(617, 71 + (22 * 14), KeysName[KeyMap.fkLeft], opValueFontC);
				else
					STTextOut(617, 71 + (22 * 14), "<...>", opValueFontC);

				STTextOut(600 - GetTextW(hdcCMain, "Firing Mode"), 71 + (22 * 15), "Firing Mode", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkRight || !AKeySelected)
					STTextOut(617, 71 + (22 * 15), KeysName[KeyMap.fkRight], opValueFontC);
				else
					STTextOut(617, 71 + (22 * 15), "<...>", opValueFontC);

				STTextOut(600 - GetTextW(hdcCMain, "Rack"), 71 + (22 * 16), "Rack", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkStrafe || !AKeySelected)
					STTextOut(617, 71 + (22 * 16), KeysName[KeyMap.fkStrafe], opValueFontC);
				else
					STTextOut(617, 71 + (22 * 16), "<...>", opValueFontC);



				STTextOut(600-GetTextW(hdcCMain,"Reverse Mouse"), 71+(22*17), "Reverse Mouse", opNameFontC);
				if (REVERSEMS)
					STTextOut(615, 71+(22*17), "On", opValueFontC);
				else
					STTextOut(615, 71+(22*17), "Off", opValueFontC);
				STTextOut(600-GetTextW(hdcCMain,"Mouse Sensitivity"), 71+(22*18), "Mouse Sensitivity", opNameFontC);
				//-> WOW that was a pain....
				//-> Add my keys....
				/*
				STTextOut(600-GetTextW(hdcCMain,"Sprint"), 71+(22*15), "Sprint", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkSprint || !AKeySelected)
					STTextOut(617, 71+(22*15), KeysName[KeyMap.fkSprint], opValueFontC);
				else
					STTextOut(617, 71+(22*15), "<...>", opValueFontC);

				STTextOut(600-GetTextW(hdcCMain,"Aim"), 71+(22*16), "Aim", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkAim || !AKeySelected)
					STTextOut(617, 71+(22*16), KeysName[KeyMap.fkAim], opValueFontC);
				else
					STTextOut(617, 71+(22*16), "<...>", opValueFontC);

				
				STTextOut(600-GetTextW(hdcCMain,"Call Supplies"), 71+(22*17), "Call Supplies", opNameFontC);
				if (PtrSelectedKey != &KeyMap.fkSupply || !AKeySelected)
					STTextOut(617, 71+(22*17), KeysName[KeyMap.fkSupply], opValueFontC);
				else
					STTextOut(617, 71+(22*17), "<...>", opValueFontC);
					*/
			  break;
		  }
	  case MENU_PREHUNT:
		  {
		  //-> Draw map list, dino list, weapon list, and accessory list. Process selected items as well and draw them with a different color
			  //-> Start y = 382. add 18 each loop
			  //-> Maps
			  x = 15;
			  y = 382;
			  startAt = 0;
			  hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
			  //-> Get Slider Pos 
			  int SliderValue = Gui_GetSliderValue("slider_maps")/2; //<- Make it less sensitive
			  //-> Start from pos, only allow as many weapons as there are slots (9 slots, so use a value of 10 because i+offset must be < offset+10)
			  for (int i = SliderValue; i < min(TotalM,SliderValue+10); i++) {
				  if (strlen(MapFile[i].name) == 0) continue;
				  if (SelectedMap != i ) {
					  //Map is not selected
					  //-> Check if you can afford it...
					  if (TrophyRoom.Score - (SpentCredits+MapFile[i].points) >= 0) {
							STTextOut(x, y, MapFile[i].name, huntAvailFontC);
							wsprintf(logt,"%d",MapFile[i].points);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntAvailFontC);
					  } else {
						  //-> Gray it out
							STTextOut(x, y, MapFile[i].name, huntOffFontC);
							wsprintf(logt,"%d",MapFile[i].points);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOffFontC);
					  }
				  } else {
					  //Is selected
					STTextOut(x, y, MapFile[i].name, huntOnFontC);
					wsprintf(logt,"%d",MapFile[i].points);
					STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOnFontC);
				  }
				  y += 16;
			  }
			  //-> Dinosaurs
			  x = 215;
			  y = 382;
			  startAt = 7;
			  //hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
			  //-> Get Slider Pos 
			  SliderValue = Gui_GetSliderValue("slider_dinos")/2; //<- Make it less sensitive
			  //-> Start from pos, only allow as many dinos as there are slots (9 slots, so use a value of 10 because i+offset must be < offset+10)
			  for (int i = startAt+SliderValue; i < min(TotalC,startAt+SliderValue+10); i++) {
				  if (!DinoInfo[i].Selected ) {
					  //Dino is not selected
					  //-> Check if you can afford it...
					  if (TrophyRoom.Score - (SpentCredits+DinoInfo[i].Price) >= 0) {
							STTextOut(x, y, DinoInfo[i].Name, huntAvailFontC);
							wsprintf(logt,"%d",DinoInfo[i].Price);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntAvailFontC);
					  } else {
						  //-> Gray it out
						  if (DinoInfo[i].Hide && TrophyRoom.Score < DinoInfo[i].Price) {
							  STTextOut(x, y, "???", huntOffFontC);
						  } else {
							  STTextOut(x, y, DinoInfo[i].Name, huntOffFontC);
							  wsprintf(logt, "%d", DinoInfo[i].Price);
							  STTextOut(x + 155 - GetTextW(hdcCMain, logt), y, logt, huntOffFontC);
						  }
					  }
				  } else {
					  //Is selected
					STTextOut(x, y, DinoInfo[i].Name, huntOnFontC);
					wsprintf(logt,"%d",DinoInfo[i].Price);
					STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOnFontC);
				  }
				  y += 16;
			  }

			  //-> Weapons
			  x = 415;
			  y = 382;
			  startAt = 0;
			  //hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
			  //-> Get Slider Pos 
			  SliderValue = Gui_GetSliderValue("slider_weapons")/2; //<- Make it less sensitive
			  //-> Start from pos, only allow as many weapons as there are slots (9 slots, so use a value of 10 because i+offset must be < offset+10)
			  for (int i = SliderValue; i < min(TotalW,SliderValue+10); i++) {
				  if (strlen(WeapInfo[i].Name) == 0) continue;
				  if (!WeapInfo[i].Selected ) {
					  //Dino is not selected
					  //-> Check if you can afford it
					  if (TrophyRoom.Score - (SpentCredits+WeapInfo[i].Price) >= 0) {
							STTextOut(x, y, WeapInfo[i].Name, huntAvailFontC);
							wsprintf(logt,"%d",WeapInfo[i].Price);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntAvailFontC);
					  } else {
						  //-> Gray it out
							STTextOut(x, y, WeapInfo[i].Name, huntOffFontC);
							wsprintf(logt,"%d",WeapInfo[i].Price);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOffFontC);
					  }
				  } else {
					  //Is selected
					STTextOut(x, y, WeapInfo[i].Name, huntOnFontC);
					wsprintf(logt,"%d",WeapInfo[i].Price);
					STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOnFontC);
				  }
				  y += 16;
			  }

			  //-> Acessories
			  x = 615;
			  y = 382;
			  startAt = 0;
			  //hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
			  //-> Get Slider Pos 
			  SliderValue = 0;
			  //-> Start from pos, only allow as many acess as there are slots (9 slots, so use a value of 10 because i+offset must be < offset+10)
			  for (int i = 0; i < TotalA; i++) {
				  if (strlen(AcessInfo[i].name) == 0) continue;
				  if (!AcessInfo[i].Selected ) {
					  //Acess is not selected
					  //-> Check if you COULD afford it...
					  if (TrophyRoom.Score - (SpentCredits+AcessInfo[i].price) >= 0) {
							STTextOut(x, y, AcessInfo[i].name, huntAvailFontC);
							wsprintf(logt,"%d",AcessInfo[i].price);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntAvailFontC);
					  } else {
						  //-> Can't afford it, so gray it out
						  	STTextOut(x, y, AcessInfo[i].name, huntOffFontC);
							wsprintf(logt,"%d",AcessInfo[i].price);
							STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOffFontC);
					  }
				  } else {
					  //Is selected
					STTextOut(x, y, AcessInfo[i].name, huntOnFontC);
					wsprintf(logt,"%d",AcessInfo[i].price);
					STTextOut(x+155-GetTextW(hdcCMain,logt), y, logt, huntOnFontC);
				  }
				  y += 16;
			  }

			  //-> Now draw Menu Info Text..
			  if (strlen(ActiveInfoText) > 0 ) {
				  //-> Loop through it looking for line breaks (character code 13).
				  y = 78;
				  x = 413;
				  
				  char * pch;
				  pch = strtok(ActiveInfoText, "\n");
				  while (pch != NULL)
				  {
					  STTextOut(x, y, pch, huntInfoFontC);
					  y += 16;
					  pch = strtok(NULL, "\n");
				  }

				  if (DinoStatType == 1) {
					  DrawStatBar(526, 204, DinoInfo[DinoStatIndex].LookK, "Sight");
					  DrawStatBar(526, 224, DinoInfo[DinoStatIndex].SmellK, "Scent");
					  DrawStatBar(526, 244, DinoInfo[DinoStatIndex].HearK, "Hearing");
					  DinoStatType = 0;
				  } else if (DinoStatType == 2) {
					  DrawStatBar(526, 204, WeapInfo[DinoStatIndex].Power / 8, "Fire power:");
					  DrawStatBar(526, 224, WeapInfo[DinoStatIndex].Prec / 2, "Shot precision:");
					  DrawStatBar(526, 244, WeapInfo[DinoStatIndex].Loud / 2, "Volume:");
					  DrawStatBar(526, 264, WeapInfo[DinoStatIndex].Rate / 2, "Rate of fire:");
					  DinoStatType = 0;
				  }
			  }

			  //-> Draw Points and spent points...
			  hfntOld = (HFONT)SelectObject(hdcCMain, fnt_BIG);
			  wsprintf(logt,"%d",TrophyRoom.Score);
			  STTextOut(330, 41, logt, scoreFontC);
			  wsprintf(logt,"%d",TrophyRoom.Score - SpentCredits);
			  STTextOut(405, 41, logt, scoreFontC);
			  break;
		  }
	  case MENU_LOGIN:
		  {
			  //-> Draw User List
			  x = 315;
			  y = 370;
			  for (int usr = 0; usr < TOTAL_PLAYERS; usr++) {
				  //->Name
				  STTextOut(x, y, PlayerFile[usr].TrophyRoom.PlayerName, regOffFontC);
				  //->Score
				  // StartX = 488
				  wsprintf(logt,"%d",PlayerFile[usr].TrophyRoom.Score);
				  STTextOut(488-GetTextW(hdcMain,logt), y, logt, regOffFontC);
				  y += 18;
			  }
			  if (TOTAL_PLAYERS < 6) {
				  //->Draw '...'/empty slots
				  for (int slot = 0; slot < 6-TOTAL_PLAYERS; slot++) {
					  STTextOut(x, y, "...", regOffFontC);
					  y += 18;
				  }
			  }
			  break;
		  }
	  case MENU_STATS:
		  {
			  //-> Draw Player name and score
			  hfntOld = (HFONT)SelectObject(hdcCMain, fnt_BIG);
			  STTextOut(90, 10, TrophyRoom.PlayerName, scoreFontC);
			  wsprintf(logt,"%d",TrophyRoom.Score);
			  STTextOut(475, 10, logt, scoreFontC);

			  //-> Draw Stats
			  hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
			  UINT uAlignPrev = GetTextAlign(hdcCMain);
			  SetTextAlign(hdcCMain, TA_RIGHT);
			  STTextOut(711, 81, "Path travelled", mainStatFontC);
			  STTextOut(711, 101, "Time hunted", mainStatFontC);
			  STTextOut(711, 121, "Shots made", mainStatFontC);
			  STTextOut(711, 141, "Shots succeed", mainStatFontC);
			  STTextOut(711, 211, "Path travelled", mainStatFontC);
			  STTextOut(711, 231, "Time hunted", mainStatFontC);
			  STTextOut(711, 251, "Shots made", mainStatFontC);
			  STTextOut(711, 271, "Shots succeed", mainStatFontC);
			  STTextOut(711, 291, "Success ratio", mainStatFontC);
			  SetTextAlign(hdcCMain, uAlignPrev);
			  
			  if (TrophyRoom.Last.path >= 1000) {
				  int pathDec = ((int)(TrophyRoom.Last.path)) % 1000;
				  int pathDem = (((int)(TrophyRoom.Last.path)) - pathDec) / 1000;
				  float pathDec2 = pathDec / 100;
				  wsprintf(logt, "%d.%d m.", pathDem, (int)(pathDec2));
			  }
			  else {
				  wsprintf(logt, "%d m.", (int)(TrophyRoom.Last.path));
			  }
			  STTextOut(719, 81, logt, mainStatFontC);

			  int timeS = (int)(TrophyRoom.Last.time)%60;
			  int timeM = (((int)(TrophyRoom.Last.time)% 3600)- timeS)/60;
			  int timeH = ((int)(TrophyRoom.Last.time)- (timeM+timeS))/ 3600;
			  wsprintf(logt, "%d:%02d:%02d", timeH, timeM, timeS);
			  STTextOut(719, 101, logt, mainStatFontC);

			  wsprintf(logt, "%d", TrophyRoom.Last.smade);
			  STTextOut(719, 121, logt, mainStatFontC);
			  wsprintf(logt, "%d", TrophyRoom.Last.success);
			  STTextOut(719, 141, logt, mainStatFontC);
			  
			  
			  if (TrophyRoom.Total.path >= 1000) {
				  int pathDec = ((int)(TrophyRoom.Total.path)) % 1000;
				  int pathDem = (((int)(TrophyRoom.Total.path)) - pathDec) / 1000;
				  float pathDec2 = pathDec / 100;
				  wsprintf(logt, "%d.%d m.", pathDem, (int)(pathDec2));
			  } else {
				  wsprintf(logt, "%d m.", (int)(TrophyRoom.Total.path));
			  }
			  STTextOut(719, 211, logt, mainStatFontC);

			  timeS = (int)(TrophyRoom.Total.time) % 60;
			  timeM = (((int)(TrophyRoom.Total.time) % 3600) - timeS) / 60;
			  timeH = ((int)(TrophyRoom.Total.time) - (timeM + timeS)) / 3600;
			  wsprintf(logt, "%d:%02d:%02d", timeH, timeM, timeS);
			  STTextOut(719, 231, logt, mainStatFontC);

			  wsprintf(logt, "%d", TrophyRoom.Total.smade);
			  STTextOut(719, 251, logt, mainStatFontC);
			  wsprintf(logt, "%d", TrophyRoom.Total.success);
			  STTextOut(719, 271, logt, mainStatFontC);
			  
			  if (TrophyRoom.Total.smade)
			  wsprintf(logt, "%d%s", (int)(((float)TrophyRoom.Total.success/(float)TrophyRoom.Total.smade) * 100), "%");
			  else wsprintf(logt, "0%s","%");
			  STTextOut(719, 291, logt, mainStatFontC);
			  

			  break;
		  }
	  case MENU_QUIT:
	  case MENU_MAIN: 
		  {
		  //-> Draw Player name and score
			  //-> Set Font Size... (big, small, or middle)
		    hfntOld = (HFONT)SelectObject(hdcCMain, fnt_BIG);
			STTextOut(90, 10, TrophyRoom.PlayerName, scoreFontC);
			wsprintf(logt,"%d",TrophyRoom.Score);
			STTextOut(475, 10, logt, scoreFontC);
			break;
		  }
  }
  //if (ExitTime && ExitTime <= 4000) {	  
	 // int y = WinH / 3;
	 // wsprintf(buf,"Preparing for evacuation...");
  //    STTextOut(VideoCX - GetTextW(hdcMain, buf)/2, y, buf, RGB(128,255,128));
	 // wsprintf(buf,"%d seconds left.", 1 + ExitTime / 1000);
	 // STTextOut(VideoCX - GetTextW(hdcMain, buf)/2, y + 18, buf, RGB(128,255,128));
  //} else if (ExitTime) {
	 // // -> Show 'Connecting to Base...'
	 // int y = WinH / 3;
	 // wsprintf(buf,"Connecting to Base... Please Stand By");
  //    STTextOut(VideoCX - GetTextW(hdcMain, buf)/2, y, buf, RGB(128,255,128));
  //}

  SelectObject(hdcCMain,hfntOld);
  SelectObject(hdcCMain, hbmpOld);		  
    
}




void _RenderObject(int x, int y)
{	 	  	  
	  int ob = OMap[y][x];

	  if (!MObjects[ob].model) {
		  //return;
		  wsprintf(logt,"Incorrect model at [%d][%d]!", x, y);
		  DoHalt(logt);
	  }	  
	  
      int FI = (FMap[y][x] >> 2) & 3;	  
	  float fi = CameraAlpha + (float)(FI * 2.f*pi / 4.f);

	  int mlight;
	  if (MObjects[ob].info.flags & (ofDEFLIGHT+ofGRNDLIGHT) ) 
		  mlight = MObjects[ob].info.DefLight; else /*
	  if (MObjects[ob].info.flags & ofGRNDLIGHT) 
	  {		  		  
		  mlight = 128;		  
		  CalcModelGroundLight(MObjects[ob].model, x*256+128, y*256+128, FI);
		  FI = 0;
	  } 
	  else */
		  mlight = -(RandomMap[y & 31][x & 31] >> 5) + (LMap[y][x]>>1) + 96;

      if (mlight >192) mlight =192;
	  if (mlight < 64) mlight = 64;
      
	  v[0].x = x*256+128 - CameraX;
      v[0].z = y*256+128 - CameraZ;                   
      v[0].y = (float)(HMapO[y][x]) * ctHScale - CameraY;

	  waterclip = FALSE;

	  if (!UNDERWATER)
	    if (FMap[y][x] & fmWaterA) 
		  if (HMapO[y][x] < WaterList[ WMap[y][x] ].wlevel) {

			 if (WaterList[ WMap[y][x] ].wlevel * ctHScale > 
				 HMapO[y][x] * ctHScale + MObjects[ob].info.YHi) return;
			                     
             waterclipbase  = v[0];
             waterclipbase.y = WaterList[ WMap[y][x] ].wlevel * ctHScale - CameraY;
             waterclipbase = RotateVector(waterclipbase);
             waterclip = TRUE;
		  }
	  

	  float zs = VectorLength(v[0]);
      
      if (v[0].y + MObjects[ob].info.YHi < (int)(HMap[y][x]+HMap[y+1][x+1]) / 2 * ctHScale - CameraY) return;
	        
	  v[0] = RotateVector(v[0]);
	  GlassL = 0;
      
      if (zs > 256 * (ctViewR-4))
	   GlassL = (int)min(255, (zs/4 - 64*(ctViewR-4)));

	  if (GlassL==255) return;
	  
            
	  if (MObjects[ob].info.flags & ofANIMATED) 
	   if (MObjects[ob].info.LastAniTime!=RealTime) {
        MObjects[ob].info.LastAniTime=RealTime;	   
	/*	CreateMorphedObject(MObjects[ob].model,
		                    MObjects[ob].vtl,
							RealTime % MObjects[ob].vtl.AniTime);*/
	   }	  

	  

      if (MObjects[ob].info.flags & ofNOBMP) zs = 0;
	  if (zs>ctViewRM*256)
	 	  RenderBMPModel(&MObjects[ob].bmpmodel, v[0].x, v[0].y, v[0].z, mlight-16);		 	  
	  else 
      if (v[0].z<-256*12 && !waterclip)
       RenderModel(MObjects[ob].model, v[0].x, v[0].y, v[0].z, mlight, FI, fi, CameraBeta);	   
      else
       RenderModelClip(MObjects[ob].model, v[0].x, v[0].y, v[0].z, mlight, FI, fi, CameraBeta);       	   
	   
	   
}




void RenderMList()
{
   rmlistselector=1-rmlistselector;
   
   for (int o=0; o<ORLCount[rmlistselector]; o++)
        _RenderObject(ORList[rmlistselector][o].x, 
		              ORList[rmlistselector][o].y);
   ORLCount[rmlistselector] = 0;
}




void CreateChRenderList()
{
//=========== ship ================//   
       
/*
   if (Ship.State!=-1)
	   if (ri < ctViewR-6) {
		   int h = (int)((Ship.pos.y - GetLandUpH(Ship.pos.x, Ship.pos.z)) / 1.8);
           AddShadowCircle((int)Ship.pos.x+h, (int)Ship.pos.z+h, 1200, 24);
	   }
 */

   if (HARD3D) return;
   for (int c=0; c<=ctViewR; c++) 
     ChRenderList[c].ICount = 0;


//=========== ship ================//   
   
   Ship.rpos.x = Ship.pos.x - CameraX;
   Ship.rpos.y = Ship.pos.y - CameraY;
   Ship.rpos.z = Ship.pos.z - CameraZ;
   float r = (float)max( fabs(Ship.rpos.x), fabs(Ship.rpos.z) );
   int ri = -1 + (int)(r / 256.f + 1.6f);

   if (Ship.State==-1) goto NOSHIP;
   if (ri < 0) ri = 0;
   if (ri < ctViewR) {
      Ship.rpos = RotateVector(Ship.rpos);
      if (Ship.rpos.z > BackViewR) goto NOSHIP;
      if ( fabs(Ship.rpos.x) > -Ship.rpos.z + BackViewR ) goto NOSHIP;	  

      int i = ChRenderList[ri].ICount++;
      ChRenderList[ri].Items[i].CType = 3;
   }
NOSHIP: ;


   if (SupplyShip.State!=-1) goto NOSHIP2;
   SupplyShip.rpos.x = SupplyShip.pos.x - CameraX;
   SupplyShip.rpos.y = SupplyShip.pos.y - CameraY;
   SupplyShip.rpos.z = SupplyShip.pos.z - CameraZ;
   r = (float)max( fabs(SupplyShip.rpos.x), fabs(SupplyShip.rpos.z) );
   ri = -1 + (int)(r / 256.f + 1.6f);
   
   if (ri < 0) ri = 0;
   if (ri < ctViewR) {
      SupplyShip.rpos = RotateVector(SupplyShip.rpos);
      if (SupplyShip.rpos.z > BackViewR) goto NOSHIP2;
      if ( fabs(SupplyShip.rpos.x) > -SupplyShip.rpos.z + BackViewR ) goto NOSHIP2;	  

      int i = ChRenderList[ri].ICount++;
      ChRenderList[ri].Items[i].CType = 4;
   }
NOSHIP2: ;

   if (!AmmoBag.State) goto NOBAG;
   AmmoBag.rpos.x = AmmoBag.pos.x - CameraX;
   AmmoBag.rpos.y = AmmoBag.pos.y - CameraY;
   AmmoBag.rpos.z = AmmoBag.pos.z - CameraZ;
   r = (float)max( fabs(AmmoBag.rpos.x), fabs(AmmoBag.rpos.z) );
   ri = -2 + (int)(r / 256.f);
   
   if (ri < 0) ri = 0;
   if (ri < ctViewR) {
      AmmoBag.rpos = RotateVector(AmmoBag.rpos);
      if (AmmoBag.rpos.z > BackViewR) goto NOBAG;
      if ( fabs(AmmoBag.rpos.x) > -AmmoBag.rpos.z + BackViewR ) goto NOBAG;

      int i = ChRenderList[ri].ICount++;
      ChRenderList[ri].Items[i].CType = 5;
   }
NOBAG: ;

//============= Dinosaurs ====================//
   TCharacter *cptr;
   for (int c=0; c<ChCount; c++) {
      cptr = &Characters[c];
      cptr->rpos.x = cptr->pos.x - CameraX;
      cptr->rpos.y = cptr->pos.y - CameraY;
      cptr->rpos.z = cptr->pos.z - CameraZ;
      
      float r = (float)max( fabs(cptr->rpos.x), fabs(cptr->rpos.z) );
      int ri = -1 + (int)(r / 256.f + 0.5f);
      if (ri < 0) ri = 0;
      if (ri > ctViewR) continue;

      cptr->rpos = RotateVector(cptr->rpos);

	  float br = BackViewR + DinoInfo[cptr->CType].Radius;
      if (cptr->rpos.z > br) continue;
      if ( fabs(cptr->rpos.x) > -cptr->rpos.z + br ) continue;            
      if ( fabs(cptr->rpos.y) > -cptr->rpos.z + br ) continue;            

/*
      if (cptr->rpos.z > BackViewR + ) continue;
      if ( fabs(cptr->rpos.x) > -cptr->rpos.z + BackViewR ) continue;      
*/
//      AddShadowCircle((int)cptr->pos.x+100, (int)cptr->pos.z+100, 360, 16);

      int i = ChRenderList[ri].ICount++;
      ChRenderList[ri].Items[i].CType = 0;
      ChRenderList[ri].Items[i].Index = c;
   }



   
}


void RenderChList(int r)
{
  if (HARD3D) return;
  for (int c=0; c<ChRenderList[r].ICount; c++) {
      if (ChRenderList[r].Items[c].CType ==0) RenderCharacter(ChRenderList[r].Items[c].Index); else
	  if (ChRenderList[r].Items[c].CType ==3) RenderShip();
      if (ChRenderList[r].Items[c].CType ==4) RenderSupplyShip();
      if (ChRenderList[r].Items[c].CType ==5) RenderAmmoBag();
  }
}





void ProcessWaterMap(int x, int y, int r)
{
   //WATERREVERSE = TRUE;
   ReverseOn = (FMap[y][x] & fmReverse);
   TDirection = (FMap[y][x] & 3);      

   int t1 = TMap1[y][x];
   int t2 = TMap2[y][x];
   
   x = x - CCX + 64;
   y = y - CCY + 64;

   if ((VMap2[y][x].DFlags & VMap2[y][x+1].DFlags & VMap2[y+1][x+1].DFlags & VMap2[y+1][x].DFlags) == 0xFFFF) return;

   if (VMap2[y][x].DFlags!=0xFFFF) ev[0] = VMap2[y][x]; else ev[0] = VMap[y][x];
   if (VMap2[y][x+1].DFlags!=0xFFFF) ev[1] = VMap2[y][x+1]; else ev[1] = VMap[y][x+1];
   
   if (ReverseOn)
    if (VMap2[y+1][x].DFlags!=0xFFFF) ev[2] = VMap2[y+1][x]; else ev[2] = VMap[y+1][x];
   else
    if (VMap2[y+1][x+1].DFlags!=0xFFFF) ev[2] = VMap2[y+1][x+1]; else ev[2] = VMap[y+1][x+1];
      
   lpTextureAddr = &(Textures[0]->DataB[0]);
   HLineT = (void*) HLineTBGlass25; 
             
   if (!t1)
    if (r>4) DrawTPlane(FALSE);
      else DrawTPlaneClip(FALSE);    

   if (ReverseOn) {     
     ev[0] = ev[2];                
     if (VMap2[y+1][x+1].DFlags!=0xFFFF) ev[2] = VMap2[y+1][x+1]; else ev[2] = VMap[y+1][x+1];
   } else {
     ev[1] = ev[2];                
     if (VMap2[y+1][x].DFlags!=0xFFFF) ev[2] = VMap2[y+1][x]; else ev[2] = VMap[y+1][x];
   }
   

   if (!t2)
    if (r>4) DrawTPlane(TRUE);
       else DrawTPlaneClip(TRUE);
}



void RenderElements()
{
}

void RenderModelsList()
{ 
}

void RenderWater()
{
}

void RenderGround()
{   
   
   rmlistselector = 0;
   ORLCount[0] = 0;
   ORLCount[1] = 0;


   for (r=ctViewR; r>=ctViewR1-2; r-=2) {
     if (r<ctViewR1) LockWater = TRUE;
     for (int x=r; x>0; x-=2) {
      ProcessMap2(CCX-x, CCY+r, r);
      ProcessMap2(CCX+x, CCY+r, r);
	  ProcessMap2(CCX-x, CCY-r, r); 		
      ProcessMap2(CCX+x, CCY-r, r); 	
     }    
    
     ProcessMap2(CCX, CCY-r, r); 	
     ProcessMap2(CCX, CCY+r, r); 	

	 for (int y=r-2; y>0; y-=2) {
      ProcessMap2(CCX+r, CCY-y, r);
      ProcessMap2(CCX+r, CCY+y, r);
      ProcessMap2(CCX-r, CCY+y, r); 
      ProcessMap2(CCX-r, CCY-y, r);
     }
     ProcessMap2(CCX-r, CCY, r);
     ProcessMap2(CCX+r, CCY, r);
     RenderMList();
	 RenderMList();
	 RenderChList(r);
	 RenderChList(r-1);
   } 

   LockWater = FALSE;

   r = ctViewR1-1;
   for (int x=r; x>-r; x--) {
	   ProcessMap(CCX+r, CCY+x, r);
	   ProcessMap(CCX+x, CCY+r, r);
   }


   for (r=ctViewR1-2; r>0; r--) {
     
     for (int x=r; x>0; x--) {
      ProcessMap(CCX-x, CCY+r, r);
      ProcessMap(CCX+x, CCY+r, r);
	  ProcessMap(CCX-x, CCY-r, r); 		
      ProcessMap(CCX+x, CCY-r, r); 	
     }    	 
    
     ProcessMap(CCX, CCY-r, r); 	
     ProcessMap(CCX, CCY+r, r); 	

	 for (int y=r-1; y>0; y--) {
      ProcessMap(CCX+r, CCY-y, r);
      ProcessMap(CCX+r, CCY+y, r);
      ProcessMap(CCX-r, CCY+y, r); 
      ProcessMap(CCX-r, CCY-y, r);
     }
     ProcessMap(CCX-r, CCY, r);
     ProcessMap(CCX+r, CCY, r);
	 RenderMList();
     RenderChList(r);
   
   } 

   ProcessMap(CCX, CCY, 0);   
   RenderMList();
   RenderMList();
   RenderChList(0);
}



void RenderObject(int x, int y)
{
	if (OMap[y][x]==255) return;
	if (!MODELS) return;
	int o = ORLCount[rmlistselector];
	if (o>2000) return;
	ORList[rmlistselector][o].x = x;
	ORList[rmlistselector][o].y = y;
	ORLCount[rmlistselector]++;
}







void ProcessMap2(int x, int y, int r)
{
   //WATERREVERSE = FALSE;
   if (x>=ctMapSize-1 || y>=ctMapSize-1 ||
	   x<0 || y<0) return;      
  
   float BackR = BackViewR;
   if (OMap[y][x]!=255) BackR+=MObjects[OMap[y][x]].info.BoundR;

   ev[0] = VMap[y-CCY+128][x-CCX+128];            if (ev[0].v.z>BackR) return;         

   
   int t1 = TMap2[y][x];   
   int hw = WaterList[ WMap[y][x] ].wlevel-1;

   ReverseOn = FALSE;
   TDirection = ((FMap[y][x]>>8) & 3);   
                 

   int _x = x;
   int _y = y;
   x = x - CCX + 128;
   y = y - CCY + 128;
   ev[1] = VMap[y][x+2];            
   if (ReverseOn) ev[2] = VMap[y+2][x];          
             else ev[2] = VMap[y+2][x+2];              
 
   float xx = (ev[0].v.x + VMap[y+2][x+2].v.x) / 2;
   float yy = (ev[0].v.y + VMap[y+2][x+2].v.y) / 2;
   float zz = (ev[0].v.z + VMap[y+2][x+2].v.z) / 2;   
   int zs;

   if ( fabs(xx) > -zz + BackR) return;
    
   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);  
   if (zs > ctViewR*256) return;   
   GlassL = 0;      
   
   if (MIPMAP) ts = (int)CameraW * 4 * 128 / zs;   
          else ts = 128;
      
   if (ts>=128) {
    lpTextureAddr = &(Textures[t1]->DataA[0]);    
    HLineT = (void*) HLineTxGOURAUD; } else 
   if (ts>=64) {
    lpTextureAddr = &(Textures[t1]->DataB[0]);    
    HLineT = (void*) HLineTxB; }
   else {
    lpTextureAddr = &(Textures[t1]->DataC[0]);    
    HLineT = (void*) HLineTxC;
   }

   if (zs > 256 * (ctViewR-4)) {	             
	  GlassL = min(255, (zs/4 - 64*(ctViewR-4)));

	  if (GlassL) lpTextureAddr = &(Textures[t1]->SDataC[1]);
      if (GlassL)
         if (GlassL>160) HLineT = (void*) HLineTDGlass25; else
          if (GlassL>80 ) HLineT = (void*) HLineTDGlass50; else
                           HLineT = (void*) HLineTDGlass75;
   }


            

   //if (!UNDERWATER)
   //if (ReverseOn) {
   //    if ( (HMap[_y][_x]<hw) && (HMap[_y][_x+2]<hw) && (HMap[_y+2][_x]<hw) )   goto S1;
   //} else {
	  // if ( (HMap[_y][_x]<hw) && (HMap[_y][_x+2]<hw) && (HMap[_y+2][_x+1]<hw) )   goto S1;
   //}

   DrawTPlane(FALSE);   

S1:

/*   if (!UNDERWATER)
   if (ReverseOn) {
	   if ( (HMap[_y][_x+2]<hw) && (HMap[_y+2][_x+2]<hw) && (HMap[_y+2][_x]<hw) )   goto S2;       
   } else {
	   if ( (HMap[_y][_x]<hw) && (HMap[_y+2][_x+2]<hw) && (HMap[_y+2][_x]<hw) )   goto S2;
   }  */ 

   if (ReverseOn) { ev[0] = ev[2]; ev[2] = VMap[y+2][x+2]; } 
             else { ev[1] = ev[2]; ev[2] = VMap[y+2][x];   }   

   DrawTPlane(TRUE);
S2:   

   x = x + CCX - 128;
   y = y + CCY - 128;
   
   if (!LockWater) {
    RenderObject(x  , y);
    RenderObject(x+1, y);
    RenderObject(x  , y+1);
    RenderObject(x+1, y+1);     
    if (FMap[y][x] & fmWaterA) ProcessMapW2(x,y,r);
   }
}



void ProcessMap(int x, int y, int r)
{    
   if (x>=ctMapSize-1 || y>=ctMapSize-1 ||
	   x<0 || y<0) return;      
  
   float BackR = BackViewR;
   if (OMap[y][x]!=255) BackR+=MObjects[OMap[y][x]].info.BoundR;
   int zs;
   float xx,yy,zz;
   int hw = WaterList[ WMap[y][x] ].wlevel; 


   ev[0] = VMap[y-CCY+128][x-CCX+128];            if (ev[0].v.z>BackR) return;   

   BOOL
   wpr = ((FMap[y  ][x  ] & fmWaterA) &&
	      (FMap[y  ][x+1] & fmWaterA) &&	   
		  (FMap[y+1][x  ] & fmWaterA) &&
		  (FMap[y+1][x+1] & fmWaterA) );
   
   

   int ob = OMap[y][x];
   if (!MODELS) ob=255;
   
   int t1 = TMap1[y][x];   
   ReverseOn = (FMap[y][x] & fmReverse);   
   TDirection = (FMap[y][x] & 3);        
   
   int _x = x;
   int _y = y;
   x = x - CCX + 128;
   y = y - CCY + 128;
   

   ev[1] = VMap[y][x+1];            
   if (ReverseOn) ev[2] = VMap[y+1][x];          
             else ev[2] = VMap[y+1][x+1];                  
 
   xx = (ev[0].v.x + VMap[y+1][x+1].v.x) / 2;
   yy = (ev[0].v.y + VMap[y+1][x+1].v.y) / 2;
   zz = (ev[0].v.z + VMap[y+1][x+1].v.z) / 2;   
   

   if ( fabs(xx) > -zz + BackR) return;
    
   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);  
   if (zs > ctViewR*256) return;
   
   GlassL = 0;
      
   
   if (MIPMAP) ts = (int)CameraW * 4 * 128 / zs;   
          else ts = 128;
      
   if (ts>=128) {
    lpTextureAddr = &(Textures[t1]->DataA[0]);    
    HLineT = (void*) HLineTxGOURAUD; } else 
   if (ts>=64) {
    lpTextureAddr = &(Textures[t1]->DataB[0]);    
    HLineT = (void*) HLineTxB; }
   else {
    lpTextureAddr = &(Textures[t1]->DataC[0]);    
    HLineT = (void*) HLineTxC;
   }
//Water Transparency: adelphspro 4.14.09
   //if (!UNDERWATER)
   //if (wpr)
   //if (ReverseOn) {
   //    if ( (HMap[_y][_x]<hw) || (HMap[_y][_x+1]<hw) || (HMap[_y+1][_x]<hw) )   goto S1;
   //} else {
	  // if ( (HMap[_y][_x]<hw) || (HMap[_y][_x+1]<hw) || (HMap[_y+1][_x+1]<hw) )   goto S1;
   //}

            
   if (r>6) DrawTPlane(FALSE);
     else DrawTPlaneClip(FALSE);    
S1:
   if (ReverseOn) { ev[0] = ev[2]; ev[2] = VMap[y+1][x+1]; } 
             else { ev[1] = ev[2]; ev[2] = VMap[y+1][x];   }

   //if (!UNDERWATER)
   //if (wpr)
   //if (ReverseOn) {
	  // if ( (HMap[_y][_x+1]<hw) || (HMap[_y+1][_x+1]<hw) || (HMap[_y+1][_x]<hw) )   goto S2;       
   //} else {
	  // if ( (HMap[_y][_x]<hw) || (HMap[_y+1][_x+1]<hw) || (HMap[_y+1][_x]<hw) )   goto S2;
   //}   

   if (r>6) DrawTPlane(TRUE);
       else DrawTPlaneClip(TRUE);
S2:
   x = x + CCX - 128;
   y = y + CCY - 128;   

//SKIP:  

   RenderObject(x, y);

   if (wpr) ProcessMapW(x,y,r);

}






void ProcessMapW(int x, int y, int r)
{ 
#ifdef __disable_water_in_software
   if (RunMode) return; // alacn
#endif
   
   
   if (x>=ctMapSize-1 || y>=ctMapSize-1 || x<0 || y<0) return;              

   int t1 = WaterList[ WMap[y][x] ].tindex;
   int hw = WaterList[ WMap[y][x] ].wlevel;

   ev[0] = VMap2[y-CCY+128][x-CCX+128];   
   if (ev[0].v.z>BackViewR) return;

   
   ReverseOn = (FMap[y][x] & fmReverse);   
   TDirection = 0;
//   if ( (HMap[y][x]>hw) || (HMap[y+1][x+1]>hw) ) ReverseOn = TRUE;

   

   int _x = x; 
   int _y = y;

   x = x - CCX + 128;
   y = y - CCY + 128;
   ev[1] = VMap2[y][x+1];            
   if (ReverseOn) ev[2] = VMap2[y+1][x];          
             else ev[2] = VMap2[y+1][x+1];          

   float xx = (ev[0].v.x + VMap2[y+1][x+1].v.x) / 2;
   float yy = (ev[0].v.y + VMap2[y+1][x+1].v.y) / 2;
   float zz = (ev[0].v.z + VMap2[y+1][x+1].v.z) / 2;   

   int zs;

   if ( fabs(xx*FOVK) > -zz + BackViewR) return;

   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);  
   if (zs > ctViewR*256) return;
   
   GlassL = 0;
         
   if (MIPMAP) ts = (int)CameraW * 4 * 128 / zs;   
          else ts = 128;

   ts = 128;

   if (ts>=128) {
    lpTextureAddr = &(Textures[t1]->DataA[0]);    
    HLineT = (void*) HLineTxGOURAUD; } else 
   if (ts>=64) { 
    lpTextureAddr = &(Textures[t1]->DataB[0]);    
    HLineT = (void*) HLineTxB; 	
   }
   else {
    lpTextureAddr = &(Textures[t1]->DataC[0]);    
    HLineT = (void*) HLineTxC;
   }            
 //-> 4.14.09: adelphospro, adding transparency to water
  // if (UNDERWATER) {
       lpTextureAddr = &(Textures[t1]->DataB[0]);    
	   HLineT = (void*) HLineTBGlass25; //From HLineTBGlass25
  // }

   WATERREVERSE = UNDERWATER;

   if (ReverseOn) {
       if ( (HMap[_y][_x]>hw) || (HMap[_y][_x+1]>hw) || (HMap[_y+1][_x]>hw) )   goto S1;
   } else {
	   if ( (HMap[_y][_x]>hw) || (HMap[_y][_x+1]>hw) || (HMap[_y+1][_x+1]>hw) )   goto S1;
   }

   if (r>6) DrawTPlane(FALSE);
     else DrawTPlaneClip(FALSE);    
S1:
   if (ReverseOn) { ev[0] = ev[2]; ev[2] = VMap2[y+1][x+1]; } 
             else { ev[1] = ev[2]; ev[2] = VMap2[y+1][x];   }

   if (ReverseOn) {
	   if ( (HMap[_y][_x+1]>hw) || (HMap[_y+1][_x+1]>hw) || (HMap[_y+1][_x]>hw) )   goto S2;       
   } else {
	   if ( (HMap[_y][_x]>hw) || (HMap[_y+1][_x+1]>hw) || (HMap[_y+1][_x]>hw) )   goto S2;
   }

   if (r>6) DrawTPlane(TRUE);
       else DrawTPlaneClip(TRUE);
S2:
   WATERREVERSE = FALSE;
}



void ProcessMapW2(int x, int y, int r)
{ 
#ifdef __disable_water_in_software
   if (RunMode) return; // alacn
#endif
   if (!( (FMap[y  ][x  ] & fmWaterA) &&
	      (FMap[y  ][x+2] & fmWaterA) &&	   
		  (FMap[y+2][x  ] & fmWaterA) &&
		  (FMap[y+2][x+2] & fmWaterA) )) return;

   if (x>=ctMapSize-1 || y>=ctMapSize-1 || x<0 || y<0) return;              

   int t1 = WaterList[ WMap[y][x] ].tindex;
   int hw = WaterList[ WMap[y][x] ].wlevel;

   ev[0] = VMap2[y-CCY+128][x-CCX+128];   
   if (ev[0].v.z>BackViewR) return;

   
// ReverseOn = (FMap[y][x] & fmReverse);   
   TDirection = 0;
   if ( (HMap[y][x]>hw) || (HMap[y+2][x+2]>hw) ) ReverseOn = TRUE;


   int _x = x; 
   int _y = y;

   x = x - CCX + 128;
   y = y - CCY + 128;
   ev[1] = VMap2[y][x+2];            
   if (ReverseOn) ev[2] = VMap2[y+2][x];          
             else ev[2] = VMap2[y+2][x+2];          

   float xx = (ev[0].v.x + VMap2[y+1][x+2].v.x) / 2;
   float yy = (ev[0].v.y + VMap2[y+1][x+2].v.y) / 2;
   float zz = (ev[0].v.z + VMap2[y+1][x+2].v.z) / 2;   

   int zs;

   if ( fabs(xx*FOVK) > -zz + BackViewR) return;

   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);  
   if (zs > ctViewR*256) return;

   
   GlassL = 0;
         
   if (MIPMAP) ts = (int)CameraW * 4 * 128 / zs;   
          else ts = 128;
   

  // if (UNDERWATER) {
		  //-> Adelphospro: 4.14.09: adding transparency to water
    lpTextureAddr = &(Textures[t1]->DataB[0]);    
    HLineT = (void*) HLineTBGlass25;
/*   } else {
    lpTextureAddr = &(Textures[t1]->DataC[0]);    
    HLineT = (void*) HLineTxC;
   }   */         


   WATERREVERSE = UNDERWATER;

   if (ReverseOn) {
       if ( (HMap[_y][_x]>hw) && (HMap[_y][_x+2]>hw) && (HMap[_y+2][_x]>hw) )   goto S1;
   } else {
	   if ( (HMap[_y][_x]>hw) && (HMap[_y][_x+2]>hw) && (HMap[_y+2][_x+2]>hw) )   goto S1;
   }

   if (r>6) DrawTPlane(FALSE);
     else DrawTPlaneClip(FALSE);    
S1:
   if (ReverseOn) { ev[0] = ev[2]; ev[2] = VMap2[y+2][x+2]; } 
             else { ev[1] = ev[2]; ev[2] = VMap2[y+2][x];   }

   if (ReverseOn) {
	   if ( (HMap[_y][_x+2]>hw) && (HMap[_y+2][_x+2]>hw) && (HMap[_y+2][_x]>hw) )   goto S2;       
   } else {
	   if ( (HMap[_y][_x]>hw) && (HMap[_y+2][_x+2]>hw) && (HMap[_y+2][_x]>hw) )   goto S2;
   }

   if (r>6) DrawTPlane(TRUE);
       else DrawTPlaneClip(TRUE);
S2:
   WATERREVERSE = FALSE;
}







#include "RenderASM.cpp"



void ClipVector(CLIPPLANE& C, int vn)
{
  int ClipRes = 0;
  float s,s1,s2;
  int vleft  = (vn-1); if (vleft <0) vleft=vused-1;
  int vright = (vn+1); if (vright>=vused) vright=0;
  
  MulVectorsScal(cp[vn].ev.v, C.nv, s); /*s=SGN(s-0.01f);*/
  if (s>=0) return;

  MulVectorsScal(cp[vleft ].ev.v, C.nv, s1); /* s1=SGN(s1+0.01f); */ //s1+=0.001f;
  MulVectorsScal(cp[vright].ev.v, C.nv, s2); /* s2=SGN(s2+0.01f); */ //s2+=0.001f;
  
  if (s1>0) {
   ClipRes+=1;
  
   /*
   CalcHitPoint(C,cp[vn].ev.v,
                  cp[vleft].ev.v, hleft.ev.v);     

   float ll = VectorLength(SubVectors(cp[vleft].ev.v, cp[vn].ev.v));
   float lc = VectorLength(SubVectors(hleft.ev.v, cp[vn].ev.v));
   lc = lc / ll;
   */
     
   
   float lc = -s / (s1-s);
   hleft.ev.v.x = cp[vn].ev.v.x + ((cp[vleft].ev.v.x - cp[vn].ev.v.x) * lc);
   hleft.ev.v.y = cp[vn].ev.v.y + ((cp[vleft].ev.v.y - cp[vn].ev.v.y) * lc);
   hleft.ev.v.z = cp[vn].ev.v.z + ((cp[vleft].ev.v.z - cp[vn].ev.v.z) * lc);
   
   hleft.tx = cp[vn].tx + (int)((cp[vleft].tx - cp[vn].tx) * lc);
   hleft.ty = cp[vn].ty + (int)((cp[vleft].ty - cp[vn].ty) * lc);
   hleft.ev.Light = cp[vn].ev.Light + (int)((cp[vleft].ev.Light - cp[vn].ev.Light) * lc);
  }

  if (s2>0) {
   ClipRes+=2;
   /*
   CalcHitPoint(C,cp[vn].ev.v,
                  cp[vright].ev.v, hright.ev.v);  

   float ll = VectorLength(SubVectors(cp[vright].ev.v, cp[vn].ev.v));
   float lc = VectorLength(SubVectors(hright.ev.v, cp[vn].ev.v));
   lc = lc / ll;
    */
   
   float lc = -s / (s2-s);
   hright.ev.v.x = cp[vn].ev.v.x + ((cp[vright].ev.v.x - cp[vn].ev.v.x) * lc);
   hright.ev.v.y = cp[vn].ev.v.y + ((cp[vright].ev.v.y - cp[vn].ev.v.y) * lc);
   hright.ev.v.z = cp[vn].ev.v.z + ((cp[vright].ev.v.z - cp[vn].ev.v.z) * lc);
   
   hright.tx = cp[vn].tx + (int)((cp[vright].tx - cp[vn].tx) * lc);
   hright.ty = cp[vn].ty + (int)((cp[vright].ty - cp[vn].ty) * lc);
   hright.ev.Light = cp[vn].ev.Light + (int)((cp[vright].ev.Light - cp[vn].ev.Light) * lc);
  }

  if (ClipRes == 0) {
      u--; vused--; 
      cp[vn] = cp[vn+1];
      cp[vn+1] = cp[vn+2];
      cp[vn+2] = cp[vn+3];
      cp[vn+3] = cp[vn+4];
      cp[vn+4] = cp[vn+5];
      cp[vn+5] = cp[vn+6];
      //memcpy(&cp[vn], &cp[vn+1], (15-vn)*sizeof(ClipPoint)); 
  }
  if (ClipRes == 1) {cp[vn] = hleft; }
  if (ClipRes == 2) {cp[vn] = hright;}
  if (ClipRes == 3) {
    u++; vused++;
    //memcpy(&cp[vn+1], &cp[vn], (15-vn)*sizeof(ClipPoint)); 
    cp[vn+6] = cp[vn+5];
    cp[vn+5] = cp[vn+4];
    cp[vn+4] = cp[vn+3];
    cp[vn+3] = cp[vn+2];
    cp[vn+2] = cp[vn+1];
    cp[vn+1] = cp[vn];
           
    cp[vn] = hleft;
    cp[vn+1] = hright;
    } 
}










int  SGNi(int f)
{ 
  if (f<0) return -1;
      else return  1;  
}


void DrawTPlaneClip(BOOL SECONT)
{	
   int n;

   if (!WATERREVERSE) {
    MulVectorsVect(SubVectors(ev[1].v, ev[0].v), SubVectors(ev[2].v, ev[0].v), nv);   
    if (nv.x*ev[0].v.x  +  nv.y*ev[0].v.y  +  nv.z*ev[0].v.z<0) return;       
   }

   cp[0].ev = ev[0]; cp[1].ev = ev[1]; cp[2].ev = ev[2];

   if (ReverseOn) 
    if (SECONT) {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
	   cp[2].tx = TCMAX;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;
       break;        
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
     }
    } else {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMAX;       
       break; 
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
     }
    }
   else
    if (SECONT) {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMAX;       
       break;
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
     } 
    } else {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;       
       break;
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
     }
    }

/*
   cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
   
   if (SECONT) {
    cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
	cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
   } else {
    cp[1].tx = TCMAX;   cp[1].ty = TCMIN;	 	 
    cp[2].tx = TCMAX;   cp[2].ty = TCMAX;
   }*/
   
   vused = 3;

   for (u=0; u<vused; u++) cp[u].ev.v.z+=12.0f;
   for (u=0; u<vused; u++) ClipVector(ClipZ,u);
   for (u=0; u<vused; u++) cp[u].ev.v.z-=12.0f;
   if (vused<3) return;

   for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) return; 
   for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) return; 
   for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) return; 
   for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) return; 
         
   //float dy = -1.1f;
   
   //if (WATERREVERSE) dy = 0;
   for (u=0; u<vused; u++) {     
     cp[u].ev.scrx = VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW);    
	 cp[u].ev.scry = VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH);  
   }
   
 

   scrp[0].x     = cp[0].ev.scrx;
   scrp[0].y     = cp[0].ev.scry;
   scrp[0].Light = cp[0].ev.Light/4;
   scrp[0].tx    = (int)cp[0].tx;
   scrp[0].ty    = (int)cp[0].ty;
   scrp[0].z     = (int)cp[0].ev.v.z;

   for (u=0; u<vused-2; u++) {
    for (n=1; n<3; n++) {
	 scrp[n].x     = cp[n+u].ev.scrx;
     scrp[n].y     = cp[n+u].ev.scry;
     scrp[n].Light = cp[n+u].ev.Light/4;
     scrp[n].tx    = (int)cp[n+u].tx;
     scrp[n].ty    = (int)cp[n+u].ty;
     scrp[n].z     = (int)cp[n+u].ev.v.z;
    }
     if (CORRECTION) DrawCorrectedTexturedFace();
                else DrawTexturedFace();
	}            
}



void DrawTPlane(BOOL SECONT)
{
   int n;   
   
   if (!WATERREVERSE)    
   if ((ev[1].scrx-ev[0].scrx)*(ev[2].scry-ev[0].scry) - 
       (ev[1].scry-ev[0].scry)*(ev[2].scrx-ev[0].scrx) < 0) return;

   Mask1=0x007F;   
   for (n=0; n<3; n++) {     
	 if (ev[n].DFlags & 128) return;     
     Mask1=Mask1 & ev[n].DFlags;  }
   if (Mask1>0) return;
    
   for (n=0; n<3; n++) {     
	 scrp[n].x = ev[n].scrx;
	 scrp[n].y = ev[n].scry;
	 scrp[n].Light = ev[n].Light / 4;	 	 
   }

   if (ReverseOn) 
    if (SECONT) {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
	   scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;        
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
     }
    } else {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;       
       break; 
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
     }
    }
   else
    if (SECONT) {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;       
       break;
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
     } 
    } else {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;       
       break;
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
     }
    }
   
   DrawTexturedFace();
}








void BuildTree()
{
    Vector2di v[3];
	Current = -1;
    TFace* fptr;
    int sg;

	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
        v[0] = gScrp[fptr->v1]; 
        v[1] = gScrp[fptr->v2]; 
        v[2] = gScrp[fptr->v3];

        if (v[0].x == 0xFFFFFF) continue;
        if (v[1].x == 0xFFFFFF) continue;
        if (v[2].x == 0xFFFFFF) continue; 

        //fptr->Flags &= 0x00FF;

        if (fptr->Flags & (sfDarkBack + sfNeedVC) ) {
           sg = (v[1].x-v[0].x)*(v[2].y-v[1].y) - (v[1].y-v[0].y)*(v[2].x-v[1].x);
           if (sg<0) continue;
/*
          if (fptr->Flags & sfNeedVC) { if (sg<0) continue; } 
                                 else if (sg<0) fptr->Flags |= sfDark; */
        }

        //if (NODARKBACK) fptr->Flags &= 0x00FF;
        
		fptr->Distant = (int)(-(rVertex[fptr->v1].z + rVertex[fptr->v2].z + rVertex[fptr->v3].z));
		fptr->Next=-1;
		if (Current==-1) Current=f; else
		 if (mptr->gFace[Current].Distant < fptr->Distant)
         { fptr->Next=Current; Current=f; } else {
			int n=Current;
		    while (mptr->gFace[n].Next!=-1 && mptr->gFace[mptr->gFace[n].Next].Distant > fptr->Distant)
		      	 n=mptr->gFace[n].Next;
            fptr->Next = mptr->gFace[n].Next;
		    mptr->gFace[n].Next = f;	}		
	}
}

void BuildTreeNoSort()
{
    Vector2di v[3];
	Current = -1;
    int LastFace = -1;
    TFace* fptr;
    int sg;
    
	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
  		v[0] = gScrp[fptr->v1]; 
        v[1] = gScrp[fptr->v2]; 
        v[2] = gScrp[fptr->v3];

        if (v[0].x == 0xFFFFFF) continue;
        if (v[1].x == 0xFFFFFF) continue;
        if (v[2].x == 0xFFFFFF) continue;         

        if (fptr->Flags & (sfDarkBack+sfNeedVC)) {
           sg = (v[1].x-v[0].x)*(v[2].y-v[1].y) - (v[1].y-v[0].y)*(v[2].x-v[1].x);
           if (sg<0) continue;
        }        
					
		fptr->Next=-1;
        if (Current==-1) { Current=f; LastFace = f; } else 
        { mptr->gFace[LastFace].Next=f; LastFace=f; }
		
	}
}














void BuildTreeClip()
{
	Current = -1;
    TFace* fptr;

	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
        
        if (fptr->Flags & (sfDarkBack + sfNeedVC) ) {
         MulVectorsVect(SubVectors(rVertex[fptr->v2], rVertex[fptr->v1]), SubVectors(rVertex[fptr->v3], rVertex[fptr->v1]), nv);
         if (nv.x*rVertex[fptr->v1].x  +  nv.y*rVertex[fptr->v1].y  +  nv.z*rVertex[fptr->v1].z<0) continue;
        }
                
		fptr->Distant = (int)(-(rVertex[fptr->v1].z + rVertex[fptr->v2].z + rVertex[fptr->v3].z));
		fptr->Next=-1;
		if (Current==-1) Current=f; else
		 if (mptr->gFace[Current].Distant < fptr->Distant)
         { fptr->Next=Current; Current=f; } else {
			int n=Current;
		    while (mptr->gFace[n].Next!=-1 && mptr->gFace[mptr->gFace[n].Next].Distant > fptr->Distant)
		      	 n=mptr->gFace[n].Next;
            fptr->Next = mptr->gFace[n].Next;
		    mptr->gFace[n].Next = f;	}		
	}
}



void BuildTreeClipNoSort()
{
	Current = -1;
    int LastFace = -1;
    TFace* fptr;

	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
        
        if (fptr->Flags & (sfDarkBack + sfNeedVC) ) {
         MulVectorsVect(SubVectors(rVertex[fptr->v2], rVertex[fptr->v1]), SubVectors(rVertex[fptr->v3], rVertex[fptr->v1]), nv);
         if (nv.x*rVertex[fptr->v1].x  +  nv.y*rVertex[fptr->v1].y  +  nv.z*rVertex[fptr->v1].z<0) continue;
        }

        fptr->Next=-1;
        if (Current==-1) { Current=f; LastFace = f; } else 
        { mptr->gFace[LastFace].Next=f; LastFace=f; }
                		
	}
}


















void RenderModelClip(TModel* _mptr, float x0, float y0, float z0, int light, int VT, float al, float bt)
{   
   int f,CMASK;   

   mptr = _mptr;
   
   float ca = (float)cos(al);
   float sa = (float)sin(al);   
   
   float cb = (float)cos(bt);
   float sb = (float)sin(bt);   

   
   
   __asm {
      mov eax,light      
      shr eax,2	  
      shl eax,16
      add eax, offset FadeTab
      mov iModelFade,eax
      mov iModelBaseFade,eax
   }

            
   HLineT = (void*) HLineTxModel;
   lpTextureAddr = (void*) mptr->lpTexture;
   
   BOOL BL = FALSE;
   for (int s=0; s<mptr->VCount; s++) {              
    rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa)   + x0;
    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;
    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) + z0;     
    if (rVertex[s].z<0) BL=TRUE;

    if (rVertex[s].z>-64) { gScrp[s].x = 0xFFFFFF; gScrp[s].y = 0xFF; }
    else {
     int f = 0;
     int sx =  VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     int sy =  VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 

     if (sx<=0    ) f+=2;
     if (sx>=WinEX) f+=1;
     if (sy>=WinEY) f+=4;
     if (sy<=0    ) f+=8;
     

     if (f) { gScrp[s].x = 0xFFFFFF; gScrp[s].y = f; }
       else { gScrp[s].x = sx; gScrp[s].y = sy; }
    }  

   }   
   
   if (!BL) return;
     
   BuildTreeClip();   

   f = Current;
   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    


//======== if face fully on screen NO 3D CLIPPING ====================/
    if (!waterclip)
    if (gScrp[fptr->v1].x != 0xFFFFFF && gScrp[fptr->v2].x != 0xFFFFFF && gScrp[fptr->v3].x != 0xFFFFFF ) 
    {        
       mscrp[0].x  = gScrp[fptr->v1].x;  mscrp[0].y  = gScrp[fptr->v1].y;
       mscrp[0].tx = fptr->tax;          mscrp[0].ty = fptr->tay;

       mscrp[1].x  = gScrp[fptr->v2].x;  mscrp[1].y  = gScrp[fptr->v2].y;
       mscrp[1].tx = fptr->tbx;          mscrp[1].ty = fptr->tby;

       mscrp[2].x  = gScrp[fptr->v3].x;  mscrp[2].y  = gScrp[fptr->v3].y;
       mscrp[2].tx = fptr->tcx;          mscrp[2].ty = fptr->tcy;     

       OpacityMode = (fptr->Flags & (sfOpacity + sfTransparent));
       if (fptr->Flags & sfDark)
            iModelFade = iModelBaseFade + 12*256*256;
       else iModelFade = iModelBaseFade;
	 
       DrawModelFace();
       goto LNEXT;
    } 
    
    
    CMASK = 0;
    if (gScrp[fptr->v1].x == 0xFFFFFF) CMASK|=gScrp[fptr->v1].y;
    if (gScrp[fptr->v2].x == 0xFFFFFF) CMASK|=gScrp[fptr->v2].y;
    if (gScrp[fptr->v3].x == 0xFFFFFF) CMASK|=gScrp[fptr->v3].y; 
//  CMASK = 0xFF;
    

    cp[0].ev.v = rVertex[fptr->v1];  cp[0].tx = (float)fptr->tax;  cp[0].ty = (float)fptr->tay; 
    cp[1].ev.v = rVertex[fptr->v2];  cp[1].tx = (float)fptr->tbx;  cp[1].ty = (float)fptr->tby; 
    cp[2].ev.v = rVertex[fptr->v3];  cp[2].tx = (float)fptr->tcx;  cp[2].ty = (float)fptr->tcy; 

    if (CMASK == 0xFF) {
     for (u=0; u<vused; u++) cp[u].ev.v.z+=12.0f;
     for (u=0; u<vused; u++) ClipVector(ClipZ,u);
     for (u=0; u<vused; u++) cp[u].ev.v.z-=12.0f;
     if (vused<3) goto LNEXT;
    }

    if (waterclip) {
     for (u=0; u<vused; u++) { cp[u].ev.v.x-=waterclipbase.x; cp[u].ev.v.y-=waterclipbase.y; cp[u].ev.v.z-=waterclipbase.z; }
     for (u=0; u<vused; u++) ClipVector(ClipW,u);
     for (u=0; u<vused; u++) { cp[u].ev.v.x+=waterclipbase.x; cp[u].ev.v.y+=waterclipbase.y; cp[u].ev.v.z+=waterclipbase.z; }
     if (vused<3) goto LNEXT;
    }
    
    if (CMASK & 1) for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) goto LNEXT;
    if (CMASK & 2) for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) goto LNEXT;
    
    if (CMASK & 4) for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) goto LNEXT;    
    if (CMASK & 8) for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) goto LNEXT;
             
    for (u=0; u<vused; u++) {     
     cp[u].ev.scrx = VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW);    
	 cp[u].ev.scry = VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH);  
    }
   
    mscrp[0].x     = cp[0].ev.scrx;
    mscrp[0].y     = cp[0].ev.scry;
    mscrp[0].tx    = (int)cp[0].tx;
    mscrp[0].ty    = (int)cp[0].ty;

    OpacityMode = (fptr->Flags & (sfOpacity + sfTransparent));

     for (u=0; u<vused-2; u++) {
      for (int n=1; n<3; n++) {
	   mscrp[n].x     = cp[n+u].ev.scrx;
       mscrp[n].y     = cp[n+u].ev.scry;     
       mscrp[n].tx    = (int)cp[n+u].tx;
       mscrp[n].ty    = (int)cp[n+u].ty;  }

       DrawModelFace();     
     }            
LNEXT:
     f = mptr->gFace[f].Next;
   }
}





void RenderModelClipWater(TModel* _mptr, float x0, float y0, float z0, int light, int VT, float al, float bt)
{   
   int f;

   mptr = _mptr;
   
   float ca = (float)cos(al);
   float sa = (float)sin(al);   
   
   float cb = (float)cos(bt);
   float sb = (float)sin(bt);   


   
   __asm {
      mov eax,light      
      shr eax,2
      shl eax,16
      add eax, offset FadeTab
      mov iModelFade,eax
      mov iModelBaseFade,eax
   }

   
//=================== select mipmap & glass =============================//

   if (!GlassL) HLineT = (void*) HLineTxModel; else
	 if (GlassL>160) HLineT = (void*) HLineTxModel25; else
	  if (GlassL>80 ) HLineT = (void*) HLineTxModel50; else	    
		HLineT = (void*) HLineTxModel75; 	   	 
         
   lpTextureAddr = (void*) mptr->lpTexture;

   if (GlassL) lpTextureAddr = (void*) mptr->lpTexture3; 
     else
   if (ts <=64)
   if (ts > 32) {
        lpTextureAddr = (void*) mptr->lpTexture2;
        HLineT = (void*) HLineTxModel2;
   } else {
        lpTextureAddr = (void*) mptr->lpTexture3;
        HLineT = (void*) HLineTxModel3;
   }


   
   BOOL BL = FALSE;
   float sg;
   for (int s=0; s<mptr->VCount; s++) {              
    rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa)   + x0;
    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;
    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) + z0;     

    v[0] = SubVectors(rVertex[s], waterclipbase);
    MulVectorsScal(v[0], ClipW.nv, sg); 
    if (sg>=0) gScrp[s].x = 0; else gScrp[s].x=1;

    if (rVertex[s].z<0) BL=TRUE;
   }   
   
   if (!BL) return;
   
   if (fabs(z0) + fabs(x0)>256*10) 
      BuildTreeClipNoSort(); else BuildTreeClip();
   

   f = Current;
   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    

    if (rVertex[fptr->v1].z > -128) goto LNEXT;
    if (rVertex[fptr->v2].z > -128) goto LNEXT;
    if (rVertex[fptr->v3].z > -128) goto LNEXT;

    if (gScrp[fptr->v1].x & gScrp[fptr->v2].x & gScrp[fptr->v3].x)  goto LNEXT;    
              
    cp[0].ev.v = rVertex[fptr->v1];  cp[0].tx = (float)fptr->tax;  cp[0].ty = (float)fptr->tay; 
    cp[1].ev.v = rVertex[fptr->v2];  cp[1].tx = (float)fptr->tbx;  cp[1].ty = (float)fptr->tby; 
    cp[2].ev.v = rVertex[fptr->v3];  cp[2].tx = (float)fptr->tcx;  cp[2].ty = (float)fptr->tcy; 

    if (!(gScrp[fptr->v1].x | gScrp[fptr->v2].x | gScrp[fptr->v3].x))  goto LNOCLIP;

    for (u=0; u<vused; u++) { cp[u].ev.v.x-=waterclipbase.x; cp[u].ev.v.y-=waterclipbase.y; cp[u].ev.v.z-=waterclipbase.z; }
    for (u=0; u<vused; u++) ClipVector(ClipW,u);
    for (u=0; u<vused; u++) { cp[u].ev.v.x+=waterclipbase.x; cp[u].ev.v.y+=waterclipbase.y; cp[u].ev.v.z+=waterclipbase.z; }
    if (vused<3) goto LNEXT;
                     
LNOCLIP:
    for (u=0; u<vused; u++) {     
     cp[u].ev.scrx = VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW);    
	 cp[u].ev.scry = VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH);  
    }
   
    mscrp[0].x     = cp[0].ev.scrx;
    mscrp[0].y     = cp[0].ev.scry;
    mscrp[0].tx    = (int)cp[0].tx;
    mscrp[0].ty    = (int)cp[0].ty;

    OpacityMode = (fptr->Flags & (sfOpacity + sfTransparent));

     for (u=0; u<vused-2; u++) {
      for (int n=1; n<3; n++) {
	   mscrp[n].x     = cp[n+u].ev.scrx;
       mscrp[n].y     = cp[n+u].ev.scry;     
       mscrp[n].tx    = (int)cp[n+u].tx;
       mscrp[n].ty    = (int)cp[n+u].ty;  }

       DrawModelFace();     
     }            
LNEXT:
     f = mptr->gFace[f].Next;
   }
}










void RenderModel(TModel* _mptr, float x0, float y0, float z0, int light, int VT, float al, float bt)
{   
   int f;   

   mptr = _mptr;
   
   float ca = (float)cos(al);
   float sa = (float)sin(al);   
   
   float cb = (float)cos(bt);
   float sb = (float)sin(bt);   

   int minx = 10241024;
   int maxx =-10241024;
   int miny = 10241024;
   int maxy =-10241024;

   
   
   __asm {
      mov eax,light      
      shr eax,2
	  shl eax,16	  
      add eax, offset FadeTab
      mov iModelFade,eax
      mov iModelBaseFade,eax
   }

   if (!GlassL) HLineT = (void*) HLineTxModel; else
	 if (GlassL>160) HLineT = (void*) HLineTxModel25; else
	  if (GlassL>80 ) HLineT = (void*) HLineTxModel50; else	    
		HLineT = (void*) HLineTxModel75; 	   	 
         
   lpTextureAddr = (void*) mptr->lpTexture;

   if (GlassL) lpTextureAddr = (void*) mptr->lpTexture3; 
     else
   if (ts <=64)
   if (ts > 32) {
        lpTextureAddr = (void*) mptr->lpTexture2;
        HLineT = (void*) HLineTxModel2;
   } else {
        lpTextureAddr = (void*) mptr->lpTexture3;
        HLineT = (void*) HLineTxModel3;
   }




   for (int s=0; s<mptr->VCount; s++) {              
    rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa)   + x0;

    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;

    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) + z0;

    if (rVertex[s].z>-64) gScrp[s].x = 0xFFFFFF; else {
     gScrp[s].x = VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     gScrp[s].y = VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); }

     if (gScrp[s].x > maxx) maxx = gScrp[s].x;
     if (gScrp[s].x < minx) minx = gScrp[s].x;
     if (gScrp[s].y > maxy) maxy = gScrp[s].y;
     if (gScrp[s].y < miny) miny = gScrp[s].y; 
   }   

   if (minx == 10241024) return;
   if (minx>WinW || maxx<0 || miny>WinH || maxy<0) return;
  
   if (fabs(z0) + fabs(x0)>256*10) 
      BuildTreeNoSort(); else BuildTree();


   if (Current != -1) DrawModelFaces(); 
   return; 

   f = Current;
   while( f!=-1 ) {  
     mscrp[0].x = gScrp[mptr->gFace[f].v1].x;
     mscrp[0].y = gScrp[mptr->gFace[f].v1].y;
     mscrp[0].tx = mptr->gFace[f].tax;
     mscrp[0].ty = mptr->gFace[f].tay;

     mscrp[1].x = gScrp[mptr->gFace[f].v2].x;
     mscrp[1].y = gScrp[mptr->gFace[f].v2].y;
     mscrp[1].tx = mptr->gFace[f].tbx;
     mscrp[1].ty = mptr->gFace[f].tby;

     mscrp[2].x = gScrp[mptr->gFace[f].v3].x;
     mscrp[2].y = gScrp[mptr->gFace[f].v3].y;
     mscrp[2].tx = mptr->gFace[f].tcx;
     mscrp[2].ty = mptr->gFace[f].tcy;

     OpacityMode = (mptr->gFace[f].Flags & (sfOpacity + sfTransparent));
     if (mptr->gFace[f].Flags & sfDark)
           iModelFade = iModelBaseFade + 12*256*256;
      else iModelFade = iModelBaseFade;
	 
     DrawModelFace();
     f = mptr->gFace[f].Next;
   }
}




void RenderBMPModel(TBMPModel* _mptr, float x0, float y0, float z0, int light)
{   
   TBMPModel *mptr = _mptr;
      

   int minx = 10241024;
   int maxx =-10241024;
   int miny = 10241024;
   int maxy =-10241024;   
   
   __asm {
      mov eax,light      
      shr eax,2
	  shl eax,16	  
      add eax, offset FadeTab
      mov iModelFade,eax
      mov iModelBaseFade,eax
   }
   
         
   lpTextureAddr = (void*) mptr->lpTexture;   
   HLineT = (void*) HLineTxModel2;
   

   for (int s=0; s<4; s++) {                

	rVertex[s].x = mptr->gVertex[s].x + x0;    	
    rVertex[s].y = mptr->gVertex[s].y + y0;
    rVertex[s].z = z0;

	if (rVertex[s].z<-256) {
     gScrp[s].x = VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     gScrp[s].y = VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 
	} else return;
    
     if (gScrp[s].x > maxx) maxx = gScrp[s].x;
     if (gScrp[s].x < minx) minx = gScrp[s].x;
     if (gScrp[s].y > maxy) maxy = gScrp[s].y;
     if (gScrp[s].y < miny) miny = gScrp[s].y; 
   }   

   if (minx == 10241024) return;
   if (minx>WinW || maxx<0 || miny>WinH || maxy<0) return;

   //int w  =  gScrp[1].x - gScrp[0].x;
   int h  =  gScrp[2].y - gScrp[1].y;
   //int xo = (gScrp[1].x + gScrp[0].x) / 2;
   int yo =  gScrp[0].y;

   if (!h) return;

   xa = gScrp[0].x<<16;
   xb = gScrp[1].x<<16;

   Y1 = yo;
   int DTY = 128*256*256 / h;
   int TY = -DTY;
   for (int y=0; y<h; y++) {
	 Y1++;
	 TY+=DTY;
	 if (Y1< 0   ) continue;
	 if (Y1>=WinH) continue;
     lpTextureAddr = (void*) (mptr->lpTexture + (TY>>16)*128);
     HLineTxModelBMP();
   }         
}







void RenderNearModel(TModel* _mptr, float x0, float y0, float z0, int light, float al, float bt)
{   
   int f;

   mptr = _mptr;
   
   float ca = (float)cos(al);
   float sa = (float)sin(al);   
   
   float cb = (float)cos(bt);
   float sb = (float)sin(bt);   


   //light = 0;
   __asm {
      mov eax,light   
	  shr eax,2
      shl eax,16
      add eax, offset FadeTab
      mov iModelFade,eax
      mov iModelBaseFade,eax
   }

            
   HLineT = (void*) HLineTxModel;
   lpTextureAddr = (void*) mptr->lpTexture;
   
   BOOL BL = FALSE;
   for (int s=0; s<mptr->VCount; s++) {              
    rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa)   + x0;
    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;
    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) + z0;     
    if (rVertex[s].z<0) BL=TRUE;
   }   
   
   if (!BL) return;
     
   BuildTreeClip();   

   f = Current;
   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    
        
    cp[0].ev.v = rVertex[fptr->v1];  cp[0].tx = (float)fptr->tax;  cp[0].ty = (float)fptr->tay; 
    cp[1].ev.v = rVertex[fptr->v2];  cp[1].tx = (float)fptr->tbx;  cp[1].ty = (float)fptr->tby; 
    cp[2].ev.v = rVertex[fptr->v3];  cp[2].tx = (float)fptr->tcx;  cp[2].ty = (float)fptr->tcy; 
    
    for (u=0; u<vused; u++) cp[u].ev.v.z+=12.0f;
    for (u=0; u<vused; u++) ClipVector(ClipZ,u);
    for (u=0; u<vused; u++) cp[u].ev.v.z-=12.0f;
    if (vused<3) goto LNEXT;    
        
    for (u=0; u<vused; u++) ClipVector(ClipA,u);
    for (u=0; u<vused; u++) ClipVector(ClipC,u);
    
    for (u=0; u<vused; u++) ClipVector(ClipB,u);
    for (u=0; u<vused; u++) ClipVector(ClipD,u); 
    if (vused<3) goto LNEXT;
             
    for (u=0; u<vused; u++) {     
     cp[u].ev.scrx = VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW);    
	 cp[u].ev.scry = VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH);  
    }
   
    scrp[0].x     = cp[0].ev.scrx;
    scrp[0].y     = cp[0].ev.scry;
    scrp[0].z     = (int)cp[0].ev.v.z;
    scrp[0].tx    = (int)cp[0].tx;
    scrp[0].ty    = (int)cp[0].ty;
    

    OpacityMode = (fptr->Flags & (sfOpacity + sfTransparent));
    if (CORRECTION) Soft_Persp_K = 2.0f;
               else Soft_Persp_K = 0.0f;
     for (u=0; u<vused-2; u++) {
      for (int n=1; n<3; n++) {
	   scrp[n].x     = cp[n+u].ev.scrx;
       scrp[n].y     = cp[n+u].ev.scry;     
       scrp[n].z     = (int)cp[n+u].ev.v.z;      
       scrp[n].tx    = (int)cp[n+u].tx;
       scrp[n].ty    = (int)cp[n+u].ty;  }

       DrawCorrectedTexturedFace();
     }            
    Soft_Persp_K = 1.5f;
LNEXT:
     f = mptr->gFace[f].Next;
   }
}







void RenderCharacter(int index)
{
   TCharacter *cptr = &Characters[index];

   float zs = (float)VectorLength( cptr->rpos );
   if (zs > ctViewR*256) return;      

   GlassL = 0;      
   if (zs > 256 * (ctViewR-4)) 
	GlassL = (int)min(255, (zs/4 - 64*(ctViewR-4)));         
   
   //CreateChMorphedModel(cptr);
   

   float wh = GetLandUpH(cptr->pos.x, cptr->pos.z);
   waterclip = FALSE;

  if (!UNDERWATER)
   if (wh > cptr->pos.y + 32*2) {
     waterclipbase.x = cptr->pos.x - CameraX;
     waterclipbase.y = wh - CameraY;
     waterclipbase.z = cptr->pos.z - CameraZ;
     waterclipbase = RotateVector(waterclipbase);
     waterclip = TRUE;
   }

   
   if ( fabs(cptr->rpos.z) + fabs(cptr->rpos.x) <2560) 
    RenderModelClip(cptr->pinfo->mptr, 
                cptr->rpos.x, cptr->rpos.y, cptr->rpos.z, 240, 0,
                -cptr->alpha + pi / 2 + CameraAlpha, 
                CameraBeta );
   else
   if (waterclip)
    RenderModelClipWater(cptr->pinfo->mptr, 
                cptr->rpos.x, cptr->rpos.y, cptr->rpos.z, 240, 0,
                -cptr->alpha + pi / 2 + CameraAlpha, 
                CameraBeta );
   else
    RenderModel(cptr->pinfo->mptr, 
                cptr->rpos.x, cptr->rpos.y, cptr->rpos.z, 240,  0,
                -cptr->alpha + pi / 2 + CameraAlpha, 
                CameraBeta );
}





void RenderShip()
{
   float zs = (float)VectorLength( Ship.rpos );
   if (zs > ctViewR*256) return;      

   GlassL = 0;      
   if (zs > 256 * (ctViewR-4)) 
	GlassL = (int)min(255, (zs/4 - 64*(ctViewR-4)));      
	

//   CreateMorphedModel(ShipModel.mptr, &ShipModel.Animation[0], Ship.FTime, 1.0);

   if ( fabs(Ship.rpos.z)  < 4000) 
    RenderModelClip(ShipModel.mptr,
                    Ship.rpos.x, Ship.rpos.y, Ship.rpos.z, 240, 0, -Ship.alpha -pi/2 + CameraAlpha, CameraBeta);
   else   
    RenderModel(ShipModel.mptr,
                Ship.rpos.x, Ship.rpos.y, Ship.rpos.z, 240, 0, -Ship.alpha -pi/2 + CameraAlpha, CameraBeta);
}



void RenderSupplyShip()
{
   float zs = (float)VectorLength( SupplyShip.rpos );
   if (zs > ctViewR*256) return;      

   GlassL = 0;      
   if (zs > 256 * (ctViewR-4)) 
	GlassL = (int)min(255, (zs/4 - 64*(ctViewR-4)));      
	   

   if ( fabs(Ship.rpos.z)  < 4000) 
    RenderModelClip(SShipModel.mptr,
                    SupplyShip.rpos.x, SupplyShip.rpos.y, SupplyShip.rpos.z, 240, 0, -SupplyShip.alpha -pi/2 + CameraAlpha, CameraBeta);
   else   
    RenderModel(SShipModel.mptr,
                SupplyShip.rpos.x, SupplyShip.rpos.y, SupplyShip.rpos.z, 240, 0, -SupplyShip.alpha -pi/2 + CameraAlpha, CameraBeta);
}



void RenderAmmoBag()
{
   float zs = (float)VectorLength( AmmoBag.rpos );
   if (zs > ctViewR*256) return;      

   GlassL = 0;      
   if (zs > 256 * (ctViewR-4)) 
	GlassL = (int)min(255, (zs/4 - 64*(ctViewR-4)));      
	

//   CreateMorphedModel(AmmoModel.mptr, &AmmoModel.Animation[0], AmmoBag.FTime, 1.0);

   if ( fabs(Ship.rpos.z)  < 4000) 
    RenderModelClip(AmmoModel.mptr,                    
                    AmmoBag.rpos.x, AmmoBag.rpos.y, AmmoBag.rpos.z, 210, 0, -AmmoBag.alpha -pi/2 + CameraAlpha, CameraBeta);
   else   
    RenderModel(AmmoModel.mptr,
                AmmoBag.rpos.x, AmmoBag.rpos.y, AmmoBag.rpos.z, 210, 0, -AmmoBag.alpha -pi/2 + CameraAlpha, CameraBeta);
}




void _FillMemoryWord(int maddr, int count, WORD w)
{
 __asm {
	 mov edi,maddr
	 mov ecx,count
	 shr ecx,2
	 mov ax,w
	 shl eax,16
	 mov ax,w
     rep stosd
 }
}

void FillMemoryWord(int maddr, int count, WORD w)
{
 WORD ww[4]; 
 ww[0] = w; ww[1] = w; ww[2] = w; ww[3] = w;
 __asm {
	 EMMS
	 mov edi,maddr
	 mov ecx,count
	 shr ecx,3
	 MOVQ MM0,ww
 } L1: __asm {
	 MOVQ [edi],MM0
	 add edi,8
	 dec cx
	 jnz L1     
	 EMMS
 }
}



void _memcpy(void* daddr, void* saddr, int count)
{ 
 __asm {
	 EMMS
	 mov edi,daddr
     mov esi,saddr
	 mov ecx,count
	 shr ecx,3	 
 } L1: __asm {
     MOVQ MM0,[esi]
	 MOVQ [edi],MM0
	 add edi,8
     add esi,8
	 dec cx
	 jnz L1     
	 EMMS
 }
}


void DrawPicture(int x, int y, TPicture &pic)
{
   for (int yy=0; yy<pic.H; yy++)
	if ( (yy+y>=0) && (yy+y < WinH) )
	 memcpy( (WORD*)lpVideoBuf + ((yy+y)<<10) + x,
	         pic.lpImage + yy*pic.W,
			 pic.W<<1);
}


void ClearVideoBuf()
{
  WORD w = HiColor(SkyR/8, SkyG/8, SkyB/8);
  
  for(int y=0; y<WinH; y++) {	
    _FillMemoryWord( (int)lpVideoBuf + y*2048, WinW*2, w);
  }
}




int CircleCX, CircleCY;

void PutPixel(int x, int y)
{ 
	if (y<0 || y>=WinH) return;
	*((WORD*)lpVideoBuf + (y<<10) + x) = 18<<5; 
}

void Put8pix(int X,int Y)
{
  PutPixel(CircleCX + X, CircleCY + Y);
  PutPixel(CircleCX + X, CircleCY - Y);
  PutPixel(CircleCX - X, CircleCY + Y);
  PutPixel(CircleCX - X, CircleCY - Y);
  PutPixel(CircleCX + Y, CircleCY + X);
  PutPixel(CircleCX + Y, CircleCY - X);
  PutPixel(CircleCX - Y, CircleCY + X);
  PutPixel(CircleCX - Y, CircleCY - X);
}

void DrawStatBar(int x, int y, float val, LPSTR StatTxt) {

	UINT uAlignPrev = GetTextAlign(hdcCMain);
	SetTextAlign(hdcCMain, TA_RIGHT);

	//HFONT hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
	STTextOut(x - 6, y - 2, StatTxt, huntInfoFontC);
	SetTextAlign(hdcCMain, uAlignPrev);

	//HFONT hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);
	//SelectObject(hdcCMain, hfntOld);
	//STTextOut(413, 93, "TEST", RGB(64, 180, 64));
	//STTextOut(x, y, pch, RGB(64, 180, 64));

	int ival = (int)(val * 119);
	byte cc = 0;
	byte gg = 173;
	WORD black555 = ((cc >> 3) << 10) | ((cc >> 3) << 5) | (cc >> 3);
	cc = 156;
	WORD grey555 = ((cc >> 3) << 10) | ((cc >> 3) << 5) | (cc >> 3);
	cc = 57;
	WORD green555 = ((cc >> 3) << 10) | ((gg >> 3) << 5) | (cc >> 3);
	
	for (int xx = x + 1; xx < x + 121; xx++) {
		*((WORD*)lpVideoBuf + (y + 1) * 1024 + xx) = black555;
		*((WORD*)lpVideoBuf + (y + 10) * 1024 + xx) = black555;
	}
	
	for (int yy = y + 1; yy < y + 10; yy++) {
		*((WORD*)lpVideoBuf + yy * 1024 + (x+1)) = black555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 31)) = black555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 61)) = black555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 91)) = black555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 121)) = black555;
	}
	
	for (int xx = x; xx < x + 121; xx++) {
		*((WORD*)lpVideoBuf + y * 1024 + xx) = grey555;
		*((WORD*)lpVideoBuf + (y+9) * 1024 + xx) = grey555;
	}
	
	for (int yy = y; yy < y + 9; yy++) {
		*((WORD*)lpVideoBuf + yy * 1024 + x) = grey555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 30)) = grey555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 60)) = grey555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 90)) = grey555;
		*((WORD*)lpVideoBuf + yy * 1024 + (x + 120)) = grey555;
	}

	if (ival) {
		for (int xx = x + 1; xx < x + ival; xx++) {
			for (int yy = y + 3; yy < y + 7; yy++) {
				*((WORD*)lpVideoBuf + (yy+1) * 1024 + (xx+1)) = black555;
				*((WORD*)lpVideoBuf + yy * 1024 + xx) = green555;
			}
		}
	}
	
}



void DrawCircle(int cx, int cy, int R)
{
   int d = 3 - (2 * R);
   int x = 0;
   int y = R;
   CircleCX=cx; 
   CircleCY=cy;
   do {
     Put8pix(x,y); x++;
     if (d < 0) d = d + (x<<2) + 6;  else 
	 { d = d + (x - y) * 4 + 10; y--; }
   } while (x<y);
   Put8pix(x,y);
}


void DrawHMap()
{
   //if (WinH < 280) return;   
   DrawPicture(VideoCX-MapPic.W/2, VideoCY - MapPic.H/2, MapPic);	
   int xx = VideoCX - 128 + (CCX>>2);
   int yy = VideoCY - 128 + (CCY>>2)+6;

   if (yy>0 || yy<WinH) {      
     DrawCircle(xx, yy, 17);

     yy++;
     *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 0;
     *((WORD*)lpVideoBuf + yy*1024 + xx+2) = 0;
     yy++;
     *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 0;
     *((WORD*)lpVideoBuf + yy*1024 + xx+2) = 0;

     yy-=2;
     *((WORD*)lpVideoBuf + yy*1024 + xx) = 31<<5;
     *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 31<<5;
     yy++;
     *((WORD*)lpVideoBuf + yy*1024 + xx) = 31<<5;
     *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 31<<5;
   }
   
  if (RadarMode)
  for (int c=0; c<ChCount; c++) {

   if (! (TargetDino & (1<<Characters[c].AI)) ) continue;
   if (!Characters[c].Health) continue;
   xx = VideoCX - 128-1 + (int)Characters[c].pos.x / 1024;
   yy = VideoCY - 128-1 + (int)Characters[c].pos.z / 1024;
   if (yy<=0 || yy>=WinH) continue;
   if (xx<=0 || xx>=WinW) continue;
   *((WORD*)lpVideoBuf + yy*1024 + xx) = 30<<10;   
   *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 30<<10;   
   *((WORD*)lpVideoBuf + yy*1024 + xx+2) = 30<<10;   
   yy++;
   *((WORD*)lpVideoBuf + yy*1024 + xx) = 30<<10;   
   *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 30<<10;   
   *((WORD*)lpVideoBuf + yy*1024 + xx+2) = 30<<10;   
   yy++;
   *((WORD*)lpVideoBuf + yy*1024 + xx) = 30<<10;   
   *((WORD*)lpVideoBuf + yy*1024 + xx+1) = 30<<10;   
   *((WORD*)lpVideoBuf + yy*1024 + xx+2) = 30<<10;   
  }
}



void DrawTrophyText(int x0, int y0)
{
	int x;

	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcCMain, hbmpVideoBuf);
    HFONT oldfont = (HFONT)SelectObject(hdcCMain, fnt_Small);  
/*
	int dtype = Characters[TrophyBody].CType;
	int tc = Characters[TrophyBody].State;
	int time = TrophyRoom.Body[tc].time;
	int date = TrophyRoom.Body[tc].date;
	int wep  = TrophyRoom.Body[tc].weapon;
	int score = TrophyRoom.Body[tc].score;
	float scale = Characters[TrophyBody].scale;*/
    int   tc = TrophyBody;	
	int   dtype = TrophyRoom.Body[tc].ctype;
	int   time  = TrophyRoom.Body[tc].time;
	int   date  = TrophyRoom.Body[tc].date;
	int   wep   = TrophyRoom.Body[tc].weapon;
	int   score = TrophyRoom.Body[tc].score;
	float scale = TrophyRoom.Body[tc].scale;
	float range = TrophyRoom.Body[tc].range;

	char t[32];

	x0+=16; y0+=18;
    x = x0;
	STTextOut(x, y0   , "Name: ", 0x00BFBFBF);  x+=GetTextW(hdcCMain,"Name: ");
    STTextOut(x, y0   , DinoInfo[dtype].Name, 0x0000BFBF);    

	x = x0;
	STTextOut(x, y0+16, "Weight: ", 0x00BFBFBF);  x+=GetTextW(hdcCMain,"Weight: ");
	
	if (OptSys)
     sprintf(t,"%3.2ft ", DinoInfo[dtype].Mass * scale * scale / 0.907);
	else
     sprintf(t,"%3.2fT ", DinoInfo[dtype].Mass * scale * scale);

    STTextOut(x, y0+16, t, 0x0000BFBF);    x+=GetTextW(hdcCMain,t);
    STTextOut(x, y0+16, "Length: ", 0x00BFBFBF); x+=GetTextW(hdcCMain,"Length: ");

	if (OptSys)
	 sprintf(t,"%3.2fft", DinoInfo[dtype].Length * scale / 0.3);
	else
	 sprintf(t,"%3.2fm", DinoInfo[dtype].Length * scale);
     
	STTextOut(x, y0+16, t, 0x0000BFBF); 
	
	x = x0;
    STTextOut(x, y0+32, "Weapon: ", 0x00BFBFBF);  x+=GetTextW(hdcCMain,"Weapon: ");
	 wsprintf(t,"%s    ", WeapInfo[wep].Name);
    STTextOut(x, y0+32, t, 0x0000BFBF);   x+=GetTextW(hdcCMain,t);
    STTextOut(x, y0+32, "Score: ", 0x00BFBFBF);   x+=GetTextW(hdcCMain,"Score: ");
	 wsprintf(t,"%d", score);
	STTextOut(x, y0+32, t, 0x0000BFBF); 


	
	x = x0;
	STTextOut(x, y0+48, "Range of kill: ", 0x00BFBFBF);  x+=GetTextW(hdcCMain,"Range of kill: ");
	if (OptSys) sprintf(t,"%3.1fft", range / 0.3);
	else        sprintf(t,"%3.1fm", range);
    STTextOut(x, y0+48, t, 0x0000BFBF);  

	
	x = x0;
	STTextOut(x, y0+64, "Date: ", 0x00BFBFBF);  x+=GetTextW(hdcCMain,"Date: ");
	if (OptSys)
	 wsprintf(t,"%d.%d.%d   ", ((date>>10) & 255), (date & 255), date>>20);
	else
     wsprintf(t,"%d.%d.%d   ", (date & 255), ((date>>10) & 255), date>>20);

    STTextOut(x, y0+64, t, 0x0000BFBF);   x+=GetTextW(hdcCMain,t);
    STTextOut(x, y0+64, "Time: ", 0x00BFBFBF);   x+=GetTextW(hdcCMain,"Time: ");
	 wsprintf(t,"%d:%02d", ((time>>10) & 255), (time & 255));
	STTextOut(x, y0+64, t, 0x0000BFBF); 

	SelectObject(hdcCMain, oldfont);
	SelectObject(hdcCMain, hbmpOld);
}




void Render_LifeInfo(int li)
{
	int x,y;
	
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcCMain, hbmpVideoBuf);
    HFONT oldfont = (HFONT)SelectObject(hdcCMain, fnt_Small);  
		
	int   ctype = Characters[li].CType;
	// float  scale = Characters[li].scale; // alacn
	char t[32];
	
    x = VideoCX + WinW / 64;
	y = VideoCY + (int)(WinH / 6.8);
		
    STTextOut(x, y, DinoInfo[ctype].Name, 0x0000b000);    
		
	// alacn ->
	/*
	if (OptSys) sprintf(t,"Weight: %3.2ft ", DinoInfo[ctype].Mass * scale * scale / 0.907);
	else        sprintf(t,"Weight: %3.2fT ", DinoInfo[ctype].Mass * scale * scale);     
	*/
	// vc++ optimizations screw the 'scale * scale' by some reason...
	if (OptSys) sprintf(t,"Weight: %3.2ft ", DinoInfo[ctype].Mass * Characters[li].scale * Characters[li].scale / 0.907);
	else        sprintf(t,"Weight: %3.2fT ", DinoInfo[ctype].Mass * Characters[li].scale * Characters[li].scale);
	// <- alacn
	STTextOut(x, y+16, t, 0x0000b000);    

	int R  = (int)(VectorLength( SubVectors(Characters[li].pos, PlayerPos) )*3 / 64.f);
	if (OptSys) sprintf(t,"Distance: %dft ", R);
	else        sprintf(t,"Distance: %dm  ", R/3);     

	STTextOut(x, y+32, t, 0x0000b000);

	SelectObject(hdcMain, oldfont);

	SelectObject(hdcCMain, oldfont);
	SelectObject(hdcCMain, hbmpOld);
}



void RotateVVector(Vector3d& v)
{
   float x = v.x * ca - v.z * sa;
   float y = v.y;
   float z = v.z * ca + v.x * sa;

   float xx = x;
   float xy = y * cb + z * sb;
   float xz = z * cb - y * sb;
   
   v.x = xx; v.y = xy; v.z = xz;
}



void RenderSkyPlane()
{
   ClearVideoBuf();
   Vector3d v,vbase;
   Vector3d tx,ty,nv;
   float p,q, qx, qy, qz, px, py, pz, rx, ry, rz, ddx, ddy;
   int lastdt = 0;

   cb = (float)cos(CameraBeta);
   sb = (float)sin(CameraBeta);
   SKYDTime = (RealTime*256) & ((256<<16) - 1);

   float sh = - CameraY;
   if (MapMinY==10241024) MapMinY=0;
   sh = (float)((int)MapMinY)*ctHScale - CameraY;

   v.x = 0;
   v.z = (ctViewR*4.f)/5.f*256.f;
   v.y = sh;

   vbase.x = v.x;
   vbase.y = v.y * cb + v.z * sb;
   vbase.z = v.z * cb - v.y * sb;   

   if (vbase.z < 128) vbase.z = 128;

   int scry = VideoCY - (int)(vbase.y / vbase.z * CameraH);
   
   if (scry<0) return; 
   if (scry>WinEY) scry = WinEY;   
   
   
   cb = (float)cos(CameraBeta-0.15);
   sb = (float)sin(CameraBeta-0.15);
   
   tx.x=0.004f; tx.y=0;    tx.z=0;
   ty.x=0.0f;   ty.y=0;    ty.z=0.004f;
   nv.x=0;      nv.y=-1.f; nv.z=0;
   
   tx.x*=0x10000;
   ty.z*=0x10000;

   RotateVVector(tx);
   RotateVVector(ty);
   RotateVVector(nv);
      
   sh = 4*512*16;
   vbase.x = -CameraX;
   vbase.y = sh;
   vbase.z = +CameraZ;
   RotateVVector(vbase);

//============= calc render params =================//
   p = nv.x * vbase.x + nv.y * vbase.y + nv.z * vbase.z;
   ddx = vbase.x * tx.x  +  vbase.y * tx.y  +  vbase.z * tx.z;
   ddy = vbase.x * ty.x  +  vbase.y * ty.y  +  vbase.z * ty.z;   

   qx = CameraH * nv.x;   qy = CameraW * nv.y;   qz = CameraW*CameraH  * nv.z;
   px = p*CameraH*tx.x;   py = p*CameraW*tx.y;   pz = p*CameraW*CameraH* tx.z;
   rx = p*CameraH*ty.x;   ry = p*CameraW*ty.y;   rz = p*CameraW*CameraH* ty.z;

   px=px - ddx*qx;  py=py - ddx*qy;   pz=pz - ddx*qz;
   rx=rx - ddy*qx;  ry=ry - ddy*qy;   rz=rz - ddy*qz;

   int sx1 = - VideoCX;
   int sx2 = + VideoCX;      

   float qx1 = qx * sx1 + qz;
   float qx2 = qx * sx2 + qz;
   float qyy;


   for (int sky=0; sky<=scry; sky++) {  
	int sy = VideoCY - sky;
	qyy = qy * sy;

	q = qx1 + qyy;
	float fxa = (px * sx1 + py * sy + pz) / q;
	float fya = (rx * sx1 + ry * sy + rz) / q;
	
	q = qx2 + qyy;	
	float fxb = (px * sx2 + py * sy + pz) / q;
	float fyb = (rx * sx2 + ry * sy + rz) / q;

    txa = ((int)fxa + SKYDTime) & ((256<<16) - 1);
	tya = ((int)fya - SKYDTime) & ((256<<16) - 1);

	ctdx = (int)(fxb-fxa);
	ctdy = (int)(fyb-fya);

    int dt = (int)(sqrt( (fxb-fxa)*(fxb-fxa) + (fyb-fya)*(fyb-fya) ) / 0x600000 ) - 7;
    if (dt>8) dt = 8;
    if (dt<lastdt) dt = lastdt;
    lastdt = dt;

    ctdx/=WinW; ctdy/=WinW;
     
    if (LoDetailSky) 
      if (dt) RenderSkyLineFadeLo(sky, dt); else RenderSkyLineLo(sky); else
      if (dt) RenderSkyLineFade(sky,dt); else RenderSkyLine(sky);
   }
}





    



void ShowVideo()
{
  HDC _hdc =  hdcCMain;
  HBITMAP hbmpOld = (HBITMAP)SelectObject(_hdc,hbmpVideoBuf);

  //-> Below code adds a light-blue overlay to the whole screen when underwater
 if (UNDERWATER & CORRECTION)
   for (int y=0; y<WinH; y++) 
    for (int x=0; x<WinW; x++)
     *((WORD*)lpVideoBuf + y*1024 + x) = FadeTab[64][*((WORD*)lpVideoBuf + y*1024 + x) & 0x7FFF];

   //-> Highlight Hotspots
   int btnID;
   for (int y=0; y<WinH; y++) 
	   for (int x=0; x<WinW; x++) {
		   if (GameMenus[CURRENT_MENU].Hotspots[y/2][x/2] != 0 && BUTTON_HOVER_ID == GameMenus[CURRENT_MENU].Hotspots[y/2][x/2]) {
			   btnID = (int)GameMenus[CURRENT_MENU].Hotspots[y/2][x/2];
			   //-> Make sure these are not the special buttons in the hunt menu....
			   if (CURRENT_MENU == MENU_PREHUNT && (btnID == 6 || btnID == 1 || btnID == 2 || btnID == 3)) continue;
			   *((WORD*)lpVideoBuf + y*1024 + x) = *(GameMenus[CURRENT_MENU].ImageOn.lpImage + y*800 + x);
		   }
	   }
  //===== Draw Selected Buttons (Day, observ, etc) =======//
	   if (ObservMode && CURRENT_MENU == MENU_PREHUNT) {
		   //-> Hightlight observe button (button6)
		   for (int y=0; y<WinH; y++) 
			   for (int x=0; x<WinW; x++) {
				   if (GameMenus[CURRENT_MENU].Hotspots[y/2][x/2] == 6) {
					   *((WORD*)lpVideoBuf + y*1024 + x) = *(GameMenus[CURRENT_MENU].ImageOn.lpImage + y*800 + x);
				   }
			   }
	   }
	   //-> Hightlight day (button1 = day0, bttn2 = day1, bttn3 = day2)
	   if (CURRENT_MENU == MENU_PREHUNT && SelectedDay != -1) {
		   //-> Hightlight the button
		   int targetBttn;
		   for (int y=0; y<WinH; y++) 
			   for (int x=0; x<WinW; x++) {
				   switch (SelectedDay) {
					   case 0: //Dawn
						   targetBttn = 1;
						   break;
					   case 1: //Day
						   targetBttn = 2;
						   break;
					   case 2: //Night
						   targetBttn = 3;
						   break;
				   }
				   if (GameMenus[CURRENT_MENU].Hotspots[y/2][x/2] == targetBttn) {
					   *((WORD*)lpVideoBuf + y*1024 + x) = *(GameMenus[CURRENT_MENU].ImageOn.lpImage + y*800 + x);
				   }
			   }
	   }

 // ====== Fading Out Code.... ========
 if (ExitTime || RestartMode) {
	 if (FADING > 1 && (ExitTime && ExitTime < 1300 || RestartMode) && MyHealth) {
		 //If fading int is bigger than 0, the time and running out, and your alive....
		 FADING -= 2;
		 int fi = FADING;
		 for (int y=0; y<WinH; y++) 
			 for (int x=0; x<WinW; x++) {
				 *((WORD*)lpVideoBuf + y*1024 + x) = FadeTab[fi][*((WORD*)lpVideoBuf + y*1024 + x)];
				 //*((WORD*)lpVideoBuf + y*1024 + x) == HiColor(R, G, B)
			 }
	 } else if (ExitTime && ExitTime < 1300 && MyHealth && !RestartMode) {
		 //If fading is done but the map hasn't exited yet....
		 int fi = 0;
		 for (int y=0; y<WinH; y++) 
			 for (int x=0; x<WinW; x++) {
				 *((WORD*)lpVideoBuf + y*1024 + x) = FadeTab[fi][*((WORD*)lpVideoBuf + y*1024 + x)];
				 //*((WORD*)lpVideoBuf + y*1024 + x) == HiColor(R, G, B)
			 }
	 } else if (RestartMode) {
		 FADING = 0;
		 _GameState = 0;
	 }
 }
 // ======= Fade In ======= //
 if (FADE_IN && FADING <= 50){
	 FADING += 4;
	 if (FADING > 65)
		 FADING = 65;
	 int fi = FADING;
	 for (int y=0; y<WinH; y++) 
		 for (int x=0; x<WinW; x++) {
			 *((WORD*)lpVideoBuf + y*1024 + x) = FadeTab[fi][*((WORD*)lpVideoBuf + y*1024 + x)];
			 //*((WORD*)lpVideoBuf + y*1024 + x) == HiColor(R, G, B)
		 }
 } else if (FADE_IN) {
	 FADING = 0;
	 FADE_IN = FALSE;
 }
 //This allows you to replace a certain color on screen with another
 //if (!UNDERWATER & CORRECTION)
 //  for (int y=0; y<WinH; y++) 
	//   for (int x=0; x<WinW; x++) {
	//		*((WORD*)lpVideoBuf + y*1024 + x) = FadeTab[32][*((WORD*)lpVideoBuf + y*1024 + x)];
	//		//*((WORD*)lpVideoBuf + y*1024 + x) == HiColor(R, G, B)
	//   }

  
  //RenderHealthBar();
  //RenderEnergyBar();

  BitBlt(hdcMain,0,0,WinW,WinH, _hdc,0,0, SRCCOPY);                		        

  SelectObject(_hdc,hbmpOld);		  
  //DeleteDC(_hdc);  
}




void RenderHealthBar()
{
  if (MyHealth >= 100000) return;
  if (MyHealth == 000000) return;

  int L = WinW / 4;
  int x0 = WinW - (WinW / 20) - L;
  int y0 = WinH / 40;
  int G = min( (MyHealth * 30 / 100000), 20);
  int R = min( ( (100000 - MyHealth) * 30 / 100000), 20);
  int HCOLOR = (G<<5) + (R<<10);
    
  int L0 = (L * MyHealth) / 100000;
  int H = WinH / 200;

  FillMemory((WORD*)lpVideoBuf + ((y0-1)<<10) + x0-1, L*2+4, 0);
  FillMemory((WORD*)lpVideoBuf + ((y0+H+1)<<10) + x0-1, L*2+4, 0);
  for (int y=0; y<=H; y++) {
   *((WORD*)lpVideoBuf + ((y0+y)<<10) + x0 - 1) = 0;
   *((WORD*)lpVideoBuf + ((y0+y)<<10) + x0 + L) = 0;
   for (int x=0; x<L0; x++)
    *((WORD*)lpVideoBuf + ((y0+y)<<10) + x0 + x) = HCOLOR;
  }
}


void RenderEnergyBar()
{
  // == 4.19.09 by Adelphospro
  if (MyEnergy >= 100000) return;
  if (MyEnergy == 000000 || MyHealth == 000000) return;

  int L = WinW / 4;
  //int x0 = WinW - (WinW / 20) - L; //Default Pos
  int x0 = (L/8); //left side of screen

  //int y0 = WinH / 40; //Default Pos
  int y0 = WinH; //Bottom of Screen

  int HCOLOR = int(RGB(138,138,138));
    
  int L0 = (L * MyEnergy) / 100000;
  int H = WinH / 200;

  y0 -= H*4; //-> y offset.

  FillMemory((WORD*)lpVideoBuf + ((y0-1)<<10) + x0-1, L*2+4, 0);
  FillMemory((WORD*)lpVideoBuf + ((y0+H+1)<<10) + x0-1, L*2+4, 0);
  for (int y=0; y<=H; y++) {
   *((WORD*)lpVideoBuf + ((y0+y)<<10) + x0 - 1) = 0;
   *((WORD*)lpVideoBuf + ((y0+y)<<10) + x0 + L) = 0;
   for (int x=0; x<L0; x++)
    *((WORD*)lpVideoBuf + ((y0+y)<<10) + x0 + x) = HCOLOR;
  }
}


void Render_Cross(int sx, int sy, float LineSize, bool DrawGap) 
{
  // == AdelphosPro Modified 4.14.09 == //
	// -> Made crosshair smaller
	// -> Drew 4 lines instead of only 2, with a gap in the middle
  //if (DEBUG) return;
  int w = int(LineSize);
  int gapsize = 0;
  if (DrawGap)
	gapsize = Weapon.shakel*10; //Base this on a fixed int or on shackle (*10 to remove decimal point)
  WORD color = 0;

  for (int t=0; t<=1; t++) {
      if (t) {
       color = (12<<5);
       sx-=1; sy-=1;
      }
//X line #1
  for (int x=-w+1; x<(w/18)-1; x++) {
	  int offset = (sy<<10) + (sx+x) - gapsize;
	  WORD C = *((WORD*)lpVideoBuf + offset);
	  *((WORD*)lpVideoBuf + offset) = ((C & 0x7BDE) >> 1)+color;
  }
//X Line #2
  for (int x=-w+1; x<(w/18)-1; x++) {
	  int offset = (sy<<10) + (sx+x) + w + gapsize;
	  WORD C = *((WORD*)lpVideoBuf + offset);
	  *((WORD*)lpVideoBuf + offset) = ((C & 0x7BDE) >> 1)+color;
  }

//Y Line #1
  for (int y=-w+1; y<(w/18); y++) {
	  int offset = ((sy+y-gapsize)<<10) + sx;
	  WORD C = *((WORD*)lpVideoBuf + offset);
	  *((WORD*)lpVideoBuf + offset) = ((C & 0x7BDE) >> 1)+color;
  }	

  //Y Line #2
  for (int y=-w+1; y<(w/18); y++) {
	  int offset = ((sy+y+gapsize+w)<<10) + sx;
	  WORD C = *((WORD*)lpVideoBuf + offset);
	  *((WORD*)lpVideoBuf + offset) = ((C & 0x7BDE) >> 1)+color;
  }	
  }
}

//======== software has no implementation for 3DHard ===========/
void Init3DHardware()
{
   PrintLog("\n");
   PrintLog("==Init Direct Draw==\n");
   HRESULT hres;

   hres = DirectDrawCreate( NULL, &lpDD, NULL );
   if( hres != DD_OK ) {      
	  wsprintf(logt, "DirectDrawCreate Error: %Xh\n", hres);
      PrintLog(logt);
	  DoHalt("");	  
   }
   PrintLog("DirectDrawCreate: Ok\n");
   
   PrintLog("Direct Draw activated.\n");
   PrintLog("\n");
   DirectActive = TRUE;
}



void Activate3DHardware()
{ 
   SetVideoMode(WinW, WinH);

   DWORD cl = DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN;
   //cl = DDSCL_NORMAL|DDSCL_FULLSCREEN; //<- For menu mode
   
#ifdef _DEBUG
   cl = DDSCL_NORMAL;
#endif
   
   HRESULT hres = lpDD->SetCooperativeLevel( hwndMain, cl);
   if( hres != DD_OK )  {
	  wsprintf(logt, "SetCooperativeLevel Error: %Xh\n", hres);
      PrintLog(logt);
	  DoHalt("");
   }
   PrintLog("SetCooperativeLevel: Ok\n");

   hres = lpDD->SetDisplayMode( WinW, WinH, 16);        
	
   if (hres != DD_OK) {
	 wsprintf(logt, "DDRAW: Error set video mode %dx%d\n", WinW, WinH);
     PrintLog(logt);
    }
}

void ShutDown3DHardware()
{
   lpDD->RestoreDisplayMode();
   lpDD->SetCooperativeLevel( hwndMain, DDSCL_NORMAL);   
}


void Render3DHardwarePosts()
{
}


void CopyHARDToDIB()
{
}


void Hardware_ZBuffer(BOOL bl)
{
}

#endif