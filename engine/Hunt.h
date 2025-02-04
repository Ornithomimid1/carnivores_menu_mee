#include "math.h"
#include "windows.h"
//== 4.15.9 -> Multithreading support by Adelphospro
#include <process.h>

#include "winuser.h"

#include "resource.h"

#include "ddraw.h"

//#include "boost/filesystem.hpp" //<- for looping through dirs



#ifdef _d3d
#include <d3d.h>
#endif

// alacn
// #define __rus
// #define __high_priority_process
// #define __disable_water_in_software

#define ctHScale  64
#define PMORPHTIME 256
#define HiColor(R,G,B) ( ((R)<<10) + ((G)<<5) + (B) )


#define TCMAX ((128<<16)-62024)
#define TCMIN ((000<<16)+62024)


#ifdef _MAIN_
 #define _EXTORNOT 
#else
 #define _EXTORNOT extern
#endif

#define pi 3.1415926535f
#define ctMapSize 1024




typedef struct tagMessageList {
   int timeleft;
   char mtext[256];
   int delayed; //Adelphospro; 4.17.09
} TMessageList;

typedef struct tagTRGB {
     BYTE B;
     BYTE G;
     BYTE R;
} TRGB;

typedef struct _Animation {
  char aniName[32];
  int aniKPS, FramesCount, AniTime;
  short int* aniData;
} TAni;

typedef struct _VTLdata {  
  int aniKPS, FramesCount, AniTime;
  short int* aniData;
} TVTL;

typedef struct _SoundFX {
  int  length;
  short int* lpData;
} TSFX;



typedef struct _TRD {
  int  RNumber, RVolume, RFreq;
  WORD REnvir, Flags;
} TRD;

typedef struct _TAmbient {
  TSFX sfx;
  TRD  rdata[16];
  int  RSFXCount;
  int  AVolume;
  int  RndTime;
} TAmbient;


typedef struct TagTEXTURE  {
  WORD DataA[128*128];
  WORD DataB[64*64];
  WORD DataC[32*32];
  WORD DataD[16*16];
  WORD SDataC[2][32*32];
  int mR, mG, mB;
} TEXTURE;



typedef struct _TPicture {
   int W,H;
   WORD* lpImage;
} TPicture;


typedef struct TagVector3d {
 float x,y,z;
} Vector3d;

typedef struct TagPoint3di {
 int x,y,z;
} TPoint3di;

typedef struct TagVector2di {
 int x,y;
} Vector2di;

typedef struct TagVector2df {
 float x,y;
} Vector2df;


typedef struct TagScrPoint {
#ifdef _soft
 int   x,y, tx,ty;
#else
 float x,y, tx,ty;
#endif

 int Light, z, r2, r3;
} ScrPoint;

typedef struct TagMScrPoint {
 int x,y, tx,ty;	 
} MScrPoint;

typedef struct tagClipPlane {
   Vector3d v1,v2,nv;   
} CLIPPLANE;





typedef struct TagEPoint {  
  Vector3d v;
  WORD DFlags;
  short int ALPHA;
#ifdef _d3d
  float  scrx, scry;
#else 
  int  scrx, scry;
#endif
  int Light;
  float Fog;
} EPoint;


typedef struct TagClipPoint {
  EPoint ev;
  float tx, ty;
} ClipPoint;


//================= MODEL ========================
typedef struct _Point3d {
	float x; 
	float y; 
	float z;
	short owner; 
	short hide;
} TPoint3d;



typedef struct _Face {
   int v1, v2, v3;   
#ifdef _soft
   int   tax, tbx, tcx, tay, tby, tcy;
#else
   float tax, tbx, tcx, tay, tby, tcy;
#endif
   WORD Flags,DMask;
   int Distant, Next, group;
   char reserv[12];  
} TFace;


typedef struct _Facef {
   int v1, v2, v3;   
   float tax, tbx, tcx, tay, tby, tcy;
   WORD Flags,DMask;
   int Distant, Next, group;
   char reserv[12];  
} TFacef;



typedef struct _Obj {
   char OName [32];
   float ox; 
   float oy;
   float oz;
   short owner; 
   short hide;
} TObj;


typedef struct TagMODEL {
    int VCount, FCount, TextureSize, TextureHeight;
    TPoint3d gVertex[1024];    
	union {
     TFace    gFace  [1024];
	 TFacef   gFacef [1024];
	};
    WORD     *lpTexture, *lpTexture2, *lpTexture3;
#ifdef _d3d
	int      VLight[4][1024];
#else
	float    VLight[4][1024];
#endif
} TModel;


//=========== END MODEL ==============================//


typedef struct _ObjInfo {
   int  Radius;
   int  YLo, YHi;
   int  linelenght, lintensity;
   int  circlerad, cintensity;
   int  flags;
   int  GrRad;
   int  DefLight;
   int  LastAniTime;   
   float BoundR;
   BYTE res[16];
} TObjInfo;

typedef struct _TBMPModel {
    Vector3d  gVertex[4];
	WORD     *lpTexture;
} TBMPModel;

typedef struct _TBound {	
	float cx, cy, a, b,  y1, y2;
} TBound;

