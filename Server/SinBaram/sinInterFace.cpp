/*----------------------------------------------------------------------------*
*	ÆÄÀÏ¸í :  sinInterFace.cpp
*	ÇÏ´ÂÀÏ :  ÀÎÅÍÆäÀÌ½º¿¡ °üÇÑ ¸ðµç°É °ü¸®ÇÑ´Ù
*	ÀÛ¼ºÀÏ :  ÃÖÁ¾¾÷µ¥ÀÌÆ® 12¿ù
*	Àû¼ºÀÚ :  ¹Ú»ó¿­
*-----------------------------------------------------------------------------*/

#include "sinLinkHeader.h"
#include "..\\tjboy\\clanmenu\\tjclan.h"
#include "..\\field.h"
/*----------------------------------------------------------------------------*
*								Àü¿ªº¯¼ö
*-----------------------------------------------------------------------------*/
cINTERFACE	cInterFace;
sSHADOWSTATE sShadowState[3];

int sinBmpLength[5] = { 0,0,0,0,0 }; //¶óÀÌÇÁ ¸¶³ª ½ºÅ×¹Ì³ª °æÇèÄ¡ 

////////////////Ã¤ÆÃ°ü·Ã Flag
int sinChatEnter = 0;         //Ã¤ÆÃ ÇÃ·¢ (¿£ÅÍÅ°¸¦ ÀÔ·ÂÇÏ¸é Ã¤ÆÃ Ã¢ÀÌ ³ª¿Â´Ù )
int SubChatHeight = 0;	      //Ã¤ÆÃÃ¢ º¯°æ À§Ä¡ ÁÂÇ¥ °ª 	
int sinChatDisplayMode = 0;   //ÇöÀç Ã¤ÆÃÀÇ ¸ðµå 
int sinChatInputMode = 0;     //Ã¤ÆÃ ÀÔ·Â ¸ðµå 
////////////////

/////////////Test Key Msg
int TestKeyAniFlag = 0;

////////////////¹öÆ°°ü·Ã Flag
int ExitButtonClick = 0;
int HelpButtonClick = 0;
int MouseButtonPosi = 0;
int sinMenuBottonPosi = 0;
int sinRunCameraMapPosi = 0; //°È±â Ä«¸Þ¶ó ¸Ê 
int SunandMoonFlag[2] = { 0,0 }; //0 ÇØ 1 ´Þ 
int InterButtonDown = 0;  //ÀÎÅÍÆäÀÌ½º ¹öÆ° ´Ù¿î ÇÃ·¢ 
int sinFireShow = 1;      //ºÒÀ» º¸¿©ÁØ´Ù 

////////////////½ºÅ³ °ÔÀÌÁö (ÇöÀç´Â »ç¿ëµÇ°í ÀÖÁö ¾Ê´Ù )
int ReSizelpGage = 0;     //°ÔÀÌÁö ±×¸²À» Á¶Á¤ÇÑ´Ù 


////////////////
int InitStateFlag = 0;    //ÃÊ±âÈ­ ÇÃ·¢ 


//////////////// ¹é¾÷ Æ÷ÀÎÅÍ 
char szInfoMsgBuff5[512];  //»ý¸í·Â ±â·Â ±Ù·Â °æÇèÄ¡ ÀÇ ¼öÄ¡ ±â·Ï ¹öÆÛ 
char szInfoMsgBuff6[128];  //»ý¸í·Â ±â·Â ±Ù·Â °æÇèÄ¡ ÀÇ ¼öÄ¡ ±â·Ï ¹öÆÛ 
POINT InfoMessageBoxPos;  //ÅØ½º¸¦ Ç¥½ÃÇÒ ÁÂÇ¥ 


