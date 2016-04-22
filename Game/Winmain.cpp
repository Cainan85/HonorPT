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

#ifdef PARKMENU
#include "TJBOY\\\\Park\\MICVOL\\MicVolume.h"
#endif

#include "WinInt\\WinIntThread.h"
#include "FullZoomMap.h"
#include "FontImage.h"

HWND hwnd;
HWND hTextWnd;
HWND hFocusWnd;

extern BOOL bMic;
extern int vrunRuning;
extern int micInit;
extern int sinChatDisplayMode; //{ 0- ¾Æ¹«°Íµµ ¾È±×¸®±â  1-º¸Åë  2-»óÁ¡ }
extern int sinChatInputMode;   //{ 0- ¾Æ¹«°Íµµ ¾È±×¸®±â  1-º¸Åë  2-»óÁ¡ }

HDC hdc;
int quit = 0;
int QuitSave = 0;
int WinSizeX = 640;
int WinSizeY = 480;
int	WinColBit = 16;

//######################################################################################
//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
float g_fWinSizeRatio_X;
float g_fWinSizeRatio_Y;
//######################################################################################


int TextMode;

int BrCtrl = 0;

DWORD	dwM_BlurTime = 0;


int MouseButton[3] = { 0,0,0 };
extern int TJwheel; //ktj
extern int keydownEnt;


//IME °ü¸® ¶óÀÌºê·¯¸® Ãß°¡
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
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved   // reserved
	);
///////////////////////// IME °ü·Ã //////////////////////////////

#include "ime.h"

ImeClass IMETest;
CStrClass sz;
LRESULT CALLBACK EditWndProc01(HWND, UINT, WPARAM, LPARAM);
WNDPROC		OldEditProc01;
char szIME_Buff[10][64];


//µð¹ö±× ÇÃ·¢ ¹é¾÷
DWORD	dwDebugBack;
DWORD	dwDebugXor;

///////////////////////// °ÔÀÓ¿¡ ÇÊ¿äÇÑ ¼±¾ðµé ///////////////////
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
int	CameraInvRot = 0;					//Ä«¸Þ¶ó È¸Àü ¹æÇâ
int	CameraSight = 0;					//Ä«¸Þ¶ó ½Ã¾ß Á¶Àý

//smPAT3D *pat3d;
//LPDIRECT3DTEXTURE2 hOBJ3DTexture;
LPDIRECTDRAWSURFACE4 lpDDSMenu;

//ccsSELECT	*lpPlaySelect = 0;

int DisplayDebug = 0;
int DispInterface = TRUE;			//ÀÎÅÍÆäÀÌ½º ±×¸®±â
int	DebugPlayer = FALSE;				//µð¹ö±ë Ä«¸Þ¶ó ÇÃ·¹ÀÌ¾î
int	LastAttackDamage = 0;			//¸¶Áö¸· °ø°Ý µ¥¹ÌÁö

int	HoMsgBoxMode = 0;				//È£µ¿ ¸Þ¼¼Áö ¹Ú½º ÇÁ·¹ÀÓ Ç¥½Ã ºñÇ¥½Ã Åä±Û

//sinTrade.cpp ¿¡ µðÆÄÀÎ µÇ ÀÖµµ´Ù.
#define TRADEBUTTONMAIN_X		513
#define TRADEBUTTONMAIN_Y		3

POINT pHoPartyMsgBox = { TRADEBUTTONMAIN_X, TRADEBUTTONMAIN_Y };	//È£µ¿ ÆÄÆ¼½ÅÃ» ¸Þ¼¼Áö ¹Ú½º

char szExitInfo_URL[128] = { 0, };		//°ÔÀÓ Á¾·á½Ã Ç¥½ÃÇÏ´Â URL °æ·Î


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


int	ImeChatLen = 0;		//IME °ü·Ã Ã¤ÆÃ ¹®ÀÚ±æÀÌ
POINT	ImePositon;		//IME À§Ä¡ ÀúÀå

int SetMousePlay(int flag);

//È£µ¿µ¿ Ãß°¡(Ä«¸Þ¶ó ÀÌÆåÆ®)
int WaveCameraMode = TRUE;
POINT3D WaveCameraPosi;
int	WaveCameraFactor = 0; // 10*fONE;		    //Èçµå´Â Å©±â
int	WaveCameraTimeCount = 0;
int	WaveCameraDelay = 0;
BOOL	WaveCameraFlag = FALSE;



//°ÔÀÓ ÃÊ±âÈ­
int GameInit();
//°ÔÀÓ ´Ý±â
int GameClose();
//°ÔÀÓ ¼±ÅÃ
int SetGameMode(int mode);

//Ã¤ÆÃÃ¢ ¹®ÀÚ ¼³Á¤¹× È°¼ºÈ­
int	SetChatingLine(char *szMessage);

int GameMode = 0;

//±×¸®±â Å©¸®Æ¼ÄÃ ¼½¼Ç
CRITICAL_SECTION	cDrawSection;

//ÇØ¿Ü¿ë ÇÔ¼ö
int HoInstallFont();	//ÆùÆ® µî·ÏÇÏ±â
int HoUninstallFont();	//ÆùÆ® »èÁ¦ÇÏ±â

HINSTANCE hinst;
BOOL WINAPI DllMain(HINSTANCE hinstDLL,  // handle to DLL module
					DWORD fdwReason,     // reason for calling function
					LPVOID lpvReserved   // reserved
					)
{

	hinst = hinstDLL;

	return TRUE;

}

#define IDC_TEXT	101

BYTE VRKeyBuff[256];


LONG APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);
void PlayD3D();
//ÄÚ¸àµå ¶óÀÎ ºÐ¼® ¼³Á¤
int DecodeCmdLine(char *lpCmdLine);

//ÆùÆ® »ý¼º
HFONT	hFont = 0;
int SetCreateFont();

//Å©¸®Æ¼ÄÃ ¼½¼Ç ¼±¾ð
CRITICAL_SECTION	cSection;

//IME Context
HIMC hImc;
HIMC hImcEdit;

extern rsRECORD_DBASE	rsRecorder;					//¼­¹ö¿¡ °ÔÀÓµ¥ÀÌÅ¸ ±â·ÏÀåÄ¡
extern INT WINAPI ServerWinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, INT nCmdShow);

//int AllUserDataSearch();
char	szCmdLine[128];			//Ä¿¸Çµå¶óÀÎ ¹®ÀÚ¿­


//Å¬·ÎÁî º£Å¸ Å×½ºÅÍ ÃÊ±âÈ­
extern int	InitCloseBetaUser();
//¼­¹ö¿¡ ±â·ÏµÈ ÀüÃ¼ µ¥ÀÌÅ¸¸¦ È®ÀÎÇÏ¿© ÀÇ½É°¡´Â À¯Àú¸¦ Ã£´Â´Ù
extern int	CheckServerRecordData();

//Ä³¸¯ÅÍ Á¤º¸ ÆÄÀÏ¿¡¼­ ÇØµ¶ÇÏ¿© ¼³Á¤ÇÑ´Ù
extern int RestoreBackupData(char *szListFile, char *BackupPath);

#ifdef	_LANGUAGE_JAPANESE

static char *StripArg0(LPSTR cmdline)
{
	char *ptr;

	ptr = cmdline;
	while (*ptr && *ptr == ' ' || *ptr == '\t')
		ptr++;

	if (*ptr == '"')
	{
		ptr++;
		while (*ptr && *ptr != '"')
			ptr++;
		if (*ptr == '"')
		{
			ptr++;
			while (*ptr && *ptr == ' ')
				ptr++;
			return ptr;
		}
	}
	else
	{
		while (*ptr && *ptr != ' ')
			ptr++;
		if (*ptr == ' ')
			return ptr + 1;
	}

	return ("");
}
#endif

/*
#include "srcserver\dllserver.h"

typedef int (*LPFN_LogITEMRecord)( char *lpszPID, LogITEM *LogItem );
typedef int (*LPFN_LogITEMTradeRecord)( char *lpszPID, LogITEM_TRADE *LogItem );
typedef int (*LPFN_LogITEMPostRecord)(char *lpszPID, LogGiftITEM *GiftItem);
*/

#ifdef _W_SERVER

#include "mini_dump.h"
Mini_Dump CMiniDump;

#endif


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, INT nCmdShow)
{
	_stprintf(szCmdLine, lpCmdLine);

	MSG msg = { 0 };
	WNDCLASS wndclass = { 0 };
	hinst = hInst;
	int	sx = 0, sy = 0;

	initSinCos();
	srand(GetCurrentTime());

	smConfigDecode();
	ReadShotcutMessage("ShortCut.ini");
	DecodeCmdLine(lpCmdLine);

	InitializeCriticalSection(&cSection);
	InitializeCriticalSection(&cDrawSection);
	InitializeCriticalSection(&cSection_Main);

	InitDirectSound();

	WinSizeX = smConfig.ScreenSize.x;
	WinSizeY = smConfig.ScreenSize.y;
	WinColBit = smConfig.ScreenColorBit;
	WindowMode = smConfig.WinMode;

	sx = WinSizeX;
	sy = WinSizeY;

	if (!hPreInst)
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DEFAULT_ICON));
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szAppName;
		RegisterClass(&wndclass);
	}

	if (WindowMode)
	{
		hwnd = CreateWindowA(szAppName, szAppName,
							 WS_OVERLAPPEDWINDOW | WS_POPUP | WS_EX_TOPMOST, CW_USEDEFAULT,
							 CW_USEDEFAULT, sx, sy, NULL, NULL,
							 hInst, NULL);
	}
	else
	{
		hwnd = CreateWindowA(szAppName, szAppName,
							 WS_VISIBLE | WS_POPUP | WS_EX_TOPMOST, CW_USEDEFAULT,
							 CW_USEDEFAULT, sx, sy, NULL, NULL,
							 hInst, NULL);
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	ShowCursor(FALSE);

	if (InitD3D(hwnd) == NULL)
		return FALSE;

	InitLoadingLamp(hwnd);

	hTextWnd = CreateWindow("EDIT", "",
							WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_WANTRETURN,
							10, 400, 500, 40,
							hwnd, (HMENU)IDC_TEXT,
							hInst, NULL);

	OldEditProc01 = (WNDPROC)SetWindowLong(hTextWnd, GWL_WNDPROC, (LONG)EditWndProc01);
	IMETest.SetHWND(hTextWnd);

	hImc = ImmGetContext(hwnd);
	hImcEdit = ImmGetContext(hTextWnd);

	SetIME_Mode(0);

	SetCreateFont();

	TextMode = 0;

	InitGameSocket();

	SetGameMode(1);
	CharacterName1[0] = 0;
	CharacterName2[0] = 0;

	SetTimer(hwnd, 0, GAME_WORLDTIME_MIN / 4, NULL);

	{
		int CurseID = LoadCurses("Image\\brazil_curse.cht");
		if (CurseID != -2194)
		{
			Log::Error("Curse inválido. Esperado: " + std::to_string(CurseID), TRUE);
		}
	}

	{
		int CurseID = LoadCursesID("Image\\brazil_curse.cht");
		if (CurseID != -2194)
		{
			Log::Error("CurseID inválido. Esperado: " + std::to_string(CurseID), TRUE);
		}
	}

	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);

			if (msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP)
			{
				if (msg.wParam != 0xF4 && msg.wParam != VK_F10)
					continue;
			}

			DispatchMessage(&msg);
		}
		else
		{
			EnterCriticalSection(&cSection_Main);

			PlayD3D();
			PlayRecvMessageQue();

			LeaveCriticalSection(&cSection_Main);
		}

		if (quit != 0 && !dwTradeMaskTime)
		{
			if (GameMode != 2) break;

			if (smWsockServer && !QuitSave)
			{
				if (cTrade.OpenFlag)
				{

					SendRequestTrade(cTrade.TradeCharCode, 3);
					cTrade.CancelTradeItem();
				}

				if (cWareHouse.OpenFlag)
				{

					cWareHouse.RestoreInvenItem();

				}
				cInvenTory.ResetMouseItem();
				SaveGameData();
				QuitSave = TRUE;
			}
			else
			{
				if (!smWsockServer || GetSaveResult() == TRUE) break;
				if ((dwPlayTime - rsRecorder.dwRecordTime) > 15000) break;
			}
		}
	}

	SetGameMode(0);
	DeleteObject(hFont);
	CloseLoadingLamp();
	CloseBindSock();
	CloseD3d();

	if (fpNetLog)
		fclose(fpNetLog);

	if (szExitInfo_URL[0])
		ShellExecute(NULL, NULL, szExitInfo_URL, NULL, NULL, NULL);

	return msg.wParam;
}

int msX = 0, msY = 0;
int msXo = 0, msYo = 0;
int xPs;
int yPs;

