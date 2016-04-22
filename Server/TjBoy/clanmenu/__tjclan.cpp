//====================================================================
//ktj : runmap3d.suoÆÄÀÏ¶§¹®¿¡ ¼¼ÀÌºê°¡¾ÈµÇ´Â°æ¿ì°¡ ÀÖÀ¸¹Ç·Î ±×·²¶§´Â ±× ÆÄÀÏÀ» Áö¿ï°Í
//====================================================================
//4.01 P10.47
//sinMain.cpp, sinInterFace.cpp¿¡¼­ ¿î¿µµÊ
#define TJCLAN_CPP
#include "tjclanDEF.h"

#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

#include "..\\..\\sinbaram\\SinLinkHeader.h"
#include "..\\..\\HoBaram\\HoLinkHeader.h"
//#include "HoParty.h"
#include "..\\..\\language.h"
#include "..\\..\\HoBaram\\HoTextFile.h"
#include "tjclan.h"
#include "tjscroll.h"
#include "cE_Cmake.h"
#include "cE_chip.h"
#include "cE_user.h"
#include "cE_isclanmember.h"
#include "cE_report.h"
#include "cE_chatlist.h"
#include "cE_deletecha.h"
#include "cE_Cjoin.h"
#include "cE_CViewClanInfo.h"
#include "cE_Updatecldata.h"

#include "..\\isaocheck\\auth.h"
#include "..\\isaocheck\\isaocheck.h"




extern HWND hTextWnd;
extern HWND hFocusWnd;

extern char UserAccount[256];		//id°¡ µé¾îÀÖ´Â ¹öÆÛ
extern char	szConnServerName[16];	//Á¢¼ÓÇÑ ¼­¹öÀÌ¸§ÀÌ µé¾îÀÖ´Â ¹öÆÛ




#define MOUSE0() TJmb[0]


#else						//=========== Æ÷¸®½ºÅÏ¼Ò½º°¡ ¾Æ´Ñ°æ¿ì

#include <windows.h>
#include <stdio.h>
#include "tjclan.h"
#include "tjscroll.h"

#include "clan_Enti.h"
#include "cE_Cmake.h"
#include "cE_chip.h"
#include "cE_user.h"
#include "cE_isclanmember.h"
#include "cE_report.h"
#include "cE_chatlist.h"
#include "cE_deletecha.h"
#include "cE_CJoin.h"
#include "cE_CViewClanInfo.h"
#include "cE_Updatecldata.h"

#include "..\\isaocheck\\auth.h"
#include "..\\isaocheck\\isaocheck.h"





#define MAX_PARTY_MEMBER	6

#define PARTY_NONE		500
#define PARTY_START		1000
#define PARTY_PROCESS	2000
#define PARTY_END		3000

char UserAccount[256];		//id°¡ µé¾îÀÖ´Â ¹öÆÛ
extern char UserAccount[256];		//id°¡ µé¾îÀÖ´Â ¹öÆÛ
char szConnServerName[16];	//Á¢¼ÓÇÑ ¼­¹öÀÌ¸§ÀÌ µé¾îÀÖ´Â ¹öÆÛ
extern char szConnServerName[16];	//Á¢¼ÓÇÑ ¼­¹öÀÌ¸§ÀÌ µé¾îÀÖ´Â ¹öÆÛ

extern int MouseButton[3] ;	//¸¶¿ì½º¹öÆ°´­¸²Ã½Å© : winmain.cpp¿¡ Á¤ÀÇÇÔ
extern int MouseDblClick ;		//tjclan.cpp


extern HWND    g_hwnd;
extern POINT	pCursorPos;
extern HINSTANCE g_hInst;


#define MOUSE0() MouseButton[0]


#endif


int ClanCharMove=0;	//Å¬·£¸Þ´ºÆÇ¾ÈÀÇ ¸¶¿ì½ºÀÇ ¿òÁ÷ÀÓ¿¡ Ä³¸¯ÅÍ°¡ ¿µÇâ¾øµµ·Ï ÇÑ´Ù.
int ClanCLCharMove=0;//Å¬·£Ã¤ÆÃ¸®½ºÆ®¸Þ´ºÆÇ¾ÈÀÇ ¸¶¿ì½ºÀÇ ¿òÁ÷ÀÓ¿¡ Ä³¸¯ÅÍ°¡ ¿µÇâ¾øµµ·Ï ÇÑ´Ù.


int  banisF[100];

//int TJwheel=0; //ktj : ÈÙµ¥ÀÌÅ¸
void fd2( char *fmt, ...);









void clan_Mouse(int msg)
{
	switch(msg){
	case WM_LBUTTONDOWN:
		TJmb2[0] = TJmb[0] = TRUE;
		break;

	case WM_LBUTTONUP:
		TJmb2[0] = TJmb[0] = FALSE;
		break;

	case WM_RBUTTONDOWN:
		TJmb2[1] = TJmb[1] = TRUE; //ktj
		break;

	case WM_RBUTTONUP:
		TJmb2[1] = TJmb[1] = FALSE; //ktj
		break;

	case WM_MBUTTONDOWN:
		TJmb2[2] = TJmb[2] = TRUE; //ktj
		break;

	case WM_MBUTTONUP:
		TJmb2[2] = TJmb[2] = FALSE; //ktj
		break;
	}
}









void ktj_imsi_clanMENU_Main()
{
	static cnt=0;
	cnt++;
	

	int mainchk;
	
	if(cnt<10) {
		mainchk = PARTY_START;
		
	}
	else {
		mainchk = PARTY_PROCESS;
	}

	

	Interface_clanmenu.Main(mainchk );
}

void ktj_imsi_clanMENU_Draw()
{
	Interface_clanmenu.Draw();
}
		

void clanMENUinit()
{
	
	Interface_clanmenu.CLANmenu_INIT();
	//ktj : Å¬·£»ó¿¡¼­ÀÇ ÇöÀç À§Ä¡¸¦ ¾Ë¾Æ³½´Ù.
	Interface_clanmenu.CLANmenu_clanPOSITION();
}





/*==================================================================================
//
// ¿ÜºÎ¿¡¼­ »ç¿ëµÇ´Â Å¬·£ÀÇ ±¸µ¿ ¼ÂÆÃ ÇÔ¼öµé								   start
//
==================================================================================*/


/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í : 
: ÇÏ ´Â ÀÏ : Å¬·£ ¸Ç Ã³À½ ¼ÂÆÃ
///////////////////////////////////////////////////////////////////////////*/

//ktj : Å¬·£¸Þ´º ÀÎÅÍÆäÀÌ½º init
//setN : 0 : ÃÖÃÊ ¼ÂÆÃ  1: µÎ¹øÂ°¼ÂÆÃ(¸ðµç 
void clanMENU_INIT(int setN)
{	
	if(!bCheckHostNameandPort) return;



	switch(setN) {
	case -1:				//½ÇÁ¦·Î´Â »ç¿ë¾ÈÇÔ¿¡ ÁÖÀÇ
		ZeroMemory(&ClanWonList,  sizeof(CLANWONLIST ));

		Interface_clanmenu.CLANmenu_INIT();
		//ktj : Å¬·£»ó¿¡¼­ÀÇ ÇöÀç À§Ä¡¸¦ ¾Ë¾Æ³½´Ù.
		Interface_clanmenu.CLANmenu_clanPOSITION();
		break;


	case 0:				//¸ÇÃ³À½¼ÂÆÃ : Ä³¸¯ÅÍ ¼±ÅÃÁ÷Àü¼ÂÆÃ

		void CreateSaveData_dir();
		CreateSaveData_dir();			//savedata/clandata¸¸µê


		ZeroMemory(&ClanWonList,  sizeof(CLANWONLIST ));
		Interface_clanmenu.CLANmenu_INIT();

		break;

	case 1:				//°× ½ÇÁ¦ ½ÃÀÛ½Ã ¼ÂÆÃ
	#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

		cBaseMenu.Load2();
		cClanChatting.Load2();
		Interface_clanmenu.Init2();	
	

		//setNÀº 0ÀÇ °æ¿ì Ä³¸¯ÀÌ¸§À» µû·Î ÁÖ¾î¾ß ÇÔ¿¡ Å©°Ô ÁÖÀÇÇÒ°Í.
		Interface_clanmenu.SetUserData(UserAccount, sinChar->szName, "¾ÆÀ£", 
			sinChar->Money,500000); //6ÀÎ ÀÌÇÏ

		//ktj: ÀÓ½Ã·Î Áö¿ò.
		//ktj : Å¬·£»ó¿¡¼­ÀÇ ÇöÀç À§Ä¡¸¦ ¾Ë¾Æ³½´Ù.
		Interface_clanmenu.CLANmenu_clanPOSITION();
		
	
	#endif
		break;
	}
}


//hoLogin.cpp¿¡¼­ »ç¿ëÇÔ.
int isDELETE_character(char *charName) 
{
	if(!bCheckHostNameandPort) return 1;		//1Àº Ä³¸¯ÅÍ¸¦ Áö¿öµµµÈ´Ù´Â¶æ


	int web_DB_state_FLAG_WAIT();
	while(1) {
		if(web_DB_state_FLAG_WAIT() == 1) 
			break;
	}

	Interface_clanmenu.SetUserData(UserAccount, charName, "¾ÆÀ£", 500000   ,500000); //6ÀÎ ÀÌÇÏ

	//ktj : Å¬·£»ó¿¡¼­ÀÇ ÇöÀç À§Ä¡¸¦ ¾Ë¾Æ³½´Ù.
	Interface_clanmenu.CLANmenu_clanPOSITION();
	cldata.myPosition = 0;//CLANNO_REGESTER;
	while(1) {
		web_DB_main();					//web db¸¦ »ç¿ëÇÏ´Â Å¬·£ ¸Þ´º¿¡¼­¸¸ ºÒ¸®¿öÁü.	
		cIsClanMember.Main();			//ktj : Å¬·£¿øÀÇ Æ÷Áö¼Ç Ã¼Å© ¸ÞÀÎ

		if(cldata.myPosition != 0) 
			break;
	}

	while(1) {
		if(web_DB_state_FLAG_WAIT() == 1) 
			break;
	}

	/*
	#define CLANUSER					100		//ÀÏ¹ÝÀ¯Àú
	#define CLANCHIP					101		//¯…
	#define CLANNO_REGESTER				102		//¹«µî·ÏÀÚ
	#define CLAN_NOCONNECT				103		//ÄÁ³ØÆ®°¡ ¾ÈµÇ¼­ ¼³Á¤ºÒ°¡
	*/
	return cldata.myPosition;
}



