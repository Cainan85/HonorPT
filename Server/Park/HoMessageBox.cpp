#include "..\\sinbaram\\SinLinkHeader.h"
#include "..\\HoBaram\\HoLinkHeader.h"
#include "..\\language.h"
#include <Wingdi.h>

HoMessage	ChatMessage;
BOOL    HoMessage::MessageFrameSelectFlag = TRUE;

HoMessage::HoMessage()
{
}

HoMessage::~HoMessage()
{
}

int HoMessage::Init()
{
	Log::Debug("Init : Requisitado porém foi removido.");
	return NULL;
}

void HoMessage::DrawMessageBox(int x, int y, char *message, int boxAttrib, int Color)
{
	Log::Debug("DrawMessageBox : Requisitado porém foi removido.");
	return;
}

int HoDrawLinearTexImage(int Mat, float x, float y, float w, float h, float texX, float texY, float texW, float texH, DWORD color)
{
	Log::Debug("HoDrawLinearTexImage : Requisitado porém foi removido.");
	return NULL;
}


int HoDrawTexImage(int Mat, float x, float y, float w, float h, float texX, float texY, float texW, float texH, int Transp)
{
	Log::Debug("HoDrawTexImage : Requisitado porém foi removido.");
	return NULL;
}

int HoDrawTexImage(int Mat, int x, int y, int w, int h, int texX, int texY, int texW, int texH, int Transp)
{
	Log::Debug("HoDrawTexImage : Requisitado porém foi removido.");
	return NULL;
}

int HoDrawTexImage(int Mat , int x, int y, int texX, int texY, int texW, int texH, int Transp)
{
	Log::Debug("HoDrawTexImage : Requisitado porém foi removido.");
	return NULL;
}