typedef struct TagObject {
   TObjInfo info;   
   TBound   bound[8];
   TBMPModel bmpmodel;
   TModel  *model;
   TVTL    vtl;   
} TObject;


typedef struct _TCharacterInfo {
  char ModelName[32];
  int AniCount,SfxCount;
  TModel* mptr;
  TAni Animation[64];
  TSFX SoundFX[64];
  int  Anifx[64];
} TCharacterInfo;

typedef struct _TWeapon {
  TCharacterInfo chinfo[10];
  TPicture       BulletPic[10];
  Vector3d       normals[1024];
  int state, FTime, TPA;
  int ironsights; //if 2, then gun should be going to ironsights position. if 1, gun should be going to default rest position. If 0, gun is in default position. If 3, gun is in ironsights position
  float xplace, yplace; //Current location of gun on graph
  float shakel,StartBeta,ClimbBeta;  
} TWeapon;

typedef struct _MMapI {
	char name[128];
	char mapfile[128];
	char rscfile[128];
	TPicture image;
	char desc[512];
	int points;
	bool selected;
} MapData;
_EXTORNOT MapData MapFile[255];
_EXTORNOT int TotalM,SelectedMap;

typedef struct _TWCircle {
    Vector3d pos;
	float scale;
	int FTime;    
} TWCircle;


typedef struct _TSnowElement {
    Vector3d pos;
	float hl, ftime;
} TSnowElement;




typedef struct _TCharacter  {
  int CType, AI;
  TCharacterInfo *pinfo;
  int StateF;
  int State;
  int NoWayCnt, NoFindCnt, AfraidTime, tgtime;
  int PPMorphTime, PrevPhase,PrevPFTime, Phase, FTime;
  
  float vspeed, rspeed, bend, scale;
  int Slide; 
  float slidex, slidez;
  float tgx, tgz;

  Vector3d pos, rpos;
  float tgalpha, alpha, beta, 
        tggamma,gamma, 
        lookx, lookz;
  int Health, BloodTime, BloodTTime;
} TCharacter;



typedef struct tagPlayer {  
  BOOL Active;
  unsigned int IPaddr;
  Vector3d pos;
  float alpha, beta, vspeed;
  int kbState;
  char NickName[16];
} TPlayer;


typedef struct _TDemoPoint {
  Vector3d pos;
  int DemoTime, CIndex;
} TDemoPoint;

typedef struct tagLevelDef {  
    char FileName[64];
    char MapName[128];
    DWORD DinosAvail;
    WORD *lpMapImage;
} TLevelDef;


typedef struct tagShipTask {
  int tcount;
  int clist[255];
} TShipTask;

typedef struct tagShip {  
  Vector3d pos, rpos, tgpos, retpos;
  float alpha, tgalpha, speed, rspeed, DeltaY;
  int State, cindex, FTime;
} TShip;


typedef struct tagAmmoBag {
  Vector3d pos, rpos, speed;
  float alpha;
  int State, FTime;
} TAmmoBag;


typedef struct tagSupplyShip {  
  Vector3d pos, rpos, tgpos, retpos;
  float alpha, tgalpha, speed, rspeed;
  int State;
} TSupplyShip;



typedef struct tagLandingList {
  int PCount;
  Vector2di list[64];
} TLandingList;


typedef struct _TPlayerR {
	char PName[128];
	int  RegNumber;
	int  Score, Rank;
} TPlayerR;

typedef struct _TTrophyItem {
  int ctype, weapon, phase,
	  height, weight, score,
	  date, time;
  float scale, range;
  int r1, r2, r3, r4;
} TTrophyItem;


typedef struct _TStats {
	int smade, success;
	float path, time;
} TStats;


typedef struct _TTrophyRoom {
  char PlayerName[128];
  int  RegNumber;
  int  Score, Rank;

  TStats Last, Total;
    
  TTrophyItem Body[24];
} TTrophyRoom;



typedef struct _TDinoInfo {
	TPicture MenuPic;
	TPicture MenuPicHidden;
	char MenuTxt[512];
	char Name[48], FName[48], PName[48];	
	int Health0, AI;
	BOOL DangerCall;
	float Mass, Length, Radius, 
		  SmellK, HearK, LookK,
		  ShDelta;
	int   Scale0, ScaleA, BaseScore;
	TPicture CallIcon;
	int Price; //For menu
	bool Hide;
	bool Selected; //For menu
	int Code; //For menu
} TDinoInfo;

_EXTORNOT bool radarDefault, camoDefault, scentDefault, doubleAmmoDefault,
			tranqDefault, supplyDefault, sonarDefault, scannerDefault, dogDefault,
			binoDefault, binTextDefault, mapviewDefault, callboxDefault;

_EXTORNOT TPicture DawnPic;
_EXTORNOT TPicture DayPic;
_EXTORNOT TPicture NightPic;
_EXTORNOT TPicture ObservPic;