void CLAN_MENU::Init2()
{
	#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	MatPartyBackGround =
		CreateTextureMaterial( "image\\Sinimage\\Help\\party_win.tga" , 0, 0, 0,0, SMMAT_BLEND_ALPHA );

	//box===========================================================================start

	hBox1 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\HelpBoxCorner.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox2 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\HelpBoxLine.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox3 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box3.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox4 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box4.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );
	hBox5 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box5.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox6 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box6.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox7 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box7.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox8 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box8.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox9 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box9.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	//box===========================================================================end
	hScl_icon_tj = CreateTextureMaterial( "image\\Sinimage\\Help\\scl_icon.tga", 0, 0, 0,0, SMMAT_BLEND_ALPHA );


	cBaseMenu.copy_LoadResource2(&cIsClanMember);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanChip);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanUser);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanMake);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanReport);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanChatting);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanDeleteCha);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.copy_LoadResource2(&cClanJoin);		//¸®¼Ò½º Ä«ÇÇ	
#endif
}

void CLAN_MENU::CLANmenu_INIT()
{
	if(!bCheckHostNameandPort) return;
	//g_ViewClanInfo.LoadData();

	Init();


#ifdef  USE_PROSTONTALE

	//SetUserData(UserAccount, sinChar->szName, szConnServerName); //6ÀÎ ÀÌÇÏ
	//ktj : ÀÌ°Ô ¸ÂÀ½.
	//SetUserData(UserAccount, sinChar->szName, szConnServerName, sinChar->Money,1000); //6ÀÎ ÀÌÇÏ

	//ktj : ¿øº»
	SetUserData(UserAccount, sinChar->szName, "¾ÆÀ£", sinChar->Money,500000); //6ÀÎ ÀÌÇÏ
	//SetUserData(UserAccount, "Çã°Æ´ö", "¾ÆÀ£", 500000 ,500000); //6ÀÎ ÀÌÇÏ

	//SetJoinData("ÇÏÇÏÇÏ","¿ïÆ®¶ó","ÇåÆ®");

#else
	
	strcpy(szConnServerName,"ÇÏ³ª");		
	SetUserData("yung6","À¶6","¾ÆÀ£",500000,100000);
	SetUserData("ddd","µðµðµð","¾ÆÀ£",500000,100000);
	//SetUserData("aaa","°¡°¡°¡","¹Ì°¥",500000,100000);
	
	//Å¬·£¿øÀ¸·Î µî·ÏµÈ idÀÓ	

#endif
}




/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í :
: ÇÏ ´Â ÀÏ : Å¬·£¿øÀ¸·Î¼­ÀÇ À§Ä¡¸¦ ¾Ë¾Æ³¿.
///////////////////////////////////////////////////////////////////////////*/
int CLAN_MENU::CLANmenu_clanPOSITION()
{
	if(!bCheckHostNameandPort) {
		cldata.myPosition = CLAN_NOCONNECT;
		return 0;
	}


	if(OpenFlag==1) return 0;		//ÀÌ ¸Þ´º°¡ ´ÝÇôÀÖÀ»¶§¸¸ °¡´ÉÇÔ.
	
	C_menuN2= 0;
	OpenFlag = 1;
	joinAnswer = 0;
	C_menuN = CLAN_isClanWon;
	

	menuInit(C_menuN );

	ClanCharMove = 1;			//ktj : ¸¶¿ì½º°¡ Ã½Å©ÇÏ´Â ¹Ú½ºÃ½Å© 
	return 1;
}


/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í :
: ÇÏ ´Â ÀÏ : NPC¿Í ¸¸³µÀ»¶§
///////////////////////////////////////////////////////////////////////////*/
void CLANmenu_meetNPC()	//newplay.cpp¿¡¼­ »ç¿ëÇÔ.
{
	Interface_clanmenu.CLANmenu_meetNPC();	
}
int CLAN_MENU::CLANmenu_meetNPC()
{	
	if(OpenFlag==1) {
		if(C_menuN == CLAN_DISPLAY_INFO)
		{
			C_menuN = CLAN_CLOSE_END_BEFORE;									
			
		}
		return 0;		//ÀÌ ¸Þ´º°¡ ´ÝÇôÀÖÀ»¶§¸¸ °¡´ÉÇÔ.
	}

	
	C_menuN2= 0;
	OpenFlag = 1;
	joinAnswer = 0;	
	if(!bCheckHostNameandPort)
	{
		C_menuN = CLANSERVER_INSPECT_BEFORE;
		
	}
	else
	{
		C_menuN = CLAN_GO_NPCSTART;
	}
	

	//menuInit(C_menuN );

	ClanCharMove = 1;			//ktj : ¸¶¿ì½º°¡ Ã½Å©ÇÏ´Â ¹Ú½ºÃ½Å© 
	return 1;
}






/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í :
: ÇÏ ´Â ÀÏ : Å¬·£¿øµéÀÇ Ã¤ÆÃ¸®½ºÆ®¸¦ º¼¶§
///////////////////////////////////////////////////////////////////////////*/
void CLANmenu_ChatList(int sw)	//newplay.cpp¿¡¼­ »ç¿ëÇÔ.l
{
	Interface_clanmenu.CLANmenu_ChatList(sw);	
}
void CLAN_MENU::CLANmenu_ChatList(int sw)
{

	if(!bCheckHostNameandPort) return;
	if(OpenFlag==1) return;
	if(bOpenChatList) return;

	ChatList_CmenuN2 = 0;

	switch(sw)
	{
	case 0: //½ºÅ©·Ñ µÇ°Ô
		
		cClanChatting.InitClanChatList();
		if(bIsReadData) ChatList_CmenuN = CLAN_DISPLAY_OPEN;
		else	ChatList_CmenuN = CHATLIST_READDATA;
		break;
	case 1: //½ºÅ©·Ñ ¾ÈµÇ°Ô
		cClanChatting.NoScroll();
		//if(bIsReadData) ChatList_CmenuN = CLAN_DISPLAY_OPEN;
		//else	ChatList_CmenuN = CHATLIST_READDATA;		

		ChatList_CmenuN = CLAN_DISPLAY_INFO;				
		cClanChatting.menuInit(ChatList_CmenuN);

#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
		
#else
	OpenFlag = 1;
	C_menuN = ChatList_CmenuN;
	
#endif


		break;
	}
}




void CLANmenu_ChatList_Main()	//newplay.cpp¿¡¼­ »ç¿ëÇÔ.l
{
	cClanChatting.Main();			//Å¬·£ Ã¤ÆÃ¸®½ºÆ®
}
void CLANmenu_ChatList_Draw()	//newplay.cpp¿¡¼­ »ç¿ëÇÔ.l
{
	cClanChatting.Draw();			//Å¬·£ Ã¤ÆÃ¸®½ºÆ®
}


/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í :
: ÇÏ ´Â ÀÏ : cldata¾÷µ¥ÀÌÆ® ÇÒ¶§
///////////////////////////////////////////////////////////////////////////*/
void Updatecldata()
{
	//Interface_clanmenu.Updatecldata();
	g_UpdateCldata.Updatecldata();
}


/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í : ±ÇÀ¯ÇÏ´Â ¸Þ´ºµé.
: ÇÏ ´Â ÀÏ : ÀÏ¹ÝÀÎ¿¡°Ô Å¬·£ÀÇ °¡ÀÔÀ» ±ÇÀ¯ÇÏ¸é ±ÇÀ¯ÇÑ´Ù´Â ¸Þ´º°¡ ¶á´Ù.
///////////////////////////////////////////////////////////////////////////*/

//±ÇÀ¯¹Þ´Â »ç¶÷ÀÌ »ç¿ëÇÏ´Â ÇÔ¼ö
void ClanJoin(char *oneClanName, char *oneChipCharid)
{
	//Interface_clanmenu.SetJoinData("¿ïÆ®¶ó","ÇåÆ®");
	Interface_clanmenu.SetJoinData(oneClanName,oneChipCharid);
	Interface_clanmenu.ClanJoin();
}

//ktj : ±ÇÀ¯¹ÞÀºÀÚ°¡ ¸®ÅÏÇØÁÖ´Â °ª(ÀÌÇÔ¼ö¸¦ »ç¿ëÇØ ¸®ÅÏÇØÁØ´Ù.)
//0:Ãë¼Ò 1:OK
int JoinAnswer()
{
	return cClanJoin.nJoinAnswer;
}

void CLAN_MENU::ClanJoin()
{
	if(OpenFlag==1) return;
	C_menuN2 = 0;
	OpenFlag = 1;
	ClanCLCharMove = 1;			//ktj : ¸¶¿ì½º°¡ Ã½Å©ÇÏ´Â ¹Ú½ºÃ½Å© 	
	C_menuN = CLAN_JOIN_before;
	
}

/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í :
: ÇÏ ´Â ÀÏ : Å¬·£ÀåÀÌ Å¬·£¿øÀ» µî·Ï½ÃÅ³¶§
///////////////////////////////////////////////////////////////////////////*/
void joinClanMember()
{
	Interface_clanmenu.joinClanMember();
}