void HoMessage::DrawClanName(int x, int y, char *clanName)
{
	Log::Debug("DrawClanName : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawClanMark(LPDIRECTDRAWSURFACE4 clanMark, char *clanName, int x, int y, BOOL frameFlag)
{
	Log::Debug("DrawClanMark : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawTwoLineMessage(int x, int y, char *message1, char *message2, DWORD color1, DWORD color2, LPDIRECTDRAWSURFACE4 clanMark, BOOL selected, int bellaMarkIndex)
{
	Log::Debug("DrawTwoLineMessage : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawBellaIcon(int x, int y, int bellaIndex)
{
	Log::Debug("DrawBellaIcon : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawClanMessageFrame(int x, int y, int messageWidth, int messageHeight)
{
	Log::Debug("DrawClanMessageFrame : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawCharacterMessageFrame(int x, int y, int messageWidth, int messageHeight, bool tailFlag, BOOL selected)
{
	Log::Debug("DrawCharacterMessageFrame : Requisitado porém foi removido.");
	return;
}

int HoMessage::textarrange(char *teststr,bool clanm)
{
	Log::Debug("textarrange : Requisitado porém foi removido.");
	return NULL;
}

void HoMessage::DrawClanMessage(int x, int y, char *message, int lineSize, DWORD color)
{
	Log::Debug("DrawClanMessage : Requisitado porém foi removido.");
	return;
}
void HoMessage::DrawCharacterMessage(int x, int y, char *message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, char *clanName, DWORD color1, BOOL selected, int bellaMarkIndex)
{
	Log::Debug("DrawCharacterMessage : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawShopSellMessage(int x, int y, char *message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, DWORD color1, BOOL selected)
{
	Log::Debug("DrawShopSellMessage : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawCharacterMessageFrame1(int x, int y, int messageWidth, int messageHeight,int linecount, bool tailFlag, BOOL selected)
{
	Log::Debug("DrawCharacterMessageFrame1 : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawShopByeMessageFrame1(int x, int y, int messageWidth, int messageHeight,int linecount, bool tailFlag, BOOL selected)
{
	Log::Debug("DrawShopByeMessageFrame1 : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawShopSellMessageFrame1(int x, int y, int messageWidth, int messageHeight,int linecount, bool tailFlag,  BOOL selected)
{
	Log::Debug("DrawShopSellMessageFrame1 : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawShopSellMessageFrame(int x, int y, int messageWidth, int messageHeight, bool tailFlag,  BOOL selected)
{
	Log::Debug("DrawShopSellMessageFrame : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawShopByeMessage(int x, int y, char *message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, DWORD color1, BOOL selected)
{
	Log::Debug("DrawShopByeMessage : Requisitado porém foi removido.");
	return;
}

void HoMessage::DrawShopByeMessageFrame(int x, int y, int messageWidth, int messageHeight, bool tailFlag, BOOL selected)
{
	Log::Debug("DrawShopByeMessageFrame : Requisitado porém foi removido.");
	return;
}

void HoMessage::Draw(int x, int y, char *message, int lineSize, int addLine, int boxAttrib)
{
	Log::Debug("Draw : Requisitado porém foi removido.");
	return;
}

int DrawCharacterShopSellMessage(int x, int y, char*message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, DWORD charColor, BOOL selected)
{
	Log::Debug("DrawCharacterShopSellMessage : Requisitado porém foi removido.");
	return NULL;
}

int DrawCharacterShopByeMessage(int x, int y, char*message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, DWORD charColor, BOOL selected)
{
	Log::Debug("DrawCharacterShopByeMessage : Requisitado porém foi removido.");
	return NULL;
}

int DrawCharacterMessage(int x, int y, char *message, int lineSize, LPDIRECTDRAWSURFACE4 clanMark, char *clanName, DWORD color1, BOOL selected, int bellaMarkIndex)
{
	Log::Debug("DrawCharacterMessage : Requisitado porém foi removido.");
	return NULL;
}

int DrawTwoLineMessage(int x, int y, char *messageOne, char *messageTwo, DWORD color1, DWORD color2, LPDIRECTDRAWSURFACE4 clanMark, BOOL selected, int bellaMarkIndex)
{
	Log::Debug("DrawTwoLineMessage : Requisitado porém foi removido.");
	return NULL;
}

int DrawClanMessage(int x, int y, char *message, int lineSize, DWORD color)
{
	Log::Debug("DrawClanMessage : Requisitado porém foi removido.");
	return NULL;
}

int DrawMessage(int x, int y, char *message, int lineSize, int boxAttrib)
{
	Log::Debug("DrawMessage : Requisitado porém foi removido.");
	return NULL;
}

BOOL TextGetDC()
{
	Log::Debug("TextGetDC : Requisitado porém foi removido.");
	return NULL;
}

BOOL TextReleaseDC()
{
	Log::Debug("TextReleaseDC : Requisitado porém foi removido.");
	return NULL;
}

BOOL TextSelectObject(HFONT font)
{
	Log::Debug("TextSelectObject : Requisitado porém foi removido.");
	return NULL;
}

BOOL TextSetColor(COLORREF color)
{
	Log::Debug("TextSetColor : Requisitado porém foi removido.");
	return NULL;
}

BOOL dsTextLineOut(int x, int y, char *lpszString, int nCount)
{
	Log::Debug("dsTextLineOut : Requisitado porém foi removido.");
	return NULL;
}

void	SetMessageFrameSelect(BOOL flag)
{
	Log::Debug("SetMessageFrameSelect : Requisitado porém foi removido.");
	return;
}

int InitMessageBox()
{
	Log::Debug("InitMessageBox : Requisitado porém foi removido.");
	return NULL;
}

BOOL	HoButton::ButtonUseFlag = FALSE;
HoButton::HoButton()
{

}
HoButton::~HoButton()
{

}

void HoButton::Init(char *stateFirstIamge, char *stateThirdImage)
{
	Log::Debug("Init : Requisitado porém foi removido.");
	return;
}

void HoButton::InitCrashRect(int left, int top, int right, int bottom)
{
	Log::Debug("InitCrashRect : Requisitado porém foi removido.");
	return;
}

void HoButton::Init(char *stateFirstImage, int sizeWidth, int sizeHeight)
{
	Log::Debug("Init : Requisitado porém foi removido.");
	return;
}

BOOL HoButton::Draw(int x, int y, int BlendFlag)
{
	Log::Debug("Draw : Requisitado porém foi removido.");
	return NULL;
}

BOOL HoButton::Main(int mouseX, int mouseY, int downFlag)
{
	Log::Debug("Main : Requisitado porém foi removido.");
	return NULL;
}

HoMessageBox::HoMessageBox()
{

}

HoMessageBox::~HoMessageBox()
{

}

BOOL HoMessageBox::Main(int mouseX, int mouseY, int downFlag)
{
	Log::Debug("Main : Requisitado porém foi removido.");
	return NULL;
}

BOOL HoMessageBox::Draw(int x, int y, char *message, int Color)
{
	Log::Debug("Draw : Requisitado porém foi removido.");
	return NULL;
}

void HoMessageBox::Init()
{
	Log::Debug("Init : Requisitado porém foi removido.");
	return;
}