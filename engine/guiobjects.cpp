//=========== AdelphosPro 4.21.09 =================//
//-> Gui Objects and Management
//-> C Style

#include "hunt.h"

//-> Typedef structs

//-> Defined in Hunt.h
//typedef struct _guiSlider {
//	char name[128]; //Used to identify the slider
//	TPicture Image; //Image
//	POINT Pos; // Pos X and Y
//	float Value,MaxValue,MinValue; //Value info
//	TSFX HoverSound; //Hover sound
//	TSFX SlideSound; //Slide sound
//	bool show; //Hide or show
//} GuiSlider;


typedef struct _guiObjects {
	int SliderCount;
	GuiSlider Sliders[128];
} GuiCore;

//-> Vars
GuiCore gui;

//-> Extern functions used:

//-> Local Functions:

void Gui_AddSlider(char name[128],char picture[128],POINT startpos,float startvalue,float minv, float maxv, char HoverSound[128],char SlideSound[128], bool show, int MenuID) 
{
	//-> Add GUI object Slider (No error checking ATM)
	strcpy(gui.Sliders[gui.SliderCount].Name,name);
	LoadPictureTGA(gui.Sliders[gui.SliderCount].Image,picture);
	gui.Sliders[gui.SliderCount].Pos.x = startpos.x;
	gui.Sliders[gui.SliderCount].Pos.y = startpos.y;
	gui.Sliders[gui.SliderCount].Value = startvalue;
	gui.Sliders[gui.SliderCount].MinValue = minv;
	gui.Sliders[gui.SliderCount].MaxValue = maxv;
	gui.Sliders[gui.SliderCount].show = show;
	gui.Sliders[gui.SliderCount].ParentMenu = MenuID;
	gui.SliderCount++;
}

void Gui_ProcessAll() 
{
	//-> Loop through GUI objects, process changes, and draw them
	// -> Loop through sliders
	float atx, aty;
	int buttonX;
	for (int id = 0; id < gui.SliderCount; id++) {
		if (gui.Sliders[id].show && (gui.Sliders[id].ParentMenu == CURRENT_MENU || gui.Sliders[id].ParentMenu == -1)) {
			//-> Process Mouse
			atx = (float)gui.Sliders[id].Pos.x;
			aty = (float)gui.Sliders[id].Pos.y;
			if (MouseAtY >= aty && MouseAtY <= aty+int(gui.Sliders[id].Image.H) && MouseAtX >= atx && MouseAtX <= atx+(int)gui.Sliders[id].Image.W) {
				// -> Now process direction...
				switch (Mouse_SlideDir) {
					 case 1: //Going up
						 gui.Sliders[id].Value++;
						 if (gui.Sliders[id].Value > gui.Sliders[id].MaxValue)
							 gui.Sliders[id].Value = gui.Sliders[id].MaxValue;
						 break;
					 case -1: //Going down
						 gui.Sliders[id].Value--;
						 if (gui.Sliders[id].Value < gui.Sliders[id].MinValue)
							 gui.Sliders[id].Value = gui.Sliders[id].MinValue;
						 break;
				}
				Mouse_SlideDir = 0;
			}
			//-> Draw Images
			DrawPicture((int)gui.Sliders[id].Pos.x,(int)gui.Sliders[id].Pos.y,gui.Sliders[id].Image);
			if (gui.Sliders[id].hasbutton) {
				//-> NOTE: this only works for HOROZONTAL sliders. Program a Y offset for virtical ones
				//x + (imageY/valueRange)*Value
				buttonX = (int)gui.Sliders[id].Pos.x+(gui.Sliders[id].Image.W/(gui.Sliders[id].MaxValue-gui.Sliders[id].MinValue))*gui.Sliders[id].Value;
				if (buttonX > (gui.Sliders[id].Pos.x+gui.Sliders[id].Image.W)-gui.Sliders[id].ButtonImage.W)
					buttonX = (gui.Sliders[id].Pos.x+gui.Sliders[id].Image.W)-(gui.Sliders[id].ButtonImage.W+2);
				DrawPicture(buttonX,(int)gui.Sliders[id].Pos.y,gui.Sliders[id].ButtonImage);
			}
		}
	}
}

float Gui_GetSliderValue(char name[128]) {
	for (int fi = 0; fi < gui.SliderCount; fi++) {
		if (!strcmp(gui.Sliders[fi].Name,name) ) { //if it matches, then we found the slider. Return the value
			return gui.Sliders[fi].Value;
		}
	}
	return -1;
}

void Gui_AddSliderButton(char name[128], char buttonfilename[128]) {
	//== Add a slider button == //
	for (int fi = 0; fi < gui.SliderCount; fi++) {
		if (!strcmp(gui.Sliders[fi].Name,name) ) { //if it matches, then we found the slider.
			//-> Add the button
			LoadPictureTGA(gui.Sliders[fi].ButtonImage,buttonfilename);
			gui.Sliders[fi].hasbutton = true;
		}
	}
}

void Gui_SetSliderAttributes(char name[128],GuiSlider newattribs) {
	//-> Set slider attributes
	for (int fi = 0; fi < gui.SliderCount; fi++) {
		if (!strcmp(gui.Sliders[fi].Name,name) ) { //if it matches, then we found the slider.
			//-> Update attributes
			gui.Sliders[fi].show = newattribs.show;
			if (newattribs.MaxValue != 0)
				gui.Sliders[fi].MaxValue	     = newattribs.MaxValue;
			if (newattribs.MinValue != 0)
				gui.Sliders[fi].MinValue	     = newattribs.MinValue;
			if (newattribs.Pos.x != 0)
				gui.Sliders[fi].Pos.x	     = newattribs.Pos.x;
			if (newattribs.Pos.y != 0)
				gui.Sliders[fi].Pos.y	     = newattribs.Pos.y;
			break;
		}
	}
}

void Gui_SetSliderValue(char name[128],int value) {
		for (int fi = 0; fi < gui.SliderCount; fi++) {
		if (!strcmp(gui.Sliders[fi].Name,name) ) { //if it matches, then we found the slider.
			//-> Update attributes
			gui.Sliders[fi].Value = value;
			break;
		}
	}
}

void Gui_HideAll()
{
	//-> Hide all gui items
	 //-> Hide Sliders
}