POINT	pCursorPos;
POINT	pRealCursorPos;
//°ÔÀÓ¿¡¼­ÀÇ À©µµ ¸Þ¼¼Áö Ã³¸®
DWORD GameWindowMessage(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
DWORD dwTimerCount = 0;

/*
BOOL cbutton=true;


BOOL FoundKeyboard(HWND hWnd,int mode)
{
	LPTSTR check;
	//char szDesc[128];
	//char szKLID[128];
	int kk=0;
	HWND	hOldWnd;

	//UINT uiRet;
	HKL hkl;

	hOldWnd = GetFocus();
	if ( hWnd && hOldWnd!=hWnd ) SetFocus(hWnd);

	//Ã£°í ½ÍÀº ¾ð¾î ½ºÅ©¸³Æ® Ãß°¡...
	//if(mode==0) check = ""; ¿µ¹®
	if(mode==1) check = "ÇÑ±Û";
	else if(mode==2) check = "Japanese";
	else if(mode==3) check = "ìýÛö";

	//memset(szKLID,0x00,128);
	//memset(szDesc,0x00,128);
	//GetKeyboardLayoutName(szKLID);
	//hkl = LoadKeyboardLayout(szKLID,KLF_ACTIVATE);
	//ImmGetDescription(hkl,szDesc,256);

	//if((mode!=0)&&(strstr(szDesc,check)!=0)) {
	//	if ( hWnd!=hOldWnd ) SetFocus(hOldWnd);
	//	return 1;
	//}
	//else if(mode==0&&szDesc[0]==0&&szDesc[0]==0&&szDesc[0]==0&&szDesc[0]==0) {
	//	if ( hWnd!=hOldWnd ) SetFocus(hOldWnd);
	//	return 1;
	//}
	hkl = ActivateKeyboardLayout((HKL)HKL_PREV,KLF_REORDER);
	hkl = ActivateKeyboardLayout((HKL)HKL_NEXT,KLF_REORDER);
	if ( hWnd!=hOldWnd ) SetFocus(hOldWnd);
	return 0;
}

//Áß¹® ¸ðµå¿¡¼­ ¿µ¹®/Áß¹® ÀÔ·Â ¼±ÅÃ
void z_SetImeMode(HWND hWnd,BOOL mode)
{

#ifdef _LANGUAGE_CHINESE
	HIMC  hIMC;
	DWORD dwConv, dwSent;
	DWORD dwTemp;

	hIMC = ImmGetContext(hWnd);
	ImmGetConversionStatus(hIMC,&dwConv,&dwSent);

	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	if(!mode) dwTemp |= (IME_CMODE_NATIVE | IME_CMODE_KATAKANA);
	dwConv = dwTemp;
	ImmSetConversionStatus(hIMC,dwConv,dwSent);

	//ÇöÀç ÀÔ·Â ¸ðµå°¡ ¹«¾ùÀÎ°¡
	//if((IME_CMODE_LANGUAGE&dwConv) == IME_CMODE_ALPHANUMERIC) SetDlgItemText(z_hWnd,ID_BUTTON1,"¿µ¹®");
	//if((IME_CMODE_LANGUAGE&dwConv) == (IME_CMODE_NATIVE | IME_CMODE_KATAKANA)) SetDlgItemText(z_hWnd,ID_BUTTON1,"ÇÑ±ÛÁß¹®");
	//EnableWindow(GetDlgItem(z_hWnd,ID_BUTTON2),FALSE);  //¿µ¹®¸ðµå
	ImmReleaseContext(hWnd,hIMC);

#endif

}

//IME »óÅÂÃ¢ À§Ä¡
void z_SetImeStatusPos(HWND hWnd,int x,int y)
{
	HIMC  hIMC;
	hIMC = ImmGetContext(hWnd);
	POINT pos;
	pos.x = x;
	pos.y = y;
	ImmSetStatusWindowPos(hIMC,&pos);
}
//IME »óÈ²¿¡¼­ ÀÔ·ÂÃ¢ÀÇ À§Ä¡
void z_SetImeStatusPos(HWND hWnd,int mode,int x,int y,int top,int left,int botton,int right)
{
	HIMC  hIMC;
	COMPOSITIONFORM compos;
	RECT  rt;
	POINT pos;

	if(mode<=1) compos.dwStyle = CFS_DEFAULT;
	else if(mode==2) compos.dwStyle = CFS_FORCE_POSITION;
	else if(mode==3) compos.dwStyle = CFS_POINT;
	else if(mode>=4) compos.dwStyle = CFS_RECT;

	pos.x = x;
	pos.y = y;

	rt.top = top;
	rt.left = left;
	rt.bottom = botton;
	rt.right = right;

	compos.ptCurrentPos = pos;
	compos.rcArea = rt;

	hIMC = ImmGetContext(hWnd);
	ImmSetCompositionWindow(hIMC,&compos);
}
//IME»óÈ²¿¡¼­ ³»¿ëÃ¢ÀÇ À§Ä¡
void z_SetImeCandidatePos(HWND hWnd,DWORD page,BOOL mode,int x,int y,int top,int left,int botton,int right)
{
	HIMC  hIMC;
	CANDIDATEFORM candidat;
	RECT  rt;
	POINT pos;

	candidat.dwIndex = page;

	if(mode) candidat.dwStyle = CFS_CANDIDATEPOS;
	else candidat.dwStyle = CFS_EXCLUDE;

	pos.x = x;
	pos.y = y;

	rt.bottom = botton;
	rt.left = left;
	rt.right = right;
	rt.top = top;

	candidat.ptCurrentPos = pos;
	candidat.rcArea = rt;

	hIMC = ImmGetContext(hWnd);
	ImmSetCandidateWindow(hIMC,&candidat);
}
*/

//IME ¸ðµå ÀüÈ¯
BOOL DisplayIME = FALSE;
int SetIME_Mode(BOOL mode)
{
	if (mode)
	{
		DisplayIME = TRUE;
		ImmSetOpenStatus(hImc, TRUE);
#ifdef	_LANGUAGE_JAPANESE
		/*

				DWORD	conv , sent;
				//ÀÏº»ÆÇ
				ImmGetConversionStatus( hImc , &conv , &sent );
				ImmSetConversionStatus( hImc , IME_CMODE_NATIVE|IME_CMODE_FULLSHAPE,sent );

				ImmGetConversionStatus( hImcEdit , &conv , &sent );
				ImmSetConversionStatus( hImcEdit , IME_CMODE_NATIVE|IME_CMODE_FULLSHAPE,sent );
		*/
		//ÀÏº»ÆÇ
		ImmSetConversionStatus(hImc, IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE, IME_SMODE_PHRASEPREDICT);
		ImmSetConversionStatus(hImcEdit, IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE, IME_SMODE_PHRASEPREDICT);


#else
		//ÇÑ±¹ÆÇ
		ImmSetConversionStatus(hImc, IME_CMODE_NATIVE, IME_CMODE_NATIVE);
		ImmSetConversionStatus(hImcEdit, IME_CMODE_NATIVE, IME_CMODE_NATIVE);
#endif

	}
	else
	{
		ImmSetConversionStatus(hImc, IME_CMODE_ALPHANUMERIC, IME_CMODE_ALPHANUMERIC);
		ImmSetConversionStatus(hImcEdit, IME_CMODE_ALPHANUMERIC, IME_CMODE_ALPHANUMERIC);

		ImmSetOpenStatus(hImc, FALSE);
		DisplayIME = FALSE;
	}


	return TRUE;
}

//2¹ÙÀÌÆ® ÄÚµå È®ÀÎ
int CheckCode_2Byte(char *Str)
{
#ifdef _LANGUAGE_ENGLISH
	//¿µ¹®ÆÇ - ¹«Á¶°Ç 1

	return 1;
#endif

#ifdef _LANGUAGE_THAI
	if (CheckTHAI_ptr(Str, 0) == 1) return 1;
	else if (CheckTHAI_ptr(Str, 0) == 2) return 2;
	return 1;
#endif

#ifdef _LANGUAGE_JAPANESE

	//ÀÏº»ÄÚµå È®ÀÎ

	if (CheckJTS_ptr(Str, 0) == 2)
	{
		return 2;
	}
	/*

		BYTE	ch;

		ch = (BYTE)Str[0];

		if ( ch>=0x81 && ch<=0x9F ) return 2;
		if ( ch>=0xE0 && ch<=0xEF ) return 2;
	*/
#else
	//ÇÑ±¹ Áß±¹ ÄÚµå
	if (Str[0] < 0)
	{
		return 2;
	}

#endif

	return 1;
}



LRESULT CALLBACK EditWndProc01(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId = LOWORD(wParam);
	wmEvent = HIWORD(wParam);

	switch (message)
	{
		case WM_KEYDOWN:
			//SetWindowText( hWnd , "" );

#ifdef	_IME_ACTIVE
			IMETest.GetInfoPerTime();
#endif

			/*
						for(int cnt=0;cnt<10;cnt++) {
							lstrcpy( szIME_Buff[cnt] , IMETest.m_szCandList[cnt].GetBuffer( IMETest.m_szCandList[cnt].GetLength() ) );
						}
			*/
			break;
	}
	return CallWindowProc(OldEditProc01, hWnd, message, wParam, lParam);
}



//////////////////////// [ ÀÏº»¾î IME Ã³¸® ÇÁ·Î½ÃÀú ] ///////////////////////////////
#ifdef	_LANGUAGE_JAPANESE

char g_bufEdit[256];
char g_bufFixed[256];

int WndProc_Japanese(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HIMC	imc;
	static DWORD conv = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN;
	static DWORD sent = IME_SMODE_PHRASEPREDICT;
	static BOOL setFont = FALSE;

	switch (messg)
	{

		case WM_IME_COMPOSITION:

			if ((imc = ImmGetContext(hWnd)))
			{
				if (lParam & GCS_RESULTSTR)
				{
					ZeroMemory(g_bufFixed, 256);
					ImmGetCompositionString(imc, GCS_RESULTSTR, g_bufFixed, 256);
				}
				else if (lParam & GCS_COMPSTR)
				{
					ZeroMemory(g_bufEdit, 256);
					ImmGetCompositionString(imc, GCS_COMPSTR, g_bufEdit, 256);
				}
				ImmReleaseContext(hWnd, imc);
			}
			break;

		case WM_IME_NOTIFY:
			switch (wParam)
			{
				case IMN_SETCONVERSIONMODE:
				case IMN_SETSENTENCEMODE:
					if ((imc = ImmGetContext(hWnd)))
					{
						ImmGetConversionStatus(imc, &conv, &sent);
						ImmReleaseContext(hWnd, imc);
					}
					break;

				case IMN_SETOPENSTATUS:
					if ((imc = ImmGetContext(hWnd)))
					{
						if (ImmGetOpenStatus(imc))
						{
							ImmSetConversionStatus(imc, conv, sent);
						}
						else
						{
							ImmGetConversionStatus(imc, &conv, &sent);
						}
						ImmReleaseContext(hWnd, imc);
					}

					if (!setFont && (imc = ImmGetContext(hWnd)))
					{
						LOGFONT lf;
						ZeroMemory(&lf, sizeof(LOGFONT));
						lf.lfHeight = 12;//-MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
						lf.lfItalic = FALSE;
						lf.lfUnderline = FALSE;
						lf.lfStrikeOut = FALSE;
						lf.lfCharSet = OEM_CHARSET;//SHIFTJIS_CHARSET;
						lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
						lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
						lf.lfQuality = DEFAULT_QUALITY;
						lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
						strcpy_s(lf.lfFaceName, _T("‚l‚r ‚oƒSƒVƒbƒN"));
						ImmSetCompositionFont(imc, &lf);
						ImmReleaseContext(hWnd, imc);
						setFont = TRUE;
					}
					break;

				case IMN_OPENCANDIDATE:
				case IMN_CHANGECANDIDATE:
					if ((imc = ImmGetContext(hWnd)))
					{
						CANDIDATEFORM cf;
						ZeroMemory(&cf, sizeof(CANDIDATEFORM));
						cf.dwStyle = CFS_CANDIDATEPOS;
						cf.ptCurrentPos.x = 100000;//GetSystemMetrics(SM_CXSCREEN);
						cf.ptCurrentPos.y = 100000;//GetSystemMetrics(SM_CYSCREEN);
						ImmSetCandidateWindow(imc, &cf);
						ImmReleaseContext(hWnd, imc);
					}
					break;
			}
			break;
	}

	return TRUE;
}
#endif

//ktj 
extern void clan_Mouse(int msg, WPARAM wParam);


LONG APIENTRY WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{

	//	PAINTSTRUCT ps;
	//	HDC	hdc;
	short zDelta;
	char strBuff[256];
	char strBuff2[256];
	char *lpString;
	int	len, CrCode;
	/*
	#ifdef	_LANGUAGE_JAPANESE
		//if ( messg==WM_IME_COMPOSITION || messg==WM_IME_NOTIFY ) {
			WndProc_Japanese( hWnd, messg, wParam, lParam);		//ÀÏº»¾î Ã³¸® ÇÁ·Î½ÃÀú
		//}
	#endif
	*/

	switch (messg)
	{
		//¹ÚÃ¶È£ : voiceÃ¤ÆÃ¿ë ==================================================
		case WM_CALLMEMMAP:    //vrun.dat ¸Þ¸ð¸®¸Ê¿¡ ¸Þ¼¼Áö
			void PacketParsing();	//process.cpp
			PacketParsing();
			break;

		case WM_CREATE:
			break;

		case WM_CLOSE:
			//°ÔÀÓ Á¾·á
			QuitGame();
			break;

		case WSA_ACCEPT:
			WSAMessage_Accept(wParam, lParam);
			break;

		case WSA_READ:
			WSAMessage_Read(wParam, lParam);
			break;

		case SWM_RECVSUCCESS:
			//				SendMessage( hwnd , SWM_RECVSUCCESS , (WPARAM)SockInfo , 0 );
			//			SockInfo = (smTHREADSOCK *)wParam;
			RecvPlayData((smTHREADSOCK *)wParam);
			break;

		case SWM_MCONNECT:
			//¸Þ¼¼Áö ¾²·¹µå¿¡¼­ ´ë½Å ¿¬°á
			ConnectOtherPlayer(wParam);
			break;

		case SWM_MDISCONNECT:
			//¸Þ¼¼Áö ¾²·¹µå¿¡¼­ ´ë½Å ²öÀ½
			((smWINSOCK *)wParam)->CloseSocket();
			break;

			//case WM_IME_KEYDOWN:
		case WM_KEYDOWN:

			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn > 0)
			{
				if (KeyFullZoomMap(wParam))
				{
					VRKeyBuff[wParam] = 1;
					break;
				}
				else
				{
					SetFullZoomMap(0);
					pCursorPos.x = g_iFzmCursorPosX;
					pCursorPos.y = g_iFzmCursorPosY;

					if (wParam == VK_ESCAPE)
						break;
				}
			}
			//######################################################################################



			//////////////¹ÚÃ¶È£
#ifdef PARKMENU
			/////////Æã¼ÇÅ°  Key Down
			//if ( GameMode==2 )
			if (wParam == 0x47)
			{
				if (vrunRuning == TRUE)
				{
					if (bMic == FALSE)
					{
						if (micInit == 0)
						{
							micInit = cmicvol.Init();
						}
						bMic = TRUE;
						void ParkSetMicOnOFF();
						ParkSetMicOnOFF();
					}
				}
			}
			//}
#endif
			//XTRAP HEAP MEMORY TEST
//			if(wParam == 0x6c || wParam == 0x4b)
//			{
//				sinChar->Life[0] -= 100;
//				ReformCharForm();
//			}
//			if(wParam == 0x70 || wParam == 0x50)
//			{
//				sinChar->Stamina[0] -= 100;
//				ReformCharForm();
//			}


			//ktj
			if (wParam == VK_RETURN)keydownEnt = 1;

			if (wParam == VK_CONTROL) VRKeyBuff[wParam] = 1;

			if (!hFocusWnd)
			{
				//######################################################################################
				//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
				//if ( VRKeyBuff[VK_CONTROL] && !sinMessageBoxShowFlag && VRKeyBuff[wParam]==0 ) {
				if (!VRKeyBuff[VK_SHIFT] && VRKeyBuff[VK_CONTROL] && !sinMessageBoxShowFlag && VRKeyBuff[wParam] == 0)
				{
					//######################################################################################
					if (wParam == VK_F1 || wParam == VK_F2 || wParam == VK_F3 || wParam == VK_F4 || wParam == VK_F5 ||
						wParam == VK_F6 || wParam == VK_F7 || wParam == VK_F8 || wParam == VK_F9 || wParam == VK_F10)
					{

						SetChatingLine("");

					}
				}
			}

			if (hFocusWnd)
			{
				lpString = 0;
				CrCode = 0;

				//######################################################################################
				//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
				//if ( VRKeyBuff[VK_CONTROL] && !sinMessageBoxShowFlag ) {
				if (!VRKeyBuff[VK_SHIFT] && VRKeyBuff[VK_CONTROL] && !sinMessageBoxShowFlag)
				{
					//######################################################################################
					if (wParam == VK_F1  && szShotCutMessage[1][0]) lpString = szShotCutMessage[1];
					if (wParam == VK_F2  && szShotCutMessage[2][0]) lpString = szShotCutMessage[2];
					if (wParam == VK_F3  && szShotCutMessage[3][0]) lpString = szShotCutMessage[3];
					if (wParam == VK_F4  && szShotCutMessage[4][0]) lpString = szShotCutMessage[4];
					if (wParam == VK_F5  && szShotCutMessage[5][0]) lpString = szShotCutMessage[5];
					if (wParam == VK_F6  && szShotCutMessage[6][0]) lpString = szShotCutMessage[6];
					if (wParam == VK_F7  && szShotCutMessage[7][0]) lpString = szShotCutMessage[7];
					if (wParam == VK_F8  && szShotCutMessage[8][0]) lpString = szShotCutMessage[8];
					if (wParam == VK_F9  && szShotCutMessage[9][0]) lpString = szShotCutMessage[9];
					if (wParam == VK_F10 && szShotCutMessage[0][0]) lpString = szShotCutMessage[0];

					if (lpString)
					{
#ifdef	_LANGUAGE_VEITNAM
						GetWindowText(hFocusWnd, strBuff2, 80);
#else
						GetWindowText(hFocusWnd, strBuff2, 90);
#endif
						len = lstrlen(strBuff2) + lstrlen(lpString);
						if (len < 80)
						{
							lstrcat(strBuff2, lpString);
							if (strBuff2[len - 1] == 0x0D)
							{
								len--;
								strBuff2[len] = 0;
								CrCode = TRUE;
							}
							SetWindowText(hFocusWnd, strBuff2);
							SendMessage(hFocusWnd, EM_SETSEL, len, len);
						}
					}
				}
				if (!sinMessageBoxShowFlag &&
					((wParam == VK_RETURN && VRKeyBuff[wParam] == 0 && !VRKeyBuff[VK_CONTROL]) || CrCode == TRUE))
				{

#ifdef	_LANGUAGE_VEITNAM
					GetWindowText(hFocusWnd, strBuff2, 80);
#else
					GetWindowText(hFocusWnd, strBuff2, 90);
#endif
					cHelpPet.PetOnOff(strBuff2);   //<==== ¿ä°ÅÃß°¡ÇØÁÖ¼¼¿ä

					if (strBuff2[0])
					{
						if (strBuff2[0] == '/' || (smConfig.DebugMode && strBuff2[0] == '~') || strBuff2[0] == '`' || strBuff2[0] == '@')
							wsprintf(strBuff, "%s", strBuff2);
						else
						{
							switch (DispChatMode)
							{
								case 0:		//ÀÏ¹Ý
								case 4:		//±Ó¸»
#ifdef	_LANGUAGE_VEITNAM
									wsprintf(strBuff, "%s: %s", PlayerName, strBuff2);
#else
									wsprintf(strBuff, "%s: %s", PlayerName, strBuff2);
#endif
									break;
								case 1:		//Å¬·£
									wsprintf(strBuff, "/CLAN> %s", strBuff2);
									break;
								case 2:		//µ¿·á
									wsprintf(strBuff, "@%s", strBuff2);
									break;
								case 3:		//°Å·¡
									wsprintf(strBuff, "/TRADE> %s", strBuff2);
									break;
							}
						}
						/*
												//######################################################################################
												//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
												if( smConfig.DebugMode )
												{
													int   Alpha;
													char  szBuff[64] = {0,};
													char *pos = GetWord( szBuff, strBuff );

													if( smRender.m_FogIsRend )
													{
														if( _stricmp( szBuff, "/Æ÷±×¸ðµå") == 0 )
														{
															pos = GetWord( szBuff, pos );
															smRender.m_FogMode = atoi( szBuff );
														}
														else if( smRender.m_FogMode )
														{
															if( _stricmp( szBuff, "/Æ÷±×½ÃÀÛ") == 0 )
															{
																pos = GetWord( szBuff, pos );
																smRender.m_fFogStNum = (float)atof( szBuff );
																lpD3DDevice->SetRenderState( D3DRENDERSTATE_FOGTABLESTART, *((DWORD*)(&smRender.m_fFogStNum)) );
															}
															else if( _stricmp( szBuff, "/Æ÷±×³¡") == 0 )
															{
																pos = GetWord( szBuff, pos );
																smRender.m_fFogEndNum = (float)atof( szBuff );
																lpD3DDevice->SetRenderState( D3DRENDERSTATE_FOGTABLEEND, *((DWORD*)(&smRender.m_fFogEndNum)) );
															}
															else if( _stricmp( szBuff, "/Æ÷±×Ä®¶ó") == 0 )
															{
																int r,g,b,a;
																pos = GetWord( szBuff, pos );
																r = atoi( szBuff );
																pos = GetWord( szBuff, pos );
																g = atoi( szBuff );
																pos = GetWord( szBuff, pos );
																b = atoi( szBuff );
																pos = GetWord( szBuff, pos );
																a = atoi( szBuff );

																smRender.m_dwFogColor = RGBA_MAKE(r,g,b,a);
																lpD3DDevice->SetRenderState( D3DRENDERSTATE_FOGCOLOR, smRender.m_dwFogColor );
															}
														}
													}

													if( IsCreateNewRenderTarget() )
													{
														if( _stricmp( szBuff, "/Á¤»ó¸ðµå") == 0 )
															SetFilterEffect( FILTER_EFFECT_NULL, 0 );
														else if( _stricmp( szBuff, "/¸ð¼Çºí·¯") == 0 )
														{
															pos = GetWord( szBuff, pos );
															Alpha = atoi(szBuff);
															if( Alpha < 80 || Alpha > 200 )		Alpha = 80;
														}
														else if( _stricmp( szBuff, "/»Ç»þ½Ã") == 0 )
														{
															pos = GetWord( szBuff, pos );
															Alpha = atoi(szBuff);
															if( Alpha < 160 || Alpha > 200 )	Alpha = 180;
															SetFilterEffect( FILTER_EFFECT_SET_BRIGHT_CONTRAST, Alpha );
														}
													}
												}
												//######################################################################################
						*/

						if (GameMode == 2)
						{
							if (!usehFocus)
								SendChatMessageToServer(strBuff);	//int chatlistSPEAKERflag();

							if (chatlistSPEAKERflag() && lstrlen(strBuff) > LastWhisperLen)
							{
								SetClanChatting();				//Å¬·£ Ã¤ÆÃ ¸ðµå
								if (szLastWhisperName[0])
								{

									szLastWhisperName[0] = 0; //±Ó¸» ÃÊ±âÈ­
								}
							}
							else
								if (szLastWhisperName[0] && lstrlen(strBuff) > LastWhisperLen)
								{
									//ÆÄÆ¼¿ø ±Ó¸» º¸³»±â
									WhisperPartyPlayer(szLastWhisperName);
								}
								else
								{
									SetWindowText(hFocusWnd, "");
									szLastWhisperName[0] = 0;
									LastWhisperLen = 0;
									InterfaceParty.chat_WhisperPartyPlayer_close();
									chatlistSPEAKERflagChg(0);		//Å¬·£ Ã¤ÆÃ Á¾·á
								}
						}
						else
						{
							hFocusWnd = 0;
						}
					}
					else
					{
						hFocusWnd = 0;
						if (GameMode == 2)
						{
							szLastWhisperName[0] = 0;
							LastWhisperLen = 0;
							InterfaceParty.chat_WhisperPartyPlayer_close();
							chatlistSPEAKERflagChg(0);		//Å¬·£ Ã¤ÆÃ Á¾·á
						}
					}
				}

				if (GameMode == 2)
				{
					if (wParam == VK_TAB && VRKeyBuff[wParam] == 0)
					{
						IsClanChatting();
						break;
					}

					if (wParam == VK_UP && VRKeyBuff[wParam] == 0 && !LastWhisperLen)
					{
						//Ã¤ÆÃ ÀúÀå ÄÚ¸Çµå º¹±¸ 
						RecallSavedChatCommand(hFocusWnd, 1);
						//VRKeyBuff[ wParam ] = 1;
						break;
					}
					if (wParam == VK_DOWN && VRKeyBuff[wParam] == 0 && !LastWhisperLen)
					{
						RecallSavedChatCommand(hFocusWnd, 0);
						//VRKeyBuff[ wParam ] = 1;
						break;
					}
					if (wParam == VK_ESCAPE && VRKeyBuff[wParam] == 0)
					{
						if (SendMessage(hTextWnd, EM_GETLIMITTEXT, 78, 0) >= 78)
						{
							hFocusWnd = 0;
							cInterFace.ChatFlag = 0;
							szLastWhisperName[0] = 0;
							LastWhisperLen = 0;
							InterfaceParty.chat_WhisperPartyPlayer_close();
							chatlistSPEAKERflagChg(0);		//Å¬·£ Ã¤ÆÃ Á¾·á
						}
						//							SendMessage( hTextWnd , EM_SETLIMITTEXT , 78 , 0 );
					}

					//Ã¤ÆÃÃ¢ ½ºÅ©·Ñ
					if (wParam == VK_NEXT && VRKeyBuff[wParam] == 0)
					{
						ChatScrollPoint -= DispChatMsgHeight;//6;
					}
					if (wParam == VK_PRIOR && VRKeyBuff[wParam] == 0)
					{
						ChatScrollPoint += DispChatMsgHeight;//6;
					}

					if (!hFocusWnd)
					{
						//ImmSimulateHotKey(hwnd,IME_CHOTKEY_IME_NONIME_TOGGLE);
						//ImmSimulateHotKey(hTextWnd,IME_CHOTKEY_IME_NONIME_TOGGLE);
						SetIME_Mode(0);		//IME ¸ðµå ÀüÈ¯
						ChatScrollPoint = 0;
						//ImmSetConversionStatus( hImc , IME_CMODE_ALPHANUMERIC,IME_CMODE_ALPHANUMERIC );
						//ImmSetConversionStatus( hImcEdit , IME_CMODE_ALPHANUMERIC,IME_CMODE_ALPHANUMERIC );
					}
				}
				/*
								IMETest.SetHWND(hFocusWnd);
								IMETest.GetInfoPerTime();
								for(int cnt=0;cnt<10;cnt++) {
									lstrcpy( szIME_Buff[cnt] , IMETest.m_szCandList[cnt].GetBuffer( IMETest.m_szCandList[cnt].GetLength() ) );
								}
				*/
				SendMessage(hFocusWnd, messg, wParam, lParam);
				break;
			}

			if (wParam == VK_ESCAPE && VRKeyBuff[wParam] == 0)
			{
				if (VRKeyBuff[VK_SHIFT])
					QuitGame();
				else
				{
					if (cInterFace.ChatFlag)
						cInterFace.ChatFlag = 0;
					else
					{
						if (cInvenTory.OpenFlag || cCharStatus.OpenFlag || cSkill.OpenFlag || ExitButtonClick)
						{
							cInterFace.CheckAllBox(SIN_ALL_CLOSE);
						}
						else
						{
							if (InterfaceParty.PartyPosState == PARTY_PROCESS)
							{
								InterfaceParty.PartyPosState = PARTY_END;
								InterfaceParty.qN_Pressf_CLEAR();
							}
							else
							{
								ExitButtonClick = 1;
							}
						}
					}
				}
			}


			if (GameMode == 2)
			{
				if (GameWindowMessage(hWnd, messg, wParam, lParam) == FALSE) break;
			}
			/*
						else {
							if ( smConfig.DebugMode && wParam == VK_F8 && VRKeyBuff[ wParam ]==0 ) {
								SetGameMode( 2 );
							}
						}
			*/
			if (GameMode == 2 && lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField] &&
				StageField[lpCurPlayer->OnStageField]->State == FIELD_STATE_ROOM)
			{
				//¾Ë±î±â ´ë±¹Àå¿¡¼­´Â È­¸é Ä¸ÃÄ ±ÝÁö

			}
			else
			{
#ifdef _LANGUAGE_ENGLISH	//ÇØ¿Ü
				if (lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField] &&
					StageField[lpCurPlayer->OnStageField]->State == FIELD_STATE_CASTLE)
				{
					//ÇÊ¸®ÇÉ °ø¼ºÇÊµå¿¡¼­ Ä¸ÃÄ±ÝÁö
				}
				else
				{
					if (wParam == VK_HOME && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
					{
						Capture(hwnd, CAPTURE_JPG);
					}
					//È­¸é Ä¸ÃÄ2
					if (wParam == VK_END && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
					{
						Capture(hwnd, CAPTURE_BMP);
					}
				}
#else
				//È­¸é Ä¸ÃÄ
				if (wParam == VK_HOME && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
				{
					Capture(hwnd, CAPTURE_JPG);
				}
				//È­¸é Ä¸ÃÄ2
				if (wParam == VK_END && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
				{
					Capture(hwnd, CAPTURE_BMP);
				}
#endif
			}

			VRKeyBuff[wParam] = 1;
			clan_Mouse(WM_KEYDOWN, wParam);

			break;

			//case WM_IME_KEYUP:
		case WM_KEYUP:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn > 0)
			{
				if (KeyFullZoomMap(wParam))
				{
					VRKeyBuff[wParam] = 0;
					break;
				}
			}
			//######################################################################################


#ifdef PARKMENU

			/////////Æã¼ÇÅ°	 Key UP
			//if ( GameMode==2 )
			//{
			if (wParam == 0x47)
			{
				if (vrunRuning == TRUE)
				{
					if (bMic == TRUE)
					{
						bMic = FALSE;
						void ParkSetMicOnOFF();
						ParkSetMicOnOFF();
					}
				}
			}
			//}
#endif

			if (hFocusWnd)
			{
				SendMessage(hFocusWnd, messg, wParam, lParam);
			}
			VRKeyBuff[wParam] = 0;
			clan_Mouse(WM_KEYUP, wParam);
			break;

		case WM_SYSKEYDOWN:
			if (wParam == VK_F10)
			{
				PostMessage(hWnd, WM_KEYDOWN, wParam, lParam);
				return FALSE;
			}
			if (hFocusWnd)
				SendMessage(hFocusWnd, messg, wParam, lParam);
			break;

		case WM_SYSKEYUP:
			if (wParam == VK_F10)
			{
				PostMessage(hWnd, WM_KEYUP, wParam, lParam);
				return FALSE;
			}
			if (hFocusWnd)
				SendMessage(hFocusWnd, messg, wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn)
			{
				g_iFzmCursorPosX = LOWORD(lParam);  // horizontal position of cursor 
				g_iFzmCursorPosY = HIWORD(lParam);  // vertical position of cursor 

				if (g_iFzmCursorPosX > 30 * g_fWinSizeRatio_X && g_iFzmCursorPosX < 350 * g_fWinSizeRatio_X &&
					g_iFzmCursorPosY > 338 * g_fWinSizeRatio_Y && g_iFzmCursorPosY < 578 * g_fWinSizeRatio_Y)
				{
					if (g_iFzmCursorFocusGame == 0)
					{
						g_iFzmCursorFocusGame = 1;

						xPs = pCursorPos.x = msXo = int(((float)g_iFzmCursorPosX - g_fZoomInAdd_x) * g_fZoomInDiv_x);
						yPs = pCursorPos.y = msYo = int(((float)g_iFzmCursorPosY - g_fZoomInAdd_y) * g_fZoomInDiv_y);
					}

					xPs = int(((float)g_iFzmCursorPosX - g_fZoomInAdd_x) * g_fZoomInDiv_x);
					yPs = int(((float)g_iFzmCursorPosY - g_fZoomInAdd_y) * g_fZoomInDiv_y);

					AutoMouse_Distance += abs(pCursorPos.x - xPs);
					AutoMouse_Distance += abs(pCursorPos.y - yPs);

					pCursorPos.x = xPs;
					pCursorPos.y = yPs;

					msX = msXo - xPs;
					msY = msYo - yPs;

					msXo = xPs;
					msYo = yPs;

					if ((wParam & MK_MBUTTON))
						SetMousePlay(3);
					else
					{
						if (wParam == MK_LBUTTON || TraceMode_DblClick)
							SetMousePlay(2);
						else if (wParam == MK_RBUTTON)
							SetMousePlay(4);
					}
					dwLastMouseMoveTime = dwPlayTime;
				}
				else
					g_iFzmCursorFocusGame = 0;
			}
			else
			{
				xPs = LOWORD(lParam);  // horizontal position of cursor 
				yPs = HIWORD(lParam);  // vertical position of cursor 

				AutoMouse_Distance += abs(pCursorPos.x - xPs);
				AutoMouse_Distance += abs(pCursorPos.y - yPs);

				//pCursorPos.x  = xPs;
				//pCursorPos.y  = yPs;
				g_iFzmCursorPosX = pCursorPos.x = xPs;
				g_iFzmCursorPosY = pCursorPos.y = yPs;
				msX = msXo - xPs;
				msY = msYo - yPs;

				msXo = xPs;
				msYo = yPs;

				if ((wParam&MK_MBUTTON))
					SetMousePlay(3);
				else
				{
					if (wParam == MK_LBUTTON || TraceMode_DblClick)
						SetMousePlay(2);
					else
						if (wParam == MK_RBUTTON)
							SetMousePlay(4);
				}

				/*
							if ( wParam==MK_LBUTTON ) {
								angY += MousemX/4;
								angX -= MousemY/4;
							}
				*/
			}
			//######################################################################################

			dwLastMouseMoveTime = dwPlayTime;
			break;

		case WM_LBUTTONDBLCLK:
			MouseDblClick = TRUE;

			if (GameMode == 2)
			{
				//¾È³»Áöµµ ¶ç¿ì±â
				if (GetFieldMapCursor() == TRUE && cInterFace.sInterFlags.MapOnFlag)
				{
					sinCheck_ShowHelpMap();
				}
				sinDoubleClick();

				dsMenuCursorPos(&pCursorPos, 2);
				if (StopCharMotion(pCursorPos.x, pCursorPos.y) != TRUE)
				{
					Moving_DblClick = TRUE;			//¸¶¿ì½º ÀÌµ¿ Ã½Å© ´õºí Å¬¸¯
					MousePos_DblClick.x = xPs;
					MousePos_DblClick.y = yPs;
					MousePos_DblClick.z = 0;
				}
				dsMenuCursorPos(&pCursorPos, 0);
			}
			break;

		case WM_LBUTTONDOWN:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame)
			{
				SetMousePlay(1);
				sinLButtonDown();
				MouseButton[0] = TRUE;

				clan_Mouse(WM_LBUTTONDOWN, wParam);
			}
			else
				g_FzmMouseButton[0] = TRUE;
			//######################################################################################

			if (GameMode == 2 && DisconnectFlag) quit = TRUE;
			break;

		case WM_LBUTTONUP:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame || MouseButton[0])
			{
				SetMousePlay(0);
				sinLButtonUp();

				MouseButton[0] = FALSE;
				MouseDblClick = FALSE;
				//ktj
				clan_Mouse(WM_LBUTTONUP, wParam);
			}
			else
			{
				MouseDblClick = FALSE;
				g_FzmMouseButton[0] = FALSE;
			}
			//######################################################################################

			break;

		case WM_RBUTTONDOWN:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame)
			{
				SetMousePlay(11);
				MouseButton[1] = TRUE;
				//ktj
				clan_Mouse(WM_RBUTTONDOWN, wParam);
			}
			else
				g_FzmMouseButton[1] = TRUE;
			//######################################################################################

			break;

		case WM_RBUTTONUP:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame)
			{
				SetMousePlay(10);
				sinRButtonUp();
				MouseButton[1] = FALSE;
				//ktj
				clan_Mouse(WM_RBUTTONUP, wParam);
			}
			else
				g_FzmMouseButton[1] = FALSE;
			//######################################################################################

			break;

		case WM_MBUTTONDOWN:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame)
			{
				MouseButton[2] = TRUE;
				//ktj
				clan_Mouse(WM_MBUTTONDOWN, wParam);
			}
			else
				g_FzmMouseButton[2] = TRUE;
			//######################################################################################
			break;

		case WM_MBUTTONUP:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame)
			{
				MouseButton[2] = FALSE;
				//ktj
				clan_Mouse(WM_MBUTTONUP, wParam);
			}
			else
				g_FzmMouseButton[2] = FALSE;
			//######################################################################################

			break;

		case WM_MOUSEWHEEL:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn <= 0 || g_iFzmCursorFocusGame)
			{
				zDelta = (short)HIWORD(wParam);    // wheel rotation

				if (cSinHelp.sinGetScrollMove(-zDelta / 16) == TRUE) break;
				if (TJBscrollWheel(zDelta) == TRUE) break;

				if (whAnx == ANX_NONE) whAnx = anx + zDelta;
				else whAnx += zDelta;
				AutoCameraFlag = FALSE;
			}
			//######################################################################################

			break;

		case WM_CHAR:
			if (wParam == 0x0D || wParam == 0x0A || (GameMode == 2 && wParam == VK_TAB)) return TRUE;
			if (!smConfig.DebugMode && wParam == 22 && lpCurPlayer->szChatMessage[0]) return TRUE;		//µµ¹è ±ÝÁö CONTROL-V ¸·À½

		case WM_SYSCHAR:
		case WM_IME_CHAR:
		case WM_IME_COMPOSITION:
			/*
					case WM_IME_CONTROL:
					case WM_IME_COMPOSITIONFULL:
					case WM_IME_ENDCOMPOSITION:
					case WM_IME_KEYDOWN:
					case WM_IME_KEYUP:
					case WM_IME_NOTIFY:
					case WM_IME_REQUEST:
					case WM_IME_SELECT:
					case WM_IME_SETCONTEXT:
					case WM_IME_STARTCOMPOSITION:
			*/
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (g_IsDxProjectZoomIn > 0) break;
			//######################################################################################
			if (hFocusWnd)
				SendMessage(hFocusWnd, messg, wParam, lParam);
			break;

		case WM_KILLFOCUS:
			SetFocus(hWnd);
			return TRUE;

		case WM_COMMAND:
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;


		case WM_ACTIVATE:
			break;

		case WM_NCACTIVATE:
