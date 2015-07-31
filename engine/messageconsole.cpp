// == Message Console Code by Adelphos Pro == //

// -> Controls a game console to enable commands to be entered
// 4.17.9

#include "hunt.h"

// -> External Functions accessed
void CallSupply();
void SwitchMode(LPSTR lps, BOOL& b);

void Console_ProcessInput(void*) {
	// == Process Console Input Commands and Vars == //
	AddVoicev(TypeSound[1].length,TypeSound[1].lpData, 256);
	//PlaySound2d("HUNTDAT\\SOUNDFX\\typego.wav");
	
	/* -> List <- */
	// -> Developer Toggles
	if (DEBUG_ASKFORPASS) {
		DEBUG_PASSOK = FALSE;
		if (strcmp(TypedText,"kamikazi") == 0) {
			SwitchMode("Developer Mode",DEBUG);
			DEBUG_PASSOK = TRUE;
		}
		DEBUG_ASKFORPASS = FALSE;
	}
	if (strcmp(TypedText,"dev_mode") == 0) {
		if (!DEBUG) { //-> Ask for password
			DEBUG_ASKFORPASS = TRUE;
		} else {
			SwitchMode("Developer Mode",DEBUG);
		}
	}

	if (strcmp(TypedText,"dev_drawfps") == 0) SwitchMode("Draw FPS",TIMER);

	// -> Gameplay Stuff (ie, "Request supplies")
	if (strcmp(TypedText,"exit") == 0) {
		if (DEBUG) {
			ExitTime = 1;
		} else { 
			//-> You could find distance to left-corner of map and use that to change the time required to exit.
			ExitTime = 1500;
			FADING = 64;
		}
	}
	/*if (strcmp(TypedText,"request supplies") == 0) {
		CallSupply();
	}*/
	if (strcmp(TypedText,"balance") == 0) {
		//-> Show the hunter his points
		wsprintf(logt,"Current Balance for Hunter [%s]: %d",TrophyRoom.PlayerName,TrophyRoom.Score);
		AddMessage(logt);
	}
	// -> Video Engine Toggles
	/*if (DEBUG && strcmp(TypedText,"vid_drawmodels") == 0) SwitchMode("3D Models",MODELS);
	if (DEBUG && strcmp(TypedText,"vid_drawfog") == 0) SwitchMode("Fog",FOGENABLE);
	if (DEBUG && strcmp(TypedText,"vid_drawclouds") == 0) SwitchMode("Cloud Shaddows",Clouds);*/

	// -> System Toggles
	/*if (strcmp(TypedText,"sys_threaded") == 0) SwitchMode("Thread System",USE_THREADS);
	if (strncmp(TypedText,"sys_maxfps",10) == 0) {
		int getFPS;
		getFPS = Console_ProcessIntValue("sys_maxfps", TypedText,MAX_FPS);
		if (getFPS >= 0) {
			MAX_FPS = getFPS;
			wsprintf(logt,"Max FPS changed to %d",getFPS);
			AddMessage(logt);
		}
	}*/

	// -> Cheats
	//if (strcmp(TypedText,"flapyourwings") == 0) SwitchMode("Fly Cheat",FLY);
	//if (strcmp(TypedText,"chucknorris") == 0) SwitchMode("God Cheat",CHEAT_GOD);
	//if (strcmp(TypedText,"starbucksaddict") == 0) SwitchMode("Fast Walk Cheat",CHEAT_FASTWALK);
	//if (strcmp(TypedText,"chillpill") == 0) SwitchMode("Slow Mode Cheat",SLOW);
	//if (strncmp(TypedText,"portto ",7) == 0) {
	//	//Portal to the X, Y
	//	int x,y;
	//	x = Console_ProcessIntValue("portto", TypedText,0);
	//	y = x;
	//	// -> Check and go
	//	if (x > 0 && x < 1024 && y > 0 && y < 1024) {
	//		PlayerX = (float)x*256.f;
	//		PlayerZ = (float)y*256.f;
	//		AddMessage("Character Relocated");
	//	} else {
	//		wsprintf(logt,"Portto must contain a value in between 0 and 1024 in the form: portto 512",x,y);
	//		AddMessage(logt);
	//	}
	//}

	// -> Misc
	if (strncmp(TypedText,"sys_messageheight",17) == 0) {
		int getHeight;
		getHeight = Console_ProcessIntValue("sys_messageheight", TypedText,MESSAGELIST_MAXLENGTH);

		if (getHeight != MESSAGELIST_MAXLENGTH) {
			getHeight = max(min(getHeight,20),1);
			MESSAGELIST_MAXLENGTH = getHeight;
			while (MessageListCnt > MESSAGELIST_MAXLENGTH)
				DropFirstMessage();
		}
	}
	if (DEBUG && strncmp(TypedText,"test_setscore",13) == 0) {
		int chargeAmount;
		chargeAmount = Console_ProcessIntValue("test_setscore", TypedText,TrophyRoom.Score);

		if (chargeAmount >= 100) {
			TrophyRoom.Score = chargeAmount;
		} else {
			AddMessage("Invalid Score or Too Low");
		}
	}

	// -> Tests
	//if (strcmp(TypedText,"help") == 0) {
	//	// -> Display game commands
	//	int CmdCnt = 4; //Number of commands in total
	//	char CmdList[4][128] = {"request exit - Begin Evacuation", "request supplies - Order a refill on ammo (costs 2 points)", "request balance - Print total points", "Also try: 'help sys', 'help vid', or 'help dev'"};
	//	int delayTime = 0; //Delay time to add too per loop

	//		for (int cmdNum = 0; cmdNum < CmdCnt; cmdNum++) {
	//			AddMessage(CmdList[cmdNum]);
	//		}
	//}
	//if (strcmp(TypedText,"help dev") == 0) {
	//	// -> Display dev commands
	//	int CmdCnt = 2; //Number of commands in total
	//	char CmdList[2][128] = {"dev_mode - Toggle Developer Mode (debug)", "dev_drawfps - Display current FPS"};
	//	int delayTime = 0; //Delay time to add too per loop

	//		for (int cmdNum = 0; cmdNum < CmdCnt; cmdNum++) {
	//			AddMessage(CmdList[cmdNum]);
	//		}
	//}
	//if (strcmp(TypedText,"help vid") == 0) {
	//	// -> Display video commands
	//	int CmdCnt = 3; //Number of commands in total
	//	char CmdList[3][128] = {"vid_drawmodels - Toggle Draw 3D Models", "vid_drawfog - Toggle fog", "vid_drawclouds - Draw cloud shadows on the ground"};
	//	int delayTime = 0; //Delay time to add too per loop

	//		for (int cmdNum = 0; cmdNum < CmdCnt; cmdNum++) {
	//			AddMessage(CmdList[cmdNum]);
	//		}
	//}
	//if (strcmp(TypedText,"help sys") == 0) {
	//	// -> Display system commands
	//	int CmdCnt = 3; //Number of commands in total
	//	char CmdList[3][128] = {"sys_threaded - Toggle Multithreading for AI and Bullet Processing", "sys_maxfps - Set the max fps. Use: sys_maxfps #. 0 = no limit", "sys_messageheight - Max amount of messages to display on screen. Use: sys_messageheight #"};
	//	int delayTime = 0; //Delay time to add too per loop

	//		for (int cmdNum = 0; cmdNum < CmdCnt; cmdNum++) {
	//			AddMessage(CmdList[cmdNum]);
	//		}
	//}

	
	/* -> End <- */
    TypingJustExit = true;
	Console_Clear();
}

int Console_ProcessIntValue(char command[128], char fullstring[128],int originalValue) {
	// -> Find the int value from a string...
	int stringLength = (int)strlen(command);
	if (stringLength == (int)strlen(fullstring)) {
		//Then show command value...
		wsprintf(logt,"%s = %d",command,originalValue);
		AddMessage(logt);
		return originalValue;
	}
	char numinChar[128];
	for (int gi = stringLength; gi < (int)strlen(fullstring); gi++) {
		if (fullstring[gi] == 0) continue;
		numinChar[gi-stringLength] = fullstring[gi];

	}
	return (int)atoi(numinChar);
}

void Console_Clear() {
	TypingMode = FALSE;
	memset(TypedText, 0, sizeof(TypedText));
	TypedTextLength = 0;
}