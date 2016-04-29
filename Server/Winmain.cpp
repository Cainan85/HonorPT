#define _SINBARAM
#define WM_CALLMEMMAP WM_USER+3

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <ZMOUSE.H>
#include <process.h>

#include "smlib3d\\smd3d.h"
#include "smwsock.h"
#include "smreg.h"

#include "character.h"
#include "playmain.h"
#include "srcsound\\dxwav.h"
#include "effectsnd.h"
#include "fileread.h"
#include "netplay.h"
#include "makeshadow.h"

#include "particle.h"
#include "field.h"
#include "hoBaram\\holinkheader.h"

#include "tjboy\\clanmenu\\tjclanDEF.h"
#include "tjboy\\clanmenu\\tjclan.h"
#include "tjboy\\clanmenu\\cE_CViewClanInfo.h"

#ifdef _SINBARAM
#include "sinbaram\\sinlinkheader.h"
#endif

#include "playsub.h"
#include "cracker.h"
#include "SkillSub.h"

#include "resource.h"

#include "TextMessage.h"
#include "language.h"
#include "srcLang\\jts.h"

#include "CurseFilter.h"
#include "damage.h"
#include "AreaServer.h"

#include "BellatraFontEffect.h"

#include "actiongame.h"

#include "WinInt\\WinIntThread.h"
#include "FullZoomMap.h"
#include "FontImage.h"
#include "sinbaram\\HaQuestBoard.h"

#include "ime.h"

char *RegPath_3DMax = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\3D Studio MAX R3.1L";
char *RegPath_3DMax2 = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\3D Studio MAX R3.1";
char *RegPath_Photoshop = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Adobe Photoshop 6.0";
char *RegPath_ACDSee = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ACDSee";

HWND hwnd;
HWND hTextWnd;
HWND hFocusWnd;

HDC hdc;
int quit = 0;
int QuitSave = 0;
int WinSizeX = 640;
int WinSizeY = 480;
int	WinColBit = 16;

float g_fWinSizeRatio_X;
float g_fWinSizeRatio_Y;

int TextMode;

int BrCtrl = 0;

DWORD	dwM_BlurTime = 0;

int MouseButton[3] = { 0,0,0 };

#pragma comment( lib, "imm32.lib" )

#define DLLEXPORT __declspec(dllexport)

extern "C" {
	DLLEXPORT BOOL _stdcall smSetCurrentWindows(HWND wnd);
	DLLEXPORT int _stdcall InitD3D(HWND hWnd);
	DLLEXPORT void _stdcall CloseD3d();
	DLLEXPORT void _stdcall smPlayD3D(int x, int y, int z, int ax, int ay, int az);
	DLLEXPORT int _stdcall smSetMode(HWND hWnd, DWORD Width, DWORD Height, DWORD BPP);
}

extern "C" BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved
	);

ImeClass IMETest;
CStrClass sz;
LRESULT CALLBACK EditWndProc01(HWND, UINT, WPARAM, LPARAM);
WNDPROC		OldEditProc01;
char szIME_Buff[10][64];

DWORD	dwDebugBack;
DWORD	dwDebugXor;

int MouseX, MouseY;
int MousemX, MousemY;
int angX = 0;
int angY = 0;
DWORD	dwLastMouseMoveTime;
DWORD	dwLastCharMoveTime;

int xPos, yPos;

POINT3D TraceCameraPosi;
POINT3D TraceTargetPosi;
int	PlayCameraMode = 1;
int AutoCameraFlag = FALSE;
int	CameraInvRot = 0;
int	CameraSight = 0;

LPDIRECTDRAWSURFACE4 lpDDSMenu;

int DisplayDebug = 0;
int DispInterface = TRUE;
int	DebugPlayer = FALSE;
int	LastAttackDamage = 0;

int	HoMsgBoxMode = 0;

#define TRADEBUTTONMAIN_X		513
#define TRADEBUTTONMAIN_Y		3

POINT pHoPartyMsgBox = { TRADEBUTTONMAIN_X, TRADEBUTTONMAIN_Y };

char szExitInfo_URL[128] = { 0 };

#define ANX_NONE	-32768

int pX;
int pY;
int pZ;
int	dpX = 0;
int dpY = 0;
int dpZ = 0;

int whAnx = 64;
int anx = 64;
int	ViewAnx = 64;
int	ViewDist = 100;
int	PlayFloor = 0;
int any = 0;
int anz = 0;
int dist = 100;
int CaTarDist = 0;
int	CaMovAngle = 0;