typedef struct _TAccessInfo {
	TPicture MenuPic;
	char MenuTxt[512];
	char name[48];
	//char CommandLine[128];
	bool Selected;
	int price;
	float scoreMod;
	bool radar, camo, scent, doubleAmmo,
		tranq, supply, sonar, scanner, dog,
		bino, binText, mapview, callbox;
} TAcessInfo;
_EXTORNOT TAcessInfo AcessInfo[512];
_EXTORNOT int SpentCredits,SelectedDay;
_EXTORNOT int * PtrSelectedKey; //Pointer to seleted key
_EXTORNOT bool AKeySelected;

_EXTORNOT char DawnTxt[512];
_EXTORNOT char DayTxt[512];
_EXTORNOT char NightTxt[512];
_EXTORNOT char ObservTxt[512];

typedef struct _TWeapInfo {
	TPicture MenuPic;
	char MenuTxt[512];
	char Name[48], FName[48], BFName[48],SFName[48]; //4.18.09 -> Enhanced Sound Support
	float Power, Prec, Loud, Rate;
	float xoffset, yoffset; //4.13.09, Adelphospro, weapon offsets
	int Shots, Optic, Fall, TraceC, Reload;
	int FTime; //4.18.09, Adelphospro -> Weapon Fire Time enhancement
	bool semiauto; //4.18.09, Adelphospro -> Weapon Fire Enhancement -> Whether or not the gun will continue to fire with the mouse pressed
	int Price; //For the menu
	bool Selected; //For menu
	int Code; //For menu
} TWeapInfo;




typedef struct _TFogEntity {
  int fogRGB;
  float YBegin;
  BOOL  Mortal;
  float Transp, FLimit; 
} TFogEntity;


typedef struct _TWaterEntity {
  int tindex, wlevel;
  float transp;
  int fogRGB;
} TWaterEntity;


typedef struct _TWind {
   float alpha;
   float speed;
   Vector3d nv;
} TWind;




typedef struct _TElement {
   Vector3d pos, speed;
   int     Flags;
   float   R;
} TElement;

typedef struct _TElements {
	int Type, ECount, EDone, LifeTime;
	int Param1, Param2, Param3;
	DWORD RGBA, RGBA2;
    Vector3d pos;
    TElement EList[32];
} TElements;


typedef struct _TBloodP {
	int LTime;
	Vector3d pos;
} TBloodP;

typedef struct _TBTrail {
	int Count;
    TBloodP Trail[512];
} TBTrail;

typedef struct _PUser {
	TTrophyRoom TrophyRoom;
} CarnPlayerFile;
_EXTORNOT int TOTAL_PLAYERS;
_EXTORNOT int SelectedPlayer;
_EXTORNOT CarnPlayerFile PlayerFile[12];

//============= functions ==========================//

void HLineTxB( void );
void HLineTxC( void );
void HLineTxGOURAUD( void );
void GetKeyChar(int,char*);

_EXTORNOT	char ActiveInfoText[512];


void HLineTxModel25( void );
void HLineTxModel75( void );
void HLineTxModel50( void );

void HLineTxModel3( void );
void HLineTxModel2( void );
void HLineTxModel( void );

void HLineTDGlass75( void );
void HLineTDGlass50( void );
void HLineTDGlass25( void );
void HLineTBGlass25( void );


void SetVideoMode(int, int);

void CreateDivTable();
void DrawTexturedFace();
int GetTextW(HDC, LPSTR);
void wait_mouse_release();

//============================== render =================================//
void ShowControlElements();
void InsertModelList(TModel* mptr, float x0, float y0, float z0, int light, float al, float bt);
void RenderGround();
void RenderWater();
void RenderElements();
void CreateChRenderList();
void RenderModelsList();
void ProcessMap  (int x, int y, int r);
void ProcessMap2 (int x, int y, int r);
void ProcessMapW (int x, int y, int r);
void ProcessMapW2(int x, int y, int r);

void DrawTPlane(BOOL);
void DrawTPlaneClip(BOOL);
void ClearVideoBuf();
void DrawTrophyText(int, int);
void DrawHMap();
void RenderCharacter(int);
void RenderShip();
void RenderPlayer(int);
void RenderSkyPlane();
void RenderHealthBar();
void RenderEnergyBar();
void Render_Cross(int, int,float,bool); //4.14.09, modified to override size by Adelphospro
void Render_LifeInfo(int);

void RenderModelClipEnvMap(TModel*, float, float, float, float, float);
void RenderModelClipPhongMap(TModel*, float, float, float, float, float);

_EXTORNOT int DinoStatType; // 1 dino  2 weap
_EXTORNOT int DinoStatIndex;

_EXTORNOT int survivalArea;
_EXTORNOT int survivalWeapon;
_EXTORNOT int survivalDTM;
_EXTORNOT int startScore;


_EXTORNOT int regOffR;
_EXTORNOT int regOffG;
_EXTORNOT int regOffB;
_EXTORNOT int regOffFontC;

_EXTORNOT int regOnR;
_EXTORNOT int regOnG;
_EXTORNOT int regOnB;
_EXTORNOT int regOnFontC;

_EXTORNOT int scoreR;
_EXTORNOT int scoreG;
_EXTORNOT int scoreB;
_EXTORNOT int scoreFontC;

_EXTORNOT int mainStatR;
_EXTORNOT int mainStatG;
_EXTORNOT int mainStatB;
_EXTORNOT int mainStatFontC;