void CLAN_MENU::joinClanMember()
{
	if(OpenFlag==1) return;
	C_menuN2 = 0;
	OpenFlag = 1;
	ClanCLCharMove = 1;			//ktj : ¸¶¿ì½º°¡ Ã½Å©ÇÏ´Â ¹Ú½ºÃ½Å© 	
	C_menuN = CLAN_JOIN_chip;
}



/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í :
: ÇÏ ´Â ÀÏ : Ä³¸¯ÅÍ »èÁ¦½Ã
///////////////////////////////////////////////////////////////////////////*/
void ktj_IMSI_DELETE()
{
	//extern char UserAccount[256];		//id°¡ µé¾îÀÖ´Â ¹öÆÛ
	//extern char szConnServerName[16];	//Á¢¼ÓÇÑ ¼­¹öÀÌ¸§ÀÌ µé¾îÀÖ´Â ¹öÆÛ
	CLANmenu_DeleteCha_SetCharacter(UserAccount, szConnServerName, "Çã°Æ´ö");
}



//ktj :  1. Ä³¸¯ Áö¿ì±âÀü¿¡ ¼ÂÆÃÇÏ´Â ÇÔ¼ö
void CLANmenu_DeleteCha_SetCharacter(char *id, char *gserver, char *chaname)
{
	cClanDeleteCha.SetDeleteChaInfo(id, gserver, chaname);

	//ktj : 2. Áö¿ì´Â ¸Þ´º¸¦ ¼ÂÆÃÇÑ´Ù.
	Interface_clanmenu.CLANmenu_DeleteCha();
}

//ktj : Ä³¸¯ Áö¿ì±âÀü¿¡ ºÒ¸®¿ì´Â ÇÔ¼ö
void CLAN_MENU::CLANmenu_DeleteCha()
{
	C_menuN2 = 0;
	OpenFlag = 1;
	ClanCLCharMove = 1;			//ktj : ¸¶¿ì½º°¡ Ã½Å©ÇÏ´Â ¹Ú½ºÃ½Å© 	
	C_menuN = CLAN_DELETECHA_START;

	cClanDeleteCha.SetHowDeleteChaFlag();
}

//ktj : 3. »óÅÂ ÇÃ·¡±×¸¦ º¸°í 2ÀÌ¸é Ä³¸¯À» »èÁ¦ÇÏ°í 3ÀÌ¸é »èÁ¦ ¸øÇÔ.(0,1ÀÌ¸é 2,3µÉ¶§±îÁö ±â´Ù¸°´Ù)
int CLANmenu_DeleteCha_flag()
{
	//»èÁ¦Àü : 0
	//»èÁ¦Áß : 1
	//»èÁ¦ ok : 2
	//»èÁ¦ ¾ÈµÊ : 3
	return cClanDeleteCha.GetHowDeleteChaFlag();
}



//////////////////////////////////////////////////////////////////////////////////
//Å¬·£ Á¤º¸ °®¾î¿À±â
//
/////////////////////////////////////////////////////////////////////////////////
int ReadClanInfo(DWORD dwClanNum)
{
	if(OpenFlag==1) return -1;		//ÀÌ ¸Þ´º°¡ ´ÝÇôÀÖÀ»¶§¸¸ °¡´ÉÇÔ.

	g_ViewClanInfo.SearchClan(dwClanNum);
	BOOL bFlag;
	bFlag = g_ViewClanInfo.GetFlag();  //ÇÏµå¿¡ ÀúÀåµÇ¾î´ÂÁö ¾Æ´Ï¸é ´Ù¿î ¹Þ¾Æ¾ß ÇÏ´ÂÁö Ã¼Å© 
	
	if(!bFlag)
	{
		//web¿¡¼­ ´Ù¿î¹ÞÀ½.
		g_ViewClanInfo.DownLoadClanInfo(dwClanNum);
	}
	return ClanInfo_nNum;		//0~99;
}







//==================================================================================
// ¿ÜºÎ¿¡¼­ »ç¿ëµÇ´Â Å¬·£ÀÇ ±¸µ¿ ¼ÂÆÃ ÇÔ¼öµé								    end
//=================================================================================







/*==================================================================================
//
// Å¬·£¸Þ´º																	   start
//
==================================================================================*/
CLAN_MENU::CLAN_MENU()
{
	nClanWon_Num = 0;
	nScl_Bar = 0;           //½ºÅ©·Ñ¹Ù
	g_nClanIconNum;			//ÇïÇÁ¾ÆÀÌÄÜ ÃÑ¼ö
	g_HelpIconNumber = 0;    
	g_HelpDisPlay = 0;

}

CLAN_MENU::~CLAN_MENU()
{
}


//¸¶¿ì½ºÀ§Ä¡¸¦ Ã½Å©¾ÈÅä·ÏÇÏ´Â°Í hoLinkHeader.cpp¿¡¼­ »ç¿ë
POINT clanBoxPos;
POINT clanCLBoxPos;
void CLAN_MENU::Init()
{
	OpenFlag = 0;

	C_menuN = 0;
	C_menuN2 =0;

	//BackStartPos.x = 70;
	//BackStartPos.y = 94;
	
	//BackStartPos.x = 100;
	//BackStartPos.y = 94;

	BackStartPos.x = 101;
	BackStartPos.y = 114;


	//Å¬·£¸Þ´º¿¡¼­ ¿ùµåÂÊ¿¡ ¸¶¿ì½º°¡ ¿òÁ÷ÀÓÀÌ Àü´Þ¾ÈµÇµµ·ÏÇÑ´Ù.
	clanBoxPos.x = BackStartPos.x;
	clanBoxPos.y = BackStartPos.y;

	//Å¬·£Ã¤ÆÃ ¸®½ºÆ®¸Þ´º¿¡¼­ ¿ùµåÂÊ¿¡ ¸¶¿ì½º°¡ ¿òÁ÷ÀÓÀÌ Àü´Þ¾ÈµÇµµ·ÏÇÑ´Ù.
	clanCLBoxPos.x = 540+60;		//60Àº ±×¸²ÀÌ ¿À¸¥Æí¿¡ ºÙ¾îÀÖ±â ¶§¹®ÀÓ
	clanCLBoxPos.y = CHATLIST_BOX_Y;


	

	ZeroMemory(szRemoveClanWon,sizeof(char)*500);	
	ZeroMemory(ClanHelpIcon,sizeof(CLANHELPICON)*CLANHELPICON_NUM);		
	ZeroMemory(cldata.szJoinClanName,sizeof(char)*64);   //Á¶ÀÎ Å¬·£ ÀÌ¸§
	ZeroMemory(cldata.ChaName,sizeof(char)*64);  //Á¶ÀÎÇÏ´Â À¯Àú Ä³¸¯ÅÍ ÀÌ¸§
	ZeroMemory(cldata.szJoinClanChipName,sizeof(char)*64);   //Á¶ÀÎÇÏ´Â Å¬·£Àå ÀÌ¸§
	ZeroMemory(szReconfReleaseMsg,sizeof(char)*2048);

	
	nClanWon_Num = 0;			//Å¬·£¿øÆÄ½ÌÈÄ Å¬·£¿øÀÇ ¸í¼ö(Å¬·£ÀåÁ¦¿Ü)
	nReleaseClanWon = 0;		//»èÁ¦ Å¬·£¿ø
	membmp = new MENUBMP;
	
	//hMark = NULL;

	Load();	
	cClanChatting.Load();

	//ktj : ¸Þ´º °ü·Ã Å¬¶ó½º ¼ÂÆÃ ===================
	
	cBaseMenu.Load();

	
	cIsClanMember.Init(1);
	cIsClanMember.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cIsClanMember);		//¸®¼Ò½º Ä«ÇÇ

	cClanChip.Init(1);
	cClanChip.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanChip);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.SetBase(&cClanChip);

	cClanUser.Init(1);
	cClanUser.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanUser);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.SetBase(&cClanUser);

	cClanMake.Init(1);
	cClanMake.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanMake);		//¸®¼Ò½º Ä«ÇÇ

	cClanReport.Init(1);
	cClanReport.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanReport);		//¸®¼Ò½º Ä«ÇÇ


	cClanChatting.Init(1);
	cClanChatting.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanChatting);		//¸®¼Ò½º Ä«ÇÇ
	cBaseMenu.SetBase(&cClanChatting);


	cClanDeleteCha.Init(1);
	cClanDeleteCha.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanDeleteCha);		//¸®¼Ò½º Ä«ÇÇ

	cClanJoin.Init(1);
	cClanJoin.setParent(&cBaseMenu);
	cBaseMenu.copy_LoadResource(&cClanJoin);		//¸®¼Ò½º Ä«ÇÇ	
	

}


/*///////////////////////////////////////////////////////////////////////////
: ÇÔ¼ö¸í   : menuInit
: ÇÏ ´Â ÀÏ : ¸Þ´º ÃÑ°ý Init
: ¹Ý È¯ °ª :
: ÆÄ¶ó¹ÌÅÍ :
///////////////////////////////////////////////////////////////////////////*/
void CLAN_MENU::menuInit(int num)
{
	
	cIsClanMember.menuInit(num);		//Å¬·£¸¸µé±â ¸Þ´º initialize
	cClanMake.menuInit(num);		//Å¬·£¸¸µé±â ¸Þ´º initialize
	cClanChip.menuInit(num);		//Å¬·»¯… ¸Þ´º initialize
	cClanUser.menuInit(num);		//Å¬·£À¯Àú ¸Þ´º initialize
	cClanChatting.menuInit(num);
	cClanDeleteCha.menuInit(num);
	cClanJoin.menuInit(num);
}