int tX = 0;
int tY = 0;
int tZ = 0;

int	ImeChatLen = 0;
POINT	ImePositon;

int SetMousePlay(int flag);

int WaveCameraMode = TRUE;
POINT3D WaveCameraPosi;
int	WaveCameraFactor = 0;
int	WaveCameraTimeCount = 0;
int	WaveCameraDelay = 0;
BOOL WaveCameraFlag = FALSE;

int GameInit();

int GameClose();

int SetGameMode(int mode);

int	SetChatingLine(char *szMessage);

int GameMode = 0;

CRITICAL_SECTION	cDrawSection;

int HoInstallFont();
int HoUninstallFont();

HINSTANCE hinst;

#define IDC_TEXT	101

BYTE VRKeyBuff[256];

LONG APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);
void PlayD3D();
//ÄÚ¸àµå ¶óÀÎ ºÐ¼® ¼³Á¤
int DecodeCmdLine(char *lpCmdLine);

HFONT	hFont = 0;
int SetCreateFont();

CRITICAL_SECTION	cSection;

HIMC hImc;
HIMC hImcEdit;

extern INT WINAPI ServerWinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, INT nCmdShow);

char	szCmdLine[128];
int msX = 0, msY = 0;
int msXo = 0, msYo = 0;
int xPs;
int yPs;

POINT	pCursorPos;
POINT	pRealCursorPos;

DWORD GameWindowMessage(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
DWORD dwTimerCount = 0;

BOOL DisplayIME = FALSE;

#define CAMERA_MOVE_STEP		(8*fONE)


int RendSightPos = 0;
int RendSightOffset = 0;


#define	FPS_TIME		(1000/70)

DWORD FrameSkipTimer = 0;
int fps = 70;
int FpsTime;

#define AC_MOVE_STEP	4
#define AC_MOVE_MIN		256

int dsCameraRotation = 0;

POINT3D TargetPosi = { 0,0,0 };
POINT3D GeoResult = { 0,0,0 };

int timeg = 0;
int RenderCnt = 0;
int DispRender;
int	DispMainLoop;

DWORD	dwLastPlayTime = 0;
DWORD	dwLastOverSpeedCount = 0;
int		PlayTimerMax = 0;
int		MainLoopCount = 0;
DWORD	dwLastRenderTime = 0;

float xr = 0;

int jcnt = 0;

float brt = 1;
float bs = 0;


int ox = 0, oy = 0, oz = 0;

int GoText = 0;

char strBuff[240];
char strBuff2[256];

int RestoreFlag = 0;

char *szRestore = "Áö±Ý ±×¸² µ¥ÀÌÅ¸¸¦ ¾ÆÁÖ ¿­½ÉÈ÷ ·ÎµåÇÏ±¸ ÀÖ½À´Ï´Ù. ÂÉ±Ý¸¸ ±â´Ù¸®¼¼¿ä !";
int NumPoly;
int Disp_tx, Disp_ty;
int Disp_rx, Disp_ry;

smCHAR	*lpCharMsgSort[OTHER_PLAYER_MAX];
int ChatMsgSortCnt;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,
					DWORD fdwReason,
					LPVOID lpvReserved
					)
{
	hinst = hinstDLL;
	return TRUE;
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, INT nCmdShow)
{
	_stprintf(szCmdLine, lpCmdLine);

	srand(GetCurrentTime());

	smConfigDecode("hotuk.ini");

	InitializeCriticalSection(&cSection);
	InitializeCriticalSection(&cDrawSection);
	InitializeCriticalSection(&cSection_Main);

	return ServerWinMain(hInst, hPreInst, lpCmdLine, nCmdShow);
}

int SetIME_Mode(BOOL mode)
{
	Log::Debug("SetIME_Mode : Requisitado porém foi removido.");
	return NULL;
}

int CheckCode_2Byte(char *Str)
{
	if (Str[0] < 0)
	{
		return 2;
	}
	return 1;
}

LRESULT CALLBACK EditWndProc01(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Log::Debug("EditWndProc01 : Requisitado porém foi removido.");
	return NULL;
}

LONG APIENTRY WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	Log::Debug("WndProc : Requisitado porém foi removido.");
	return NULL;
}

int MoveAngle(int step, int angY)
{
	Log::Debug("MoveAngle : Requisitado porém foi removido.");
	return NULL;
}