_EXTORNOT int huntOffR;
_EXTORNOT int huntOffG;
_EXTORNOT int huntOffB;
_EXTORNOT int huntOffFontC;

_EXTORNOT int huntAvailR;
_EXTORNOT int huntAvailG;
_EXTORNOT int huntAvailB;
_EXTORNOT int huntAvailFontC;

_EXTORNOT int huntOnR;
_EXTORNOT int huntOnG;
_EXTORNOT int huntOnB;
_EXTORNOT int huntOnFontC;

_EXTORNOT int huntInfoR;
_EXTORNOT int huntInfoG;
_EXTORNOT int huntInfoB;
_EXTORNOT int huntInfoFontC;

_EXTORNOT int opNameR;
_EXTORNOT int opNameG;
_EXTORNOT int opNameB;
_EXTORNOT int opNameFontC;

_EXTORNOT int opValueR;
_EXTORNOT int opValueG;
_EXTORNOT int opValueB;
_EXTORNOT int opValueFontC;


void RenderModel         (TModel*, float, float, float, int, int, float, float);
void RenderBMPModel      (TBMPModel*, float, float, float, int);
void RenderModelClipWater(TModel*, float, float, float, int, int, float, float);
void RenderModelClip     (TModel*, float, float, float, int, int, float, float);
void RenderNearModel     (TModel*, float, float, float, int, float, float);
void DrawPicture         (int x, int y, TPicture &pic);
void DrawStatBar         (int x, int y, float val, LPSTR txt);

void InitClips();
void InitDirectDraw();
void WaitRetrace();

//============= Characters =======================
void Characters_AddSecondaryOne(int ctype);
void AddDeadBody(TCharacter *cptr, int);
void PlaceCharacters();
void PlaceTrophy();
void AnimateCharacters(void *);
void MakeNoise(Vector3d, float);
void CheckAfraid();
void CreateChMorphedModel(TCharacter* cptr);
void CreateMorphedObject(TModel* mptr, TVTL &vtl, int FTime);
void CreateMorphedModel(TModel* mptr, TAni *aptr, int FTime, float scale);

//=============================== Math ==================================//

void CalcLights  (TModel* mptr);
void CalcModelGroundLight(TModel *mptr, float x0, float z0, int FI);
void CalcNormals (TModel* mptr, Vector3d *nvs);
void CalcGouraud (TModel* mptr, Vector3d *nvs);

void CalcPhongMapping(TModel* mptr, Vector3d *nv);
void CalcEnvMapping(TModel* mptr, Vector3d *nv);

void CalcBoundBox(TModel* mptr, TBound *bound);
void  NormVector(Vector3d&, float); 
float SGN(float);
void  DeltaFunc(float &a, float b, float d);
void  MulVectorsScal(Vector3d&, Vector3d&, float&);
void  MulVectorsVect(Vector3d&, Vector3d&, Vector3d&);
Vector3d SubVectors( Vector3d&, Vector3d& );
Vector3d AddVectors( Vector3d&, Vector3d& );
Vector3d RotateVector(Vector3d&);
float VectorLength(Vector3d);
int   siRand(int);
int   rRand(int);
void  CalcHitPoint(CLIPPLANE&, Vector3d&, Vector3d&, Vector3d&);
void  ClipVector(CLIPPLANE& C, int vn);
float FindVectorAlpha(float, float);
float AngleDifference(float a, float b);

int   TraceShot(float ax, float ay, float az,
                float &bx, float &by, float &bz);
int   TraceLook(float ax, float ay, float az,
                float bx, float by, float bz);


void CheckCollision(float&, float&);
float CalcFogLevel(Vector3d v);
//=================================================================//
void AddMessage(LPSTR mt);
void AddMessageDelayed(LPSTR mt, int delaytime);
void DropFirstMessage();
_EXTORNOT int MESSAGELIST_MAXLENGTH;
_EXTORNOT bool TypingJustExit;

int Console_ProcessIntValue(char command[128], char fullstring[128],int originalValue);
void Console_ProcessInput(void*);
void Console_Clear();
void CreateTMap();

void ProcessButton1();
void ProcessButton2();

void ProcessButton4();


void LoadSky();
void LoadSkyMap();
void LoadTexture(TEXTURE*&);
void LoadWav(char* FName, TSFX &sfx);


void ApplyAlphaFlags(WORD*, int);
WORD conv_565(WORD c);
int  conv_xGx(int);
void conv_pic(TPicture &pic);
void LoadPicture(TPicture &pic, LPSTR pname);
void LoadPictureTGA(TPicture &pic, LPSTR pname);
void LoadCharacterInfo(TCharacterInfo&, char*);
void LoadModelEx(TModel* &mptr, char* FName);
void LoadModel(TModel*&);
void LoadResources();
void ReInitGame();


void SaveScreenShot();
void CreateWaterTab();
void CreateFadeTab();
void CreateVideoDIB();
void RenderLightMap();

void MulVectorsVect(Vector3d& v1, Vector3d& v2, Vector3d& r );
void MulVectorsScal(Vector3d& v1,Vector3d& v2, float& r);
Vector3d SubVectors( Vector3d& v1, Vector3d& v2 );
void NormVector(Vector3d& v, float Scale);