#ifdef _WINMODE_DEBUG
			if (!smConfig.DebugMode)
#endif
#ifndef _LANGUAGE_WINDOW
				SetForegroundWindow(hwnd);
#endif

			//¿©±â¿¡ ÇØÅ· Åø »ðÀÔ
			if (GameMode == 2 && lpCurPlayer->dwObjectSerial)
			{
				//¿ÀÇÁ¶óÀÎ Å©·¢ °Ë»ö
				CheckCracker();
				//Å©·¢À» ´Ù½Ã Ã£´Â´Ù
				CheckCracker(NULL); //(¿Â¶óÀÎ °Ë»ö)
				//Ä³¸¯ÅÍ Á¤º¸ ÀÎÁõ È®ÀÎ
				CheckCharForm();
			}

			break;

		case WM_TIMER:
			if ((dwTimerCount & 3) == 0)
			{
				dwGameWorldTime++;			//°ÔÀÓÀÇ ¿ùµå½Ã°£

				dwGameHour = dwGameWorldTime + dwGameTimeOffset;
				dwGameHour = dwGameHour / 60;
				dwGameHour = dwGameHour - (((int)(dwGameHour / 24)) * 24);			//°ÔÀÓ³»ºÎ¿¡¼­ÀÇ ½Ã
				dwGameMin = dwGameWorldTime - (((int)(dwGameWorldTime / 60)) * 60);		//°ÔÀÓ³»ºÎ¿¡¼­ÀÇ ºÐ

				//dwGameHour = 1;		//¹ã ¸¸µé±â
			}

			if (smWsockServer && smWsockDataServer && smWsockUserServer)
			{
				smCheckWaitMessage();
			}

			dwTimerCount++;
			/*
						if ( (dwTimerCount&0x1F)==0 ) {
							//nProtect È®ÀÎ
							if ( Check_nProtect()==FALSE ) {
								quit = TRUE;
							}
						}
			*/
			break;

		case WM_NPROTECT_EXIT_TWO:
			//nProtect Á¾·á ¸Þ¼¼Áö
			DisconnectServerCode = 4;
			DisconnectFlag = GetCurrentTime();
			break;

		case WM_NPROTECT_SPEEDHACK:
		case WM_NPROTECT_SH_ERROR:
		case WM_NPROTECT_SOFTICE:
			DisconnectServerCode = 4;
			DisconnectFlag = GetCurrentTime();
			quit = 1;
			break;

		default:
			return DefWindowProc(hWnd, messg, wParam, lParam);
			break;
	}
	return 0L;

}




int MoveAngle(int step, int angY)
{
	int x, z;

	x = ((pX << 16) + GetSin[angY&ANGLE_MASK] * step) >> 16;
	z = ((pZ << 16) + GetCos[angY&ANGLE_MASK] * step) >> 16;
	/*
		y = smStage->GetHeight( x,pY,z );
		if ( (y-pY)>64 ) return FALSE;
		y = smStage->GetHeight( x-24,pY,z-24 );
		if ( (y-pY)>64 ) return FALSE;
		y = smStage->GetHeight( x+24,pY,z-24 );
		if ( (y-pY)>64 ) return FALSE;
		y = smStage->GetHeight( x-24,pY,z+24 );
		if ( (y-pY)>64 ) return FALSE;
		y = smStage->GetHeight( x+24,pY,z+24 );
		if ( (y-pY)>64 ) return FALSE;
	*/

	pX = x;
	pZ = z;

	return TRUE;
}

#define CAMERA_MOVE_STEP		(8*fONE)

int TraceCameraMain()
{

	if (TraceCameraPosi.x < TraceTargetPosi.x)
	{
		TraceCameraPosi.x += CAMERA_MOVE_STEP;
		if (TraceCameraPosi.x > TraceTargetPosi.x)
			TraceCameraPosi.x = TraceTargetPosi.x;
	}
	if (TraceCameraPosi.x > TraceTargetPosi.x)
	{
		TraceCameraPosi.x -= CAMERA_MOVE_STEP;
		if (TraceCameraPosi.x < TraceTargetPosi.x)
			TraceCameraPosi.x = TraceTargetPosi.x;
	}

	if (TraceCameraPosi.y < TraceTargetPosi.y)
	{
		TraceCameraPosi.y += CAMERA_MOVE_STEP;
		if (TraceCameraPosi.y > TraceTargetPosi.y)
			TraceCameraPosi.y = TraceTargetPosi.y;
	}
	if (TraceCameraPosi.y > TraceTargetPosi.y)
	{
		TraceCameraPosi.y -= CAMERA_MOVE_STEP / 2;
		if (TraceCameraPosi.y < TraceTargetPosi.y)
			TraceCameraPosi.y = TraceTargetPosi.y;
	}


	if (TraceCameraPosi.z < TraceTargetPosi.z)
	{
		TraceCameraPosi.z += CAMERA_MOVE_STEP;
		if (TraceCameraPosi.z > TraceTargetPosi.z)
			TraceCameraPosi.z = TraceTargetPosi.z;
	}
	if (TraceCameraPosi.z > TraceTargetPosi.z)
	{
		TraceCameraPosi.z -= CAMERA_MOVE_STEP;
		if (TraceCameraPosi.z < TraceTargetPosi.z)
			TraceCameraPosi.z = TraceTargetPosi.z;
	}

	return TRUE;
}


int RendSightPos = 0;
int RendSightOffset = 0;

//////////////////// ·»´õ¸µ ºä °Å¸® ¼³Á¤ ////////////////////
int SetRendSight()
{
	if (smRender.m_GameFieldView)
	{
		ViewPointLen = 38 * SizeMAPCELL + RendSightPos;
		ccDistZMin = ViewPointLen - (20 * SizeMAPCELL + RendSightPos / 4);

		return TRUE;
	}

	return FALSE;
}

int RendSightSub(int flag)
{
	if (lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField]->FieldSight)
	{
		RendSightOffset = StageField[lpCurPlayer->OnStageField]->FieldSight * SizeMAPCELL;
	}
	else
	{
		RendSightOffset = smRender.m_GameFieldViewStep * SizeMAPCELL;
		if (flag) RendSightPos = RendSightOffset;
	}

	if (RendSightPos < RendSightOffset) RendSightPos += 4;
	else
		if (RendSightPos > RendSightOffset) RendSightPos -= 4;


	return TRUE;
}
/////////////////////////////////////////////////////////////////////




#define	FPS_TIME		(1000/70)

DWORD FrameSkipTimer = 0;
int fps = 70;
int FpsTime;

#define AC_MOVE_STEP	4
#define AC_MOVE_MIN		256

//int aMoveStep = 0;
int dsCameraRotation = 0;			//´øÀü½ÃÁî Ä«¸Þ¶ó È¸Àü°ª

void PlayMain()
{
	int mv;

	if (ActionGameMode)
	{
		//¾×¼Ç°ÔÀÓ Å°Á¶ÀÛ
		ActionGameMain();
	}
	else if (cSinHelp.sinGetHelpShowFlag() != TRUE)
	{

		if (CameraInvRot)
		{
			if (VRKeyBuff[VK_RIGHT]) { any += 16; AutoCameraFlag = FALSE; }
			if (VRKeyBuff[VK_LEFT]) { any -= 16; AutoCameraFlag = FALSE; }
		}
		else
		{
			if (VRKeyBuff[VK_RIGHT]) { any -= 16; AutoCameraFlag = FALSE; }
			if (VRKeyBuff[VK_LEFT]) { any += 16; AutoCameraFlag = FALSE; }
		}

		if (VRKeyBuff[VK_CONTROL])
		{
			if (VRKeyBuff[VK_UP]) { anx -= 16; AutoCameraFlag = FALSE; }
			if (VRKeyBuff[VK_DOWN]) { anx += 16; AutoCameraFlag = FALSE; }
		}
		else
		{
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (VRKeyBuff[VK_UP])
			{
				dist -= 8;
				if (dist < 100)
				{
					if (anx <= 40)
					{
						if (dist < 40)
							dist = 40;
					}
					else
						dist = 100;

					CaTarDist = 0;
				}
			}

			if (VRKeyBuff[VK_DOWN])
			{
				dist += 8;
				if (dist > 440) { dist = 440; CaTarDist = 0; }
			}
			//######################################################################################
		}

		if (VRKeyBuff[VK_NEXT]) { anx -= 16; AutoCameraFlag = FALSE; }
		if (VRKeyBuff[VK_PRIOR]) { anx += 16; AutoCameraFlag = FALSE; }
	}

	if (cInterFace.sInterFlags.CameraAutoFlag != 2)
	{
		//´øÀü½ÃÁî Ä«¸Þ¶ó
		if (pCursorPos.x >= 8 && pCursorPos.x <= WinSizeX - 8)
		{
			dsCameraRotation = 0;
		}

		if (pCursorPos.x < 8)
		{
			if (!dsCameraRotation) dsCameraRotation = -512;
			mv = dsCameraRotation / 16;
			if (mv<-16) mv = -16;
			any += mv;
			dsCameraRotation++;
			if (dsCameraRotation >= 0)
			{
				dsCameraRotation = 0;
				pCursorPos.x = 8;
			}
			AutoCameraFlag = FALSE;
		}

		if (pCursorPos.x>WinSizeX - 8)
		{
			if (!dsCameraRotation) dsCameraRotation = 512;
			mv = dsCameraRotation / 16;
			if (mv > 16) mv = 16;
			any += mv;
			dsCameraRotation--;
			if (dsCameraRotation <= 0)
			{
				dsCameraRotation = 0;
				pCursorPos.x = WinSizeX - 8;
			}
			AutoCameraFlag = FALSE;
		}


		//if ( pCursorPos.y<16 ) { anx-=8; AutoCameraFlag=FALSE; }
		//if ( pCursorPos.y>WinSizeY-16 ) { anx+=8; AutoCameraFlag=FALSE; }
	}

	if (CaTarDist)
	{
		if (dist < CaTarDist)
		{
			dist += 12;
			if (dist >= CaTarDist) { dist = CaTarDist; CaTarDist = 0; }
		}
		else
			if (dist > CaTarDist)
			{
				dist -= 12;
				if (dist <= CaTarDist) { dist = CaTarDist; CaTarDist = 0; }
			}
	}

	if (CaMovAngle)
	{
		mv = CaMovAngle >> 4;
		if (CaMovAngle > 0)
		{
			if (mv > 32) mv = 32;
			any += mv; CaMovAngle -= mv;
			if (CaMovAngle < 0) CaMovAngle = 0;
		}
		else
		{
			if (mv < -32) mv = -32;
			any += mv; CaMovAngle -= mv;
			if (CaMovAngle > 0) CaMovAngle = 0;
		}
		if (mv == 0) CaMovAngle = 0;

		if (CaMovAngle == 0)
			AutoCameraFlag = TRUE;
		else
			AutoCameraFlag = FALSE;
	}

	if (whAnx != ANX_NONE)
	{
		//ÈÙ¿¡ ÀÇÇÑ °¢µµ º¯°æ
		if (anx < whAnx)
		{
			anx += 8;
			if (anx >= whAnx)
			{
				whAnx = ANX_NONE;
				AutoCameraFlag = TRUE;
			}
		}
		else
		{
			anx -= 8;
			if (anx <= whAnx)
			{
				whAnx = ANX_NONE;
				AutoCameraFlag = TRUE;
			}
		}
	}

	if (PlayFloor > 0)
	{
		if (ViewAnx < 500)
			ViewAnx += 8;

		if (ViewDist > 250)
			ViewDist -= 8;

		PlayFloor--;
	}
	else
	{
		if (ViewAnx < anx)
		{
			ViewAnx += 8;
			if (ViewAnx > anx) ViewAnx = anx;
		}
		if (ViewAnx > anx)
		{
			ViewAnx -= 8;
			if (ViewAnx < anx) ViewAnx = anx;
		}


		if (CaTarDist)
			mv = 100;
		else
			mv = 8;

		if (ViewDist < dist)
		{
			ViewDist += mv;
			if (ViewDist > dist) ViewDist = dist;
		}
		if (ViewDist > dist)
		{
			ViewDist -= mv;
			if (ViewDist < dist) ViewDist = dist;
		}
	}


	if (DebugPlayer)
	{
		if (dpX == 0 && dpZ == 0)
		{
			dpX = lpCurPlayer->pX;
			dpY = lpCurPlayer->pY;
			dpZ = lpCurPlayer->pZ;
		}

		if (VRKeyBuff[VK_SPACE])
		{
			GetMoveLocation(0, 0, fONE * 2, anx, any, 0);
			dpX += GeoResult_X;
			dpY += GeoResult_Y;
			dpZ += GeoResult_Z;
		}

		if (VRKeyBuff['Z']) anz -= 8;
		if (VRKeyBuff['X']) anz += 8;

		lpCurPlayer->pX = dpX;
		lpCurPlayer->pZ = dpZ;

		anx &= ANGCLIP;

		if (anx >= ANGLE_90 && anx < ANGLE_270)
		{
			if (anx < ANGLE_180)
				anx = ANGLE_90 - 32;
			else
				anx = ANGLE_270 + 32;
		}
		ViewAnx = anx;
		whAnx = ANX_NONE;

	}
	else
	{
		if (anx >= (ANGLE_90 - 48))
		{
			anx = (ANGLE_90 - 48);
			whAnx = ANX_NONE;
		}
		if (anx < 40)
		{
			anx = 40;
			whAnx = ANX_NONE;
		}
	}

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (anx > 40 && dist < 100) { dist = 100; CaTarDist = 0; }
	if (dist > 440) { dist = 440; CaTarDist = 0; }
	//######################################################################################

	int ay;
	int astep;

	if (PlayCameraMode != cInterFace.sInterFlags.CameraAutoFlag)
	{
		if (cInterFace.sInterFlags.CameraAutoFlag == 2) any = ANGLE_45;
		PlayCameraMode = cInterFace.sInterFlags.CameraAutoFlag;

		if (!dwM_BlurTime && IsCreateNewRenderTarget())
		{
			dwM_BlurTime = dwPlayTime + 600;
			SetFilterEffect(FILTER_EFFECT_SET_MOTION_BLUR, 80);
		}
	}

	if (PlayCameraMode == 1 && AutoCameraFlag && any != lpCurPlayer->Angle.y && lpCurPlayer->MoveFlag)
	{
		ay = (any - lpCurPlayer->Angle.y) & ANGCLIP;

		if (ay >= ANGLE_180)
		{
			ay = ay - ANGLE_360;
		}
		/*
				if ( abs(ay)>AC_MOVE_MIN && abs(ay)<(ANGLE_90+180) ) {
					astep = ay>>6;
					if ( ay<0 ) {
						if ( astep<-AC_MOVE_STEP ) astep = -AC_MOVE_STEP;
		//				astep = -AC_MOVE_STEP;
					}
					else {
						if ( astep>AC_MOVE_STEP ) astep = AC_MOVE_STEP;
		//				astep = AC_MOVE_STEP;
					}
		//			aMoveStep += astep;
					any = ( any-astep ) & ANGCLIP;
				}
		*/


		if (abs(ay) < (ANGLE_90 + 180))
		{

			if (ay < 0)
			{
				astep = abs(ay) >> 6;
				if (astep < AC_MOVE_STEP) astep = AC_MOVE_STEP;

				if (ay < -AC_MOVE_MIN)
				{
					ay += astep;
					if (ay > 0)
					{
						any = lpCurPlayer->Angle.y;
					}
					else
						any = (any + astep) & ANGCLIP;
				}
			}
			else
			{
				astep = abs(ay) >> 6;
				if (astep < AC_MOVE_STEP) astep = AC_MOVE_STEP;

				if (ay > AC_MOVE_MIN)
				{
					ay -= astep;
					if (ay < 0)
					{
						any = lpCurPlayer->Angle.y;
					}
					else
						any = (any - astep) & ANGCLIP;
				}
			}

		}

	}
	/*
		if ( aMoveStep>0 ) {
			aMoveStep -= 1;
			if ( aMoveStep<0 ) aMoveStep=0;
			any = ( any-aMoveStep ) & ANGCLIP;
		}
		if ( aMoveStep<0 ) {
			aMoveStep += 1;
			if ( aMoveStep>0 ) aMoveStep=0;
			any = ( any-aMoveStep ) & ANGCLIP;
		}
	*/

	NetWorkPlay();
	PlayPat3D();

	MainEffect();							//ÀÌÆåÆ® ¸ÞÀÎ

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (g_IsDxProjectZoomIn)
		MainFullZoomMap();
	//######################################################################################

	dwMemError = dwMemError^dwPlayTime;
	dwPlayTime = GetCurrentTime();			//½Ã°£ ±â·Ï
	dwMemError = dwMemError^dwPlayTime;

	TraceCameraMain();

	RendSightSub(0);				//·»´õ¸µ ½Ã¾ß º¯°æ
}

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