/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º ¸í: 
: ÇÏ ´Â ÀÏ : ¸¶¿ì½º·Î ´©¸¥ ¸Þ´º ¹øÈ£ Ã½Å©ÇÔ¼ö.
: ¹Ý È¯ °ª :
:ÆÄ¶ó¹ÌÅÍ  :
///////////////////////////////////////////////////////////////////////////*/
int CLAN_MENU::chkMenu(int x, int y, RECT *r) 
{
	if(r->left <= x   &&   x<= r->right ) 
		if(r->top <= y   &&   y<= r->bottom ) 
			return 1;
	return 0;			//¾Æ¹«°Íµµ ¼±ÅÃ¾ÈÇÔ.
}
int CLAN_MENU::chkeckMenuN()
{
	int i;
	
	for(i=0; i<menuSu; i++) {
		if(  chkMenu(pCursorPos.x, pCursorPos.y, &menuPos[i])  ) {
			return i;
		}
	}
	return -1;
}





//¯…ÀÌ ±ÇÀ¯ÇÏ¸é ±ÇÀ¯¹ÞÀº »ç¶÷ÇÑÅ× ¶ß´Â ¸Þ´ºÀÓ.(ÇöÀç netplay.cpp¿¡¼­ »ç¿ëÁß)
void menu_joinOpen(char *chipCharID)
{
	//Interface_clanmenu.menu_joinOpen(chipCharID);
}


//Á¶ÀÎ¸Þ´º´Â Å¬·£¯…ÀÌ ±ÇÀ¯ÇÏ¸é ±ÇÀ¯¹ÞÀº »ç¶÷ÇÑÅ× ¸Þ´º°¡ ¶á´Ù.
int CLAN_MENU::menu_joinOpen(char *chipCharID)		//¯…ÀÇ Ä³¸¯ÅÍ id
{
	/*
	if(OpenFlag==1) return 0;					//ÀÌ ¸Þ´º°¡ ´ÝÇôÀÖÀ»¶§¸¸ °¡´ÉÇÔ.
	if( cldata.myPosition == 100 ||				//100(Å¬·£¿ø), 101:Å¬·£¯…¿¡°Ô ±ÇÀ¯ÇÒ¼ö ¾ø´Ù.
		cldata.myPosition == 101)				return 0;

	strcpy(cldata.chipCharID, chipCharID);
	C_menuN   = CLAN_JOIN_before;		//Á¶ÀÎ¿¡ ÀûÇÕÇÑÁö ¿©ºÎ.
	C_menuN2  = 0;
	OpenFlag  = 1;
	joinAnswer= 0;

	//ktj
	ClanCharMove = 1;
	*/

	return 1;
	
}


void menu_joinOpen_Chip(int isOK, char *clanWonID, char *clanWonCharID)
{
	Interface_clanmenu.menu_joinOpen_Chip(isOK, clanWonID, clanWonCharID);		//¯…ÀÇ Ä³¸¯ÅÍ id;
}


//±ÇÀ¯¹ÞÀº »ç¶÷ÀÌ OKÇÏ¸é ±×¸í·ÉÀÌ Å¬·£¯…¿¡°Ô µÇµ¹¾Æ¿Í Å¬·£¯…ÀÌ °¡ÀÔ½ÃÅ°´Â ¸Þ´ºÀÓ.
//ÁÖÀÇ : È­¸é»ó¿¡´Â ¸Þ´º°¡ ¶ßÁö¾ÊÀ½.
int CLAN_MENU::menu_joinOpen_Chip(int joinOK, char *clanWonID, char *clanWonCharID)		//¯…ÀÇ Ä³¸¯ÅÍ id
{
	if(OpenFlag==1) return 0;		//ÀÌ ¸Þ´º°¡ ´ÝÇôÀÖÀ»¶§¸¸ °¡´ÉÇÔ.

	if(cldata.myPosition != 101) return 0;	//101:Å¬·£¯…¸¸ »ç¿ë°¡´ÉÇÑ ¸Þ´ºÀÓ.

	/*
	if(joinOK==0) {
		C_menuN= CLAN_JOIN_chip_NO	;		//Å¬·£¿¡ °¡ÀÔÇÏ±â¸¦ °ÅºÎÇÔ.
		C_menuN2= -1;
	}
	else {
		*/
		C_menuN = CLAN_JOIN_chip		;	//Å¬·£¿¡ µî·Ï½ÃÅ²´Ù.
		strcpy(cldata.clanJoinID, clanWonID);
		strcpy(cldata.clanJoinCharID, clanWonCharID);

		C_menuN2= 0;
	//}
	OpenFlag = 1;
	joinAnswer = 0;

	//ktj
	ClanCharMove = 1;


	return 1;
}








//int chkN11 = -1;
//int chkN22 = -1;

extern BOOL bip_port_error;
int CLAN_MENU::Main(int sw)
{
	if(bip_port_error) return 0;
	web_DB_main();					//web db¸¦ »ç¿ëÇÏ´Â Å¬·£ ¸Þ´º¿¡¼­¸¸ ºÒ¸®¿öÁü.	

	//char		szErrorMsg[256];
	if(OpenFlag==0) return 0;		//ÀÌ ¸Þ´º´Â´ÝÇûÀ½


	switch(sw) {
	case PARTY_START:				//Ã³À½ ºÒ¸®¿öÁú¶§ÀÇ Ã³¸®
		return 1;
	case PARTY_END:					//³¡³¯Áò¿¡ ºÒ¸®¿öÁú¶§ÀÇ Ã³¸®
		return 1;
	}

	//ClanCharMove = 1;
									//ktj : auth.cppÀÇ web_DBÃ½Å©¿ëÀÓ.
	cBaseMenu.Main();				//ktj : °ø¿ë ¸Þ´º ÀÇ ¸ÞÀÎ
	cIsClanMember.Main();			//ktj : Å¬·£¿øÀÇ Æ÷Áö¼Ç Ã¼Å© ¸ÞÀÎ
	cClanReport.Main();				//ktj : ¸Þ½ÃÁö Ã³¸®¿ë ¸ÞÀÎ
	cClanMake.Main();				//ktj : Å¬·£À» ¸¸µé°æ¿ìÀÇ ¸ÞÀÎ
	cClanChip.Main();				//Å¬·£ Ä¨ ¸Þ¼¼Áö Ã³¸®
	cClanUser.Main();				//Å¬·£ À¯Àú ¸Þ¼¼ÁöÃ³¸®
	cClanJoin.Main();
#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	//cClanChatting.Main();			//Å¬·£ Ã¤ÆÃ¸®½ºÆ®
#else
	cClanChatting.Main();			//Å¬·£ Ã¤ÆÃ¸®½ºÆ®
#endif

	cClanDeleteCha.Main();            //Ä³¸¯ÅÍ »èÁ¦½Ã
	return TRUE;
}




char loadis[256] = {0,};
char loadis2[256] = {0,};
char updateis[256] = {0,};