LPVOID _HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);
BOOL _HeapFree(HANDLE hHeap, DWORD  dwFlags, LPVOID lpMem);

//============ game ===========================//
float GetLandCeilH(float, float);
float GetLandH(float, float);
float GetLandOH(int, int);
float GetLandLt(float, float);
float GetLandUpH(float, float);
float GetLandQH(float, float);
float GetLandQHNoObj(float, float);
float GetLandHObj(float, float);

void LoadResourcesScript();
void InitEngine();
void ShutDownEngine();
void ProcessSyncro();
void AddShipTask(int);
void LoadTrophy();
void InitGUIitems();
void Gui_SetSliderValue(char name[128],int value);
//void LoadPlayersInfo();
void SaveTrophy();
void LoadUserList();
void RemoveCurrentTrophy();
void MakeCall();
void MakeShot(float ax, float ay, float az,
              float bx, float by, float bz);

void AddBloodTrail(TCharacter *cptr);
void AddElements(float, float, float, int, int);
void AddWCircle(float, float, float);
void AnimateProcesses();
void DoHalt(LPSTR);

_EXTORNOT   char logt[128],TypedText[128];
_EXTORNOT	int TypedTextLength,BlinkTime;
void CreateLog();
void PrintLog(LPSTR l);
void CloseLog();

_EXTORNOT   float BackViewR;
_EXTORNOT   int   BackViewRR;
_EXTORNOT   int   UnderWaterT;
_EXTORNOT   int   TotalC, TotalW, TotalA;


//========== common ==================//
_EXTORNOT   HWND    hwndMain;
_EXTORNOT   HANDLE  hInst;
_EXTORNOT   HANDLE  Heap;
_EXTORNOT   HDC     hdcMain, hdcCMain;
_EXTORNOT   BOOL    blActive;
_EXTORNOT   BYTE    KeyboardState[256];
_EXTORNOT   int     KeyFlags, _shotcounter;

_EXTORNOT   TMessageList MessageList[32],MessageListDelayed[128]; //Max of 32 on screen at one time
_EXTORNOT   char    ProjectName[128];
_EXTORNOT   int     _GameState,MessageListCnt,MessageListDelayedCnt;
_EXTORNOT   TSFX    fxCall[10][3], fxScream[4], TypeSound[4],MENUAMB,MENUGO,MENUMOV;
_EXTORNOT   TSFX    fxUnderwater, fxWaterIn, fxWaterOut, fxJump, fxStep[3], fxStepW[3];
//========== map =====================//
_EXTORNOT   byte HMap[ctMapSize][ctMapSize];
_EXTORNOT   byte WMap[ctMapSize][ctMapSize];
_EXTORNOT   byte HMapO[ctMapSize][ctMapSize];
_EXTORNOT   WORD FMap[ctMapSize][ctMapSize];
_EXTORNOT   byte LMap[ctMapSize][ctMapSize];
_EXTORNOT   WORD TMap1[ctMapSize][ctMapSize];
_EXTORNOT   WORD TMap2[ctMapSize][ctMapSize];
_EXTORNOT   byte OMap[ctMapSize][ctMapSize];

_EXTORNOT   byte FogsMap[512][512];
_EXTORNOT   byte AmbMap[512][512];

_EXTORNOT   TFogEntity    FogsList[256];
_EXTORNOT   TWaterEntity  WaterList[256];
_EXTORNOT   TWind       Wind;
_EXTORNOT   TShip       Ship;
_EXTORNOT   TAmmoBag    AmmoBag; 
_EXTORNOT   TSupplyShip SupplyShip;
_EXTORNOT   TShipTask   ShipTask;

_EXTORNOT   int SkyR, SkyG, SkyB, WaterR, WaterG, WaterB, WaterA,
                SkyTR,SkyTG,SkyTB, CurFogColor;
_EXTORNOT   int RandomMap[32][32];

_EXTORNOT   Vector2df PhongMapping[1024];
_EXTORNOT   TPicture TFX_SPECULAR, TFX_ENVMAP;
_EXTORNOT   WORD SkyPic[256*256];
_EXTORNOT   WORD SkyFade[9][128*128];
_EXTORNOT   BYTE SkyMap[128*128];

_EXTORNOT   TEXTURE* Textures[1024];
_EXTORNOT   TAmbient Ambient[256];
_EXTORNOT   TSFX     RandSound[256];

//========= GAME ====================//
_EXTORNOT int TargetDino, TargetArea, TargetWeapon, WeaponPres, TargetCall,
              TrophyTime, ObservMode, Tranq, ObjectsOnLook,
			  CurrentWeapon, ShotsLeft[10], AmmoMag[10];

_EXTORNOT Vector3d answpos;
_EXTORNOT int answtime, answcall;

_EXTORNOT BOOL ScentMode, CamoMode, 
               RadarMode, LockLanding, 
			   TrophyMode, DoubleAmmo, Supply, SupplyUsed;