//////////////// Àü»ç (´Ù½Ã½ÃÀÛ )
int ReStartFlag = 0;  //°ªÀÌ µé¾î¿À¸é ´Ù½Ã½ÃÀÛ ÀÎÅÍÆäÀÌ½º¸¦ º¸¿©ÁØ´Ù 
POINT ReStartMainXY = { 0,0 }; //´Ù½Ã½ÃÀÛ ÁÂÇ¥ 
int ReStartIndex = 0;
POINT ReStartTextXY = { 0,0 };
char szReStartMsg[128];
char szReStartMsg2[128];

int RestartCheckExp = 0;
int ReStartOptionIndex = 0;
int ShowExpPercentFlag = 0;

//////////////// ½ºÅ³ Àü¾÷ Á¾·á  (´Ù½Ã½ÃÀÛ )

int SkillNpcFlag = 0;  //°ªÀÌ µé¾î¿À¸é ´Ù½Ã½ÃÀÛ ÀÎÅÍÆäÀÌ½º¸¦ º¸¿©ÁØ´Ù 
POINT SkillMasterMainXY = { 0,0 }; //´Ù½Ã½ÃÀÛ ÁÂÇ¥ 
int SkillMasterIndex = 0;
POINT SkillMasterTextXY = { 0,0 };
char szSkillMasterMsg[128];
char szSkillMasterMsg2[128];
int SkillMasterOptionIndex = 0;

////////½ºÅ³ »ç¿ë ºÒÃæÁ· ÀÌÆåÆ® 
int NotUseSkillEffect[3] = { 0,0,0 };

int SkillMouseButtonPosi[2] = { 0,0 };

/////////// ³· ¹ã
int DayOrNightFlag = 0;
int sinGageTime2 = 0;


int ReStartButtonRect[3][4] = {
	{21,22,21 + 44,22 + 44},			//ÇÊµå¿¡¼­ ½ÃÀÛ 
	{72,22,72 + 44,22 + 44},			//¸¶À»¿¡¼­ ½ÃÀÛ 
	{123,22,123 + 44,22 + 44},			//°ÔÀÓ Á¾·á 

};

int ButtonRect[6][5] = {   //¹Ú½º ¿ÀÇÂ ¹öÆ° 
	{648,560,648 + 25,560 + 27},
	{673,560,673 + 25,560 + 27},
	{698,560,698 + 25,560 + 27},
	{723,560,723 + 25,560 + 27},
	{748,560,748 + 25,560 + 27},
	{772,560,772 + 25,560 + 27},

};
/*
int ButtonRect[6][5]= {   //¹Ú½º ¿ÀÇÂ ¹öÆ°
	{655,566,678,598},
	{678,566,701,598},
	{701,566,724,598},
	{724,566,747,598},
	{747,566,770,598},
	{770,566,792,598},

};
*/
int sinMenuRect[3][4] = {  //°ÔÀÓ Á¾·á ....
	{692,492,784,511},
	{692,517,784,536},
	{692,542,784,561}, //Á¾·á 


};

int sinRunCameraMap[3][4] = { //°È±â , Ä«¸Þ¶ó , ¸Ê 
	{569,555,569 + 26,555 + 26},
	{569 + 26,555,569 + 26 + 26,555 + 26},
	{569 + 26 + 26,555,569 + 26 + 26 + 26,555 + 26},

};


// Àü¾÷ °ü·Ã 
int ChangeJobButtonclick = 0;


//PK°ü·Ã
int sinPKWarningExpFlag = 0;
int sinPKWarnigExpTime = 0;
LPDIRECTDRAWSURFACE4 lpGrowInterLeft;
LPDIRECTDRAWSURFACE4 lpGrowInterMiddle;
LPDIRECTDRAWSURFACE4 lpGrowInterRight;