int CLAN_MENU::Draw()
{	
	if(bip_port_error) return 0;
	
	/*
	//ktj : ÀÓ½Ã·Î ¹ö±×Ã½Å©
	char buf[256];
	
	wsprintf(buf, "bug :  %d ================ : (myPos:%d ) (Å¬·£¸¶Å©¹øÈ£%s,%d)  cldata.money %d ", C_menuN, cldata.myPosition, cldata.ClanMark, cldata.intClanMark, cldata.money);
	Draw_C_Text(buf, 0,0,   0);
	

	wsprintf(buf, "cldata.szID %s  cldata.ChaName %s cldata.name %s (C_menuN %d C_menuN2 %d) ================ ", cldata.szID,cldata.ChaName, cldata.name, C_menuN, C_menuN2);
	Draw_C_Text(buf, 0,16,   0);
	*/

	/*
	wsprintf(buf, "Å¬·£Ã¤ÆÃ¸®½ºÆ®·Îµå´Â  %s ================ ", loadis);
	Draw_C_Text(buf, 0,32,   0);
	wsprintf(buf, "Å¬·£Ã¤ÆÃ¸®½ºÆ®·Îµå2   %s ================ ", loadis2);
	Draw_C_Text(buf, 0,32+16,   0);

	wsprintf(buf, "¾øµ¥ÀÌÆ®Ä«¿îÆ® %s ================ ", updateis);
	Draw_C_Text(buf, 0,64,   0);
	*/
	
	
	


	

	//InvalidateRect(g_hwnd,NULL,1);
	if(OpenFlag==0) return 0;	


#ifndef USE_PROSTONTALE		//=========== À©µµÇÔ¼ö

	window_renderINIT();			//Ã³À½ ÇÑ¹ø¼ÂÆÃÇÔ.
	HDC hdc = GetDC(g_hwnd);	
	HDC sorDC;
	sorDC  = CreateCompatibleDC(hdc);
	HDC destdc=CreateCompatibleDC(hdc);
	SelectObject(destdc, Dback.hb );
	PatBlt(destdc,0,0,800,600,PATCOPY);
	char szBuf[256];
	extern POINT	pCursorPos;
	
	// 0 : ÀÛ¾÷¼öÇàÇÏ±âÀü 1: ´Ù¿î·Îµå ¼öÇàÁß 2 : ÀÛ¾÷¿Ï·á 3: ¿¡·¯
	wsprintf(szBuf,"C_menuN : %d  ,x = %d, y = %d , ÁøÇà °úÁ¤ : %d Å¬·£ ÀÌ¸§ : %s",
		C_menuN,pCursorPos.x,pCursorPos.y,nCheckDownLoad,cldata.name);
	TextOut(destdc,100,0,szBuf,lstrlen(szBuf));

	SelectObject(hdc,ClanInfo[ClanInfo_nNum].hClanMark );
	BitBlt(destdc,0,0,32,32,sorDC,0,0,SRCCOPY);
	TextOut(destdc,0,50,ClanInfo[ClanInfo_nNum].ClanInfoHeader.ClanReport,lstrlen(ClanInfo[ClanInfo_nNum].ClanInfoHeader.ClanReport));
	DeleteDC(destdc);
	DeleteDC(sorDC);
	ReleaseDC(g_hwnd,hdc);

#endif
	
	/*
	if(OpenFlag){
	if(ANSdata.ret_val == -1){
		DrawBox(BackStartPos.x,BackStartPos.y,3,3);		
		//Loading();
		}
	}
	*/

	switch(C_menuN) {
	case CLANSERVER_INSPECT:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();						
		break;
	case CLAN_READ_CLANMARK:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();				
		break;	
	case CLAN_isCheckClanJang_Result:
	case CLAN_isCheckClanJang:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();				
		break;	
	case CLAN_MAKE_is:		//Å¬·£°á¼ºÀÇ»ç È®ÀÎ Y/N

		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();				
		break;
	case CLANMAKE_NOMONEY:
	case CLAN_MONEY_is:		// ´ë±ÝÁöºÒÀÇ»ç¹¯±â

		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();

		break;
	case CLAN_NAME_is:		//Å¬·£ÀÌ¸§ ¼³Á¤.
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_NONAME:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_NAME_BEING:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_MARK_expl:		//Å¬·£¸¶Å©¼³¸í
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		
		break;
	case CLAN_INSPECTION_expl:		//½É»ç±â°£¼³¸í
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		
		break;
	case CLAN_NotMake:	//Å¬·£°á¼º¸øÇÏ´Ï µ¹¾Æ°¡¶ó
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLANMAKE_DOWN_INFO:
	case CLANMAKE_READ_CLANMARK:
	case CLAN_MAKE_FINISH:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_AllEnd:	//Å¬·£ ³¡³ª°í Å¬¸®¾îÇÏ´Â¼ÂÆÃºÎºÐ.
		break;


	case CLAN_ERROR:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;


	//============¯…¿ë¸Þ´º
	//case CLAN_CHIP_ReadDATA:		
	//	Draw_C_menuN();
	//	break;
	case CLAN_CHIP:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
		//ycy
		//========= Ãß¹æ
	case CLAN_REMOVE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_REMOVE_SHORTCLANWON_RESULT:		
	case CLAN_REMOVE_RESULT:
	DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_REMOVE_SHORTCLANWON:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_RELEASE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_RECONF_RELEASE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_FINISH_RELEASE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLANCHIP_RELEASEMESSAGE:
	case CLAN_WAREHOUSE_RIGHT:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_MARK1:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_MARK2:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_MARK3:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_HELP:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_HELP_CONTENT:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;	

	case ISPFLAG_CHIP:
	case CLANCHIP_MAKE_FIRSTMESSAGE: 
	case CLANCHIP_DOWN6_CLANWON:    
	case CLANCHIP_DOWN6_CLANWON_MESSAGE:
	case CLAN_NOMAKE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	//=========Ä¨ À¯Àú¸Þ´º
	case CLANUSER_REMOVE ://Å¬·£¿øÀÌ Ãß¹æ´çÇßÀ»°æ¿ì
	case	CLANUSER_AUTORELEASE://ÀÚµ¿ÇØÃ¼
	case CLAN_USER:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	//Å¬·£À» ¾ÆÁ÷ °á¼º ¸øÇÔ
	case CLAN_USER_NOMAKE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_SECESSION:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_RECONF_SECESSION:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_FINISH_SECESSION:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLANUSER_MAKE_FIRSTMESSAGE:			
	case CLANUSER_DOWN6_CLANWON_MESSAGE:
	case CLANUSER_DOWN6_CLANWON:			
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;

	//Å¬·£¯…ÀÌ °¡ÀÔ±ÇÀ¯ÇÒ¶§ ¶ß´Â ¸Þ´º
	case CLANUSER_JOIN:
	case CLAN_JOIN:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;

	case CLAN_USER_WAREHOUSE:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_USER_HELP:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_USER_HELP_CONTENT:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;	

	case CLAN_DISPLAY_END:
	case CLAN_DISPLAY_OPEN:		
	case CLAN_DISPLAY_CLOSE:
	case CLAN_CLOSE_ENDING:
	case CLAN_DISPLAY_INFO:
	case CHATLIST_READDATA:	
#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
		//cClanChatting.Draw();
#else
		cClanChatting.Draw();
#endif
		break;	


	case CLAN_DELETECHA_RELEASECLAN_BEFORE: 
	case CLAN_DELETECHA_RELEASECLAN:      
	case	CLAN_DELETECHA_SECESSION_BEFORE:	
	case	CLAN_DELETECHA_SECESSION:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		cClanDeleteCha.Draw();
		break;
	case CLAN_REAMMAKR_ERROR:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;
	case CLAN_RELEASE_LOADING:
	case CLAN_REMOVE_SHORTCLANWON_LOADING:
	case CLAN_REMOVE_LOADING:
	case CLAN_CHIP_ReadDATA:
	case CLAN_NPC_START:
	case CLAN_USER_ReadDATA:
	case ISPFLAG_USER:
	case CLANUSER_RELEASEMESSAGE:
	case CLANUSER_RELEASE_FLAG:
	case CLAN_RECONF_SECESSION_LOADING:
		DrawBox(BackStartPos.x,BackStartPos.y,4,5);		
		Draw_C_menuN();
		break;	
	
	}	

#ifndef USE_PROSTONTALE		//=========== À©µµÇÔ¼ö
	window_renderFLIP();
#endif

	return TRUE;
}


//sw==0 : ÀÏ¹Ý sw==1: Å°ÀÔ·ÂÀ» ¹Þ´Â°æ¿ì sw==2: Å¬·£ÀÌ¸§À» Å¸ÀÌÆ²¿¡ ¾µ°æ¿ì»ç¿ë(±×¸²ÀÚÃ³¸®)
void CLAN_MENU::Draw_C_Text(char *str, int x, int y, int sw=0)
{
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
	char strbuf[1024];

	HDC hdc;
	HFONT oldFont;
	lpDDSBack->GetDC(&hdc);
	SetBkMode( hdc, TRANSPARENT );

	if(hFont != NULL)
		oldFont = (HFONT)SelectObject( hdc , hFont );


	ZeroMemory(strbuf, sizeof(strbuf));

	int strL = strlen(str);
	int cnt=0;
	int i=0;

	while(1) {
		

		strbuf[i] = str[cnt];
		cnt++;
		if(strbuf[i] == '\n')  {
			strbuf[i] =0;
			i=0;
jmp1:


			
			if(sw==2){
				SetTextColor( hdc, RGB(100, 100, 100));	//±×¸²ÀÚ¿ë ±ÛÀÚ»ö
				//SetTextColor( hdc, RGB(255, 255,255));	//±×¸²ÀÚ¿ë ±ÛÀÚ»ö
				TextOut(hdc, x+1, y+1, strbuf, lstrlen(strbuf));

				SetTextColor( hdc, RGB(230, 255, 240));	//ÀÏ¹Ý±ÛÀÚ»ö
				TextOut(hdc, x, y, strbuf, lstrlen(strbuf));
			}
			else     {
				SetTextColor( hdc, RGB(230, 255, 240));	//ÀÏ¹Ý±ÛÀÚ»ö
				TextOut(hdc, x, y, strbuf, lstrlen(strbuf));
			}
			
			/*
			if(sw==2){
				SetTextColor( hdc, RGB(255, 255, 255));	//Å¬·£ÀÌ¸§ÀÔ·Â½Ã Èò»ö				
			}
			else {
				SetTextColor( hdc, RGB(230, 255, 240));	//ÀÏ¹Ý±ÛÀÚ»ö
			}
			TextOut(hdc, x, y, strbuf, lstrlen(strbuf));
			*/
			

			ZeroMemory(strbuf, sizeof(strbuf));
			y+=16;

			if(cnt>=strL) break;
		}
		else if(cnt>=strL)
		{
			goto jmp1;
		}
		else {
			i++;
		}
	}
	

	if(sw==1) 
	{
		static cnt=0;
		cnt++;
		if(cnt<20) {
			SetTextColor( hdc, RGB(255, 255, 255));
			TextOut(hdc, x + (lstrlen(str)*6) , y-16, "|", 1);
		}
		if(cnt>=40) cnt=0;
	}

	if(oldFont)
		SelectObject( hdc, oldFont);
	lpDDSBack->ReleaseDC(hdc);
#else

#endif
}