_EXTORNOT TTrophyRoom TrophyRoom;
//_EXTORNOT TPlayerR PlayerR[16];
_EXTORNOT TPicture LandPic,DinoPic,DinoPicM, MapPic, WepPic,MenuPic[32],MenuPicOn[32];
_EXTORNOT HFONT fnt_BIG, fnt_Small, fnt_Midd;
_EXTORNOT TLandingList LandingList;

//======== MODEL ======================//
_EXTORNOT TObject  MObjects[256];
_EXTORNOT TModel* mptr;
_EXTORNOT TWeapon Weapon;


_EXTORNOT int   OCount, iModelFade, iModelBaseFade, Current;
_EXTORNOT Vector3d  rVertex[1024];
_EXTORNOT TObj      gObj[1024];
_EXTORNOT Vector2di gScrp[1024];
_EXTORNOT Vector2df gScrpf[1024];

//============= Characters ==============//
_EXTORNOT TPicture  PausePic, ExitPic, TrophyExit, TrophyPic;
_EXTORNOT TModel *SunModel;
_EXTORNOT TCharacterInfo WCircleModel;
_EXTORNOT TModel *CompasModel;
_EXTORNOT TModel *Binocular;
_EXTORNOT TDinoInfo DinoInfo[255];
_EXTORNOT TWeapInfo WeapInfo[255];
_EXTORNOT TCharacterInfo ShipModel, SShipModel, AmmoModel;
_EXTORNOT int AI_to_CIndex[32];
_EXTORNOT int ChCount, WCCount, ElCount, SnCount,
              ShotDino, TrophyBody;
_EXTORNOT TCharacterInfo WindModel;
_EXTORNOT TCharacterInfo PlayerInfo;
_EXTORNOT TCharacterInfo ChInfo[32];
_EXTORNOT TCharacter     Characters[256];
_EXTORNOT TWCircle       WCircles[128];
_EXTORNOT TSnowElement   Snow[8024];
_EXTORNOT TDemoPoint     DemoPoint;

_EXTORNOT TPlayer        Players[16];
_EXTORNOT Vector3d       PlayerPos, CameraPos;

//========== MENU ====================//
_EXTORNOT   int CURRENT_MENU,BUTTON_HOVER_ID,Mouse_SlideDir;
_EXTORNOT	float MouseAtX,MouseAtY;
_EXTORNOT	char CREATE_USER_NAME[128];
_EXTORNOT	BOOL MENU_MUSICPLAYING;

//========== GUI ================//
typedef struct _guiSlider {
	char Name[128]; //Used to identify the slider
	TPicture Image; //Image
	TPicture ButtonImage;//Button image
	POINT Pos; // Pos X and Y
	float Value,MaxValue,MinValue; //Value info
	TSFX HoverSound; //Hover sound
	TSFX SlideSound; //Slide sound
	bool show; //Hide or show
	bool hasbutton;
	int ParentMenu; //Menu that owns the object. Set to -1 for all
} GuiSlider;

void Gui_AddSlider(char name[128],char picture[128],POINT startpos,float startvalue,float minv, float maxv, char HoverSound[128],char SlideSound[128], bool show, int MenuID);
void Gui_SetSliderAttributes(char name[128],GuiSlider newattribs);
void Gui_ProcessAll();
float Gui_GetSliderValue(char name[128]);
void Gui_AddSliderButton(char name[128], char buttonfilename[128]);
void Gui_HideAll();

typedef struct _menItem {
	byte Hotspots[300][400];
	TPicture Image,ImageOn;
	char rawName[128];
	int MENU_ID;
} MenuItem;

_EXTORNOT	MenuItem GameMenus[32];
#define MENU_LOGIN 0
#define MENU_WARNINGUSER 1
#define MENU_DELETEUSER 2

#define MENU_MAIN 3
#define MENU_OPTIONS 4
#define MENU_STATS 5
#define MENU_PREHUNT 6
#define MENU_CREDITS 7
#define MENU_QUIT 8

//========== Render ==================//
_EXTORNOT   LPDIRECTDRAW lpDD;
_EXTORNOT   LPDIRECTDRAW2 lpDD2;
//_EXTORNOT   LPDIRECTINPUT lpDI;

_EXTORNOT   void* lpVideoRAM;
_EXTORNOT   LPDIRECTDRAWSURFACE lpddsPrimary;
_EXTORNOT   BOOL DirectActive, RestartMode;
_EXTORNOT   BOOL LoDetailSky;
_EXTORNOT   int  WinW,WinH,WinEX,WinEY,VideoCX,VideoCY,VideoCX16,VideoCY16,iBytesPerLine,ts,r,MapMinY;
//int XFootStepTime; float XFootStep;
_EXTORNOT   float VideoCXf, VideoCYf, CameraW,CameraH,CameraW16,CameraH16,Soft_Persp_K, stepdy, stepdd,stepsoundtime, SunShadowK, FOVK;
_EXTORNOT   CLIPPLANE ClipA,ClipB,ClipC,ClipD,ClipZ,ClipW;
_EXTORNOT   int u,vused, CCX, CCY;

_EXTORNOT   DWORD Mask1,Mask2;
_EXTORNOT   DWORD HeapAllocated, HeapReleased;


_EXTORNOT   EPoint VMap[256][256];
_EXTORNOT   EPoint VMap2[256][256];
_EXTORNOT   EPoint ev[3];