void PlayD3D()
{
	DWORD dwTime;

	dwTime = GetCurrentTime();
	if (dwLastPlayTime)
	{
		//½ºÇÇµåÇÙ °æ°í
		if (dwTime < dwLastPlayTime && dwLastOverSpeedCount>1000)
		{
			//ÇØÅ· ½ÃµµÇÑ À¯Àú ÀÚµ¿ ½Å°í
			SendSetHackUser2(1200, dwLastPlayTime - dwTime);
			dwLastOverSpeedCount = 0;
		}

		//ÇÁ·¹ÀÓ Å¸ÀÌ¸Ó Â÷¸¦ ±â·Ï ( ÃÖ´ëÄ¡¸¦ ³ªÁß¿¡ È®ÀÎÇÏ¿© ÇÁ·Î¼¼½º µð¹ö±×°¡ »ç¿ëµÆ´ÂÁö °Ë»ç )
		//SubTime = dwTime-dwLastPlayTime;
		//if ( SubTime>PlayTimerMax ) dwLastOverSpeedCount = SubTime;

	}
	dwLastPlayTime = dwTime;

	if (FrameSkipTimer == 0)
	{
		FrameSkipTimer = dwTime;
		FpsTime = 1000 / fps;
	}

	if (((int)(dwTime - FrameSkipTimer)) > 5000)
	{
		FrameSkipTimer = dwTime;//-5000;
	}

	if (FrameSkipTimer > dwTime)
	{
		Sleep(FrameSkipTimer - dwTime);
		dwTime = GetCurrentTime();
		dwLastPlayTime = dwTime;
	}

	pRealCursorPos.x = pCursorPos.x;
	pRealCursorPos.y = pCursorPos.y;

	while (1)
	{
		if (FrameSkipTimer >= dwTime) break;

		FrameSkipTimer += FPS_TIME;

		switch (GameMode)
		{
			case 1:
				//lpPlaySelect->Main();
				if (MainOpening() == TRUE)
				{
					SetGameMode(2);
				}
				break;
			case 2:
				PlayMain();
				//Ä³¸¯ÅÍ Á¤º¸ ÀÎÁõ È®ÀÎ
#ifdef _SINBARAM
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
				if (g_IsDxProjectZoomIn <= 0)
				{
					dsMenuCursorPos(&pCursorPos, 1);				//¸Þ´ºÄ¿¼­ ½Ã¹°·¹ÀÌ¼Ç
					sinMain();
					dsMenuCursorPos(&pCursorPos, 0);				//¸Þ´ºÄ¿¼­ ½Ã¹°·¹ÀÌ¼Ç º¹±¸

					MainInterfaceParty(pHoPartyMsgBox.x, pHoPartyMsgBox.y);		//ÆÄÆ¼±ÇÀ¯ ¸Þ´º
				}
				else
					MainSub();
				//######################################################################################
#endif
				if (BellatraEffectInitFlag)		//º§¶óÆ®¶ó ÀÌÆåÆ®
					MainBellatraFontEffect();

				break;
		}

		MainLoopCount++;
		dwLastOverSpeedCount++;
	}

	if (ParkPlayMode)
	{
		if (ParkPlayMode<0)
		{
			if ((smFlipCount & 1) == 0)		//¼­ÆäÀÌ½º ¸é Ã½Å©
				return;
		}
		else
		{
			if (ParkPlayMode <= 1000)
			{
				if ((dwLastRenderTime + ParkPlayMode)>dwTime)
					return;
			}
		}

	}
	else
	{
		if (GetForegroundWindow() != hwnd)
			Sleep(200);
	}

	dwLastRenderTime = dwTime;

	int i;
	//	HDC hdc;
	//	char strBuff[256];

	i = GetCurrentTime();
	if (timeg < i)
	{
		timeg = i + 1000;
		DispRender = RenderCnt;
		RenderCnt = 0;
		DispMainLoop = MainLoopCount;
		MainLoopCount = 0;
	}
	RenderCnt++;


	switch (GameMode)
	{
		case 1:
			/*
			lpPlaySelect->Draw();

			lpDDSBack->GetDC( &hdc );
			SelectObject( hdc , hFont );
			SetBkMode( hdc, TRANSPARENT );
			wsprintf( strBuff , "%d fps", DispRender );
			SetTextColor( hdc, RGB(0, 0, 0) );
			dsTextLineOut( hdc , 11,11, strBuff , lstrlen( strBuff ) );
			SetTextColor( hdc, RGB(255, 255, 255) );
			dsTextLineOut( hdc , 10,10, strBuff , lstrlen( strBuff ) );
			lpDDSBack->ReleaseDC( hdc );

			*/
			DrawOpening();
			if (Flip() == FALSE)
			{
				quit = 1;
			}
			return;
	}


	//Ä«¸Þ¶ó °íÁ¤ ½ÃÁ¡
	if (PlayCameraMode == 2)
	{
		dist = 400;
		anx = ANGLE_45 - 128;
		ViewAnx = anx;
		ViewDist = dist;
		//any = ANGLE_45;
	}

	any &= ANGCLIP;
	anx &= ANGCLIP;


	//È«È£µ¿ Ãß°¡(Ä«¸Þ¶ó ÀÌÆåÆ®)      ¾Æ·¡ºÎºÐ ¼öÁ¤ÀÔ´Ï´Ù.
	if (WaveCameraFlag && WaveCameraMode) // && g_IsCreateFilterEffect )
	{
		WaveCameraTimeCount++;

		if ((WaveCameraTimeCount > WaveCameraDelay))
		{
			WaveCameraTimeCount = 0;
			if (WaveCameraDelay > 1 && WaveCameraFactor < 40)
				WaveCameraFactor = -int((float)WaveCameraFactor / 10.f*9.f);
			else
				WaveCameraFactor = -int((float)WaveCameraFactor / 10.f*8.f);
			ViewDist += WaveCameraFactor;
		}

		if (abs(WaveCameraFactor) < 1)
		{
			WaveCameraFlag = FALSE;
			WaveCameraTimeCount = 0;
		}
	}
	else
	{
		WaveCameraFlag = FALSE;
	}

	GetMoveLocation(0, 0, -(ViewDist << FLOATNS), ViewAnx&ANGCLIP, any, 0);
	//GetMoveLocation( 0 , 0, -(dist<<FLOATNS), anx, any, 0 );
	if (GeoResult_X == GeoResult.x && GeoResult_Y == GeoResult.y && GeoResult_Z == GeoResult.z)
	{
		pX += lpCurPlayer->pX - TargetPosi.x;
		pY += lpCurPlayer->pY - TargetPosi.y;
		pZ += lpCurPlayer->pZ - TargetPosi.z;
	}
	else
	{
		pX = lpCurPlayer->pX;
		pY = lpCurPlayer->pY;
		pZ = lpCurPlayer->pZ;

		pX += GeoResult_X;
		pZ += GeoResult_Z;
		pY += GeoResult_Y;

		GeoResult.x = GeoResult_X;
		GeoResult.y = GeoResult_Y;
		GeoResult.z = GeoResult_Z;
	}

	TargetPosi.x = lpCurPlayer->pX;
	TargetPosi.y = lpCurPlayer->pY;
	TargetPosi.z = lpCurPlayer->pZ;


	if (DebugPlayer)
	{

		lpCurPlayer->Angle.y = any;

		pX = dpX;
		pY = dpY;
		pZ = dpZ;

		//anx = lpCurPlayer->Angle.x;
		any = lpCurPlayer->Angle.y;
	}
	else
		anz = 0;



	/*
		if ( pX<tX ) tX-=512;
		if ( pX>tX ) tX+=512;
		if ( pY<tY ) tY-=512;
		if ( pY>tY ) tY+=512;
		if ( pZ<tZ ) tZ-=512;
		if ( pZ>tZ ) tZ+=512;
	*/
	/*
		tX = pX>>FLOATNS;
		tY = pY>>FLOATNS;
		tZ = pZ>>FLOATNS;
	*/

	if (smConfig.DebugMode && VRKeyBuff[VK_SHIFT])
	{
		if (VRKeyBuff[VK_HOME])
			DarkLevel = 220;
		if (VRKeyBuff[VK_END])
			DarkLevel = 0;
	}

	//¼Óµµ º¯¼ö Lock
	LockSpeedProtect(lpCurPlayer);

	//Å©¸®Æ¼Ä® ¼½¼Ç ¼±¾ð
	EnterCriticalSection(&cDrawSection);
	smEnterTextureCriticalSection();


	//½ºÅ³ Æ÷ÀÎÆ® ¼÷·Ãµµ ·»´õ¸µÁß ¹Ù²î¸é ÇØÅ·
	DWORD	dwSkilChkSum = 0;
	DWORD	dwChkSum;
	DWORD	dwLevelQuestSum;
	int		cnt, k;

	for (cnt = 0; cnt < SIN_MAX_USE_SKILL; cnt++)
	{
		k = (cnt + 5) << 2;
		dwSkilChkSum += sinSkill.UseSkill[cnt].UseSkillCount*k;
		dwSkilChkSum += sinSkill.UseSkill[cnt].Point*k;
		dwSkilChkSum += sinSkill.UseSkill[cnt].Mastery*k;
		dwSkilChkSum += sinSkill.UseSkill[cnt].GageLength*k;
		dwSkilChkSum += sinSkill.UseSkill[cnt].Skill_Info.UseStamina[0] * k;
		dwSkilChkSum += sinSkill.UseSkill[cnt].Skill_Info.UseStamina[1] * k;
	}

	dwLevelQuestSum = sinQuest_levelLog & 0x576321cc;

	//È­¸é ·»´õ¸µ ÇÔ¼ö
	smPlayD3D(pX, pY, pZ, (ViewAnx&ANGCLIP), (any&ANGCLIP), (anz&ANGCLIP));

	dwChkSum = 0;
	for (cnt = 0; cnt < SIN_MAX_USE_SKILL; cnt++)
	{
		k = (cnt + 5) << 2;
		dwChkSum += sinSkill.UseSkill[cnt].UseSkillCount*k;
		dwChkSum += sinSkill.UseSkill[cnt].Point*k;
		dwChkSum += sinSkill.UseSkill[cnt].Mastery*k;
		dwChkSum += sinSkill.UseSkill[cnt].GageLength*k;
		dwChkSum += sinSkill.UseSkill[cnt].Skill_Info.UseStamina[0] * k;
		dwChkSum += sinSkill.UseSkill[cnt].Skill_Info.UseStamina[1] * k;
	}

	if (dwSkilChkSum != dwChkSum)
	{
		//½ºÅ³ º¯°æ µÆÀ½ , ¼ö»óÇÑÁ¡ ¹ß°ß
		SendSetHackUser3(8540, dwSkilChkSum, dwChkSum);
	}

	if (dwLevelQuestSum != (sinQuest_levelLog & 0x576321cc))
	{
		//Äù½ºÆ® ÄÚµåº¯°æ , ¼ö»óÇÑÁ¡ ¹ß°ß
		SendSetHackUser3(8820, (dwLevelQuestSum & 0x576321cc), sinQuest_levelLog);
	}


	int mapY;
	int x, z, y;


	GetMoveLocation(0, 0, -(dist << FLOATNS), anx, any, 0);

	x = lpCurPlayer->pX + GeoResult_X;
	y = lpCurPlayer->pY + GeoResult_Y;
	z = lpCurPlayer->pZ + GeoResult_Z;

	PlayFloor = 0;
	y = 0;

	if (!DebugPlayer)
	{
		/*
				if 	( lpCurPlayer->OnStageField>=0 && StageField[ lpCurPlayer->OnStageField ]->FieldCode<16 ) {

					if ( smGameStage[0] ) {
						mapY = (smGameStage[0]->GetHeight( x , z  ));
						if ( mapY>CLIP_OUT ) y++;
					}
					if ( smGameStage[1] ) {
						mapY = (smGameStage[1]->GetHeight( x , z  ));
						if ( mapY>CLIP_OUT ) y++;
					}
				}
				else {
		*/
		if (smGameStage[0])
		{
			mapY = (smGameStage[0]->GetHeight2(x, z));
			if (mapY > CLIP_OUT) y++;
		}
		if (smGameStage[1])
		{
			mapY = (smGameStage[1]->GetHeight2(x, z));
			if (mapY > CLIP_OUT) y++;
		}
		//		}
		if (!y) PlayFloor = 64;
	}


	//Å©¸®Æ¼Ä® ¼½¼Ç ÇØÁ¦
	smLeaveTextureCriticalSection();
	LeaveCriticalSection(&cDrawSection);

	//¼Óµµ º¯¼ö UnLock
	UnlockSpeedProtect(lpCurPlayer);

}



extern int DispPolyCnt;			// ½ÇÀç Ãâ·ÂµÈ Æú¸®°ï Ä«¿îÅÍ (Åë°è¿ë)
extern int GetMouseMapPoint(int x, int y);

extern int xframe;

int GetPlayMouseAngle()
{
	int ax, az, ay;

	ax = xPs - (WinSizeX >> 1);
	az = yPs - (WinSizeY >> 1);
	ay = GetRadian2D(0, 0, ax, -az);

	return ay + any;
}

int GetActionGame_PlayMouseAngle()
{
	int ax, az, ay;

	ax = xPs - lpCurPlayer->RendPoint.x;
	az = yPs - lpCurPlayer->RendPoint.x;
	ay = GetRadian2D(0, 0, ax, -az);

	return ay;;
}


int SetMousePlay(int flag)
{
	int ax, az, ay;
	char szBuff[256];
	int	cnt;
	//POINT	pmCursor;

	if (GameMode != 2) return TRUE;

	if (!lpCurPlayer->MotionInfo || dwNextWarpDelay || lpCurPlayer->PlayStunCount) return FALSE;

	if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) return FALSE;


#ifdef _SINBARAM
	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (g_IsDxProjectZoomIn <= 0)
	{
		//pmCursor.x = xPs; 
		//pmCursor.y = yPs; 
		dsMenuCursorPos(&pCursorPos, 2);
		if (StopCharMotion(pCursorPos.x, pCursorPos.y) == TRUE)
		{
			if (lpCurPlayer->MotionInfo->State < 0x100 && lpCurPlayer->MotionInfo->State != CHRMOTION_STATE_STAND)
			{
				lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_STAND);
				lpCurPlayer->MoveFlag = FALSE;
			}
			flag = 0;
			//return TRUE;
		}
		dsMenuCursorPos(&pCursorPos, 0);
	}
	//######################################################################################
#endif

	switch (flag)
	{
		case 1:

#ifdef	_NEW_PARTRADE

			if (EachTradeButton && chrEachMaster)
			{
				cnt = GetPartyTradeButtonPos(xPs, yPs);
				if (cnt >= 0)
				{
					EachTradeButton = 0;
					switch (cnt)
					{
						case 0:
							//±³È¯ °Å·¡ °Å¸® È®ÀÎ
							if (GetTradeDistanceFromCode(chrEachMaster->dwObjectSerial) == TRUE)
							{
								//¾ÆÀÌÅÛ ±³È¯ ½ÅÃ»
								SendRequestTrade(chrEachMaster->dwObjectSerial, 0);
								wsprintf(szBuff, mgRequestTrade, chrEachMaster->smCharInfo.szName);
							}
							else
							{
								wsprintf(szBuff, mgRequestTrade2, chrEachMaster->smCharInfo.szName);
							}
							return TRUE;

						case 1:
							//µ¿·á ½ÅÃ»
							wsprintf(szBuff, "//party %s", chrEachMaster->smCharInfo.szName);
							SendChatMessageToServer(szBuff);
							return TRUE;

						case 2:
							//Ä£±¸ µî·Ï
							InterfaceParty.latest_Insert(chrEachMaster->smCharInfo.szName);	//±Ó¼Ó¸»ÇÑ»ç¶÷ÀÇ ¸®½ºÆ® Ãß°¡.
							InterfaceParty.friend_Insert(chrEachMaster->smCharInfo.szName);		//Ä£±¸¸ñ·Ï

							InterfaceParty.Main_menuSet(2);	//¸ÞÀÎ¸Þ´º Äù½ºÆ®, µ¿·á, Ä£±¸¸¦ ¹Ù²Þ.
							InterfaceParty.chat_changeMENU(1);	//Ä£±¸Áß ÃÖ±Ù¸ñ·Ï(0), Ä£±¸¸ñ·Ï(1), °ÅºÎÀÚ¸ñ·ÏÀ»¹Ù²Þ.
							if (InterfaceParty.PartyPosState == PARTY_NONE)	ShowParty();
							return TRUE;

						case 3:
							//Å¬·£ ¸Þ¼¼Áö º¸³»±â
							//SendClanJoinService( 1 , chrEachMaster );
							g_IsCheckClanMember(chrEachMaster);
							return TRUE;

					}
				}
			}


#else
			if (EachTradeButton == 2 && chrEachMaster)
			{
				EachTradeButton = 0;

				//±³È¯ °Å·¡ °Å¸® È®ÀÎ
				if (GetTradeDistanceFromCode(chrEachMaster->dwObjectSerial) == TRUE)
				{
					//¾ÆÀÌÅÛ ±³È¯ ½ÅÃ»
					SendRequestTrade(chrEachMaster->dwObjectSerial, 0);
					//wsprintf( szBuff , "%s´Ô²² °Å·¡¸¦ ½ÅÃ»Çß½À´Ï´Ù" , chrEachMaster->smCharInfo.szName );
					wsprintf(szBuff, mgRequestTrade, chrEachMaster->smCharInfo.szName);
				}
				else
				{
					//				wsprintf( szBuff , "%s´ÔÀº °Å¸®°¡ ¸Ö¾î¼­ °Å·¡ ½ÅÃ»À» ÇÒ¼ö ¾ø½À´Ï´Ù" , chrEachMaster->smCharInfo.szName );
					wsprintf(szBuff, mgRequestTrade2, chrEachMaster->smCharInfo.szName);
				}

				AddChatBuff(szBuff);
				break;
			}
#endif
			//¾È³»Áöµµ ¶ç¿ì±â
			if (GetFieldMapCursor() == TRUE && cInterFace.sInterFlags.MapOnFlag)
			{
				if (lpCurPlayer->smCharInfo.Level < 20)
				{
					sinCheck_ShowHelpMap();
				}
				break;
			}

			if (!ActionGameMode)
			{

				//¸¶¿ì½º ¹öÆ° ´©¸§
				if (lpCharSelPlayer || lpSelItem)
				{
					SelMouseButton = 1;				//¿ÞÂÊ ¹öÆ°
					TraceAttackPlay();
					AutoCameraFlag = FALSE;
				}
				else
				{
					if (MsTraceMode && !lpCharSelPlayer && !lpSelItem)
					{
						if (lpCurPlayer->MotionInfo->State != CHRMOTION_STATE_ATTACK &&
							lpCurPlayer->MotionInfo->State != CHRMOTION_STATE_SKILL)
							lpCurPlayer->SetTargetPosi(0, 0);
						CancelAttack();
					}
					AutoCameraFlag = TRUE;
				}
			}
			else
			{
				ActionMouseClick[0] = 1;
			}

			AutoMouse_WM_Count++;				//ÀÚµ¿ ¸¶¿ì½º °¨Áö¿ë
			//TraceMode_DblClick = 0;

			if (SkillMasterFlag)
			{
				//½ºÅ³ ¹è¿ì±â¸¦ ¸¶Ä£´Ù 
				sinSkillMasterClose();
			}

			lpCurPlayer->MoveFlag = TRUE;
			DispEachMode = 0;

			if (hFocusWnd)
			{
				GetWindowText(hFocusWnd, szBuff, 240);
				if (szBuff[0] == 0)
				{
					hFocusWnd = 0;
					szLastWhisperName[0] = 0;
					LastWhisperLen = 0;
					InterfaceParty.chat_WhisperPartyPlayer_close();
					chatlistSPEAKERflagChg(0);		//Å¬·£ Ã¤ÆÃ Á¾·á
					SetIME_Mode(0);		//IME ¸ðµå ÀüÈ¯
					ChatScrollPoint = 0;
					//ImmSetConversionStatus( hImc , IME_CMODE_ALPHANUMERIC,IME_CMODE_ALPHANUMERIC );
					//ImmSetConversionStatus( hImcEdit , IME_CMODE_ALPHANUMERIC,IME_CMODE_ALPHANUMERIC );
				}
			}
			break;

		case 0:
			//¸¶¿ì½º ¹öÆ° ³õÀ½
			//if ( !lpCharSelPlayer && !lpSelItem ) {
			if (Moving_DblClick)
			{
				TraceMode_DblClick = TRUE;
				Moving_DblClick = 0;
				lpCurPlayer->MoveFlag = TRUE;
				ActionMouseClick[0] = 2;

			}
			else
			{
				if (!lpCharMsTrace && !lpMsTraceItem)
				{
					lpCurPlayer->SetAction(0);
				}
			}
			return TRUE;

		case 11:
			if (hFocusWnd)
			{
				GetWindowText(hFocusWnd, szBuff, 240);
				if (szBuff[0] == 0)
				{
					hFocusWnd = 0;
					SetIME_Mode(0);		//IME ¸ðµå ÀüÈ¯
					ChatScrollPoint = 0;
					//ImmSetConversionStatus( hImc , IME_CMODE_ALPHANUMERIC,IME_CMODE_ALPHANUMERIC );
					//ImmSetConversionStatus( hImcEdit , IME_CMODE_ALPHANUMERIC,IME_CMODE_ALPHANUMERIC );
				}
			}
			//¸¶¿ì½º ¿À¸£ÂÊ ¹öÆ° ´©¸§
			if (sinSkill.pRightSkill && lpCurPlayer->MotionInfo->State != CHRMOTION_STATE_ATTACK &&
				lpCurPlayer->MotionInfo->State != CHRMOTION_STATE_SKILL)
			{

				if (lpCurPlayer->MotionInfo->State != CHRMOTION_STATE_EAT)
				{
					//½ºÅ³ »ç¿ë
					if (OpenPlaySkill(sinSkill.pRightSkill))
						break;
				}
			}

			if (!ActionGameMode)
			{
				if (lpCharSelPlayer)
				{ //|| lpSelItem ) {
					SelMouseButton = 2;				//¿À¸¥ ¹öÆ°
					TraceAttackPlay();
					AutoCameraFlag = FALSE;
					lpCurPlayer->MoveFlag = TRUE;
					DispEachMode = 0;
				}
			}
			else
			{
				ActionMouseClick[1] = TRUE;
			}


			AutoMouse_WM_Count++;				//ÀÚµ¿ ¸¶¿ì½º °¨Áö¿ë
			break;
		case 10:
			//¸¶¿ì½º ¿À¸£ÂÊ ¹öÆ° ³õÀ½
			if (!lpCharMsTrace && !lpMsTraceItem)
			{
				lpCurPlayer->SetAction(0);
			}
			break;

		case 3:
			//°¡¿îµ¥ ¹öÆ° ´©¸¥ »óÅÂ¿¡¼­ ÀÌµ¿
			//	msX = xPs-msXo;
			//msY = yPs-msXo;

			ay = msY * 4;//>>1;
			ax = msX * 8;

			if (ay)
			{
				if (!CaTarDist) CaTarDist = dist;
				CaTarDist -= ay;
				//######################################################################################
				//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
				if (CaTarDist < 100)
				{
					if (anx <= 40)
					{
						if (CaTarDist < 40)
							CaTarDist = 40;
					}
					else
						CaTarDist = 100;
				}

				if (CaTarDist > 440) { CaTarDist = 440; }
				//######################################################################################
			}

			if (ax)
			{
				if (CameraInvRot) ax = -ax;

				az = ANGLE_45 >> 1;
				if (ax < -az) ax = -az;
				if (ax > az) ax = az;

				CaMovAngle += ax;
			}

			return TRUE;

		case 4:
			if (DebugPlayer)
			{
				/*
				msXo = WinSizeX>>1;
				msYo = WinSizeY>>1;

				SetCursorPos( msXo, msYo );
				*/

				ay = msY * 2;
				ax = msX * 2;

				any += ax;
				anx += ay;

				any &= ANGCLIP;
				anx &= ANGCLIP;

			}
			return TRUE;
	}

	if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_ATTACK ||
		lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_SKILL ||
		lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_YAHOO
		) return FALSE;


	if (lpCurPlayer->MoveFlag)
	{

		if (MsTraceMode)
		{
			lpCurPlayer->Angle.y = GetMouseSelAngle();
			//if ( ay>=0) lpCurPlayer->Angle.y = ay;
		}
		else
		{
			if (ActionGameMode)
				lpCurPlayer->Angle.y = GetActionGame_PlayMouseAngle();
			else
				lpCurPlayer->Angle.y = GetPlayMouseAngle();
		}
	}

	return TRUE;
}

extern int TestTraceMatrix();