//ycy : ¹Ú½º ±×¸®±â
int CLAN_MENU::DrawBox(int x,int y,int x_width,int y_height)
{

	isDrawClanMenu = 1;


#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	#define BOXFONTSIZE 64

	if(lpD3DDevice == NULL)
		return FALSE;

	lpD3DDevice->BeginScene();


	//µÞ¹è°æÂï±â
	//dsDrawColorBox( RGBA_MAKE(0,15,128,125), x+32, y+32, 	(x_width+1)*64, 	(y_height+1)*64);
	//dsDrawColorBox( RGBA_MAKE(0,20,10,125), x+32, y+32, 	((x_width+1)*64)+1, 	((y_height+1)*64)+1);
	
	sinDrawTexImage( hBox5, (float)x+64, (float)y+32, (float)( 64+(  ( x_width-2) *32) ), (float)(   32+(  (y_height-1) *32) ),
				0,0,
				0, 0, 32, 32, 
				255);
	//DrawSprite(x  ,y , hBox1_ktj ,  0, 0, 32, 32, 1);
	//int sinDrawTexImage( int Mat, float x, float y, float w, float h, 
	//						float surW, float surH, 
	//						float tLeft, float tTop, float tRight, float tBottom, 
	//						int Transp);
	//left, top
	sinDrawTexImage( hBox1,(float) x,(float) y, (float)64, (float)32,    
				(float)0,(float)0,  
				(float)0, (float)0, (float)64, (float)32,
				255);
	//right, top
	sinDrawTexImage( hBox3,(float)(   x+(32*x_width) +64 ), (float)y, 64, 32,    
				(float)0,(float)0,  
				(float)0, (float)0, (float)64, (float)32,
				255);	
	//left, bottom
	sinDrawTexImage( hBox7, (float)x, (float)(  y+(32*y_height) + 32 ), 64, 64 ,
				(float)0,(float)0,  
				(float)0, (float)0, (float)64, (float)32,
				255);
	//right, top
	sinDrawTexImage( hBox9,(float)(   x+(32*x_width) +64 ), (float)(  y+(32*y_height) + 32  ), 64, 64,    
				(float)0,(float)0,  
				(float)0, (float)0, (float)64, (float)32,
				255);
				
	int line;
	int sx = x+64;
	int ey=  y+(32*y_height) + 32 ;
	for(line=0; line<x_width; line++) {
		//À§ÂÊ ¶óÀÎ
		sinDrawTexImage( hBox2, (float) sx, (float)y, 32, 32,
				(float)0,(float)0,  
				(float)0, (float)0, (float)32, (float)32,
				255);
		
		//¾Æ·¡ÂÊ ¶óÀÎ
		sinDrawTexImage( hBox8, (float)sx, (float)( ey ), 32, 64,
				0,0,
				0, 0, 32, 64,
				255);
				
		sx+=32;
	}

	int sy = y+32;
	int ex = x+(32*x_width) + 64 ;
	for(line=0; line<y_height; line++) {
		//¿À¸¥ÂÊ¶óÀÎ
		sinDrawTexImage( hBox4, (float)x, (float)sy, 64, 32,
				0,0,
				0, 0, 64, 32,
				255);
		//¿ÞÂÊ ¶óÀÎ
		sinDrawTexImage( hBox6,(float)ex, (float)sy, 64, 32,
				0,0,
				0, 0, 64, 32,
				255);
		sy+=32;
	}
	
	





	lpD3DDevice->EndScene();

	//DrawSprite(x+128,y+6, hClanTitle ,  0, 0, 139, 32, 0);
	//DrawSprite(x+ ( ((x_width*64)/2))-2 , y-2, hClanTitle ,  0, 0, 139, 32, 1);
	//DrawSprite(x + 90 ,y -10, hClanTitle ,  0, 0, 139, 32, 1);


#else



	//HBITMAP backu_BMP;
	//HDC backu_dc;



	
	BITMAP bit;
	int bx,by;
	int i;
	int nTotal_Width = 0;
	int nTotal_Height = 0;
	//int nStart_x;
	





	HDC hdc=GetDC(g_hwnd);

	HDC sorDC =CreateCompatibleDC(hdc);
	HDC destdc=CreateCompatibleDC(hdc);

	SelectObject(destdc, Dback.hb );		//off½ºÅ©¸°¼³Á¤.
	//SelectObject(sorDC, backBMP );

	/*
	for(int i=0; i<coordNum; i++) {

		BitBlt(
			destDC,
			coordB[i].left,coordB[i].top,
			//ÁÖÀÇ : ±æÀÌÀÓ.
			coordB[i].right, coordB[i].bottom,	//offscrÀ» À©µµÃ¢À¸·Î Ä«ÇÇ
			sorDC,
			coordB[i].left,coordB[i].top,
			SRCCOPY);

	}

	
	*/


	//¿ÞÂÊ top
	SelectObject(sorDC, hBox1);
	GetObject(hBox1,sizeof(BITMAP),&bit);
	bx = bit.bmWidth;
	nTotal_Width+=bx;
	by = bit.bmHeight;	
	BitBlt(destdc,x,y,bx,by,sorDC,0,0,SRCCOPY);

	for(i=0; i< x_width; i++)
	{
		SelectObject(sorDC,hBox2);
		GetObject(hBox2,sizeof(BITMAP),&bit);	
		bx = bit.bmWidth;
		nTotal_Width +=bx;
		by = bit.bmHeight;
		StretchBlt(destdc,x+BOXCORNER_WIDTH+(bx*i),y,bx,by,sorDC,0,0,bx,by,SRCCOPY);
	}

	//¿À¸¥ÂÊ top

	SelectObject(sorDC, hBox3 );
	GetObject(hBox3,sizeof(BITMAP),&bit);	
	bx = bit.bmWidth;
	by = bit.bmHeight;	
	StretchBlt(destdc,x+nTotal_Width,y,bx,by,sorDC,0,0,bx,by,SRCCOPY);


    //¿ÞÂÊ Å×µÎ¸®

	for(i=0; i< y_height; i++)
	{
		SelectObject(sorDC, hBox4 );
		GetObject(hBox4,sizeof(BITMAP),&bit);	
		bx = bit.bmWidth;
		by = bit.bmHeight;
		nTotal_Height +=by ;
		StretchBlt(destdc,x,y+BOXCORNER_HEIGHT+(by*i),bx,by,sorDC,0,0,bx,by,SRCCOPY);
	}



	
	
	//¿ÞÂÊ BOTTOM 	
	SelectObject(sorDC,hBox7);
	GetObject(hBox7,sizeof(BITMAP),&bit);	
	bx = bit.bmWidth;
	by = bit.bmHeight;
		
	StretchBlt(destdc,x,y +BOXCORNER_HEIGHT+nTotal_Height,bx,by,sorDC,0,0,bx,by,SRCCOPY);

	for(i=0; i< x_width; i++)
	{
		SelectObject(sorDC,hBox8);
		GetObject(hBox8,sizeof(BITMAP),&bit);	
		bx = bit.bmWidth;		
		by = bit.bmHeight;
		StretchBlt(destdc,x+BOXCORNER_WIDTH+(bx*i),y +BOXCORNER_HEIGHT+nTotal_Height,bx,by,sorDC,0,0,bx,by,SRCCOPY);
	}	

	//¿À¸¥ÂÊ BOTTOM
	SelectObject(sorDC,hBox9);
	GetObject(hBox9,sizeof(BITMAP),&bit);	
	bx = bit.bmWidth;
	by = bit.bmHeight;	
	StretchBlt(destdc,x+nTotal_Width,y +BOXCORNER_HEIGHT+nTotal_Height,bx,by,sorDC,0,0,bx,by,SRCCOPY);	

 
	//¿À¸¥ÂÊ Å×µÎ¸®
	for(i=0; i< y_height; i++)
	{
		SelectObject(sorDC, hBox6 );
		GetObject(hBox6,sizeof(BITMAP),&bit);	
		bx = bit.bmWidth;
		by = bit.bmHeight;		
		StretchBlt(destdc,x+nTotal_Width,y+BOXCORNER_HEIGHT+(by*i),bx,by,sorDC,0,0,bx,by,SRCCOPY);
	}


	DeleteDC(sorDC);
	DeleteDC(destdc);

	ReleaseDC(g_hwnd, hdc);


#endif

	return TRUE;
}

void CLAN_MENU::RenderMenu()
{
	/*
	HDC hdc,sorDC;
	HBITMAP oldbmp;

	hdc = GetDC(g_hwnd);
	sorDC  = CreateCompatibleDC(hdc );
	oldbmp = (HBITMAP)SelectObject(sorDC,membmp->membmp);
	BitBlt(hdc,0,0,800,600,sorDC,0,0,SRCCOPY);
	SelectObject(sorDC,oldbmp);	
	DeleteDC(sorDC);
	ReleaseDC(g_hwnd,hdc);
	*/
}
/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º ¸í: class CLAN_MENU
: ÇÏ ´Â ÀÏ : 
: ¼­    ½Ä :
: ÀÎ    ÀÚ :
: ¹Ý È¯ °ª :
: ³í    Æò : À¥db¸¦ »ç¿ëÇØ µ¥ÀÌÅ¸¸¦ »Ì¾Æ³»¾ß ÇÒ°æ¿ì º» Å¬¶ó½º¸¦ ÀÌ¿ëÇÑ´Ù.
: ¹ö    Àü :
: PROJECT  :
///////////////////////////////////////////////////////////////////////////*/
void CLAN_MENU::Draw_C_menuN()
{		
	cBaseMenu.Draw();
	cClanChip.Draw();	
	cClanUser.Draw();	
	cClanMake.Draw();	
	cClanReport.Draw();
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
#else
	cClanChatting.Draw();
#endif
	cClanJoin.Draw();
}


//=============µå·Î¿ì °ü·Ã ÇÔ¼ö ==================================================================end









/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º ¸í: Å¬·£ ½ºÆ®·°Ã³ µ¥ÀÌÅ¸ ¼ÂÆÃ ÇÔ¼öµé
: ÇÏ ´Â ÀÏ : 
: ¹Ý È¯ °ª :
///////////////////////////////////////////////////////////////////////////*/
//À¯Àú id, Ä³¸¯ÅÍ id, ±º¼­¹ö ÀÌ¸§.
void CLAN_MENU::SetUserData(char* id, char* chaname, char* gserver,int money, int ability)
{
	strcpy(cldata.szID,  id);
	strcpy(cldata.ChaName, chaname);
	strcpy(cldata.gserver, gserver);

	cldata.money = money;
	//ktj : ÀÓ½Ã
	//if(cldata.money ==0) cldata.money = 100000;
	cldata.ability = ability;
}



//±ÇÀ¯¸¦ ¹Þ´Â »ç¶÷ÇÑÅ×¿À´Â µ¥ÀÌÅ¸
void CLAN_MENU::SetJoinData(char *joinclanname, char *joinclanchip)
{
	strcpy(cldata.szJoinClanName,    joinclanname);			//°¡ÀÔµÉ Å¬·£ ÀÌ¸§
	strcpy(cldata.szJoinClanChipName,joinclanchip);		//°¡ÀÔµÉ Å¬·£ÀÇ Å¬·£Àå id
}











#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