_EXTORNOT   ClipPoint cp[16];
_EXTORNOT   ClipPoint hleft,hright;


_EXTORNOT   void  *HLineT;
_EXTORNOT   int   rTColor;
_EXTORNOT   int   SKYMin, SKYDTime, GlassL, ctViewR, ctViewR1, ctViewRM,
                  dFacesCount, ReverseOn, TDirection;
_EXTORNOT   WORD  FadeTab[65][0x8000];
_EXTORNOT   TElements Elements[32];
_EXTORNOT   TBTrail   BloodTrail;

_EXTORNOT   int     PrevTime, TimeDt, T, Takt, RealTime, StepTime, MyHealth, ExitTime,
                    ChCallTime, CallLockTime, NextCall,MyEnergy,LastStepSoundTime; //ADP, 4.19.09, added MyEnergy
_EXTORNOT   float   DeltaT;
_EXTORNOT   float   CameraX, CameraY, CameraZ, CameraAlpha, CameraBeta;
_EXTORNOT   float   PlayerX, PlayerY, PlayerZ, PlayerAlpha, PlayerBeta, 
                    HeadY, HeadBackR, HeadBSpeed, HeadAlpha, HeadBeta,
                    SSpeed,VSpeed,RSpeed,YSpeed;      
_EXTORNOT   Vector3d PlayerNv;

_EXTORNOT   float   ca,sa,cb,sb, wpnDAlpha, wpnDBeta;
_EXTORNOT   void    *lpVideoBuf, *lpTextureAddr;
_EXTORNOT   HBITMAP hbmpVideoBuf;
_EXTORNOT   HCURSOR hcArrow;
_EXTORNOT   int     DivTbl[10240];

_EXTORNOT   Vector3d  v[3];
_EXTORNOT   ScrPoint  scrp[3];
_EXTORNOT   MScrPoint mscrp[3];
_EXTORNOT   Vector3d  nv, waterclipbase, Sun3dPos;

//=========== Threads ================//
_EXTORNOT BOOL USE_THREADS, AI_THREAD_ACTIVE;

//=========== Developer Tools =======//
_EXTORNOT BOOL TypingMode; //Game is in type mode
_EXTORNOT int ENGINE_FPS,MAX_FPS;


typedef struct _TKeyMap {
#ifdef _iceage // alacn
		int fkForward, fkBackward, fkUp, fkDown, fkLeft, fkRight, fkFire, fkShow, fkSLeft, fkSRight, fkStrafe, fkJump, fkRun, fkCrouch, fkCall, fkCCall, fkBinoc, fkSupply;
#else
		int fkForward, fkBackward, fkUp, fkDown, fkLeft, fkRight, fkFire, fkShow, fkSLeft, fkSRight, fkStrafe, fkJump, fkRun, fkCrouch, fkCall, fkCCall, fkBinoc;//, fkSupply, fkSprint, fkAim
#endif
	} TKeyMap;

_EXTORNOT TKeyMap KeyMap;

#define kfForward     0x00000001
#define kfBackward    0x00000002
#define kfLeft        0x00000004
#define kfRight       0x00000008
#define kfLookUp      0x00000010
#define kfLookDn      0x00000020
#define kfJump        0x00000040
#define kfCall        0x00000100

#define kfSLeft       0x00001000
#define kfSRight      0x00002000
#define kfStrafe      0x00004000

#define fmWater   0x0080
#define fmWater2  0x8000
#define fmNOWAY   0x0020
#define fmReverse 0x0010

#define fmWaterA  0x8080


#define tresGround 1
#define tresWater  2
#define tresModel  3
#define tresChar   4

#define sfDoubleSide         1
#define sfDarkBack           2
#define sfOpacity            4
#define sfTransparent        8
#define sfMortal        0x0010
#define sfPhong         0x0030
#define sfEnvMap        0x0050

#define sfNeedVC        0x0080
#define sfDark          0x8000

#define ofPLACEWATER       1
#define ofPLACEGROUND      2
#define ofPLACEUSER        4
#define ofCIRCLE           8
#define ofBOUND            16
#define ofNOBMP            32
#define ofNOLIGHT          64
#define ofDEFLIGHT         128
#define ofGRNDLIGHT        256
#define ofNOSOFT           512
#define ofNOSOFT2          1024
#define ofANIMATED         0x80000000

#define csONWATER          0x00010000
#define MAX_HEALTH         128000
#define MAX_ENERGY		   100000 //Set to 99999 for a bar that's always visible

#define HUNT_EAT      0
#define HUNT_BREATH   1
#define HUNT_FALL     2
#define HUNT_KILL     3 


#ifdef _iceage // alacn

#define AI_PIG        1
#define AI_ARCHEO     2

#define AI_BRONT      10
#define AI_HOG        11
#define AI_WOLF       12
#define AI_RHINO      13
#define AI_DIATR      14
#define AI_DEER       15
#define AI_SMILO      16
#define AI_MAMM       17
#define AI_BEAR       18
#define AI_BIGFOOT    19

#else // _iceage