int _stdcall InitD3D(HWND hWnd)
{
	hwnd = hWnd;

	MidX = WinSizeX / 2;
	MidY = WinSizeY / 2;
	MidY -= 59;
	viewdistZ = WinSizeX;

	if (!CreateD3D())
		return FALSE;

	if (!SetDisplayMode(hWnd, WinSizeX, WinSizeY, WinColBit))
		return FALSE;

	if (!CreateViewport())
		return FALSE;

	TestTraceMatrix();

	InitRender();
	InitTexture();

	smSetTextureLevel(smConfig.TextureQuality);

	LPD3DPRIMCAPS lpD3dCaps = &lpD3DDeviceDesc->Desc.dpcTriCaps;
	if (TextureSwapMode == FALSE && lpD3DDeviceDesc->bIsHardware)
	{
		if (lpD3dCaps->dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR)
		{
			MipMapModeCreate = NOSQUARE_MODE;
			if (lpD3dCaps->dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
				MipMapModeCreate = SQUARE_MODE;
		}
	}

	if (lpD3dCaps->dwRasterCaps & D3DPRASTERCAPS_FOGTABLE)
	{
		smRender.m_FogIsRend = TRUE;

		D3DMATRIX matProj;
		D3D_SetProjectionMatrix(matProj, (g_PI / 4.4f), (float(WinSizeX) / float(WinSizeY)), 20.f, 4000.f);
		lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

		lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
	}

	HBITMAP hBitmap;
	HDC	hdcBitmap;

	hdc = GetDC(hwnd);

	hBitmap = LoadBitmap(hinst, "IDB_LOADING_BRAZIL");

	hdcBitmap = CreateCompatibleDC(hdc);

	SelectObject(hdcBitmap, hBitmap);

	BitBlt(hdc, WinSizeX / 2 - 265, WinSizeY / 2 - 180, 498, 300, hdcBitmap, 0, 0, SRCCOPY);

	DeleteDC(hdcBitmap);
	DeleteObject(hBitmap);
	ReleaseDC(hwnd, hdc);

	Check_CodeSafe((DWORD)GameInit);

	InitSoundEffect(hwnd);
	InitPatterns();

	lpCurPlayer = InitRotPlayer();

	return Code_VRamBuffOpen();
}

void _stdcall CloseD3d()
{
	CloseRotPlayer();
	CloseSoundEffect();
	CloseTexture();

	ReleaseD3D();
}


//°ÔÀÓ ÃÊ±âÈ­
int GameInit()
{
	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	g_IsReadTextures = 1;
	//######################################################################################

	//¼­¹ö¿¡ ±ÇÇÑ ¼³Á¤
	if (smConfig.DebugMode) SendAdminMode(TRUE);

	npSetUserID(UserAccount);		//°ÔÀÓ°¡µå¿¡ ID Åëº¸

	dwPlayTime = GetCurrentTime();
	dwMemError = dwMemError^dwPlayTime;

	Log::Debug("nProtect : nProtect solicitado.");

	// ¸ÞÆ®¸®¾ó ÃÊ±âÈ­
	InitMaterial();
	smRender.SetMaterialGroup(smMaterialGroup);			//±âº» ¸ÞÆ®¸®¾ó ±×·ì

	//ZeroMemory( &lpCurPlayer->smCharInfo , sizeof( smCHAR_INFO ) );
	ReformCharForm();

	InitEffect();			//È£ ÀÌÆåÆ® ÃÊ±âÈ­

	InitMotionBlur();		//¸ð¼Ç ºÎ·Á ÃÊ±âÈ­
	InitBackGround();		//¹è°æ ÃÊ±âÈ­

	Check_CodeSafe((DWORD)CloseD3d);	//ÄÚµå º¸È£ ½ÇÇà

	InitStage();
	InitPat3D();

#ifdef _XTRAP_GUARD_4_CLIENT
	XTrap_C_SetUserInfo(UserAccount, szConnServerName, lpCurPlayer->smCharInfo.szName, "class name", 1);	//XTrapD5
#endif

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	CreateItem2PassTexture();
	//######################################################################################

	CheckCharForm();

	//¸Þ¸ð¸® Ã½Å© ÃÊ±âÈ­
	//InitKeepMemFunc();

	//¸Þ¸ð¸® ÀüÃ¼ Ã½Å©
	//CheckKeepMemFull();


#ifdef _SINBARAM
	lpDDSMenu = 0;
	sinInit();
#else
	lpDDSMenu = LoadDibSurfaceOffscreen(smConfig.szFile_Menu);
#endif

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	g_fWinSizeRatio_X = float(WinSizeX) / 800.f;
	g_fWinSizeRatio_Y = float(WinSizeY) / 600.f;

	CreateBeforeFullZoomMap();
	CreateFontImage();
	//######################################################################################

	TraceCameraPosi.x = lpCurPlayer->pX;
	TraceCameraPosi.y = lpCurPlayer->pY;
	TraceCameraPosi.z = lpCurPlayer->pZ;

	TraceTargetPosi.x = lpCurPlayer->pX;
	TraceTargetPosi.y = lpCurPlayer->pY;
	TraceTargetPosi.z = lpCurPlayer->pZ;

	InitMessageBox();

	//·»´õ¸µ ±âº» °ª
	smRender.SMMULT_PERSPECTIVE_HEIGHT = RENDCLIP_DEFAULT_MULT_PERSPECTIVE_HEIGHT;
	MidX = WinSizeX / 2;
	MidY = WinSizeY / 2;
	viewdistZ = WinSizeX;

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	g_IsReadTextures = 1;
	//######################################################################################

	ReadTextures();

	CheckOftenMeshTextureSwap();	//ÀÚÁÖ¾µ ¸Þ½Ã ÅØ½ºÃÄ ½º¿ÒÃ½Å©

	//À½¾Ç ¿¬ÁÖ
	if (smConfig.BGM_Mode)
	{
		if (StageField[0])
			PlayBGM_Direct(StageField[0]->BackMusicCode);
		else
		{
			OpenBGM("wav\\bgm\\Field - Desert - Pilgrim.bgm");
			PlayBGM();
		}
	}
	CharPlaySound(lpCurPlayer);
	StartEffect(lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ, EFFECT_GAME_START1);
	RestartPlayCount = 700;		//10ÃÊ µ¿¾È ¹«Àû


	hFocusWnd = 0;
	szLastWhisperName[0] = 0;
	LastWhisperLen = 0;
	InterfaceParty.chat_WhisperPartyPlayer_close();
	chatlistSPEAKERflagChg(0);		//Å¬·£ Ã¤ÆÃ Á¾·á
	SendMessage(hTextWnd, EM_SETLIMITTEXT, 78, 0);			//Ã¤ÆÃ 80±ÛÀÚ Á¦ÇÑ

	MouseButton[0] = 0;
	MouseButton[1] = 0;
	MouseButton[2] = 0;

	//ÁÖÀÎ°ø Ä³¸¯ÅÍ Æ÷ÀÎÅÍ º¯°æ		//kyle xtrapHeap

#ifdef _XTRAP_GUARD_4_CLIENT //HEAP MEMORY TEST
	XTrap_CE1_Func11_Protect(&sinChar, sizeof(sinChar));	//º¸È£¿µ¿ª ¹«°á¼º Ã¼Å©
#endif

#ifdef _XIGNCODE_CLIENT
	// ¹ÚÀç¿ø - XignCode
	Xigncode_Client_Start();
#endif

	smCHAR	*lpTempChar;
	lpTempChar = SelectRotPlayer(lpCurPlayer);
	if (lpTempChar)
	{
		lpCurPlayer = lpTempChar;
		sinChar = &lpCurPlayer->smCharInfo;
	}
	lpTempChar = SelectRotPlayer(lpCurPlayer);
	if (lpTempChar)
	{
		lpCurPlayer = lpTempChar;
		sinChar = &lpCurPlayer->smCharInfo;
	}

#ifdef _XTRAP_GUARD_4_CLIENT //HEAP MEMORY TEST
	XTrap_CE1_Func12_Protect(&sinChar, sizeof(sinChar));	//º¸È£¿µ¿ª º¸È£
	XTrap_CE1_Func13_Free(&sinChar, sizeof(sinChar));		//º¸È£¿µ¿ª ÇØÁ¦
#endif

	SetIME_Mode(0);		//IME ¸ðµå ÀüÈ¯

	//½ºÅ³º¸È£°ª ÀüºÎ ÃÊ±âÈ­
	ReformSkillInfo();


	HoMsgBoxMode = 1;
	SetMessageFrameSelect(HoMsgBoxMode);		//È£¸Þ¼¼ÁöÃ¢ ÇÁ·¹ÀÓ ¸ðµå

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	CreateWinIntThread();
	//######################################################################################

	return TRUE;
}

//°ÔÀÓ ´Ý±â
int GameClose()
{

#ifdef _XIGNCODE_CLIENT
	//¹ÚÀç¿ø - XignCode
	ZCWAVE_Cleanup();
	ZCWAVE_SysExit();
#endif

	if (lpDDSMenu) lpDDSMenu->Release();

	//¸Þ¸ð¸® Ã½Å© Á¾·á
	//CloseKeepMem();

	ClosePat3D();
	CloseBackGround();
	CloseStage();

#ifdef _SINBARAM
	sinClose();
#endif
	CloseMaterial();

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	DestroyWinIntThread();
	//######################################################################################

	if (BellatraEffectInitFlag)
	{
		DestroyBellatraFontEffect();
	}

	return TRUE;
}

//·Î°í°¡ ÀÖÀ¸¸é ·Î°í¸¦ È­¸é¿¡ º¸¿©ÁØ´Ù (0.8ÃÊ)
int DIsplayLogoImage()
{
	LPDIRECTDRAWSURFACE4	LogoImage;
	LPDIRECTDRAWSURFACE4	LogoImage2;

	LogoImage = LoadDibSurfaceOffscreen("Image\\Logo.bmp");
	LogoImage2 = LoadDibSurfaceOffscreen("Image\\Logo2.bmp");
	if (LogoImage)
	{
		LogoImage->SetColorKey(DDCKEY_COLORSPACE, 0);
#ifdef _LANGUAGE_KOREAN
		if (LogoImage2)
			LogoImage2->SetColorKey(DDCKEY_COLORSPACE, 0);
#endif

		// ·£µå¸µ ½ÃÀÛ
		BeginRender();
#ifdef _LANGUAGE_BRAZIL
		DrawSprite(WinSizeX / 2 - 400, WinSizeY / 2 - 300, LogoImage, 0, 0, 800, 600);
#else
#ifdef _LANGUAGE_ARGENTINA
		DrawSprite(WinSizeX / 2 - 250, WinSizeY / 2 - 150, LogoImage, 0, 0, 500, 300);
#else
#ifdef _LANGUAGE_CHINESE
		DrawSprite(WinSizeX / 2 - 150, WinSizeY / 2 - 125, LogoImage, 0, 0, 300, 250);
#else
		DrawSprite(WinSizeX / 2 - 250, WinSizeY / 2 - 150, LogoImage, 0, 0, 500, 300);
#endif
#endif
#endif

		//DrawSprite( 150, 150, LogoImage, 0, 0, 500, 300);
#ifdef _LANGUAGE_KOREAN
		if (LogoImage2)
			DrawSprite(WinSizeX - 100, 16, LogoImage2, 0, 0, 80, 45);
#endif
		Flip();

#ifdef	_LANGUAGE_CHINESE
		Sleep(1000);
#else
		Sleep(600);
#endif
		LogoImage->Release();
	}
	return TRUE;
}


//°ÔÀÓ ¼±ÅÃ
int SetGameMode(int mode)
{
	int OldMode;

	OldMode = GameMode;
	GameMode = mode;
	//½Å¹Ù¶÷¿¡ Ä³¸¯ÅÍ Æ÷ÀÎÅÍ ¼³Á¤

#ifdef _XTRAP_GUARD_4_CLIENT //HEAP MEMORY TEST
	XTrap_CE1_Func11_Protect(&sinChar, sizeof(sinChar));	//º¸È£¿µ¿ª ¹«°á¼º Ã¼Å©
#endif

	sinChar = &lpCurPlayer->smCharInfo;

#ifdef _XTRAP_GUARD_4_CLIENT //HEAP MEMORY TEST
	XTrap_CE1_Func12_Protect(&sinChar, sizeof(sinChar));	//º¸È£¿µ¿ª º¸È£
	XTrap_CE1_Func13_Free(&sinChar, sizeof(sinChar));		//º¸È£¿µ¿ª ÇØÁ¦
#endif

	switch (OldMode)
	{
		case 1:
			/*
			lpPlaySelect->Close();
			if (lpPlaySelect) { delete lpPlaySelect; lpPlaySelect=0; }
			*/
			//±¹¹®¿ë
			//#ifdef _npGAME_GUARD
			//		KeyCrypt.OnExitKeyCrypt();
			//#endif

			/* temp by 2neo
			#ifndef _WINMODE_DEBUG
			#ifndef _nProtect_For_Debug
					KeyCrypt.OnExitKeyCrypt();		//Å°Å©¸³Æ®
			#endif
			#endif
			*/

			CloseOpening();
			CloseMaterial();
			break;

		case 2:
			GameClose();
			break;

	}

	switch (GameMode)
	{
		case 1:
			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			SetDxProjection((g_PI / 4.4f), WinSizeX, WinSizeY, 20.f, 4000.f);
			//######################################################################################

			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			smRender.CreateRenderBuff(CameraSight);
			//######################################################################################


			//lpPlaySelect = new ccsSELECT();
			// ¸ÞÆ®¸®¾ó ÃÊ±âÈ­
			InitMaterial();
			smRender.SetMaterialGroup(smMaterialGroup);			//±âº» ¸ÞÆ®¸®¾ó ±×·ì
			InitEffect();			//È£ ÀÌÆåÆ® ÃÊ±âÈ­

			DIsplayLogoImage();			//·Î°í¸¦ º¸¿©ÁØ´Ù
	//±¹¹®¿ë
	//#ifdef _npGAME_GUARD
	//		KeyCrypt.OnLoadKeyCrypt();
	//#endif
			//Å°Å©¸³Æ®

	/* temp by 2neo
	#ifndef _WINMODE_DEBUG
	#ifndef	_nProtect_For_Debug
			KeyCrypt.OnLoadKeyCrypt();
	#endif
	#endif
	*/


			InitOpening();
			//·»´õ¸µ ¿ÍµåÅ¬¸®ÇÎ ¸ðµå
			smRender.SMMULT_PERSPECTIVE_HEIGHT = RENDCLIP_WIDE_MULT_PERSPECTIVE_HEIGHT;
			MidX = WinSizeX / 2;
			MidY = WinSizeY / 2;

			// ½ÇÇàÁßÀÎ ÇÁ·Î¼¼½º¸¦ °Ë»çÇÑ´Ù.
			if (CheckCrackProcess(TRUE)) quit = 1;

			if (IsCreateNewRenderTarget())
				SetFilterEffect(FILTER_EFFECT_SET_BRIGHT_CONTRAST, 180);

			dwM_BlurTime = 0;

			//Init_ParkPlayer();
			//Play_ParkPlayer( "mms://hapkido.re.kr/hapkido/8833/movie_158.asf" , 0,0, 400,400 , -1 );
			//Play_ParkPlayer( "mms://hapkido.re.kr/hapkido/8833/movie_158.asf" , 200,200, 300,300 , 150 );

	//±¹¹®¿ë
	//#ifdef _npGAME_GUARD
	//		KeyCrypt.OnRegisterwindow(hwnd);
	//		SetHwnd_nProtect(hwnd);
	//#endif
	//Å°Å©¸³Æ®

	/* temp by 2neo
	#ifndef _WINMODE_DEBUG
	#ifndef _nProtect_For_Debug
			KeyCrypt.OnExitKeyCrypt();		//Å°Å©¸³Æ®
	#endif
	#endif
	*/


			break;

		case 2:
			if (IsCreateNewRenderTarget())
				SetFilterEffect(FILTER_EFFECT_SET_BRIGHT_CONTRAST, 160);

			//######################################################################################
			//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
			if (smRender.m_GameFieldView)
			{
				smRender.m_GameFieldViewStep = 22;
				smRender.SetGameFieldViewStep();
			}
			//######################################################################################

			GameInit();

			// ½ÇÇàÁßÀÎ ÇÁ·Î¼¼½º¸¦ °Ë»çÇÑ´Ù.
			if (CheckCrackProcess()) quit = 1;
			break;
	}

	WaveCameraFlag = FALSE;

	return TRUE;
}


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

int RestoreAll()
{
	HDC hdc;

	// Back Surface Áö¿ì±â
	DDBLTFX bltFx;
	ZeroMemory(&bltFx, sizeof(DDBLTFX));
	bltFx.dwSize = sizeof(DDBLTFX);
	bltFx.dwFillColor = 0;
	lpDDSPrimary->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltFx);

	VramTotal = 0;

	lpDDSPrimary->GetDC(&hdc);
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	dsTextLineOut(hdc, 16, WinSizeY - 32, szRestore, lstrlen(szRestore));
	lpDDSPrimary->ReleaseDC(hdc);

	RestoreTextures();
	if (lpDDSMenu)
	{
		lpDDSMenu->Release();
		lpDDSMenu = LoadDibSurfaceOffscreen(smConfig.szFile_Menu);
	}

	sinReload();

	return TRUE;

}


//¹Ýº¹ È®ÀÎ ¹è°æ Ä«¿ìÅÍ
//extern int DEBUG_ReCheckedCnt;

int NumPoly;
int Disp_tx, Disp_ty;
int Disp_rx, Disp_ry;
//Ã¤ÆÃÃ¢ ¼ÒÆÃ¿ë ¹Ú½º
smCHAR	*lpCharMsgSort[OTHER_PLAYER_MAX];
int ChatMsgSortCnt;

extern int Debug_RecvCount1;
extern int Debug_RecvCount2;
extern int Debug_SendCount;

//ÅØ½ºÃÄ ¿É¼ÇÀ» ±âº»°ªÀ¸·Î
int RestoreInterfaceTexture()
{
	int cnt;

	smRender.Color_R = 0;
	smRender.Color_G = 0;
	smRender.Color_B = 0;
	smRender.Color_A = 0;

	cnt = 0;

	lpD3DDevice->SetTextureStageState(cnt, D3DTSS_COLOROP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(cnt, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	lpD3DDevice->SetTextureStageState(cnt, D3DTSS_COLORARG2, D3DTA_CURRENT);

	lpD3DDevice->SetTextureStageState(cnt, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	lpD3DDevice->SetTextureStageState(cnt, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	lpD3DDevice->SetTextureStageState(cnt, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	lpD3DDevice->SetTexture(cnt, 0);

	for (cnt = 1; cnt < 8; cnt++)
	{
		lpD3DDevice->SetTextureStageState(cnt, D3DTSS_COLOROP, D3DTOP_DISABLE);
		lpD3DDevice->SetTextureStageState(cnt, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		lpD3DDevice->SetTexture(cnt, 0);
	}

	return TRUE;
}

//######################################################################################
//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
void VirtualDrawGameState(void)
{
	smRender.ClearLight();

	int BackDarkLevel;

	BackDarkLevel = DarkLevel;
	DarkLevel = 0;

	///////////////////////////////////////////////////////////////////////
	//ÅØ½ºÃÄ ¿É¼ÇÀ» ±âº»°ªÀ¸·Î	
	RestoreInterfaceTexture();
	///////////////////////////////////////////////////////////////////////

	if (DisconnectFlag)
	{
		//¼­¹ö¿ÍÀÇ ¿¬°á ²÷¾îÁü
		if (DisconnectServerCode == 0)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 1)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect1, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 2)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect2, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 3)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect3, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 4)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect4, 36, BOX_ONE);
		}

#ifdef	_WINMODE_DEBUG
		if (!smConfig.DebugMode && !quit && ((DWORD)DisconnectFlag + 5000) < dwPlayTime) quit = TRUE;
#else
		if (!quit && ((DWORD)DisconnectFlag + 5000) < dwPlayTime) quit = TRUE;
#endif

	}
	else
	{
		if (quit)
			DrawMessage(MidX - 40, MidY, mgCloseGame, 36, BOX_ONE);
		else
		{
			if (dwCloseBoxTime && dwCloseBoxTime > dwPlayTime)
			{
				DrawMessage(MidX - 100, MidY, mgCloseWindow, 36, BOX_ONE);
			}
			else
			{
				if (dwBattleQuitTime)
				{
					if ((dwBattleQuitTime + 5000) > dwPlayTime)
						DrawMessage(MidX - 40, MidY, mgCloseBattle, 36, BOX_ONE);
					else
						dwBattleQuitTime = 0;
				}
			}
		}
	}

	//
	if (lpD3DDevice)
	{
		lpD3DDevice->BeginScene();
		DrawCursor();
		lpD3DDevice->EndScene();
	}

	DarkLevel = BackDarkLevel;
	if (Flip() == FALSE)
	{
		DisconnectFlag = dwPlayTime;
		quit = 1;
	}
}
//######################################################################################