void DrawSprite_TJB(int winX ,int winY, int winxlen, int winylen, LPDIRECTDRAWSURFACE4 pdds,int x, int y, int width, int height,int Bltflag)
{
	if(!pdds) return;

	//int dx,dy,sx,sy,px,py;
    RECT rcRect;
	RECT dRect;

	dRect.left = winX;
	dRect.right = winX+winxlen;
	dRect.top = winY;
	dRect.bottom = winY+winylen;

	rcRect.top		= 0;
	rcRect.left		= 0;
	rcRect.bottom   = 32;
	rcRect.right    = 32;

//if(Bltflag)
	//{
		//ddrval=lpDDSPrimary->Blt(&destRect,lpDDSBack,&rcRect,NULL,NULL);

		//lpDDSBack->BltFast(winX+px,winY+py,pdds,&rcRect,DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);

		lpDDSBack->Blt(&dRect,
					pdds,
					&rcRect,
					DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY,
					NULL);
/*
	}
	else
		//lpDDSBack->BltFast( winX,winY, pdds, &rcRect,DDBLTFAST_WAIT);
		lpDDSBack->BltFast( winX,winY, pdds, &rcRect,DDBLTFAST_WAIT);
		*/

}
#endif




#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
#else

void Draw_Text(int x, int y,char *str)
{
	HANDLE hFont,hOldFont;
	HDC hdc;	
	hdc = GetDC(g_hwnd);
    	hFont = CreateFont( 12,
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
                        "±¼¸²Ã¼" );

	hOldFont = SelectObject(hdc,hFont);
	char strbuf[1024];


	ZeroMemory(strbuf, sizeof(strbuf));

	int strL = strlen(str);
	int cnt=0;
	int i=0;
	
	while(1)
	{
		strbuf[i] = str[cnt];
		cnt++;
		if(strbuf[i] == '\n')  {
			strbuf[i] =0;
			i=0;
jmp1:		
			
			TextOut(hdc, x, y, strbuf, lstrlen(strbuf));

			ZeroMemory(strbuf, sizeof(strbuf));
			y+=16;

			if(cnt>=strL) break;
		}
		else if(cnt>=strL)
		{
			goto jmp1;
		}
		else {
			i++;
		}
	}
	
	//DrawText(GetDC(g_hwnd),string,-1,&rect,DT_LEFT | DT_WORDBREAK);

	SelectObject(hdc,hOldFont);
	DeleteObject(hFont);

	ReleaseDC(g_hwnd,hdc);
	
}
#endif



void Text_Out1(HDC hdc,int x, int y,char *str)
{
}

void Text_Out2(HDC hdc,int x, int y,char *str)
{
}

void Text_Out3(HDC hdc,int x, int y,char *str)
{
}
void Text_Out(HDC hdc,int x, int y,char *str)
{
	HANDLE hFont,hOldFont;

	
    	hFont = CreateFont( 12,
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
                        "±¼¸²Ã¼" );

	hOldFont = SelectObject(hdc,hFont);
	char strbuf[1024];


	ZeroMemory(strbuf, sizeof(strbuf));

	int strL = strlen(str);
	int cnt=0;
	int i=0;
	
	while(1)
	{
		strbuf[i] = str[cnt];
		cnt++;
		if(strbuf[i] == '\n')  {
			strbuf[i] =0;
			i=0;
jmp1:		
			
			TextOut(hdc, x, y, strbuf, lstrlen(strbuf));

			ZeroMemory(strbuf, sizeof(strbuf));
			y+=16;

			if(cnt>=strL) break;
		}
		else if(cnt>=strL)
		{
			goto jmp1;
		}
		else {
			i++;
		}
	}
	
	//DrawText(GetDC(g_hwnd),string,-1,&rect,DT_LEFT | DT_WORDBREAK);

	SelectObject(hdc,hOldFont);
	DeleteObject(hFont);
	
	
}



void CLAN_MENU::Load()
{
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	MatPartyBackGround =
		CreateTextureMaterial( "image\\Sinimage\\Help\\party_win.tga" , 0, 0, 0,0, SMMAT_BLEND_ALPHA );
	

	//box===========================================================================start

	hBox1 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\HelpBoxCorner.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox2 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\HelpBoxLine.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox3 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box3.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox4 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box4.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );
	hBox5 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box5.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox6 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box6.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox7 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box7.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox8 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box8.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hBox9 = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\box9.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );

	//box===========================================================================end


	/*
	//ktj : scroll
	hScl_Bar_tj= CreateTextureMaterial( "image\\Sinimage\\Help\\scl_bar.tga", 0, 0, 0,0, SMMAT_BLEND_ALPHA );
	hScl_icon_tj = CreateTextureMaterial( "image\\Sinimage\\Help\\scl_icon.tga", 0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hOk_Gray  = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Ok_Glay.bmp");
	hOk_Yellow= LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Ok_Yellow.bmp");
	//hCancel_Gray = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Cancel_Glay.bmp");
	//hCancel_Yellow = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Cancel_Yellow.bmp");
	hYes_Gray = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Yes_Glay.bmp");
	hYes_Yellow = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Yes_Yellow.bmp");
	//hNo_Gray = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_No_Glay.bmp");
	//hNo_Yellow =  LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_No_Yellow.bmp");

	//CLAN CHIP
	//hMark = LoadDibSurfaceOffscreen("image\\Sinimage\\chipbutton\\Mark.bmp");
	hClanName = LoadDibSurfaceOffscreen("image\\Sinimage\\Help\\box-test-title.bmp");
	hWareHouse[0]= LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whc.bmp");
	hWareHouse[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whc1.bmp");
	hWHRight[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whr.bmp");
	hWHRight[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whr1.bmp");
	//hClanMark[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\m.bmp");
	hClanMark[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\m1.bmp");
	hRemove[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\r.bmp");
	hRemove[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\r1.bmp");
	hClanRelease[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\cr.bmp");
	hClanRelease[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\cr1.bmp");
	hHelp[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\h.bmp");
	hHelp[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\h1.bmp");
	hOut[0] = LoadDibSurfaceOffscreen("image\\clanImage\\MessageBox\\o.bmp");
	hOut[1] = LoadDibSurfaceOffscreen("image\\clanImage\\MessageBox\\o1.bmp");
	//Å¬·£ ±¸¼º¿ø
	hSecession[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\s.bmp");
	hSecession[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\s1.bmp");
	//Ãß¹æ,Ã¢°í ±ÇÇÑ
	hIcon = LoadDibSurfaceOffscreen("image\\party\\party_man_0.bmp");
	hIcon_Click = LoadDibSurfaceOffscreen("image\\party\\party_man_1.bmp");

										
	hClose[0] = LoadDibSurfaceOffscreen("image\\clanImage\\MessageBox\\c.bmp");
	hClose[1] = LoadDibSurfaceOffscreen("image\\clanImage\\MessageBox\\c1.bmp");
	//ÇïÇÁ
	hHelp1 = LoadDibSurfaceOffscreen("image\\Sinimage\\chipbutton\\help1.bmp");


	//hButton_Box = LoadDibSurfaceOffscreen("image\\Sinimage\\Help\\box-test-title2.bmp");
	hClanTitle = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\title.bmp");



	//Ãß¹æ¸Þ´º¾È¿¡¼­ Ãß¹æ ¹öÆ°
	hRemoveToMenu[0]= LoadDibSurfaceOffscreen("image\\clanImage\\MessageBox\\Button_deport_Glay.bmp");
	hRemoveToMenu[1] = LoadDibSurfaceOffscreen("image\\clanImage\\MessageBox\\Button_deport_Yellow.bmp");

	//½ºÅ©·Ñ¹Ù
	hScl_Bar= LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\scl_bar.bmp");
	hScl_icon = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\msBox.bmp");
	
	hClanNameInput = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\clan_name.bmp");

	//====Å¬·£ ¹öÅÏ¼³¸í ºÎºÐ
	hTxt_Remove[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_r_.bmp");

	hTxt_Remove[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_r.bmp");

	hTxt_Release[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_cr_.bmp");

	hTxt_Release[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_cr.bmp");

	hTxt_Mark[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_m_.bmp");

	hTxt_Mark[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_m.bmp");

	hTxt_Help[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_h_.bmp");

	hTxt_Help[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_h.bmp");

	hTxt_Secession[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_out_.bmp");

	hTxt_Secession[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\txt_out.bmp");

	*/

#else
	

	MatPartyBackGround = (HBITMAP)LoadImage(
					NULL, 
					"image\\Sinimage\\Help\\box-test-title2.bmp",
					IMAGE_BITMAP,
					0,0,
					LR_LOADFROMFILE);