int TraceCameraMain()
{
	Log::Debug("TraceCameraMain : Requisitado porém foi removido.");
	return NULL;
}
int SetRendSight()
{
	Log::Debug("SetRendSight : Requisitado porém foi removido.");
	return NULL;
}

int RendSightSub(int flag)
{
	Log::Debug("RendSightSub : Requisitado porém foi removido.");
	return NULL;
}

void PlayMain()
{
	Log::Debug("PlayMain : Requisitado porém foi removido.");
	return;
}

void PlayD3D()
{
	Log::Debug("PlayD3D : Requisitado porém foi removido.");
	return;
}

int GetPlayMouseAngle()
{
	Log::Debug("GetPlayMouseAngle : Requisitado porém foi removido.");
	return NULL;
}

int GetActionGame_PlayMouseAngle()
{
	Log::Debug("GetActionGame_PlayMouseAngle : Requisitado porém foi removido.");
	return NULL;
}

int SetMousePlay(int flag)
{
	Log::Debug("SetMousePlay : Requisitado porém foi removido.");
	return NULL;
}

int _stdcall InitD3D(HWND hWnd)
{
	Log::Debug("InitD3D : Requisitado porém foi removido.");
	return NULL;
}

void _stdcall CloseD3d()
{
	Log::Debug("CloseD3d : Requisitado porém foi removido.");
	return;
}

int GameInit()
{
	Log::Debug("GameInit : Requisitado porém foi removido.");
	return NULL;
}

int GameClose()
{
	Log::Debug("GameClose : Requisitado porém foi removido.");
	return NULL;
}

int DIsplayLogoImage()
{
	Log::Debug("DIsplayLogoImage : Requisitado porém foi removido.");
	return NULL;
}

int SetGameMode(int mode)
{
	Log::Debug("SetGameMode : Requisitado porém foi removido.");
	return NULL;
}

int RestoreAll()
{
	Log::Debug("RestoreAll : Requisitado porém foi removido.");
	return NULL;
}

int RestoreInterfaceTexture()
{
	Log::Debug("RestoreInterfaceTexture : Requisitado porém foi removido.");
	return NULL;
}

void VirtualDrawGameState(void)
{
	Log::Debug("VirtualDrawGameState : Requisitado porém foi removido.");
	return;
}

int DrawGameState()
{
	Log::Debug("DrawGameState : Requisitado porém foi removido.");
	return NULL;
}

void _stdcall smPlayD3D(int x, int y, int z, int ax, int ay, int az)
{
	Log::Debug("smPlayD3D : Requisitado porém foi removido.");
	return;
}

char *CompCmdStr(char *strCmdLine, char *strword)
{
	int len, len2;
	int cnt, cnt2;

	len = lstrlen(strCmdLine);
	len2 = lstrlen(strword);

	for (cnt = 0; cnt < len - len2; cnt++)
	{
		for (cnt2 = 0; cnt2 < len2; cnt2++)
		{
			if (strword[cnt2] != strCmdLine[cnt + cnt2]) break;
		}
		if (cnt2 == len2)
			return &strCmdLine[cnt + cnt2];
	}

	return NULL;
};

int DecodeCmdLine(char *lpCmdLine)
{
	Log::Debug("DecodeCmdLine : Requisitado porém foi removido.");
	return NULL;
}

int SetCreateFont()
{
	Log::Debug("SetCreateFont : Requisitado porém foi removido.");
	return NULL;
}

int	SetChatingLine(char *szMessage)
{
	Log::Debug("SetChatingLine : Requisitado porém foi removido.");
	return NULL;
}

DWORD GameWindowMessage(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	Log::Debug("GameWindowMessage : Requisitado porém foi removido.");
	return NULL;
}

int	StartQuest_Code(DWORD wCode)
{
	SetQuestBoard();

	if (InterfaceParty.PartyPosState == PARTY_NONE)
	{
		ShowQuest();
		InterfaceParty.quest_Sel_Progress();
	}

	return TRUE;
}

int EndQuest_Code(DWORD wCode)
{
	Record_LastQuest((WORD)wCode);
	SetQuestBoard();

	return TRUE;
}

int HoInstallFont()
{
	Log::Debug("HoInstallFont : Requisitado porém foi removido.");
	return NULL;
}

int HoUninstallFont()
{
	Log::Debug("HoUninstallFont : Requisitado porém foi removido.");
	return NULL;
}