int DrawGameState()
{
	int i;
	HDC	hdc;
	int cnt, cnt2;//,len;
//	int x,y;
	int	y, DispBar, DispMaster;
	//	POINT	ppoint;
	smCHAR	*lpChar;
	scITEM	*lpItem;
	RECT	ddRect;
	int BackDarkLevel;
	DWORD	dwColor;

	/*
		ddRect.left   = 0;
		ddRect.right  = 640;
		ddRect.top    = 340;
		ddRect.bottom = 480;
	*/
	ddRect.left = 0;
	ddRect.right = 800;
	ddRect.top = 0;
	ddRect.bottom = 150;

	//	lpDDSBack->BltFast(0,WinSizeY-120,lpDDSMenu,&ddRect,DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);

	if (lpDDSMenu) i = lpDDSBack->BltFast(0, WinSizeY - 150, lpDDSMenu, &ddRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

	smRender.ClearLight();

	BackDarkLevel = DarkLevel;
	DarkLevel = 0;

	///////////////////////////////////////////////////////////////////////
	//ÅØ½ºÃÄ ¿É¼ÇÀ» ±âº»°ªÀ¸·Î
	RestoreInterfaceTexture();

	if (lpCharMsTrace && lpCharMsTrace->RendSucess)
	{
		Disp_tx = lpCharMsTrace->RendPoint.x - 32;
		Disp_ty = lpCharMsTrace->RendPoint.y - 12;
		//		Disp_ty = lpCharMsTrace->RendRect2D.top+16;
	}

	if (lpCharSelPlayer && lpCharMsTrace != lpCharSelPlayer)
	{
		if (lpCharSelPlayer->RendSucess)
		{
			Disp_rx = lpCharSelPlayer->RendPoint.x - 32;
			Disp_ry = lpCharSelPlayer->RendPoint.y - 12;
			//			Disp_ry = lpCharSelPlayer->RendRect2D.top+16;
		}
	}
	/*
		for( cnt=0;cnt<OTHER_PLAYER_MAX;cnt++) {
			if ( chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].RendSucess &&
				( chrOtherPlayer[cnt].PartyFlag ||
				( chrOtherPlayer[cnt].smCharInfo.State==smCHAR_STATE_ENEMY && chrOtherPlayer[cnt].smCharInfo.Brood==smCHAR_MONSTER_USER && chrOtherPlayer[cnt].smCharInfo.Next_Exp==lpCurPlayer->dwObjectSerial && chrOtherPlayer[cnt].smCharInfo.Life[0]>0 ) ) ) {

				i = 0;

				if ( lpCharMsTrace==&chrOtherPlayer[cnt] ) {
					x = Disp_tx;
					y = Disp_ty;
				}
				else {
					if ( lpCharSelPlayer==&chrOtherPlayer[cnt] ) {
						x = Disp_rx;
						y = Disp_ry;
					}
					else {
						x = chrOtherPlayer[cnt].RendPoint.x - 32;
						y = chrOtherPlayer[cnt].RendPoint.y - 12;
						i = 1;
					}
				}


				wsprintf( strBuff , "%s",chrOtherPlayer[cnt].smCharInfo.szName );
				len = lstrlen(strBuff);

				lpD3DDevice->EndScene();
				chrOtherPlayer[cnt].DrawStateBar2( x+8,y+14 );
				lpD3DDevice->BeginScene();

				x = (x+32)-(len>>1)*6;

				if ( i ) {
					lpDDSBack->GetDC( &hdc );
					SelectObject( hdc , hFont );
					SetBkMode( hdc, TRANSPARENT );


					if ( chrOtherPlayer[cnt].smCharInfo.State==smCHAR_STATE_ENEMY && chrOtherPlayer[cnt].smCharInfo.Brood==smCHAR_MONSTER_USER && chrOtherPlayer[cnt].smCharInfo.Next_Exp==lpCurPlayer->dwObjectSerial ) {
						SetTextColor( hdc, RGB(0,0,0) );
						dsTextLineOut( hdc , x-1 , y-1 , strBuff , len );
						SetTextColor( hdc, RGB(255, 200, 160) );
						dsTextLineOut( hdc , x , y , strBuff , len );
					}
					else {
						SetTextColor( hdc, RGB(0,0,0) );
						dsTextLineOut( hdc , x-1 , y-1 , strBuff , len );
						SetTextColor( hdc, RGB(200, 255, 160) );
						dsTextLineOut( hdc , x , y , strBuff , len );
					}

					lpDDSBack->ReleaseDC( hdc );
				}
			}
		}
	*/
	////////////////////////////////////////////////////////////////////////////
	lpChar = 0;
	lpItem = 0;
	/*
		lpDDSBack->GetDC( &hdc );
		SelectObject( hdc , hFont );
		SetBkMode( hdc, TRANSPARENT );

	*/
	if (lpMsTraceItem)
		lpItem = lpMsTraceItem;
	else
		lpItem = lpSelItem;
	/*
		if ( lpCharMsTrace ) {
			lpChar = lpCharMsTrace;

			if ( lpChar->RendSucess ) {
				//Disp_tx = lpChar->RendPoint.x - 32;
				//Disp_ty = lpChar->RendPoint.y;

				//wsprintf( strBuff , "%s" , lpChar->smCharInfo.szName );

				len = lstrlen(strBuff);
				x = (Disp_tx+32)-(len>>1)*6;
				y = Disp_ty;

	//			SetTextColor( hdc, RGB(0, 0, 0) );
	//			dsTextLineOut( hdc , x-1 , y-1 , strBuff , len );


				if ( lpChar->smCharInfo.State==smCHAR_STATE_ENEMY )
					dwColor = RGB(255, 180, 180);
				else
					dwColor = RGB(180, 180, 255);

	//			dsTextLineOut( hdc , x , y , strBuff , len );


				if ( !lpChar->szChatMessage[0] ) {
					DrawCharacterMessage( lpChar->RendPoint.x, lpChar->RendRect2D.top,
						lpChar->smCharInfo.szName , 0 , 0, 0 , dwColor  );
				}

				if ( lpChar->smCharInfo.Brood==smCHAR_MONSTER_USER && lpChar->smCharInfo.Next_Exp!=lpCurPlayer->dwObjectSerial ) {
					wsprintf( strBuff , "(%s)" , lpChar->smCharInfo.szModelName2+1 );
					len = lstrlen(strBuff);
					x = (Disp_tx+32)-(len>>1)*6;
					y = Disp_ty;

					SetTextColor( hdc, RGB(0, 0, 0) );
					dsTextLineOut( hdc , x-1 , y+11 , strBuff , len );
					SetTextColor( hdc, RGB(255, 220, 160) );
					dsTextLineOut( hdc , x , y+12 , strBuff , len );
				}
				else if ( lpChar->smCharInfo.ClassClan && lpChar->ClanInfoNum>=0 ) {
					//Å¬·£ ÀÌ¸§ Ç¥½Ã ÀÓ½Ã
					wsprintf( strBuff , "[%s]" , ClanInfo[lpChar->ClanInfoNum].ClanInfoHeader.ClanName );
					len = lstrlen(strBuff);
					x = (Disp_tx+32)-(len>>1)*6;
					y = Disp_ty;

					SetTextColor( hdc, RGB(0, 0, 0) );
					dsTextLineOut( hdc , x-1 , y+11 , strBuff , len );
					SetTextColor( hdc, RGB(160, 220, 220) );
					dsTextLineOut( hdc , x , y+12 , strBuff , len );

				}

			}
		}

		if ( lpCharSelPlayer && lpCharMsTrace!=lpCharSelPlayer ) {
			lpChar = lpCharSelPlayer;
			if ( lpChar->RendSucess ) {
				//Disp_tx = lpChar->RendPoint.x - 32;
				//Disp_ty = lpChar->RendPoint.y;

				//wsprintf( strBuff , "%s" , lpChar->smCharInfo.szName );

				len = lstrlen(strBuff);
				x = (Disp_rx+32)-(len>>1)*6;
				y = Disp_ry;

	//			SetTextColor( hdc, RGB(0, 0, 0) );
	//			dsTextLineOut( hdc , x-1 , y-1 , strBuff , len );
	//			SetTextColor( hdc, RGB(255, 255, 255) );
	//			dsTextLineOut( hdc , x , y , strBuff , len );

				if ( !lpChar->szChatMessage[0] ) {
					DrawCharacterMessage( lpChar->RendPoint.x, lpChar->RendRect2D.top,
						lpChar->smCharInfo.szName , 0 , 0, 0 , RGB(255, 255, 255)  );
				}

				if ( lpChar->smCharInfo.State==smCHAR_STATE_ENEMY && lpChar->smCharInfo.Brood==smCHAR_MONSTER_USER && lpChar->smCharInfo.Next_Exp!=lpCurPlayer->dwObjectSerial ) {
					wsprintf( strBuff , "(%s)" , lpChar->smCharInfo.szModelName2+1 );
					len = lstrlen(strBuff);
					x = (Disp_rx+32)-(len>>1)*6;
					y = Disp_ry;

					SetTextColor( hdc, RGB(0, 0, 0) );
					dsTextLineOut( hdc , x-1 , y+11 , strBuff , len );
					SetTextColor( hdc, RGB(255, 220, 160) );
					dsTextLineOut( hdc , x , y+12 , strBuff , len );
				}
				else if ( lpChar->smCharInfo.ClassClan && lpChar->ClanInfoNum>=0 ) {
					//Å¬·£ ÀÌ¸§ Ç¥½Ã ÀÓ½Ã
					wsprintf( strBuff , "[%s]" , ClanInfo[lpChar->ClanInfoNum].ClanInfoHeader.ClanName );
					len = lstrlen(strBuff);
					x = (Disp_rx+32)-(len>>1)*6;
					y = Disp_ry;

					SetTextColor( hdc, RGB(0, 0, 0) );
					dsTextLineOut( hdc , x-1 , y+11 , strBuff , len );
					SetTextColor( hdc, RGB(160, 220, 220) );
					dsTextLineOut( hdc , x , y+12 , strBuff , len );
				}

			}
		}
	*/

	if (VRKeyBuff['A'])
	{
		for (cnt = 0; cnt < DISP_ITEM_MAX; cnt++)
		{
			if (scItems[cnt].Flag && scItems[cnt].ItemCode != 0 && lpSelItem != &scItems[cnt])
			{

				if (scItems[cnt].RendPoint.z >= 32 * fONE && scItems[cnt].RendPoint.z < 64 * 12 * fONE)
				{

					DrawCharacterMessage(scItems[cnt].RendPoint.x, scItems[cnt].RendPoint.y,
										 scItems[cnt].szName, 0, 0, 0, RGB(110, 110, 110));
				}
			}
		}
	}

	if (lpSelItem && !lpCharSelPlayer && !lpCharMsTrace)
	{
		//¾ÆÀÌÅÛ ÀÌ¸§ Ç¥½Ã
		Disp_tx = MsSelPos.x;
		Disp_ty = MsSelPos.y;

		DrawCharacterMessage(Disp_tx, Disp_ty,
							 lpSelItem->szName, 0, 0, 0, RGB(180, 180, 180));

	}


	//	lpDDSBack->ReleaseDC( hdc );

	////////////////////////////////////////////////////////////////////////////////


	int	SelFlag;

	ChatMsgSortCnt = 0;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].RendSucess && chrOtherPlayer[cnt].smCharInfo.szName[0])
		{
			SelFlag = 0;
			if (chrOtherPlayer[cnt].RendPoint.z < 12 * 64 * fONE)
			{
				if (chrOtherPlayer[cnt].szChatMessage[0])
				{
					//Ã¤ÆÃ Ã¢ °Ë»ç
					if (chrOtherPlayer[cnt].dwChatMessageTimer < dwPlayTime)
						chrOtherPlayer[cnt].szChatMessage[0] = 0;
					else
					{
						if (chrOtherPlayer[cnt].smCharInfo.State)
						{
							lpCharMsgSort[ChatMsgSortCnt++] = &chrOtherPlayer[cnt];
							SelFlag++;
						}
					}
				}

				if (chrOtherPlayer[cnt].dwTradeMsgCode && !SelFlag)
				{	//°³ÀÎ»óÁ¡ ¿ÀÇÂ
					lpCharMsgSort[ChatMsgSortCnt++] = &chrOtherPlayer[cnt];
					SelFlag++;
				}
			}



			if (!SelFlag)
			{
				if (lpCharMsTrace == &chrOtherPlayer[cnt] ||
					lpCharSelPlayer == &chrOtherPlayer[cnt] ||
					(chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_NPC && chrOtherPlayer[cnt].RendPoint.z < 12 * 64 * fONE) ||
					(chrOtherPlayer[cnt].smCharInfo.Life[0]>0 && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY && chrOtherPlayer[cnt].smCharInfo.Brood == smCHAR_MONSTER_USER) ||
					(chrOtherPlayer[cnt].smCharInfo.ClassClan && chrOtherPlayer[cnt].smCharInfo.ClassClan == lpCurPlayer->smCharInfo.ClassClan) ||
					(HoMsgBoxMode && chrOtherPlayer[cnt].dwClanManageBit) ||
					chrOtherPlayer[cnt].PartyFlag)
				{

					lpCharMsgSort[ChatMsgSortCnt++] = &chrOtherPlayer[cnt];
				}
			}

		}
	}
	if (lpCurPlayer->szChatMessage[0])
	{
		if (lpCurPlayer->dwChatMessageTimer < dwPlayTime)
			lpCurPlayer->szChatMessage[0] = 0;
		else
			lpCharMsgSort[ChatMsgSortCnt++] = lpCurPlayer;
	}
	else
	{
		if (lpCurPlayer->dwTradeMsgCode)
		{
			lpCharMsgSort[ChatMsgSortCnt++] = lpCurPlayer;
		}
	}

	//lpCharMsgSort[ChatMsgSortCnt++] = &chrOtherPlayer[cnt];

	//¸Þ¼¼Áö ¹Ú½º ¼ÒÆÃ
	if (ChatMsgSortCnt)
	{
		for (cnt = 0; cnt < ChatMsgSortCnt; cnt++)
		{
			for (cnt2 = cnt + 1; cnt2 < ChatMsgSortCnt; cnt2++)
			{
				if (lpCharMsgSort[cnt]->RendPoint.z < lpCharMsgSort[cnt2]->RendPoint.z ||
					lpCharMsgSort[cnt] == lpCharMsTrace || lpCharMsgSort[cnt] == lpCharSelPlayer)
				{

					lpChar = lpCharMsgSort[cnt];
					lpCharMsgSort[cnt] = lpCharMsgSort[cnt2];
					lpCharMsgSort[cnt2] = lpChar;
				}
			}
		}
	}
	/*

	//x, y ÂïÀ» À§Ä¡ ÀÌ À§Ä¡¿¡¼­ ÁÂ¿ì·Î Å©±â Á¤¿­ÇØ¼­ ÂïÀ½..
	// message: ÀÏ¹ÝÀÌ¸§ ÀÌ³ª Ã¤ÆÃ ¸Þ½ÃÁö¸¦ ³Ö¾îÁÖ¼¼¿ä (ÀÏ¹ÝÀÌ¸§°ú Ã¤ÆÃ ¸Þ½ÃÁö´Â ´Ù¸£°Ô ¾Ë¾Æ¼­ ÂïÈû) //NULL ÀÏ °æ¿ì Ã¤ÆÃ ¸Þ½ÃÁö ÂïÈ÷Áö ¾ÊÀ½.
	// lineSize: ±ÛÀÌ ±æ°æ¿ì °¡·Î ¹®ÀÚ¿­ Å©±â Á¦ÇÑ..
	// clanMark: clanMark ÀÌ¹ÌÁö NULL ÀÏ°æ¿ì´Â ÂïÈ÷Áö ¾ÊÀ½..
	// clanName: Å¬·£ ÀÌ¸§ NULLÀÏ °æ¿ì´Â ÂïÈ÷Áö ¾ÊÀ½..
	// Æ÷ÀÎÅÍ °ªÁß ¾²Áö ¾Ê´Â °ªÀº NULLÀ» ³Ö¾î ÁÖ¼¼¿ä..
	DrawCharacterMessage(int x, int y, char *message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, char *clanName)
	*/

	LPDIRECTDRAWSURFACE4 lpDDS_clanMark;
	char *szClanName;
	char *szBoxMsg;
	BOOL  selectedBox;


	y = 8 + (ViewAnx >> 6);

	//¼øÂ÷ Á¤·ÄµÈ Ã¤ÆÃ ¹Ú½º ±×¸®±â
	for (cnt = 0; cnt < ChatMsgSortCnt; cnt++)
	{
		/*
		#ifdef _LANGUAGE_ENGLISH
				DrawChatMessage( lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top,//+24,
					lpCharMsgSort[cnt]->szChatMessage ,32);
		#else
		*/
		/*
		DrawChatMessage( lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top,//+24,
			lpCharMsgSort[cnt]->szChatMessage ,30);
		*/
		/*
			( chrOtherPlayer[cnt].PartyFlag ||
			( chrOtherPlayer[cnt].smCharInfo.State==smCHAR_STATE_ENEMY && chrOtherPlayer[cnt].smCharInfo.Brood==smCHAR_MONSTER_USER && chrOtherPlayer[cnt].smCharInfo.Next_Exp==lpCurPlayer->dwObjectSerial && chrOtherPlayer[cnt].smCharInfo.Life[0]>0 ) ) ) {

				dwColor = RGB(255, 180, 180);
			else
				dwColor = RGB(180, 180, 255);

			*/

		dwColor = RGB(255, 255, 200);

		/*
				if ( lpCharMsgSort[cnt]==lpCharMsTrace ) {
					dwColor = RGB(255, 255, 255);
				}
				else if ( lpCharMsgSort[cnt]==lpCharSelPlayer ) {
					dwColor = RGB(255, 255, 255);
				}
		*/
		DispBar = FALSE;
		DispMaster = FALSE;
		selectedBox = FALSE;

		if (lpCharMsgSort[cnt]->PartyFlag)
		{
			dwColor = RGB(220, 255, 160);
			DispBar = TRUE;
		}
		else if (lpCharMsgSort[cnt]->smCharInfo.State == smCHAR_STATE_ENEMY)
		{
			if (lpCharMsgSort[cnt]->smCharInfo.Brood == smCHAR_MONSTER_USER)
			{
				dwColor = RGB(220, 255, 160);

				if (lpCharMsgSort[cnt]->smCharInfo.Next_Exp == lpCurPlayer->dwObjectSerial || lpCharMsgSort[cnt]->smCharInfo.ClassClan)
					DispBar = TRUE;
				else
					DispMaster = TRUE;
			}
			else
				dwColor = RGB(255, 210, 210);
		}
		else if (lpCharMsgSort[cnt]->smCharInfo.State == smCHAR_STATE_NPC)
		{
			dwColor = RGB(180, 180, 255);
		}

		if (lpCharMsgSort[cnt] == lpCharMsTrace)
		{
			if (lpCharMsgSort[cnt]->smCharInfo.State == smCHAR_STATE_ENEMY && lpCharMsgSort[cnt]->smCharInfo.Brood != smCHAR_MONSTER_USER)
				dwColor = RGB(255, 230, 200);
			else
				dwColor = RGB(255, 255, 255);

			selectedBox = TRUE;
		}
		else
		{
			if (lpCharMsgSort[cnt] == lpCharSelPlayer && lpCharMsgSort[cnt]->szChatMessage[0]) selectedBox = TRUE;
		}

		/*
		LPDIRECTDRAWSURFACE4 lpDDS_clanMark;
		char *szClanName;
					else if ( lpChar->smCharInfo.ClassClan && lpChar->ClanInfoNum>=0 ) {
						//Å¬·£ ÀÌ¸§ Ç¥½Ã ÀÓ½Ã
						wsprintf( strBuff , "[%s]" , ClanInfo[lpChar->ClanInfoNum].ClanInfoHeader.ClanName );
		*/
		lpDDS_clanMark = 0;
		szClanName = 0;
		DWORD	dwClanMgrBit;

		if (lpCharMsgSort[cnt]->smCharInfo.ClassClan)
		{
			if (lpCharMsgSort[cnt] == lpCurPlayer)
			{
				lpDDS_clanMark = cldata.hClanMark16;
				szClanName = cldata.name;
			}
			else if (lpCharMsgSort[cnt]->ClanInfoNum >= 0)
			{
				lpDDS_clanMark = ClanInfo[lpCharMsgSort[cnt]->ClanInfoNum].hClanMark;
				szClanName = ClanInfo[lpCharMsgSort[cnt]->ClanInfoNum].ClanInfoHeader.ClanName;
			}
		}


		if (((lpCharMsgSort[cnt]->szChatMessage[0] || lpCharMsgSort[cnt]->dwTradeMsgCode) && lpCharMsgSort[cnt]->smCharInfo.State && lpCharMsgSort[cnt]->RendPoint.z < 12 * 64 * fONE) ||
			lpCharMsgSort[cnt] == lpCurPlayer)
		{

			if (szClanName && lpCharMsgSort[cnt] != lpCharMsTrace && lpCharMsgSort[cnt] != lpCharSelPlayer)
			{
				if (!lpCurPlayer->smCharInfo.ClassClan || lpCharMsgSort[cnt]->smCharInfo.ClassClan != lpCurPlayer->smCharInfo.ClassClan)
					szClanName = 0;
			}

			if (lpCharMsgSort[cnt]->szChatMessage[0])
			{
				DrawCharacterMessage(lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top + y,
									 lpCharMsgSort[cnt]->szChatMessage, 30, lpDDS_clanMark, szClanName, dwColor, selectedBox);
			}
			else
			{
				if (lpCharMsgSort[cnt]->szTradeMessage[0])
				{
					DrawCharacterShopByeMessage(lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top + y,
												lpCharMsgSort[cnt]->szTradeMessage, 30, lpDDS_clanMark, dwColor, selectedBox);
				}
			}
		}
		else
		{
			if (DispMaster && !lpCharMsgSort[cnt]->smCharInfo.ClassClan)
			{

				wsprintf(strBuff, "(%s)", lpCharMsgSort[cnt]->smCharInfo.szModelName2 + 1);
				DrawTwoLineMessage(lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top + y,
								   lpCharMsgSort[cnt]->smCharInfo.szName, strBuff, dwColor, RGB(255, 255, 200), 0, selectedBox);

			}
			else
			{
				/*
				if ( lpDDS_clanMark && lpCharMsgSort[cnt]!=lpCharSelPlayer && lpCharMsgSort[cnt]!=lpCharMsTrace && !DispBar ) {
					szClanName = 0;
				}*/

				if (HoMsgBoxMode)
					dwClanMgrBit = lpCharMsgSort[cnt]->dwClanManageBit;
				else
					dwClanMgrBit = 0;

				if (szClanName)
				{
					if (smConfig.DebugMode && VRKeyBuff[VK_CONTROL])
						wsprintf(strBuff, "%d/%d", lpCharMsgSort[cnt]->smCharInfo.ClassClan, lpCharMsgSort[cnt]->ClanInfoNum);
					else
						wsprintf(strBuff, "[%s]", szClanName);

					DrawTwoLineMessage(lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top + y,
									   strBuff, lpCharMsgSort[cnt]->smCharInfo.szName, RGB(150, 255, 200), dwColor, lpDDS_clanMark, selectedBox, dwClanMgrBit);
				}
				else
				{
					szBoxMsg = lpCharMsgSort[cnt]->smCharInfo.szName;
					DrawCharacterMessage(lpCharMsgSort[cnt]->RendPoint.x, lpCharMsgSort[cnt]->RendRect2D.top + y,
										 szBoxMsg, 30, lpDDS_clanMark, szClanName, dwColor, selectedBox, dwClanMgrBit);
				}
			}
		}

		if (DispBar)
		{
			lpD3DDevice->EndScene();
			lpCharMsgSort[cnt]->DrawStateBar2(lpCharMsgSort[cnt]->RendPoint.x - 30, lpCharMsgSort[cnt]->RendRect2D.top + y - 14);
			lpD3DDevice->BeginScene();
		}


		//#endif
	}

	//½ºÅ³ Â÷Â¡ °ÔÀÌÁö
	if (lpCurPlayer->AttackSkil)
	{
		switch (lpCurPlayer->AttackSkil & 0xFF)
		{
			case SKILL_PLAY_CHARGING_STRIKE:

				cnt = lpCurPlayer->frame - lpCurPlayer->MotionInfo->StartFrame * 160;
				if (lpCurPlayer->MotionInfo->EventFrame[0]<(DWORD)cnt && lpCurPlayer->MotionInfo->EventFrame[1]>(DWORD)cnt)
				{

					cnt -= (int)lpCurPlayer->MotionInfo->EventFrame[0];
					cnt2 = (int)(lpCurPlayer->MotionInfo->EventFrame[1] - lpCurPlayer->MotionInfo->EventFrame[0]);

					lpD3DDevice->EndScene();
					lpCurPlayer->DrawChargingBar(lpCurPlayer->RendPoint.x - 30, lpCurPlayer->RendRect2D.bottom, cnt, cnt2);
					lpD3DDevice->BeginScene();
				}
				break;
			case SKILL_PLAY_PHOENIX_SHOT:

				cnt = lpCurPlayer->frame - lpCurPlayer->MotionInfo->StartFrame * 160;
				if (lpCurPlayer->MotionInfo->EventFrame[0]<(DWORD)cnt && lpCurPlayer->MotionInfo->EventFrame[1]>(DWORD)cnt)
				{

					cnt -= (int)lpCurPlayer->MotionInfo->EventFrame[0];
					cnt2 = (int)(lpCurPlayer->MotionInfo->EventFrame[1] - lpCurPlayer->MotionInfo->EventFrame[0]);

					lpD3DDevice->EndScene();
					lpCurPlayer->DrawChargingBar(lpCurPlayer->RendPoint.x - 30, lpCurPlayer->RendRect2D.bottom, cnt, cnt2);
					lpD3DDevice->BeginScene();
				}
				break;
		}
	}

	if (DispInterface)
	{
		if (DispEachMode)
		{
			//DrawEachPlayer( 0.74f , 0.34f , 1 );					//È®´ë Ä³¸¯ÅÍ ±×¸®±â
			DrawEachPlayer(0.74f, 0.32f, 1);					//È®´ë Ä³¸¯ÅÍ ±×¸®±â
		}
		else
		{
			DrawEachPlayer(0.92f, 0.1f, 0);					//È®´ë Ä³¸¯ÅÍ ±×¸®±â

			dsDrawOffset_X = WinSizeX - 800;
			dsDrawOffset_Y = WinSizeY - 600;
			dsDrawOffsetArray = dsARRAY_RIGHT | dsARRAY_BOTTOM;
			dsMenuCursorPos(&pCursorPos, 3);				//¸Þ´ºÄ¿¼­ ½Ã¹°·¹ÀÌ¼Ç
			//ÆÄÆ¼ ¸Þ´º ±×¸®±â
			DrawInterfaceParty();
			dsMenuCursorPos(&pCursorPos, 0);				//¸Þ´ºÄ¿¼­ ½Ã¹°·¹ÀÌ¼Ç º¹±¸
			dsDrawOffsetArray = dsARRAY_TOP;
			dsDrawOffset_X = 0;
			dsDrawOffset_Y = 0;

			DrawInterfaceParty(pHoPartyMsgBox.x, pHoPartyMsgBox.y);		//ÆÄÆ¼±ÇÀ¯ ¸Þ´º
		}
		//######################################################################################
		//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
		if (smConfig.DebugMode)
		{
			extern int Debug_TalkZoomMode;
			if (Debug_TalkZoomMode && lpCurPlayer->TalkPattern)
			{
				extern void DrawTalkZoom(smCHAR *lpChar, smPAT3D *lpPattern, float fx, float fy);
				DrawTalkZoom(lpCurPlayer, lpCurPlayer->TalkPattern, 0.2f, 0.32f);
			}
		}
		//######################################################################################

	}

	//////////////////////////////
#ifdef	_NEW_PARTRADE
	if (EachTradeButton && chrEachMaster)
	{
		//ÆÄÆ¼ Æ®·¹ÀÌµå ½ÅÃ» ¹öÆ® Å¬¸¯
		DisplayPartyTradeButton();
	}
#endif


	//SodÁ¡¼ö Ç¥½Ã ÇÔ¼ö
	DisplaySodScore();

#ifdef _SINBARAM
	///////////////////////////////////////////////////////////////////////
	//ÅØ½ºÃÄ ¿É¼ÇÀ» ±âº»°ªÀ¸·Î
	RestoreInterfaceTexture();

	dsDrawOffset_X = WinSizeX - 800;
	dsDrawOffset_Y = WinSizeY - 600;

	if (DispInterface) sinDraw();

	dsDrawOffset_X = 0;
	dsDrawOffset_Y = 0;