//box===========================================================================start

	hBox1= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hBox2= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box2.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hBox3= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box3.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hBox4= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box4.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hBox6= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box6.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hBox7= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box7.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hBox8= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box8.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hBox9= (HBITMAP)LoadImage(NULL,"image\\clanimage\\MessageBox\\box9.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	
	
	//box===========================================================================end



	hOk_Gray= (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_Ok_Glay.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);	

	hOk_Yellow = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_Ok_Yellow.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);


	if(hOk_Yellow == NULL)
	{
		char temp[50];
		wsprintf(temp,"Num : %d",GetLastError());
	}

	hCancel_Gray = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_Cancel_Glay.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hCancel_Yellow = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_Cancel_Yellow.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hYes_Gray = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_Yes_Glay.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hYes_Yellow = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_Yes_Yellow.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hNo_Gray = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_No_Glay.bmp",
						IMAGE_BITMAP,	
						0,0,
						LR_LOADFROMFILE);

	hNo_Yellow =  (HBITMAP)LoadImage(NULL,"image\\Sinimage\\MessageBox\\Button_No_Yellow.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);




	//CLAN CHIP
	//hMark = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\chipbutton\\Mark.bmp",
	//					IMAGE_BITMAP,
	//					0,0,
	//					LR_LOADFROMFILE);
	hClanName = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\Help\\box-test-title.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hWareHouse[0]= (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\whc.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hWareHouse[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\whc1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hWHRight[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\whr.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hWHRight[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\whr1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hClanMark[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\m.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hClanMark[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\m1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hRemove[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\r.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hRemove[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\r1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hClanRelease[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\cr.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hClanRelease[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\cr1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hHelp[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\h.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hHelp[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\h1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hOut[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\MessageBox\\o.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hOut[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\MessageBox\\o1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	//Å¬·£ ±¸¼º¿ø
	hSecession[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\s.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hSecession[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\s1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
//Ãß¹æ,Ã¢°í ±ÇÇÑ
	hIcon = (HBITMAP)LoadImage(NULL,"image\\party\\party_man_0.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hIcon_Click = (HBITMAP)LoadImage(NULL,"image\\party\\party_man_1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
//Ãß¹æ¸Þ´º¾È¿¡¼­ Ãß¹æ ¹öÆ°
	hRemoveToMenu[0]= (HBITMAP)LoadImage(NULL,"image\\clanImage\\MessageBox\\Button_deport_Glay.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hRemoveToMenu[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\MessageBox\\Button_deport_Yellow.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hClose[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\MessageBox\\c.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hClose[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\MessageBox\\c1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	//ÇïÇÁ
	hHelp1 = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\chipbutton\\help1.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hButton_Box = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\msBox.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hClanTitle = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\title.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hScl_Bar = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\scl_bar.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	hScl_icon = (HBITMAP)LoadImage(NULL,"image\\Sinimage\\help\\scl_icon.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);
	//====Å¬·£ ¹öÅÏ¼³¸í ºÎºÐ
	hTxt_Remove[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_r_.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Remove[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_r.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Release[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_cr_.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Release[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_cr.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Mark[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_m_.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Mark[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_m.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Help[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_h_.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Help[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_h.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Secession[0] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_out_.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hTxt_Secession[1] = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\txt_out.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	


	
	

#endif
}



//simMain.cpp¿¡¼­ ºÒ·¯¼­ Å¬·ÎÁî ÇÑ´Ù.
void clanMENU_close()
{
	Interface_clanmenu.Close();
}

void CLAN_MENU::Close()
{
	#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	/*if(	hOk_Gray ) {   hOk_Gray->Release();  hOk_Gray = NULL;}
	if(	hOk_Yellow ) {   hOk_Yellow->Release();  hOk_Yellow= NULL;}
	if(	hCancel_Gray ) {   hCancel_Gray->Release();  hCancel_Gray= NULL;}
	if(	hCancel_Yellow) {   hCancel_Yellow->Release();  hCancel_Yellow= NULL;}
	if(	hYes_Gray ) {   hYes_Gray->Release();  hYes_Gray = NULL;}
	if(	hYes_Yellow ) {   hYes_Yellow->Release();  hYes_Yellow = NULL;}
	if(	hNo_Gray ) {   hNo_Gray->Release();  hNo_Gray = NULL;}
	if(	hNo_Yellow ) {   hNo_Yellow ->Release();  hNo_Yellow = NULL;}

	//CLAN CHIP
	if(	hMark ) {   hMark->Release();  hMark = NULL;}
	if(	hClanName ) {   hClanName->Release();  hClanName = NULL;}
	


	for(int i=0 ; i <2; i++)
	{
		if(hWHRight[i]) { hWHRight[i]->Release(); hWHRight[i]= NULL; } 
		if(hWareHouse[i]) { hWareHouse[i]->Release(); hWareHouse[i] = NULL;}
		if(hClanMark[i]) { hClanMark[i]->Release(); hClanMark[i] = NULL;}
		if(hRemove[i]) { hRemove[i]->Release(); hRemove[i] = NULL;}
		if(hClanRelease[i]) { hClanRelease[i]->Release(); hClanRelease[i] = NULL;}
		if(hHelp[i]) { hHelp[i]->Release(); hHelp[i] = NULL;}	
		if(hOut[i]) { hOut[i]->Release(); hOut[i] = NULL;}	
		if(hClose[i]) { hClose[i]->Release(); hClose[i] = NULL;}	
		if(hSecession[i]) { hSecession[i]->Release(); hSecession[i] = NULL;}	
		if(hTxt_Remove[i]) { hTxt_Remove[i]->Release(); hTxt_Remove[i] = NULL;}	
		if(hTxt_Release[i]) { hTxt_Release[i]->Release(); hTxt_Release[i] = NULL;}	
		if(hTxt_Mark[i]) { hTxt_Mark[i]->Release(); hTxt_Mark[i] = NULL;}	
		if(hTxt_Help[i]) { hTxt_Help[i]->Release(); hTxt_Help[i] = NULL;}	
		if(hTxt_Secession[i]) { hTxt_Secession[i]->Release(); hTxt_Secession[i] = NULL;}
		
		
	}

	//Ãß¹æ
	if(	hIcon ) {   hIcon->Release();  hIcon = NULL;}
	//ÇïÇÁ
	if(	hHelp1 ) {   hHelp1->Release();  hHelp1 = NULL;}

	if(	hButton_Box) {   hClanTitle ->Release();  hClanTitle = NULL;}
	if(	hClanTitle ) {   hClanTitle ->Release();  hClanTitle = NULL;}


	//ygy ½ºÅ©·Ñ
	if(	hScl_Bar) {   hScl_Bar->Release();  hScl_Bar = NULL;}
	if(	hScl_icon ) {   hScl_icon->Release();  hScl_icon = NULL;}

	//ygyÅ¬·£ÀÌ¸§ ÀÔ·Â ÀÌ¹ÌÁö
	if(	hClanNameInput ) {   hClanNameInput->Release();  hClanNameInput = NULL;}*/
	


	if(cldata.hClanMark) { cldata.hClanMark->Release(); cldata.hClanMark = NULL;}
	if(cldata.hClanMark16) {cldata.hClanMark16->Release(); cldata.hClanMark16 = NULL;}

	cClanChatting.Close();
	cBaseMenu.Close();


	#else

	//bitmap¾ø¾Ù°Í.
	DeleteObject( MatPartyBackGround );
//	DeleteObject(hBoxCorner);
//	DeleteObject(hBoxLine);
	//DeleteObject(hBoxLine1);
	DeleteObject(hOk_Gray);
	DeleteObject(hOk_Yellow);
	DeleteObject(hCancel_Gray);
	DeleteObject(hCancel_Yellow);
	DeleteObject(hYes_Yellow);
	DeleteObject(hYes_Gray);
	DeleteObject(hNo_Gray);
	DeleteObject(hNo_Yellow);
	DeleteObject(hMark);
	DeleteObject(hClanName);
	DeleteObject(hScl_Bar);
	DeleteObject(hScl_icon);


	for(int i=0 ; i <2; i++)
	{
		DeleteObject(hWHRight[i]);
		DeleteObject(hWareHouse[i]);
		DeleteObject(hClanMark[i]);
		DeleteObject(hRemove[i]);
		DeleteObject(hClanRelease[i]);
		DeleteObject(hHelp[i]);
		DeleteObject(hOut[i]);
		DeleteObject(hClose[i]);
		DeleteObject(hSecession[i]);
		DeleteObject(hTxt_Remove[i]);
		DeleteObject(hTxt_Release[i]);
		DeleteObject(hTxt_Mark[i]);
		DeleteObject(hTxt_Help[i]);
		DeleteObject(hTxt_Secession[i]);
	}
	


	DeleteObject(hIcon);
	DeleteObject(hHelp1);



	#endif
}

void CLAN_MENU::Loading()
{
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
	Draw_C_Text(szLoading, BackStartPos.x + 80,BackStartPos.y + 90);
#else

	HDC hdc, sorDC,destdc;

	hdc = GetDC(g_hwnd);
	sorDC  = CreateCompatibleDC(hdc);
	destdc = CreateCompatibleDC(hdc);
	SelectObject(destdc, Dback.hb );		//off½ºÅ©¸°¼³Á¤.	
	Text_Out(destdc,BackStartPos.x + 80,BackStartPos.y + 90 ,szLoading);

	DeleteDC(sorDC);
	DeleteDC(destdc);
	ReleaseDC(g_hwnd, hdc );
#endif

}


#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
//Å¬·£¿ø Á¤º¸ Àü¼Û¿ë ÇÔ¼ö
int SendClanCommandUser( smWINSOCK *lpsmSock , DWORD	dwCode , int Param1, int Param2 , CLANWONLIST *lpClanUserList , int ClanUserCount );
#endif


void SetClanWon()
{
	if(cldata.nTotalClanWonCount==0) return;

	memcpy(ClanWonList.clanWon,cldata.clanWon,sizeof(cldata.clanWon));

	//========================================================

#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
	//Å¬·£¿ø Á¤º¸ Àü¼Û¿ë ÇÔ¼ö
	SendClanCommandUser( smWsockDataServer , smTRANSCODE_CLAN_UPDATE  , 0,0, &ClanWonList , cldata.nTotalClanWonCount );

	//========================================================
#endif

}





//savedata\\clanDATA ¸¸µé±â==============================================start
BOOL SearchDirectory(char *dir)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;	
	char t[50];
	wsprintf(t,"%s\\*.*",dir);
	hFind = FindFirstFile(t,&wfd);
	if(hFind == INVALID_HANDLE_VALUE)
	{		
		return FALSE;
	}	

	return TRUE;	
}

void MakeDirectory(char *dir)
{
	CreateDirectory(dir,NULL);
}


void CreateSaveData_dir()
{	
	char szTemp[100];
	if(!SearchDirectory(SAVEDATA_DIR)) MakeDirectory(SAVEDATA_DIR);
	if(!SearchDirectory(CLANDATA_DIR)) MakeDirectory(CLANDATA_DIR);

	ZeroMemory(szTemp,sizeof(szTemp));
	wsprintf(szTemp,"%s",CLANDATA_DIR);
	wsprintf(szTemp,"%s\\%s",szTemp,szConnServerName);

	if(!SearchDirectory(szTemp)) MakeDirectory(szTemp);

}

//savedata\\clanDATA ¸¸µé±â==============================================end

void Text_Out1()
{
}