//±ÛÀÚ ÀÌÆåÆ®°ü·Ã ===================================================================
struct HAEFFECT
{
	int   Flag;
	int   Time;
	POINT Posi;
	POINT Size;
	int   Mat;
	int   Alpha;
};
char *HaEffectFilePath[] = {
	0,
	"Image\\SinImage\\Inter\\defense.tga", //1
	"Image\\SinImage\\Inter\\block.tga",   //2
	"Image\\SinImage\\Inter\\Evade.tga",   //3
	0,
};
#define  MAX_HAEFFECTNUM 10 //ÃÑ°¹¼ö

int HA_EffectIndex[4] = { 0, }; //

HAEFFECT HaEffect[MAX_HAEFFECTNUM];

int DefaultLength[4] = { 94,94,76,86 };
float sinOldNowState[4] = { 0,0,0,0 };
float sinOldMaxState[4] = { 0,0,0,0 };
char InputSecretCode[256];
char sinBuff[256];
int  ResetQuset3Flag = 0;
DWORD dwCheckTime; //½Ã°£À» Ã¼Å©ÇÑ´Ù 
int   CheckNotUseSkillTime[3] = { 0,0,0 };
int   CheckNotUseSkillCount[3] = { 0,0,0 };
int   CheckTime2[3] = { 0,0,0 };
int   CheckExpFlag = 0;
int   CheckExpFlag2 = 0;
int   CheckExpTime = 0;
DWORD dwTestKeyTime = 0;
int   TeskKeyMessageCnt = 0;
int sinWinSizeX[] = { 1024,1280,1600 };
int sinWinSizeXindex[] = { 2,6,11 };
int nCheckSkillDraw = 0;		//ÇØ¿Ü
char *szTestKeyMsg[6] = { "³ª!  &(^_^)&    ",
"³ë!  /(^_^)/   ",
"³ª!  *(^_^)*  ",
"",
"",
"", };

cINTERFACE::cINTERFACE()
{

}
cINTERFACE::~cINTERFACE()
{

}
void cINTERFACE::Init()
{
	Log::Debug("Init : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::Load()
{
	Log::Debug("Load : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::Release()
{
	Log::Debug("Release : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::Draw()
{
	Log::Debug("Draw : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::Main()
{
	Log::Debug("Main : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::Close()
{
	Log::Debug("Close : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::LButtonDown(int x, int y)
{
	Log::Debug("LButtonDown : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::LButtonUp(int x, int y)
{
	Log::Debug("LButtonUp : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::RButtonDown(int x, int y)
{
	Log::Debug("RButtonDown : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::RButtonUp(int x, int y)
{

}
void cINTERFACE::KeyDown()
{
	Log::Debug("KeyDown : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::InitState()
{
	Log::Debug("InitState : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::CheckingState()
{
	Log::Debug("CheckingState : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::ShowParaState()
{
	Log::Debug("ShowParaState : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::DrawInterText()
{
	Log::Debug("DrawInterText : Requisitado porém foi removido.");
	return;
}
void cINTERFACE::CheckAllBox(int ButtonFlag)
{
	Log::Debug("CheckAllBox : Requisitado porém foi removido.");
	return;
}
int cINTERFACE::DrawClockArrow(int x, int y, int Angle)
{
	Log::Debug("DrawClockArrow : Requisitado porém foi removido.");
	return NULL;
}
int cINTERFACE::DrawCompass(int x, int y, int Angle)
{
	Log::Debug("DrawCompass : Requisitado porém foi removido.");
	return NULL;
}
int cINTERFACE::CheckExpPercentControl()
{
	Log::Debug("CheckExpPercentControl : Requisitado porém foi removido.");
	return NULL;
}
int cINTERFACE::GetWordTimeDisplay()
{
	Log::Debug("GetWordTimeDisplay : Requisitado porém foi removido.");
	return NULL;
}
void cINTERFACE::ShowExpPercent()
{
	Log::Debug("ShowExpPercent : Requisitado porém foi removido.");
	return;
}
int cINTERFACE::SetStringEffect(int Index)
{
	Log::Debug("SetStringEffect : Requisitado porém foi removido.");
	return NULL;
}