#endif

	if (BellatraEffectInitFlag)		//º§¶óÆ®¶ó ÀÌÆåÆ®
		DrawBellatraFontEffect();

	lpDDSBack->GetDC(&hdc);

	SelectObject(hdc, hFont);

	//			RecvPopCount = 0;			//°­Á¦·Î ¹Þ±â ½Ãµµ Ä«¿îÆ®
	//		RecvPopErrorCount = 0;		//°­Á¦·Î ¹Þ±â ½Ãµµ ½ÇÆÐ Ä«¿îÆ®

#ifdef	_IME_ACTIVE

	////////////////////////// IME Ç¥½Ã //////////////////////////
	char *ImeStr;
	//int Num,ImeY;
	int ImeY;
	//if ( hFocusWnd ) {
	if (DisplayIME)
	{

		SetBkColor(hdc, RGB(255, 255, 200));
		SetTextColor(hdc, RGB(0, 0, 0));

		if (sinChatDisplayMode == 1)
			ImeY = 442;
		else
			ImeY = 340;

		//ImeStr = IMETest.m_szCOMPOSTR.GetBuffer( IMETest.m_szCOMPOSTR.GetLength() );
		//if ( ImeStr[0] ) 
		//	dsTextLineOut( hdc , 8,ImeY-12, ImeStr , lstrlen( ImeStr ) );
		IMETest.GetImeInfo();
		ImeStr = IMETest.m_szTitle.GetBuffer(IMETest.m_szTitle.GetLength());
		if (ImeStr[0])
			//dsTextLineOut( hdc , 100,ImeY-12, ImeStr , lstrlen( ImeStr ) );
			dsTextLineOut(hdc, 8, ImeY - 12, ImeStr, lstrlen(ImeStr));

		//wsprintf( strBuff , "%s %s", 
		//	IMETest.m_szConvMode.GetBuffer( IMETest.m_szConvMode.GetLength() ) ,
		//	IMETest.m_szHalfMode.GetBuffer( IMETest.m_szHalfMode.GetLength() ) );

		//dsTextLineOut( hdc , 300,ImeY-12, strBuff , lstrlen( strBuff ) );

		SetBkColor(hdc, RGB(255, 255, 255));
		/*
		for( cnt=0;cnt<IMETest.nCandListCount;cnt++ ) {

			ImeStr = IMETest.m_szCandList[cnt].GetBuffer( IMETest.m_szCandList[cnt].GetLength() );

			if ( cnt>=9 ) Num=0;
			else Num = cnt+1;

			wsprintf( strBuff , " %d: %s ", Num , ImeStr );

			dsTextLineOut( hdc , 344,ImeY+cnt*12, strBuff , lstrlen( strBuff ) );
		}
		*/
	}

#endif

	SetBkMode(hdc, TRANSPARENT);

#ifdef _WINMODE_DEBUG

	int	rcv1, rcv2, snd1, snd2, arcv1, brcv1, arcv2, brcv2;
	int LineY;

	if (DisplayDebug)
	{
		//wsprintf( strBuff , "%d  %d  x=%d z=%d y=%d  VRAM=%d Rcv1=%d Rcv2=%d", DispRender , DispPolyCnt , lpCurPlayer->pX>>(FLOATNS) , lpCurPlayer->pZ>>(FLOATNS) , lpCurPlayer->pY>>(FLOATNS), VramTotal , Debug_RecvCount1 , Debug_RecvCount2  );

		rcv1 = 0;
		rcv2 = 0;
		snd1 = 0;
		snd2 = 0;
		arcv1 = 0;
		brcv1 = 0;
		arcv2 = 0;
		brcv2 = 0;

		if (smWsockServer)
		{
			rcv1 = smWsockServer->RecvPacketCount;
			snd1 = smWsockServer->SendPacketCount;
			arcv1 = smWsockServer->RecvPopCount;
			brcv1 = smWsockServer->RecvPopErrorCount;
		}
		else { rcv1 = 0; snd1 = 0; }

		if (smWsockUserServer)
		{
			rcv2 = smWsockUserServer->RecvPacketCount;
			snd2 = smWsockUserServer->SendPacketCount;
			arcv2 = smWsockUserServer->RecvPopCount;
			brcv2 = smWsockUserServer->RecvPopErrorCount;
		}
		else { rcv2 = 0; snd2 = 0; }

		/*
				wsprintf( strBuff , "%d(%d) %d x=%d z=%d y=%d VRAM=%d R1=%d(%d) R2=%d(%d) S1=%d S2=%d (%d/%d) (%d/%d) Recon( %d %d )", DispRender ,DispMainLoop , DispPolyCnt , lpCurPlayer->pX>>(FLOATNS) , lpCurPlayer->pZ>>(FLOATNS) , lpCurPlayer->pY>>(FLOATNS), (VramTotal/(1024*1024)) ,
					rcv1,Debug_RecvCount1,rcv2,Debug_RecvCount2,snd1,snd2 , brcv1,arcv1, brcv2,arcv2 , ReconnServer ,ReconnDataServer );
		*/
		wsprintf(strBuff, "%d(%d) %d x=%d z=%d y=%d VRAM=%d R1=%d(%d) R2=%d(%d) S1=%d S2=%d (%d/%d) (%d/%d) RcvTurb( %d )", DispRender, DispMainLoop, DispPolyCnt, lpCurPlayer->pX >> (FLOATNS), lpCurPlayer->pZ >> (FLOATNS), lpCurPlayer->pY >> (FLOATNS), (VramTotal / (1024 * 1024)),
				 rcv1, Debug_RecvCount1, rcv2, Debug_RecvCount2, snd1, snd2, brcv1, arcv1, brcv2, arcv2, smTransTurbRcvMode);	//ReconnServer ,ReconnDataServer );

		SetTextColor(hdc, RGB(0, 0, 0));
		dsTextLineOut(hdc, 11, 11, strBuff, lstrlen(strBuff));
		SetTextColor(hdc, RGB(255, 255, 255));
		dsTextLineOut(hdc, 10, 10, strBuff, lstrlen(strBuff));

		wsprintf(strBuff, "Stage : %s , %s", szGameStageName[0], szGameStageName[1]);
		SetTextColor(hdc, RGB(0, 0, 0));
		dsTextLineOut(hdc, 11, 31, strBuff, lstrlen(strBuff));
		SetTextColor(hdc, RGB(255, 255, 255));
		dsTextLineOut(hdc, 10, 30, strBuff, lstrlen(strBuff));

		wsprintf(strBuff, "World Time (%d:%d)", dwGameHour, dwGameMin);
		SetTextColor(hdc, RGB(0, 0, 0));
		dsTextLineOut(hdc, (WinSizeX >> 1) + 1, 31, strBuff, lstrlen(strBuff));
		SetTextColor(hdc, RGB(255, 255, 255));
		dsTextLineOut(hdc, (WinSizeX >> 1), 30, strBuff, lstrlen(strBuff));


		if (lpCurPlayer->PatLoading == FALSE)
		{
			if (AdminCharMode)
				wsprintf(strBuff, "%s", lpCurPlayer->smCharInfo.szName);
			else
				wsprintf(strBuff, "%s", lpCurPlayer->lpDinaPattern->szPatName);

			SetTextColor(hdc, RGB(0, 0, 0));
			dsTextLineOut(hdc, (WinSizeX >> 1) + 1 + 120, 31, strBuff, lstrlen(strBuff));
			SetTextColor(hdc, RGB(255, 255, 255));
			dsTextLineOut(hdc, (WinSizeX >> 1) + 120, 30, strBuff, lstrlen(strBuff));

			wsprintf(strBuff, "Damage : %d", LastAttackDamage);
			SetTextColor(hdc, RGB(0, 0, 0));
			dsTextLineOut(hdc, 9, 48, strBuff, lstrlen(strBuff));
			SetTextColor(hdc, RGB(255, 255, 255));
			dsTextLineOut(hdc, 8, 47, strBuff, lstrlen(strBuff));

			wsprintf(strBuff, "RcvDamage:(%d) [%d] %d %d", Record_TotalRecvDamageCount, Record_RecvDamageCount, Record_DefenceCount, Record_BlockCount);
			SetTextColor(hdc, RGB(0, 0, 0));
			dsTextLineOut(hdc, 9, 62, strBuff, lstrlen(strBuff));
			SetTextColor(hdc, RGB(255, 255, 255));
			dsTextLineOut(hdc, 8, 61, strBuff, lstrlen(strBuff));

			if (AreaServerMode)
			{
				if (lpWSockServer_Area[0])
				{
					wsprintf(strBuff, "Area Server[0]: (%s)", lpWSockServer_Area[0]->szIPAddr);
					SetTextColor(hdc, RGB(0, 0, 0));
					dsTextLineOut(hdc, 9, 82, strBuff, lstrlen(strBuff));
					SetTextColor(hdc, RGB(255, 255, 255));
					dsTextLineOut(hdc, 8, 81, strBuff, lstrlen(strBuff));

				}
				if (lpWSockServer_Area[1])
				{
					wsprintf(strBuff, "Area Server[1]: (%s)", lpWSockServer_Area[1]->szIPAddr);
					SetTextColor(hdc, RGB(0, 0, 0));
					dsTextLineOut(hdc, 9, 96, strBuff, lstrlen(strBuff));
					SetTextColor(hdc, RGB(255, 255, 255));
					dsTextLineOut(hdc, 8, 95, strBuff, lstrlen(strBuff));

				}

				if (lpWSockServer_DispArea[0])
				{
					wsprintf(strBuff, "Area Server[0]: (%s)", lpWSockServer_DispArea[0]->szIPAddr);
					SetTextColor(hdc, RGB(0, 0, 0));
					dsTextLineOut(hdc, 209, 82, strBuff, lstrlen(strBuff));
					SetTextColor(hdc, RGB(255, 255, 255));
					dsTextLineOut(hdc, 208, 81, strBuff, lstrlen(strBuff));

				}
				if (lpWSockServer_DispArea[1])
				{
					wsprintf(strBuff, "Area Server[1]: (%s)", lpWSockServer_DispArea[1]->szIPAddr);
					SetTextColor(hdc, RGB(0, 0, 0));
					dsTextLineOut(hdc, 209, 96, strBuff, lstrlen(strBuff));
					SetTextColor(hdc, RGB(255, 255, 255));
					dsTextLineOut(hdc, 208, 95, strBuff, lstrlen(strBuff));

				}


				wsprintf(strBuff, "AreaCount(%d) AreaConn(%d) AreaIP( [%d] %d %d ) Step(%d)",
						 dwDebugAreaCount, dwDebugAreaConnCount,
						 dwDebugAreaIP[0], dwDebugAreaIP[1], dwDebugAreaIP[2], dwDebugAreaStep);

				SetTextColor(hdc, RGB(0, 0, 0));
				dsTextLineOut(hdc, 9, 108, strBuff, lstrlen(strBuff));
				SetTextColor(hdc, RGB(255, 255, 255));
				dsTextLineOut(hdc, 8, 108, strBuff, lstrlen(strBuff));

			}

		}


		SetTextColor(hdc, RGB(255, 255, 255));
		//DisplayError( hdc , 0, 48 );

/*
	wsprintf( strBuff , "ReChecked=%d" ,DEBUG_ReCheckedCnt ) ;
	SetTextColor( hdc, RGB(0, 0, 0) );
	dsTextLineOut( hdc , 11,31, strBuff , lstrlen( strBuff ) );
	SetTextColor( hdc, RGB(255, 255, 255) );
	dsTextLineOut( hdc , 10,30, strBuff , lstrlen( strBuff ) );
	DEBUG_ReCheckedCnt=0;
*/
		LineY = 48;
		for (i = 0; i < OTHER_PLAYER_MAX; i++)
		{
			if (chrOtherPlayer[i].Flag && chrOtherPlayer[i].Pattern && chrOtherPlayer[i].smCharInfo.State == smCHAR_STATE_USER)
			{
				wsprintf(strBuff, "%s", chrOtherPlayer[i].smCharInfo.szName);//, chrOtherPlayer[i].TransSock->szIPAddr , chrOtherPlayer[i].TransLastSendTime );
				SetTextColor(hdc, RGB(0, 0, 0));
				dsTextLineOut(hdc, WinSizeX - 220, LineY + 1, strBuff, lstrlen(strBuff));
				SetTextColor(hdc, RGB(255, 255, 255));
				dsTextLineOut(hdc, WinSizeX - 221, LineY, strBuff, lstrlen(strBuff));
				LineY += 16;
				if (LineY > 460) break;
			}
		}

		int	MonCharBuff[OTHER_PLAYER_MAX];
		int MonCharCnt = 0;
		int	MonCnt;

		for (i = 0; i < OTHER_PLAYER_MAX; i++)
		{
			if (chrOtherPlayer[i].Flag && chrOtherPlayer[i].Pattern && chrOtherPlayer[i].smCharInfo.State != smCHAR_STATE_USER)
			{
				MonCharBuff[MonCharCnt++] = i;
			}
		}

		LineY = 48;
		for (i = 0; i < MonCharCnt; i++)
		{
			if (MonCharBuff[i] >= 0)
			{
				MonCnt = 1;
				for (cnt = i + 1; cnt < MonCharCnt; cnt++)
				{
					if (MonCharBuff[cnt] >= 0 &&
						lstrcmp(chrOtherPlayer[MonCharBuff[cnt]].smCharInfo.szName,
								chrOtherPlayer[MonCharBuff[i]].smCharInfo.szName) == 0)
					{
						MonCharBuff[cnt] = -1;
						MonCnt++;
					}
				}

				wsprintf(strBuff, "%s x %d", chrOtherPlayer[MonCharBuff[i]].smCharInfo.szName, MonCnt);//, chrOtherPlayer[i].TransSock->szIPAddr , chrOtherPlayer[i].TransLastSendTime );
				SetTextColor(hdc, RGB(0, 0, 0));
				dsTextLineOut(hdc, WinSizeX - 380, LineY + 1, strBuff, lstrlen(strBuff));
				if (chrOtherPlayer[MonCharBuff[i]].smCharInfo.State == smCHAR_STATE_NPC)
					SetTextColor(hdc, RGB(192, 192, 255));
				else
					SetTextColor(hdc, RGB(255, 192, 192));
				dsTextLineOut(hdc, WinSizeX - 381, LineY, strBuff, lstrlen(strBuff));
				LineY += 16;
				if (LineY > 460) break;

			}
		}

	}
#endif

	//	smCHAR	*lpChar;


	//extern int sinChatDisplayMode; //{ 0- ¾Æ¹«°Íµµ ¾È±×¸®±â  1-º¸Åë  2-»óÁ¡ }
	//extern int sinChatInputMode;   //{ 0- ¾Æ¹«°Íµµ ¾È±×¸®±â  1-º¸Åë  2-»óÁ¡ }
	/*

		char *lpInput;

		if ( hFocusWnd && sinChatInputMode ) {
			GetWindowText( hTextWnd , strBuff2 , 240 );
			strBuff2[242]=0x0D;
			len = lstrlen( strBuff2 );

			SetBkMode( hdc, TRANSPARENT );

			wsprintf( strBuff, "¢º %s" , strBuff2 );

			len=lstrlen( strBuff );
			if ( len>=52 ) {
				lpInput = &strBuff[len-52];
				len=52;
			}
			else
				lpInput = strBuff;

			if ( sinChatInputMode==1 ) {
				x = 16 ; y = 582;
			}
			else {
				x = 16 ; y = 432;
			}

			SetTextColor( hdc, RGB(0, 0, 0) );
			dsTextLineOut( hdc , x+1, y+1 , lpInput , len );
			SetTextColor( hdc, RGB(255, 255, 255) );
			dsTextLineOut( hdc , x , y , lpInput , len );
		}

		if ( sinChatDisplayMode ) {
			if ( sinChatDisplayMode==1 ) {
				x = 16 ; y = 562; i = 10;
			}
			else {
				x = 16 ; y = 412; i = 6;
			}
			//Ã¤ÆÃ Á¤º¸ È­¸é¿¡ Ç¥½Ã
			DisplayChatMessage2( hdc , x , y , 52 , i );
		}
	*/
	lpDDSBack->ReleaseDC(hdc);

	/*
	char *mgDiconnect = "¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù";
	char *mgDiconnect1 = "¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù (1)";
	char *mgDiconnect2 = "¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù (2)";
	char *mgDiconnect3 = "¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù (3)";
	char *mgDiconnect4 = "¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù (4)";

	char *mgCloseGame = "°ÔÀÓÀ» Á¾·á ÇÕ´Ï´Ù";
	char *mgCloseWindow = "»ç¿ëÁßÀÎ Ã¢À» ´Ý°í Á¾·á ÇÏ½Ê½Ã¿À";
	char *mgCloseBattle = "ÀüÅõÁß¿¡´Â Á¾·áÇÒ¼ö ¾ø½À´Ï´Ù";
	*/

	if (DisconnectFlag)
	{
		//¼­¹ö¿ÍÀÇ ¿¬°á ²÷¾îÁü
		if (DisconnectServerCode == 0)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 1)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect1, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 2)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect2, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 3)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect3, 36, BOX_ONE);
		}
		if (DisconnectServerCode == 4)
		{
			DrawMessage(MidX - 64, MidY, mgDiconnect4, 36, BOX_ONE);
		}

#ifdef	_WINMODE_DEBUG
		if (!smConfig.DebugMode && !quit && ((DWORD)DisconnectFlag + 5000) < dwPlayTime) quit = TRUE;
#else
		if (!quit && ((DWORD)DisconnectFlag + 5000) < dwPlayTime) quit = TRUE;
#endif

	}
	else
	{
		if (quit)
			DrawMessage(MidX - 40, MidY, mgCloseGame, 36, BOX_ONE);
		else
		{
			if (dwCloseBoxTime && dwCloseBoxTime > dwPlayTime)
			{
				DrawMessage(MidX - 100, MidY, mgCloseWindow, 36, BOX_ONE);
			}
			else
			{
				if (dwBattleQuitTime)
				{
					if ((dwBattleQuitTime + 5000) > dwPlayTime)
						DrawMessage(MidX - 40, MidY, mgCloseBattle, 36, BOX_ONE);
					else
						dwBattleQuitTime = 0;
				}
			}
		}
	}


	//int CloseEachPlayer();					//È®´ë Ä³¸¯ÅÍ ´Ý±â
	/*
		lpD3DDevice->BeginScene();
		smRender.ClearLight();
		smRender.Color_A = 0;
		smRender.Color_R = 0;
		smRender.Color_G = 0;
		smRender.Color_B = 0;
		DrawEffect(0,0,0, 0,0,0 );					//È£ÀÌÆåÆ® ±×¸®±â
		lpD3DDevice->EndScene();
	*/
	DarkLevel = BackDarkLevel;

	if (Flip() == FALSE)
	{
		DisconnectFlag = dwPlayTime;
		quit = 1;
	}

	return TRUE;
}