#define AI_MOSH       1
#define AI_GALL       2
#define AI_DIMOR      3
#define AI_PTERA      4
#define AI_DIMET      5
#define AI_BRACH      6

#define AI_PARA       10
#define AI_ANKY       11
#define AI_STEGO      12
#define AI_ALLO       13
#define AI_CHASM      14
#define AI_VELO       15
#define AI_SPINO      16
#define AI_CERAT      17
#define AI_TREX       18

#endif // _iceage


_EXTORNOT BOOL WATERANI,Clouds,SKY,GOURAUD,
               MODELS,TIMER,BITMAPP,MIPMAP,
               NOCLIP,CLIP3D,NODARKBACK,CORRECTION, LOWRESTX, 
			   FOGENABLE, FOGON, CAMERAINFOG,
               WATERREVERSE,waterclip,UNDERWATER, ONWATER, NeedWater,
               SNOW, SWIM, PAUSE, OPTICMODE, BINMODE, EXITMODE, MapMode, RunMode, CrouchMode, SprintMode; //-> 4.13.09, ADP, Added SprintMode
// -> Cheats
_EXTORNOT BOOL FLY, CHEAT_GOD,CHEAT_FASTWALK, CHEAT_NOFEAR;

_EXTORNOT int  CameraFogI, QUITMODE,FADING;
_EXTORNOT int OptDayNight, OptAgres, OptDens, OptSens, OptRes, OptViewR,
              OptMsSens, OptBrightness, OptSound, OptRender,
              OptText, OptSys, WaitKey, OPT_ALPHA_COLORKEY;
_EXTORNOT BOOL SHADOWS3D,REVERSEMS;

_EXTORNOT BOOL SLOW, DEBUG, MORPHP, MORPHA,FADE_IN;
_EXTORNOT BOOL DEBUG_ASKFORPASS, DEBUG_PASSOK;
_EXTORNOT HANDLE hlog;


//========== for audio ==============//
void  AddVoicev  (int, short int*, int);
void  AddVoice3dv(int, short int*, float, float, float, int);
void  AddVoice3d (int, short int*, float, float, float);

void SetAmbient3d(int, short int*, float, float, float);
void SetAmbient(int, short int*, int);
void AudioSetCameraPos(float, float, float, float, float);
void InitAudioSystem(HWND, HANDLE, int);
void Audio_Restore();
void AudioStop();
void Audio_Shutdown();
void Audio_SetEnvironment(int, float);

//========== Enhanced Audio =========//
//bool InitSoundEngine();
//void ShutdownSoundEngine();
//void PlaySound2d(char fname[128]);
//void PlaySound2dLoop(char fname[128]);

//========== for 3d hardware =============//
_EXTORNOT BOOL HARD3D;
void ShowVideo();
void Init3DHardware();
void Activate3DHardware();
void ShutDown3DHardware();
void Render3DHardwarePosts();
void CopyBackToDIB();
void CopyHARDToDIB();
void Hardware_ZBuffer(BOOL zb);


//=========== loading =============
void StartLoading();
void EndLoading();
void PrintLoad(char *t);

#ifdef _MAIN_
_EXTORNOT char KeysName[256][24] = {
"...",
"Left Mouse",
"Right Mouse",
"Scroll Lock",
"undefined",
"undefined",
"undefined",
"undefined",
"undefined",
"Backspace",
"Tab",
"undefined",
"undefined",
"Numpad 5",
"Enter",
"undefined",
"Shift",
"Ctrl",
"Alt",
"undefined",
"Pause",
"Caps Lock",
"undefined",
"undefined",
"undefined",
"undefined",
"undefined",
"undefined",
"Escape",
"undefined",
"undefined",
"undefined",
"Space",
"",
"Page Up",
"Page Down",
"End",
"Home",
"Arrow Left",
"Arrow Up",
"Arrow Right",
"Arrow Down",
"undefined",
"undefined",
"undefined/blank",
"undefined/blank",
"Insert",
"Delete",
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"",
"undefined",
"undefined",
"undefined",
"undefined",
"undefined",
"undefined",
"A",
"B",
"C",
"D",
"E",
"F",
"G",
"H",
"I",
"J",
"K",
"L",
"M",
"N",
"O",
"P",
"Q",
"R",
"S",
"T",
"U",
"V",
"W",
"X",
"Y",
"Z",
"Windows Key",
"undefined/blank",
"Menu",
"undefined",
"undefined/blank",
"Num Insert",
"Num End",
"Num Down",
"Num Page Down",
"Num Page Left",
"Numpad 5",
"Num Right",
"Num Home",
"Num Up",
"Num Page Up",
"*",
"+",
"undefined",
"Delete",
"/",
"F1",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"F10",
"F11",
"F12",
"undefined",
"undefined",
"undefined",
"undefined",
"", "", "", "", "", "", "", "", "NLock", "SLock", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "Ctrl", "Ctrl", "Shift", "Shift", "Alt", "Alt", "", "",
"", "", "", "", "M", "D", "C", "B", "Q", "J", "G", "", "", "", "F", "",
"", ";", "=", ",", "_", ".", "/", "~", "", "Middle Mouse", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",""
	};
#else
   _EXTORNOT char KeysName[128][24];
#endif