void _stdcall smPlayD3D(int x, int y, int z, int ax, int ay, int az)
{

	eCAMERA_TRACE	eTrace;
	int	ap;

	//	xr+=.001;


	DispPolyCnt = 0;

	if (lpDDSBack->IsLost() == DDERR_SURFACELOST || lpDDSPrimary->IsLost() == DDERR_SURFACELOST)
	{
		//¼­ÆäÀÌ½º ·Î½ºÆ® ¿¬°á Á¾·á
		DisconnectFlag = dwPlayTime;
		quit = TRUE;

		lpDDSPrimary->Restore();
		lpDDSBack->Restore();
		RestoreFlag = TRUE;
		return;
	}
	else
	{
		if (RestoreFlag)
		{
			RestoreAll();
			RestoreFlag = FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////
	//¸Þ¸ð¸® Ã½Å©
	//CheckKeepMem( 70 );
	//////////////////////////////////////////////////////////////////////

	SetRendSight();		//·»´õ¸µ ½Ã¾ß ¼³Á¤

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (g_IsDxProjectZoomIn <= 0)
		DrawGameState();
	else
		VirtualDrawGameState();
	//######################################################################################

	if ((dwDebugBack^dwDebugXor) == 0)
	{
		if (smConfig.DebugMode)
		{
			//ÇØÅ· ½ÃµµÇÑ À¯Àú ÀÚµ¿ ½Å°í
			SendSetHackUser(TRUE);
			smConfig.DebugMode = 0;
		}
	}


	int Mapfl;
	int cy;
	int mapY;

	Mapfl = 0;
	cy = y + 16 * fONE;

	if (!DebugPlayer)
	{
		if (smGameStage[0])
		{
			mapY = (smGameStage[0]->GetHeight(x, z));
			if (y < mapY)
			{
				y = mapY;
				y += 8 << FLOATNS;
			}
			if (mapY > CLIP_OUT) Mapfl++;
		}
		if (smGameStage[1])
		{
			mapY = (smGameStage[1]->GetHeight(x, z));
			if (y < mapY)
			{
				y = mapY;
				y += 8 << FLOATNS;
			}
			if (mapY > CLIP_OUT) Mapfl++;
		}
	}

	Mix_CodeVram();		//ÄÚµå ¹öÆÛ ¹Í¼­

	smRender.Color_R = BrCtrl;
	smRender.Color_G = BrCtrl;
	smRender.Color_B = BrCtrl;
	smRender.Color_A = BrCtrl;


	//SetDynLight( lpCurPlayer->pX,lpCurPlayer->pY+32*fONE,lpCurPlayer->pZ , 100, 100,100,0, 200 );
	smRender.Color_R = -DarkLevel + BackColor_R;
	smRender.Color_G = -DarkLevel + BackColor_G;
	smRender.Color_B = -DarkLevel + BackColor_B;


	if (dwM_BlurTime && IsCreateNewRenderTarget())
	{
		if (dwM_BlurTime < dwPlayTime)
		{
			dwM_BlurTime = 0;
			SetFilterEffect(FILTER_EFFECT_SET_BRIGHT_CONTRAST, 160);
			SetFilterEffect(FILTER_EFFECT_DEL_MOTION_BLUR, 0);
		}
	}



	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (GetFilterEffect())
	{
		BeginRender();
		ChangeRenderTarget(NEW_TARGET_BACK);
	}
	//######################################################################################


	BeginRender();
	/*
		//######################################################################################
		//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
		if( g_IsDxProjectZoomIn )
		{
			DrawFullZoomMap();
			lpD3DViewport->Clear2( 1UL, &D3DRect, D3DCLEAR_ZBUFFER, smBackColor, 1, 0L );
		}
		//######################################################################################
	*/
	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	y += (32 << FLOATNS);

	if (anx <= 40 && dist <= 100)
		y -= ((110 - dist) << FLOATNS);
	//######################################################################################

	TraceTargetPosi.x = x;
	TraceTargetPosi.y = y;
	TraceTargetPosi.z = z;

	if (AutoCameraFlag)
	{

		TraceCameraMain();

		x = TraceCameraPosi.x;
		y = TraceCameraPosi.y;
		z = TraceCameraPosi.z;
	}

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	int ey = lpCurPlayer->pY + (32 * fONE);
	if (anx <= 40 && dist <= 100)
		ey += ((100 - dist) * fONE);


	ActionGameMode = FALSE;
	/*
			x = lpCurPlayer->pX;
			y = StageField[ lpCurPlayer->OnStageField ]->ActionCamera.FixPos.y*fONE;
			z = StageField[ lpCurPlayer->OnStageField ]->ActionCamera.FixPos.z*fONE;

			if ( x<StageField[ lpCurPlayer->OnStageField ]->ActionCamera.LeftX*fONE ) x= StageField[ lpCurPlayer->OnStageField ]->ActionCamera.LeftX*fONE;
			else if ( x>StageField[ lpCurPlayer->OnStageField ]->ActionCamera.RightX*fONE ) x=StageField[ lpCurPlayer->OnStageField ]->ActionCamera.RightX*fONE;

			MakeTraceMatrix( &eTrace, x,y,z, lpCurPlayer->pX, lpCurPlayer->pY , (StageField[ lpCurPlayer->OnStageField ]->ActionCamera.FixPos.z+290)*fONE );
	*/


	if (lpCurPlayer && lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField]->State == FIELD_STATE_ACTION)
	{
		//È¾ ½ºÅ©·Ñ ¾×¼Ç ¸ðµå
		x = lpCurPlayer->pX;
		y = StageField[lpCurPlayer->OnStageField]->ActionCamera.FixPos.y + 80 * fONE;
		z = StageField[lpCurPlayer->OnStageField]->ActionCamera.FixPos.z*fONE;

		if (x<StageField[lpCurPlayer->OnStageField]->ActionCamera.LeftX*fONE) x = StageField[lpCurPlayer->OnStageField]->ActionCamera.LeftX*fONE;
		else if (x>StageField[lpCurPlayer->OnStageField]->ActionCamera.RightX*fONE) x = StageField[lpCurPlayer->OnStageField]->ActionCamera.RightX*fONE;

		MakeTraceMatrix(&eTrace, x, y, z, x, lpCurPlayer->pY, 326711 * fONE);

		/*
				x = lpCurPlayer->pX;
				y = 213140+80*fONE;
				z = (36711-290)*fONE;

				MakeTraceMatrix( &eTrace, x,y,z, lpCurPlayer->pX, lpCurPlayer->pY , 326711*fONE );
		*/
		ax = eTrace.AngX;
		ay = eTrace.AngY;
		smRender.OpenCameraPosi(x, y, z, &eTrace.eRotMatrix);

		ActionGameMode = TRUE;
	}
	else if (!DebugPlayer)
	{
		MakeTraceMatrix(&eTrace, x, y, z, lpCurPlayer->pX, ey, lpCurPlayer->pZ);

		ax = eTrace.AngX;
		ay = eTrace.AngY;
		smRender.OpenCameraPosi(x, y, z, &eTrace.eRotMatrix);
	}
	//######################################################################################




	smRender.ClearLight();

	if (DarkLevel > 0)
	{

		ap = DarkLevel + (DarkLevel >> 2);

		if (StageField[lpCurPlayer->OnStageField]->State == FIELD_STATE_DUNGEON)
		{
			DarkLightRange = 400 * fONE;		//320
			ap = DarkLevel;
		}
		else
			DarkLightRange = 260 * fONE;		//220
			//DarkLightRange = 180*fONE;


		if (lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField]->State == FIELD_STATE_VILLAGE)
		{
			//ap = (ap*190)>>8;
			//smRender.AddDynamicLight( lpCurPlayer->pX,lpCurPlayer->pY+32*fONE,lpCurPlayer->pZ, ap,ap,ap,0, DarkLightRange );
		}
		else
			smRender.AddDynamicLight(lpCurPlayer->pX, lpCurPlayer->pY + 32 * fONE, lpCurPlayer->pZ, ap, ap, ap, 0, DarkLightRange);
	}

	DynLightApply();	//µ¿Àû ±¤¿ø ·»´õ·¯¿¡ Àû¿ë


	//DrawBG( x,  y,  z,  ax, ay, az );	
	DrawSky(x, y, z, ax, ay, az);

	smRender.DeviceRendMode = FALSE;
	lpD3DDevice->BeginScene();

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (smRender.m_FogIsRend && smRender.m_FogMode)
		lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
	//######################################################################################

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	smRender.bApplyRendObjLight = TRUE;
	DrawPat3D(x, y, z, ax, ay, az);

	smRender.bApplyRendObjLight = FALSE;
	NumPoly = DisplayStage(x, y, z, ax, ay, az);

	smRender.bApplyRendObjLight = TRUE;
	DrawPat3D_Alpha();

	smRender.ClearObjLight();
	//######################################################################################

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (smRender.m_FogIsRend && smRender.m_FogMode)
		lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
	//######################################################################################

	smRender.ClearLight();
	smRender.Color_A = 0;
	smRender.Color_R = 0;
	smRender.Color_G = 0;
	smRender.Color_B = 0;

	//±×¸²ÀÚ ±×¸®±â
	DrawPatShadow(x, y, z, ax, ay, az);

	lpD3DDevice->EndScene();
	smRender.DeviceRendMode = TRUE;

	//ÅØ½ºÃÄ ¿É¼ÇÀ» ±âº»°ªÀ¸·Î
	RestoreInterfaceTexture();

	DrawEffect(x, y, z, ax, ay, az);					//È£ÀÌÆåÆ® ±×¸®±â
	cSin3D.Draw(x, y, z, ax, ay, az);
	DrawPat2D(x, y, z, ax, ay, az);


	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (GetFilterEffect() && GetRenderTarget() == NEW_TARGET_BACK)
	{
		SetNewTargetTextureState();
		lpD3DDevice->BeginScene();
		RenderFilterEffect();
		lpD3DDevice->EndScene();
		DefaultsNewTargetTextureState();
	}
	//######################################################################################

	//######################################################################################
	//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
	if (g_IsDxProjectZoomIn)
	{
		DrawFullZoomMap();
	}
	else
	{
		if (cInterFace.sInterFlags.MapOnFlag)
		{
			//ÇÊµå ¸Ê ±×¸®±â
			if (DispInterface) DrawFieldMap();
		}
	}
	//######################################################################################

	if ((!MsTraceMode && MouseButton[0] == 0) ||
		lpCurPlayer->smCharInfo.Stamina[0] > (lpCurPlayer->smCharInfo.Stamina[1] >> 2))
	{
		//°È±â ¶Ù±â ¸ðµå
		lpCurPlayer->MoveMode = cInterFace.sInterFlags.RunFlag;

		if (ActionGameMode)
		{
			lpCurPlayer->MoveMode = ActionDashMode;
		}

	}
	else
	{
		if (lpCurPlayer->smCharInfo.Stamina[0] == 0)
			lpCurPlayer->MoveMode = FALSE;
	}

	/*
		if ( lpCurPlayer->smCharInfo.Stamina[0] ) {
			//°È±â ¶Ù±â ¸ðµå
			lpCurPlayer->MoveMode = cInterFace.sInterFlags.RunFlag;
		}
		else {
			lpCurPlayer->MoveMode = FALSE;
		}
	*/
	smRender.CloseCameraPosi();

	/*
		if ( GoText ) {
			TestSetNewText( strBuff2 ) ;
			GoText =0;
		}
	*/

	//	smRender.RenderD3D();

	Disp_tx = MsSelPos.x - 32;
	Disp_ty = MsSelPos.y;


	EndRender();
	//ÅØ½ºÃÄ ÇÁ·¹ÀÓ Áõ°¡
	IncTextureFrame();


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

//ÄÚ¸àµå ¶óÀÎ ºÐ¼® ¼³Á¤
int DecodeCmdLine(char *lpCmdLine)
{
	char *lpChar;
	int	cnt;
	//	char lpBuff[32];

	lpChar = CompCmdStr(lpCmdLine, "/reload=");
	if (lpChar)
	{
		for (cnt = 0; cnt < 16; cnt++)
		{
			if (lpChar[cnt] == ' ' || lpChar[cnt] == '&' || lpChar[cnt] == 0) break;
		}
		if (atoi(lpChar) == 0)
		{
			smSetMeshReload(0, 1);
		}
	}

	return TRUE;
}



//szAppName
//sCraftItem_Info



char *RegPath_3DMax = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\3D Studio MAX R3.1L";
char *RegPath_3DMax2 = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\3D Studio MAX R3.1";
char *RegPath_Photoshop = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Adobe Photoshop 6.0";
char *RegPath_ACDSee = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ACDSee";

//ÆùÆ® »ý¼º
int SetCreateFont()
{

#ifdef _LANGUAGE_ENGLISH
#ifdef	_CUSTOM_FONT
	hFont = CreateFont(16,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   DEFAULT_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   VARIABLE_PITCH,
					   "Comic Sans MS");
#else
	hFont = CreateFont(16,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   DEFAULT_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   VARIABLE_PITCH,
					   "Comic Sans MS");

#endif
#endif

#ifdef _LANGUAGE_TAIWAN
	hFont = CreateFont(12,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   CHINESEBIG5_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   VARIABLE_PITCH,
					   "MingLiu");
#endif


#ifdef _LANGUAGE_JAPANESE

#include "Japanese\\j_font.h"

	hFont = CreateFont(12,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   SHIFTJIS_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   FIXED_PITCH | FF_MODERN,
					   j_font);
#endif

#ifdef _LANGUAGE_CHINESE

	hFont = CreateFont(12,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   GB2312_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   VARIABLE_PITCH,
					   "SimSun");
#endif

#ifdef _LANGUAGE_THAI
#ifdef _CUSTOM_FONT
	/*#ifdef	_ZHOON_TEST
		hFont = CreateFont(z_fonttest.setdata[0].z_nHeight,
							z_fonttest.setdata[0].z_nWidth,
							z_fonttest.setdata[0].z_nEscapement,
							z_fonttest.setdata[0].z_nOrientation,
							z_fonttest.setdata[0].z_nWeight,
							z_fonttest.setdata[0].z_bItalic,
							z_fonttest.setdata[0].z_bUnderline,
							z_fonttest.setdata[0].z_cStrikeOut,
							z_fonttest.setdata[0].z_nCharSet,
							z_fonttest.setdata[0].z_nOutPrecision,
							z_fonttest.setdata[0].z_nClipPrecision,
							z_fonttest.setdata[0].z_nQuality,
						  z_fonttest.setdata[0].z_nPitchAndFamily,
							z_fonttest.setdata[0].z_lpszFacename);
	#else*/
	hFont = CreateFont(13,
					   0,
					   0,
					   0,
					   FW_THIN,
					   FALSE,
					   FALSE,
					   FALSE,
					   THAI_CHARSET,
					   OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   ANTIALIASED_QUALITY,
					   DEFAULT_PITCH | FF_DONTCARE,
					   "MS Sans Serif");
	//#endif

#else
	hFont = CreateFont(13,
					   0,
					   0,
					   0,
					   FW_THIN,
					   FALSE,
					   FALSE,
					   FALSE,
					   THAI_CHARSET,
					   OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   ANTIALIASED_QUALITY,
					   FIXED_PITCH | FF_MODERN,
					   "FreesiaUPC");
#endif					
#endif
#ifdef _LANGUAGE_BRAZIL	
	hFont = CreateFont(16,
					   0,
					   0,
					   0,
					   FW_THIN,
					   FALSE,
					   FALSE,
					   FALSE,
					   ANSI_CHARSET | FS_LATIN1,
					   OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   ANTIALIASED_QUALITY,
					   FIXED_PITCH | FF_MODERN,
					   "Comic Sans MS");
#endif					

#ifdef _LANGUAGE_ARGENTINA
	hFont = CreateFont(16,
					   0,
					   0,
					   0,
					   FW_THIN,
					   FALSE,
					   FALSE,
					   FALSE,
					   EASTEUROPE_CHARSET,
					   OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   ANTIALIASED_QUALITY,
					   FIXED_PITCH | FF_MODERN,
					   "Tahoma");
#endif

#ifdef _LANGUAGE_KOREAN

	hFont = CreateFont(12,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   HANGEUL_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   VARIABLE_PITCH,
					   "±¼¸²Ã¼");
#endif
#ifdef _LANGUAGE_VEITNAM

	hFont = CreateFont(14,
					   0,
					   0,
					   0,
					   FW_NORMAL,
					   FALSE,
					   FALSE,
					   FALSE,
					   VIETNAMESE_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   VARIABLE_PITCH,
					   "Tahoma");
#endif

	return TRUE;

}

//Ã¤ÆÃÃ¢ ¹®ÀÚ ¼³Á¤¹× È°¼ºÈ­
int	SetChatingLine(char *szMessage)
{

	cInterFace.ChatFlag = TRUE;
	hFocusWnd = hTextWnd;
	SetWindowText(hFocusWnd, szMessage);
	SetIME_Mode(1);		//IME ¸ðµå ÀüÈ¯

	//ImmSetConversionStatus( hImc , IME_CMODE_NATIVE,IME_CMODE_NATIVE );
	//ImmSetConversionStatus( hImcEdit , IME_CMODE_NATIVE,IME_CMODE_NATIVE );

	return TRUE;
}

//°ÔÀÓ¿¡¼­ÀÇ À©µµ ¸Þ¼¼Áö Ã³¸®
DWORD GameWindowMessage(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	int stm, cnt;

	switch (messg)
	{

		//case WM_IME_KEYDOWN:
		case WM_KEYDOWN:

			if (GameMode == 2)
			{

				if (wParam == VK_RETURN && VRKeyBuff[wParam] == 0 && !sinMessageBoxShowFlag &&
					!VRKeyBuff[VK_CONTROL])
				{

					SetChatingLine("");
				}

				if (wParam == VK_BACK && VRKeyBuff[wParam] == 0)
				{
					if (HoMsgBoxMode)
						HoMsgBoxMode = 0;
					else
						HoMsgBoxMode = 1;

					SetMessageFrameSelect(HoMsgBoxMode);
				}

				if (wParam == 'M' && VRKeyBuff[wParam] == 0)
				{
					//¾È³»Áöµµ ¶ç¿ì±â
					sinCheck_ShowHelpMap();
				}

				if (dwYahooTime > dwPlayTime && dwYahooTime < (dwPlayTime + 60 * 1000))
				{
					if (wParam == VK_SPACE && VRKeyBuff[wParam] == 0)
					{
						if (lpCurPlayer && lpCurPlayer->MotionInfo)
						{
							stm = sinGetStamina();
							cnt = (lpCurPlayer->smCharInfo.Stamina[1] * 3) / 100;
							if (lpCurPlayer->MotionInfo->State<0x100 && stm>cnt)
							{
								if (lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_YAHOO))
								{
									//lpCurPlayer->HideWeapon = TRUE;
									if (cnt > 0) sinSetStamina(stm - cnt);
								}
							}
						}
						VRKeyBuff[wParam] = 1;
						return FALSE;
					}
				}

				//######################################################################################
				//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
				if (smRender.m_GameFieldView && !VRKeyBuff[wParam])
				{
					if (wParam == VK_ADD)
					{
						if (smRender.m_GameFieldViewStep < 22)
						{
							smRender.m_GameFieldViewStep++;
							//smRender.SetGameFieldViewStep();
							RendSightSub(1);
						}
					}
					else if (wParam == VK_SUBTRACT)
					{
						if (smRender.m_GameFieldViewStep > 1)
						{
							smRender.m_GameFieldViewStep--;
							//smRender.SetGameFieldViewStep();
							RendSightSub(1);
						}
					}
				}
				//######################################################################################



#ifdef _WINMODE_DEBUG

				if (smConfig.DebugMode)
				{
					//µð¹ö±ë¿ë ±â´É
					if (wParam == VK_F9 && VRKeyBuff[VK_F9] == 0)
					{
						if (DisplayDebug) DisplayDebug = FALSE;
						else DisplayDebug = TRUE;
					}
					/*
						E_BL_LODING		   = 0,
						E_BL_CHANGE_TRUE   = 1,
						E_BL_CHANGE_FALSE  = 2,
						E_BL_FONT_ROUND    = 3,
						E_BL_FONT_COUNT_1  = 4,
						E_BL_FONT_COUNT_2  = 5,
						E_BL_FONT_COUNT_3  = 6,
						E_BL_FONT_COUNT_4  = 7,
						E_BL_FONT_COUNT_5  = 8,
						E_BL_FONT_COUNT_6  = 9,
						E_BL_FONT_COUNT_7  = 10,
						E_BL_FONT_START    = 11,
						E_BL_FONT_STAGE    = 12,
						E_BL_FONT_COMPLETE = 13,
						E_BL_FONT_FAIL	   = 14,
					*/
					if (VRKeyBuff[VK_CONTROL] && wParam == 'E' && VRKeyBuff['E'] == 0)
					{
						//¿¬°á È¿°úÀ½ »ðÀÔ
						//PlayBGM_Direct( BGM_CODE_SOD1 );
/*
						if ( !BellatraEffectInitFlag ) {
							CreateBellatraFontEffect();
							BellatraEffectInitFlag = TRUE;
						}

						SetBellatraFontEffect(E_BL_CHANGE_TRUE);
*/
/*
extern char	szSOD_String[64];
extern BYTE	bSOD_StringColor[4];
extern int	SOD_StringCount;
*/
						lstrcpy(szSOD_String, "Score up 5000 pts");
						bSOD_StringColor[0] = 128;
						bSOD_StringColor[1] = 255;
						bSOD_StringColor[2] = 128;
						SOD_StringCount = 256;
					}


					//¸ó½ºÅÍ »ý¼º
					//int SendOpenMonster( int State )
					//if ( lpCurPlayer->smCharInfo.State!=smCHAR_STATE_USER && VRKeyBuff[wParam]==0 ) {
/*
					if ( VRKeyBuff[VK_CONTROL] && VRKeyBuff[wParam]==0 ) {
						//¸ó½ºÅÍ »ý¼º
						if ( wParam == '6' ){
								SendOpenMonster( -1 );				//·£´ý
						}
						//¸ó½ºÅÍ »ý¼º
						if ( wParam == '7' ){
								SendOpenMonster( 0 );
						}
						//¸ó½ºÅÍ »ý¼º
						if ( wParam == '8' ) {
								SendOpenMonster( 1 );
						}
						//¸ó½ºÅÍ »ý¼º
						if ( wParam == '9' ) {
								SendOpenMonster( 2 );
						}
						//¸ó½ºÅÍ »ý¼º
						if ( wParam == '0' ) {
								SendOpenMonster( 3 );
						}
						//¸ó½ºÅÍ »ý¼º
						if ( wParam == 189 ) {//'-' ) {
								SendOpenMonster( 4 );
						}
					}
*/
//######################################################################################
//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
//if ( VRKeyBuff[VK_SHIFT] ) {
					if (VRKeyBuff[VK_SHIFT] && !VRKeyBuff[VK_CONTROL])
					{
						//######################################################################################					
						if (wParam == VK_F5 && VRKeyBuff[VK_F5] == 0)
						{
							SkipNextField = 1;
						}
						if (wParam == VK_F6 && VRKeyBuff[VK_F6] == 0)
						{
							SkipNextField = 2;
						}

						//¾Èº¸ÀÌ´Â º® Ç¥½Ã ºñÇ¥½Ã
						if (wParam == VK_F7 && VRKeyBuff[VK_F7] == 0)
						{
							if (smRender.dwMatDispMask)
								smRender.dwMatDispMask = 0;
							else
								smRender.dwMatDispMask = sMATS_SCRIPT_NOTVIEW;
						}
						/*
												if ( wParam==VK_F11 && VRKeyBuff[wParam]==0 ) {
													if ( DispInterface==0 ) DispInterface=TRUE;
													else DispInterface=FALSE;
												}
						*/

						if (wParam == VK_F8 && VRKeyBuff[wParam] == 0)
						{
							//Å¬·£ Á¤º¸ Ç¥½Ã
							if (ktj_imsiDRAWinfo) ktj_imsiDRAWinfo = 0;
							else ktj_imsiDRAWinfo = 1;
						}
					}

					if (wParam == VK_F11 && VRKeyBuff[wParam] == 0)
					{
						if (DispInterface == 0) DispInterface = TRUE;
						else DispInterface = FALSE;
					}



					if (AdminCharMode)
					{
						if (wParam == VK_INSERT && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
						{
							//½ÃÀÛ ÁöÁ¡ Ãß°¡
							SendAdd_Npc(lpCurPlayer, 0);
						}
						if (wParam == VK_DELETE && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
						{
							//½ÃÀÛ ÁöÁ¡ Ãß°¡
							if (lpCharSelPlayer)
							{
								//NPC Ä³¸¯ÅÍ Á¦°Å
								SendDelete_Npc(lpCharSelPlayer);
							}
						}
					}
					else
					{
						if (wParam == VK_INSERT && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
						{
							//½ÃÀÛ ÁöÁ¡ Ãß°¡
							SendAddStartPoint(lpCurPlayer->pX, lpCurPlayer->pZ);
						}

						if (wParam == VK_DELETE && VRKeyBuff[wParam] == 0 && VRKeyBuff[VK_CONTROL])
						{
							//½ÃÀÛ ÁöÁ¡ Ãß°¡
							if (lpSelItem)
							{
								SendDeleteStartPoint(lpSelItem->pX, lpSelItem->pZ);
							}
						}
					}
				}
#endif

			}
			break;
	}

	return TRUE;
}



//Äù½ºÆ® ¸Þ¼¼Áö º¸µå ¼³Á¤ ÇÏ´Â°÷
#include "sinbaram\\HaQuestBoard.h"




//Äù½ºÆ® ½ÃÀÛ
int	StartQuest_Code(DWORD wCode)
{
	SetQuestBoard();


	if (InterfaceParty.PartyPosState == PARTY_NONE)
	{
		//Äù½ºÆ® Ã¢À» º¸¿©ÁØ´Ù 
		ShowQuest();
		InterfaceParty.quest_Sel_Progress();	//Äù½ºÆ®ÁøÇà¹öÆ°´©¸¥°É·Î ¼ÂÆÃÇÔ.
	}

	return TRUE;
}

//Äù½ºÆ® ¿Ï·á
int EndQuest_Code(DWORD wCode)
{
	//Á¾·áµÈ Äù½ºÆ® ±â·Ï Ãß°¡
	Record_LastQuest((WORD)wCode);
	SetQuestBoard();

	return TRUE;
}

//ÇØ¿Ü¿ë ÇÔ¼ö
int HoInstallFont()
{

	//ÆùÆ®¸¦ µî·ÏÇÑ´Ù
#ifdef	_LANGUAGE_ENGLISH		//C7
	AddFontResource("ptz.ttf");
#endif

#ifdef	_LANGUAGE_THAI		//C7
	/*#ifdef	_ZHOON_TEST
		AddFontResource(z_fonttest.z_FontFileName);
	#else*/
	AddFontResource("ssee874.fon");
	//#endif	
#endif



	//·¹Áö½ºÆ®¸®¿¡ µî·ÏÇÑ´Ù.
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	char szKey[255];
	GetVersionEx(&vi);
	if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		strcpy_s(szKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
	else
		strcpy_s(szKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts");

#ifdef	_LANGUAGE_ENGLISH		//C7
	SetRegString(HKEY_LOCAL_MACHINE, szKey, "ptz", "ptz.ttf");
#endif
#ifdef	_LANGUAGE_THAI	
	/*#ifdef	_ZHOON_TEST
		SetRegString(HKEY_LOCAL_MACHINE, szKey, z_fonttest.z_FontName,z_fonttest.z_FontFileName);
	#else*/
	SetRegString(HKEY_LOCAL_MACHINE, szKey, "MS Sans Serif", "ssee874.fon");
	//#endif	
#endif


	return TRUE;
}

//ÇØ¿Ü¿ë ÇÔ¼ö
//ÆùÆ® »èÁ¦ÇÏ±â
int HoUninstallFont()
{
	//ÆùÆ®¸¦ Á¦°ÅÇÑ´Ù.
#ifdef	_LANGUAGE_ENGLISH		//C7
	RemoveFontResource("ptz.ttf");
#endif
#ifdef	_LANGUAGE_THAI		//C7
	/*#ifdef	_ZHOON_TEST
		RemoveFontResource(z_fonttest.z_FontFileName);
	#else*/
	RemoveFontResource("ssee874.fon");
	//#endif
#endif	

	//·¹Áö½ºÆ®¸® °ªÀ» Áö¿î´Ù.
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	char szKey[255];
	GetVersionEx(&vi);
	if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		strcpy_s(szKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
	else
		strcpy_s(szKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts");
	HKEY key;
	DWORD dwDisp;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, NULL,
				   REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);

#ifdef	_LANGUAGE_ENGLISH		//C7
	RegDeleteValue(key, "ptz");
#endif
#ifdef	_LANGUAGE_THAI		//C7
	/*#ifdef	_ZHOON_TEST
		RegDeleteValue(key,z_fonttest.z_FontName);
	#else*/
	RegDeleteValue(key, "MS Sans Serif");
	//#endif	
#endif		

	RegCloseKey(key);

	return TRUE;
}
