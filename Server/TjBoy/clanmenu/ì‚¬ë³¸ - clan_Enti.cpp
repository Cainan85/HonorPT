#define CLAN_EITI_CPP
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>

#include "tjclanDEF.h"

#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	#include "..\\..\\sinbaram\\SinLinkHeader.h"
	#include "..\\..\\HoBaram\\HoLinkHeader.h"
	#include "..\\..\\language.h"
	#include "..\\..\\HoBaram\\HoTextFile.h"
	#include "cE_CViewClanInfo.h"
#endif

#include "tjclan.h"
#include "clan_enti.h"
#include "cE_report.h"

#include "..\\isaocheck\\auth.h"
#include "..\\isaocheck\\isaocheck.h"

extern char szConnServerName[16];	

#ifdef USE_PROSTONTALE		//=========== ÇÁ¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì


#else						//=========== À©µµ¿ì ¼Ò½ºÀÎ °æ¿ì


#endif						//endif

void fd2( char *fmt, ...);

//ktj : °ø¿ëÀ¸·Î »ç¿ëÇÏ´Â ¸Þ´º¸¦ ¿©±â¼­ ¼ÂÆÃÇÏ°í ÀÖÀ½.
//case CLAN_AllEnd:			
//case CLAN_READ_CLANMARK:
//case CLAN_NPC_START:			//npc¿Í ¸¸³µÀ»¶§ À¯ÀúÀÇ Æ÷Áö¼ÇÀ» ¾Ë¾Æ³½ÈÄ °¢±â ´Ù¸¥ ¸Þ´º¸¦ ºÒ·¯ÁØ´Ù.



CLANEntity::CLANEntity()
{
	life =0;

}	

//***************************************************************************************
CLANEntity::~CLANEntity()
{
}



/*///////////////////////////////////////////////////////////////////////////
: ÇÔ ¼ö ¸í : Init()
: ³í    Æò :
: PROJECT  :
///////////////////////////////////////////////////////////////////////////*/
int CLANEntity::Init()
{
	return 1;
}
int CLANEntity::Clear()
{
	return 1;
}


/*///////////////////////////////////////////////////////////////////////////
: ÇÔ ¼ö ¸í : Main()
: ³í    Æò :
: PROJECT  :
///////////////////////////////////////////////////////////////////////////*/
void CLANEntity::Main()
{	
	char szFile[255];
	char szFile2[255];	
	FILE *fp;
	FILE *fp2;
	char server;
	char UniqueClanMark[64];
	int nClanMark;

	int i;

	switch(C_menuN){		
		case CLANSERVER_INSPECT_BEFORE:
			C_menuN = CLANSERVER_INSPECT;
			menuInit(C_menuN);
			break;
		case CLANSERVER_INSPECT:
			i = chkeckMenuN();
			switch(i) {
			case 0:
				menuMouseOver[0] = 1;
				break;
			default:
				memset(menuMouseOver,-1,sizeof(int)*30);
				break;
			}
			if(MOUSE0())			
			{	
				MOUSE0()=0;
				i = i;
				switch(i) 
				{
				case 0:
					C_menuN = CLAN_AllEnd;
					//C_menuN = CLAN_ERROR_ret;		//³ª°¡±â : ³¡³»´Â°É·Î°£´Ù.
					//menuInit(C_menuN );

					C_menuN2 =-1;
					CLAN_ERROR_ret=-1;
					break;	
				default:
					i = i;
					break;

				}
			}
			break;			
		case UPDATE_ISCLANMEMBER:
			switch(C_menuN2){
				case 0:
					ANSdata.ret_val= -1;			

					WebDB.Resume() ;		//À¥db¸¦ Àç°³ÇÑ´Ù.
					WebDB.isClanMember(cldata.szID,cldata.gserver,cldata.ChaName);		//ÀÌ id°¡ Å¬·£¿øÀÎÁö¸¦ ÆÄ¾ÇÇÑ´Ù.						
					C_menuN2=1;
					break;
				case 1:
					switch(ANSdata.ret_val){
						case 0://¹«µî·Ï   				
							WebDB.Suspend(); 
							bIsLoading = FALSE;
							C_menuN2=0;
							C_menuN=CLAN_AllEnd;
							cldata.myPosition = CLANNO_REGESTER;
							
							break;


						case 1://Å¬·£¿øµî·Ï
							WebDB.Suspend(); //À¥db ÁßÁö
							bIsLoading = FALSE;
							//ygy : ²À »ì¸±°Í
							ParsingIsClanMember(ANSdata.ret_Buf);
							
							cldata.myPosition = CLANUSER;			//Å¬·£¿ø Æ÷Áö¼Ç ÄÚµå´Â 100¹øÀÓ							
							C_menuN2=0;
							C_menuN=UPDATE_CLANWON;
							break;

						case 2://Å¬·£Àåµî·Ï
							WebDB.Suspend(); //À¥db ÁßÁö
							bIsLoading = FALSE;
							//ygy : ²À »ì¸±°Í
							ParsingIsClanMember(ANSdata.ret_Buf);
							C_menuN2=0;
							C_menuN=UPDATE_CLANWON;
							cldata.myPosition = CLANCHIP;				//Å¬·£Àå Æ÷Áö¼Ç ÄÚµå´Â 101¹øÀÓ
							break;

						case 100:
							WebDB.Suspend();							
							C_menuN = CLAN_AllEnd;
							menuInit(C_menuN);
							break;
						case ISAO_ERR_TCP_CONNECT:
							WebDB.Suspend();							
							C_menuN = CLAN_AllEnd;
							menuInit(C_menuN);
							
							break;
						default:
							if(ANSdata.ret_val != -1) {		//ÀüÇô¾û¶×ÇÑ ´äÀÌ ¿Í¼­ ¿¡·¯°¡ ³µ´Ù.
								WebDB.Suspend();
								bIsLoading = FALSE;
								C_menuN2=0;
								C_menuN=CLAN_AllEnd;
								cldata.myPosition = CLAN_NOCONNECT;
								}
							break;
					}
				}
				break;
		case UPDATE_CLANWON:
			switch(C_menuN2){
				case 0:
					ANSdata.ret_val= -1;
					
					WebDB.Resume() ;		//À¥db¸¦ Àç°³ÇÑ´Ù.
					//Å¬·£ÀÇ ¸â¹öµ¥ÀÌÅ¸¸¦ ÀÐ´Â´Ù.
					WebDB.ReadClan(cldata.szID,cldata.gserver,cldata.ClanZang_Name);
					C_menuN2=1;
					break;
				case 1:
					switch(ANSdata.ret_val) 
					{					
					case 1://Á¤»óÀûÀ¸·Î Å¬·£¸âº¸µ¥ÀÌÅ¸¹Þ¾Æ ÆÄ½Ì
						WebDB.Suspend();
						ParsigClanWon(ANSdata.ret_Buf);
						C_menuN=CLAN_AllEnd;
						CLAN_ERROR_ret=CLAN_AllEnd;		//¿¡·¯¸Þ´ºº¸¿©ÁØÈÄ±×³É³¡³½´Ù.			
						C_menuN2=-1;


						server = cldata.ClanMark[0];
						strcpy(UniqueClanMark,&cldata.ClanMark[3]);
						wsprintf(szFile,"%s\\%s\\%c_%s.%s",SAVE_DIR,szConnServerName,server,UniqueClanMark,SAVE_EXE );
						wsprintf(szFile2,"%s\\%s\\%c_%s_16.%s",SAVE_DIR,szConnServerName,server,UniqueClanMark,SAVE_EXE );

						#ifdef  USE_PROSTONTALE
						cldata.hClanMark = LoadDibSurfaceOffscreen(szFile);
						cldata.hClanMark16 = LoadDibSurfaceOffscreen(szFile2);
						
						#else
						cldata.hClanMark = (HBITMAP)LoadImage(NULL,szFile,
									IMAGE_BITMAP,
									0,0,
									LR_LOADFROMFILE);
						cldata.hClanMark16 = (HBITMAP)LoadImage(NULL,szFile2,
									IMAGE_BITMAP,
									0,0,
									LR_LOADFROMFILE);
						#endif
						if((cldata.hClanMark)&&(cldata.hClanMark16)) break;
						else{
							nClanMark = atoi(cldata.ClanMark);
							g_ViewClanInfo.DownLoadClanInfo(nClanMark);
						}

						
						break;
					case 100:
						WebDB.Suspend();
						//ygy : »ì·Á¾ßÇÔ
						C_menuN = CLAN_ERROR;
						cClanReport.InitMsg(sz100Error);
						cClanReport.menuInit(C_menuN);				
						CLAN_ERROR_ret=CLAN_AllEnd;		//¿¡·¯¸Þ´ºº¸¿©ÁØÈÄ±×³É³¡³½´Ù.			
						C_menuN2=-1;
						break;
					case ISAO_ERR_TCP_CONNECT:
						WebDB.Suspend();
						//ygy : »ì·Á¾ßÇÔ
						C_menuN = CLAN_ERROR;
						cClanReport.InitMsg(szServerConnectErrorMsg);
						cClanReport.menuInit(C_menuN);
						break;
					
					default:
						if(ANSdata.ret_val != -1) {		//ÀüÇô¾û¶×ÇÑ ´äÀÌ ¿Í¼­ ¿¡·¯°¡ ³µ´Ù.
							WebDB.Suspend();
							C_menuN = 254;				//Å¬·£°á¼º¸øÇÏ´Ï µ¹¾Æ°¡¶ó°í ÇÏ´Â¸Þ´º·Î °£´Ù.
							menuInit(C_menuN );

							C_menuN2=-1;
						}					
						break;
				}
			}
			break;

	//======================================================================
	case CLAN_AllEnd:			//Å¬·£ ³¡³ª°í Å¬¸®¾îÇÏ´Â¼ÂÆÃºÎºÐ.
	//======================================================================
		OpenFlag = 0;
		C_menuN = 0;
		C_menuN2 = 0;
		//ktj
		ClanCharMove = 0;
		ClanCLCharMove = 0;
		break;


	//======================================================================
	case CLAN_READ_CLANMARK:
	//======================================================================
		
		switch(C_menuN2) {
		case 0:
			ANSdata.ret_val= -1;
			
			WebDB.Resume();		//À¥db¸¦ Àç°³ÇÑ´Ù.
			//WebDB.MarkREAD(cldata.ClanMark);
			WebDB.MarkREAD(cldata.name,cldata.ClanMark);
			C_menuN2=1;
			break;
		case 1:		
			
			switch(ANSdata.ret_val) 
			{
				//ygy
			case 1://Å¬·£ ¸¶Å© ÀÐ±â ¼º°ø
				WebDB.Suspend();
				//wsprintf(szFile,"image\\clanDATA\\%s.%s",cldata.name,cldata.ClanMark);
				server = cldata.ClanMark[0];
				strcpy(UniqueClanMark,&cldata.ClanMark[3]);
				wsprintf(szFile,"%s\\%s\\%c_%s.%s",SAVE_DIR,szConnServerName,server,UniqueClanMark,SAVE_EXE );
				wsprintf(szFile2,"%s\\%s\\%c_%s_16.%s",SAVE_DIR,szConnServerName,server,UniqueClanMark,SAVE_EXE);
				#ifdef  USE_PROSTONTALE
				if(cldata.hClanMark16) {
					cldata.hClanMark16->Release();
					cldata.hClanMark16 = NULL;
				}

				if(cldata.hClanMark){
					cldata.hClanMark->Release();
					cldata.hClanMark = NULL;
				}

				cldata.hClanMark16= LoadDibSurfaceOffscreen(szFile2);
				cldata.hClanMark= LoadDibSurfaceOffscreen(szFile);
				
				#else
				hMark = (HBITMAP)LoadImage(NULL,szFile,
							IMAGE_BITMAP,
							0,0,
							LR_LOADFROMFILE);
				#endif
				if(cldata.myPosition == CLANUSER)			//Å¬·£¿ø Æ÷Áö¼Ç ÄÚµå´Â 100¹øÀÓ)
				{
					C_menuN = CLAN_USER_ReadDATA;		
					C_menuN2=0;	
					/*if(strcmp(cldata.stats,"0") == 0)
					{
						C_menuN = CLAN_USER_NOMAKE;
						menuInit(C_menuN );									
						C_menuN2=0;
						//C_menuN2=-1;
					}
					else
					{
						C_menuN = CLAN_USER;
						menuInit(C_menuN );									
						C_menuN2=0;
						//C_menuN2=-1;
					}*/				
				}
				if(cldata.myPosition == CLANCHIP)
				{
					C_menuN = CLAN_CHIP_ReadDATA;		
					menuInit(C_menuN);
					C_menuN2=0;	
				}
				
				
				break;
			case 2://Å¬·£ ¸¶Å© ÀÐ±â ½ÇÆÐ
				WebDB.Suspend();
				wsprintf(szErrorMsg, szReadClanMarkErrorMsg);
				C_menuN = CLAN_REAMMAKR_ERROR;
				//cClanReport.InitMsg(szReadClanMarkErrorMsg);
				menuInit(C_menuN);
				//CLAN_ERROR_ret=CLAN_AllEnd;		//¿¡·¯¸Þ´ºº¸¿©ÁØÈÄ±×³É³¡³½´Ù.			
				C_menuN2=-1;	
				break;	
			case ISAO_ERR_TCP_CONNECT:
				WebDB.Suspend();
				wsprintf(szErrorMsg, szServerConnectErrorMsg);
				C_menuN = CLAN_REAMMAKR_ERROR;
				//cClanReport.InitMsg(szServerConnectErrorMsg);				
				menuInit(C_menuN);
				C_menuN2=-1;	
				break;
			
			default:
				if(ANSdata.ret_val != -1) {		//ÀüÇô¾û¶×ÇÑ ´äÀÌ ¿Í¼­ ¿¡·¯°¡ ³µ´Ù.
					WebDB.Suspend();

					C_menuN = 254;				//Å¬·£°á¼º¸øÇÏ´Ï µ¹¾Æ°¡¶ó°í ÇÏ´Â¸Þ´º·Î °£´Ù.
					menuInit(C_menuN );
					C_menuN2=-1;
				}
				break;
			}
		}
			break;

		case CLAN_REAMMAKR_ERROR:
		
			i = chkeckMenuN();
			switch(i) {
			case 0:
				menuMouseOver[0] = 1;
				break;
			default:
				memset(menuMouseOver,-1,sizeof(int)*30);
				break;
			}
			if(MOUSE0())			
			{	
				MOUSE0()=0;
				i = i;
				switch(i) 
				{
				case 0:
					C_menuN = CLAN_AllEnd;					

					C_menuN2 =-1;
					CLAN_ERROR_ret=-1;
					break;	
				default:
					i = i;
					break;

				}
			}
			break;

			
		
	case CLAN_GO_NPCSTART:
		C_menuN2=0;
		C_menuN = CLAN_NPC_START;
		menuInit(C_menuN);
		break;
	//======================================================================
	case CLAN_NPC_START:
	//======================================================================
		switch(C_menuN2) {
		case 0:

			//¸Þ´º ¶ã¶§¸¶´Ù ¼ÂÆÃÇÏ´Â ÇÔ¼ö.
			#ifdef  USE_PROSTONTALE
			cldata.money = sinChar->Money;
			if(cldata.money ==0) cldata.money = 100000;	//ktj : Áö¿ï°Í
			#else
			cldata.money = 4000;
			#endif

			ANSdata.ret_val= -1;

			WebDB.Resume() ;		//À¥db¸¦ Àç°³ÇÑ´Ù.
			//char id[64] = "ktj";
			WebDB.isClanMember(cldata.szID, cldata.gserver, cldata.ChaName);		//ÀÌ id°¡ Å¬·£¿øÀÎÁö¸¦ ÆÄ¾ÇÇÑ´Ù.
			C_menuN2=1;


			break;
		case 1:
			switch(ANSdata.ret_val) 
			{
			case -1:
				
				//C_menuN = CLAN_NPC_START_LOADING;
				//menuInit(C_menuN);
				break;
			case 0:		//¹«µî·ÏÀÚ : Å¬·£À» ¸¸µå´Â ¸Þ´º·Î °£´Ù.
				//fd2(" ´ä¹ÞÀ½. isClanMemberR == 102 ");
				WebDB.Suspend(); 

				//ktj : ÀÓ½Ã
				//C_menuN = CLAN_MAKE_is;
				//C_menuN = CLAN_isCheckClanJang;
				C_menuN = CLAN_GO_MAKE;				
				
				C_menuN2=1;
				cldata.myPosition = CLANNO_REGESTER;
				break;

			case 1:						//ÀÏ¹Ý Å¬·£¿ø
				//ygy
				WebDB.Suspend(); //À¥db ÁßÁö
				//ygy : ²À »ì¸±°Í
				//C_menuN2=0;
				ParsingIsClanMember(ANSdata.ret_Buf);
				
				//C_menuN=CLAN_AllEnd;
				cldata.myPosition = CLANUSER;			//Å¬·£¿ø Æ÷Áö¼Ç ÄÚµå´Â 100¹øÀÓ


				//ktj : ÀÓ½Ã
				//C_menuN2=0;
				//C_menuN=CLAN_USER;
				//menuInit(C_menuN);
				WriteMyClanInfo();				
				wsprintf(szFile,"%s\\%s\\%c_%s.%s",SAVE_DIR,szConnServerName,MyClanInfo.Gserver,MyClanInfo.UniqueClaClanInfo_nNum,SAVE_EXE);
				wsprintf(szFile2,"%s\\%s\\%c_%s_16.%s",SAVE_DIR,szConnServerName,MyClanInfo.Gserver,MyClanInfo.UniqueClaClanInfo_nNum,SAVE_EXE);

				fp = fopen(szFile,"rb");
				fp2 = fopen(szFile2,"rb");
				
				if((fp>0)&&(fp2>0))
				{
					#ifdef  USE_PROSTONTALE
					if(cldata.hClanMark == NULL)					
						cldata.hClanMark = LoadDibSurfaceOffscreen(szFile);
					if(cldata.hClanMark16 == NULL)					
						cldata.hClanMark16 = LoadDibSurfaceOffscreen(szFile2);
					
					#else
					hMark = (HBITMAP)LoadImage(NULL,szFile,
								IMAGE_BITMAP,
								0,0,
								LR_LOADFROMFILE);
					#endif					
					strcpy(cldata.szClanMarkName,szFile);
					C_menuN = CLAN_USER_FLAG;
				
				}
				else
				{
					C_menuN2=0;
					C_menuN = CLAN_READ_CLANMARK;
					menuInit(C_menuN);
				}

				/*
				#ifdef  USE_PROSTONTALE
				//ÀÏ´Ü ³¡³»¾ßÇÔ.
				C_menuN=CLAN_AllEnd;
				#else
				#endif
				*/

				/*tjy
				fd2(" ´ä¹ÞÀ½. ret_val == 100 ");
				WebDB.Suspend(); //À¥db ÁßÁö

				//============================ ktj : ÀÌºÎºÐ ´Ù½Ã¼³Á¤ÇÒ°Í
				C_menuN = CLAN_USER;
				//============================ ktj : ÀÌºÎºÐ ´Ù½Ã¼³Á¤ÇÒ°Í

				menuInit(C_menuN );

				C_menuN2=0;
				cldata.myPosition = CLANUSER;*/
				fclose(fp);
				fclose(fp2);
				break;

			case 2://Å¬·£ÀåÀ¸·Î µî·ÏµÈÀÚ //Å¬·£¯…( µ¥ÀÌÅ¸ÀÐ±â·Î °£´Ù)	
				WebDB.Suspend(); //À¥db ÁßÁö
				//ygy : ²À »ì¸±°Í
				C_menuN2=0;
				ParsingIsClanMember(ANSdata.ret_Buf);
				WriteMyClanInfo();
				
				//C_menuN=CLAN_AllEnd;
				cldata.myPosition = CLANCHIP;				//Å¬·£Àå Æ÷Áö¼Ç ÄÚµå´Â 101¹øÀÓ

				wsprintf(szFile,"%s\\%s\\%c_%s.%s",SAVE_DIR,szConnServerName,MyClanInfo.Gserver,MyClanInfo.UniqueClaClanInfo_nNum,SAVE_EXE);
				wsprintf(szFile2,"%s\\%s\\%c_%s_16.%s",SAVE_DIR,szConnServerName,MyClanInfo.Gserver,MyClanInfo.UniqueClaClanInfo_nNum,SAVE_EXE);

				fp = fopen(szFile,"rb");
				fp2 = fopen(szFile2,"rb");				
				if((fp>0)&&(fp2>0))
				{
					#ifdef  USE_PROSTONTALE
					if(cldata.hClanMark == NULL)					
						cldata.hClanMark = LoadDibSurfaceOffscreen(szFile);
					if(cldata.hClanMark16 == NULL)					
						cldata.hClanMark16 = LoadDibSurfaceOffscreen(szFile2);						
					#else
					cldata.hClanMark = (HBITMAP)LoadImage(NULL,szFile,
							IMAGE_BITMAP,
							0,0,
							LR_LOADFROMFILE);
					cldata.hClanMark16 = (HBITMAP)LoadImage(NULL,szFile2,
							IMAGE_BITMAP,
							0,0,
							LR_LOADFROMFILE);
					#endif

					C_menuN2=0;
					C_menuN=CLAN_CHIP_ReadDATA;
					menuInit(C_menuN);									
					
				}
				else
				{
					C_menuN2=0;
					C_menuN = CLAN_READ_CLANMARK;
					menuInit(C_menuN);					
					
				}

				if(fp > 0) fclose(fp);
				if(fp2 > 0) fclose(fp2);
				

				/*tjy
				WebDB.Suspend();
				C_menuN = CLAN_CHIP_ReadDATA;		
				menuInit(C_menuN );

				C_menuN2=0;
				cldata.myPosition = CLANCHIP;

				chk_readStrBuf("clanName=", ANSdata.ret_Buf, cldata.name);		//À¥ db¿¡¼­ ¿Â ½ºÆ®¸µ¿¡¼­ ¿øÇÏ´Â ½ºÆ®¸µÀ» »Ì¾Æ³¿.
				strncpy(cldata.ClanZang_Name,  cldata.ChaName, strlen(cldata.ChaName));
				*/
				break;
			case 3: //ÇØÃ¼µÈ Å¬·£¿ø
				WebDB.Suspend();				
				ParsingIsClanMember2(ANSdata.ret_Buf);
				C_menuN = CLANUSER_GO_RELEASEMESSAGE;
				C_menuN2 = -1;
				cldata.myPosition = CLANNO_REGESTER;
				break;
			case ISAO_ERR_TCP_CONNECT:
				WebDB.Suspend();
				wsprintf(szErrorMsg, szServerConnectErrorMsg);
				C_menuN = CLAN_REAMMAKR_ERROR;
				menuInit(C_menuN);
				cldata.myPosition = CLAN_NOCONNECT;
				break;

			default:
				if(ANSdata.ret_val != -1) {			//ÀüÇô¾û¶×ÇÑ ´äÀÌ ¿Í¼­ ¿¡·¯°¡ ³µ´Ù.
					WebDB.Suspend();

					wsprintf(szErrorMsg, Replay);
					C_menuN = CLAN_REAMMAKR_ERROR;
					menuInit(C_menuN);
					cldata.myPosition = CLAN_NOCONNECT;				
					C_menuN2=0;
				}
				break;
			}
			break;
		}
		break;

	   
	}

}


/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º¸í : 
: ÇÏ ´Â ÀÏ : ¸¶¿ì½º·Î ´©¸¥ ¸Þ´º ¹øÈ£ Ã½Å©ÇÔ¼ö.
: ¹Ý È¯ °ª :
: ÆÄ¶ó¹ÌÅÍ :
///////////////////////////////////////////////////////////////////////////*/
int CLANEntity::chkMenu(int x, int y, RECT *r) 
{
	if(r->left <= x   &&   x<= r->right ) 
		if(r->top <= y   &&   y<= r->bottom ) 
			return 1;
	return 0;			//¾Æ¹«°Íµµ ¼±ÅÃ¾ÈÇÔ.
}
int CLANEntity::chkeckMenuN()
{
	int i;
	
	for(i=0; i<menuSu; i++) {
		if(  chkMenu(pCursorPos.x, pCursorPos.y, &menuPos[i])  ) {
			return i;
		}
	}
	return -1;
}









/*///////////////////////////////////////////////////////////////////////////
: ÇÔ ¼ö ¸í : RenderMain()
: ³í    Æò :
: PROJECT  :
///////////////////////////////////////////////////////////////////////////*/
void CLANEntity::RenderMain()
{


}






















/*///////////////////////////////////////////////////////////////////////////
: ÇÔ ¼ö ¸í : Load()
: ³í    Æò :
: PROJECT  : ÀÌ¹ÌÁö °ü·Ã
///////////////////////////////////////////////////////////////////////////*/


void CLANEntity::Load2()
{
	#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
	hScl_icon_tj = CreateTextureMaterial( "image\\Sinimage\\Help\\scl_icon.tga", 0, 0, 0,0, SMMAT_BLEND_ALPHA );
	#endif

}

void CLANEntity::Load()
{
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	/*
	MatPartyBackGround =
		CreateTextureMaterial( "image\\Sinimage\\Help\\party_win.tga" , 0, 0, 0,0, SMMAT_BLEND_ALPHA );
	hBoxCorner =
		CreateTextureMaterial( "image\\Sinimage\\Help\\HelpBoxCorner.tga" , 0, 0, 0,0, SMMAT_BLEND_ALPHA );
	hBoxLine = 
		CreateTextureMaterial( "image\\Sinimage\\Help\\HelpBoxLine.tga",  0, 0, 0,0, SMMAT_BLEND_ALPHA );
	

	//ktj : scroll
	hScl_Bar_tj= CreateTextureMaterial( "image\\Sinimage\\Help\\scl_bar.tga", 0, 0, 0,0, SMMAT_BLEND_ALPHA );
	*/

	hScl_icon_tj = CreateTextureMaterial( "image\\Sinimage\\Help\\scl_icon.tga", 0, 0, 0,0, SMMAT_BLEND_ALPHA );

	hMark_box= LoadDibSurfaceOffscreen("image\\clanimage\\chipbutton\\mark-box.bmp");

	hOk_Gray  = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Ok_Glay.bmp");
	hOk_Yellow= LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Ok_Yellow.bmp");
	hCancel_Gray = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Cancel_Glay.bmp");
	hCancel_Yellow = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Cancel_Yellow.bmp");
	hYes_Gray = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Yes_Glay.bmp");
	hYes_Yellow = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_Yes_Yellow.bmp");
	hNo_Gray = LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_No_Glay.bmp");
	hNo_Yellow =  LoadDibSurfaceOffscreen("image\\Sinimage\\MessageBox\\Button_No_Yellow.bmp");



		//CLAN CHIP
	//hMark = LoadDibSurfaceOffscreen("image\\Sinimage\\chipbutton\\Mark.bmp");
	hClanName = LoadDibSurfaceOffscreen("image\\Sinimage\\Help\\box-test-title.bmp");
	hWareHouse[0]= LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whc.bmp");
	hWareHouse[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whc1.bmp");
	hWHRight[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whr.bmp");
	hWHRight[1] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\whr1.bmp");
	hClanMark[0] = LoadDibSurfaceOffscreen("image\\clanImage\\chipbutton\\m.bmp");
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


	hButton_Box = LoadDibSurfaceOffscreen("image\\Sinimage\\Help\\box-test-title2.bmp");
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

	hMark_box= (HBITMAP)LoadImage(NULL,"image\\clanimage\\chipbutton\\mark-box.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	hBt_Help= (HBITMAP)LoadImage(NULL,"image\\clanimage\\chipbutton\\bt_help.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	
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
	hClanName = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\name.bmp",
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

	hClanNameInput = (HBITMAP)LoadImage(NULL,"image\\clanImage\\chipbutton\\clan_name.bmp",
						IMAGE_BITMAP,
						0,0,
						LR_LOADFROMFILE);

	


	//Å¬·£ ÀÌ¸§ ³Ö´Â ¿¡µðÆ®
	m_hClanName = CreateWindowEx(WS_EX_TOPMOST,"Edit",NULL,WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			70,150,130,25,g_hwnd,NULL,g_hInst,NULL);
	
	ShowWindow(m_hClanName,SW_HIDE);
	UpdateWindow(m_hClanName);

#endif
}





void CLANEntity::Close()
{
	#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

	if(	hOk_Gray ) {   hOk_Gray->Release();  hOk_Gray = NULL;}
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

	if(	hButton_Box) {   hButton_Box->Release();  hButton_Box= NULL;}
	if(	hClanTitle ) {   hClanTitle ->Release();  hClanTitle = NULL;}


	//ygy ½ºÅ©·Ñ
	if(	hScl_Bar) {   hScl_Bar->Release();  hScl_Bar = NULL;}
	if(	hScl_icon ) {   hScl_icon->Release();  hScl_icon = NULL;}

	//ygyÅ¬·£ÀÌ¸§ ÀÔ·Â ÀÌ¹ÌÁö
	if(	hClanNameInput ) {   hClanNameInput->Release();  hClanNameInput = NULL;}


	#else

	//bitmap¾ø¾Ù°Í.
	DeleteObject( MatPartyBackGround );
//	DeleteObject(hBoxCorner);
//	DeleteObject(hBoxLine);
//	DeleteObject(hBoxLine1);
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

	if(m_hClanName)
	{
		DestroyWindow(m_hClanName);
	}

	#endif
}



/*///////////////////////////////////////////////////////////////////////////
: ÇÔ¼ö¸í   : 
: ÇÏ ´Â ÀÏ : ±×¸²ÆÄÀÏ¸®¼Ò½ºÀÇ Æ÷ÀÎÅÍ¸¦ Ä«ÇÇÇØÁØ´Ù.
: ¹Ý È¯ °ª :
: ÆÄ¶ó¹ÌÅÍ :
///////////////////////////////////////////////////////////////////////////*/

void CLANEntity::copy_LoadResource2(CLANEntity *copyDest)
{
	#ifdef USE_PROSTONTALE
	copyDest->hScl_Bar_tj = this->hScl_Bar_tj;
	copyDest->hScl_icon_tj = this->hScl_icon_tj;
	copyDest->hClanNameInput = this->hClanNameInput;
	#endif

}


void CLANEntity::copy_LoadResource(CLANEntity *copyDest)
{
	#ifdef USE_PROSTONTALE
	//copyDest->hScl_Bar_tj = this->hScl_Bar_tj;
	copyDest->hScl_icon_tj = this->hScl_icon_tj;
	copyDest->hClanNameInput = this->hClanNameInput;
	#endif
	//copyDest->MatPartyBackGround = this->MatPartyBackGround;
	//copyDest->hBoxCorner = this->hBoxCorner;
	//copyDest->hBoxLine = this->hBoxLine;

	copyDest->hClanNameInput = this->hClanNameInput;	
	copyDest->hOk_Gray = this->hOk_Gray;
	copyDest->hOk_Yellow = this->hOk_Yellow;
	copyDest->hButton_Box = this->hButton_Box;
	copyDest->hCancel_Gray = this->hCancel_Gray;
	copyDest->hCancel_Yellow = this->hCancel_Yellow;
	copyDest->hYes_Gray = this->hYes_Gray;
	copyDest->hYes_Yellow = this->hYes_Yellow;
	copyDest->hNo_Gray = this->hNo_Gray;
	copyDest->hNo_Yellow = this->hNo_Yellow;
	copyDest->hClanName = this->hClanName;
	copyDest->hIcon = this->hIcon;
	copyDest->hIcon_Click = this->hIcon_Click;
	copyDest->hHelp1 = this->hHelp1;
	copyDest->hClanTitle = this->hClanTitle;
	copyDest->hScl_Bar = this->hScl_Bar;
	copyDest->hScl_icon = this->hScl_icon;
	copyDest->hMark = this->hMark;
	copyDest->hMark_box = this->hMark_box;
	copyDest->hBt_Help= this->hBt_Help;

#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì

#else

	copyDest->m_hClanName = this->m_hClanName;
#endif
	
	for(int i = 0; i< 2; i++)
	{
		copyDest->hTxt_Secession[i] = this->hTxt_Secession[i];
		copyDest->hTxt_Help[i] = this->hTxt_Help[i];
		copyDest->hTxt_Mark[i] = this->hTxt_Mark[i];
		copyDest->hTxt_Release[i] = this->hTxt_Release[i];
		copyDest->hTxt_Remove[i] = this->hTxt_Remove[i];
		copyDest->hRemoveToMenu[i] = this->hRemoveToMenu[i];
		copyDest->hClose[i] = this->hClose[i];
		copyDest->hSecession[i] = this->hSecession[i];
		copyDest->hOut[i] = this->hOut[i];
		copyDest->hHelp[i] = this->hHelp[i];
		copyDest->hClanRelease[i] = this->hClanRelease[i];
		copyDest->hRemove[i] = this->hRemove[i];
		copyDest->hClanMark[i] = this->hClanMark[i];
		copyDest->hWHRight[i] = this->hWHRight[i];
		copyDest->hWareHouse[i] = this->hWareHouse[i];
	}
}




/*///////////////////////////////////////////////////////////////////////////
: ÇÔ¼ö¸í   : 
: ÇÏ ´Â ÀÏ : cldata ÆÄ½ÌÇÔ¼öµé
: ¹Ý È¯ °ª :
: ÆÄ¶ó¹ÌÅÍ :
///////////////////////////////////////////////////////////////////////////*/


//À¥ db¿¡¼­ ¿Â ½ºÆ®¸µ¿¡¼­ ¿øÇÏ´Â ½ºÆ®¸µÀ» »Ì¾Æ³¿.
int CLANEntity::chk_readStrBuf(char *cmd, char *readbuf, char *retBuf)
{
	int iRejectCode = -1;			//1Àº Á¦´ë·Î µé¾î¿Â°ÍÀÓ.
	char *pBuf;

	pBuf = strstr(readbuf,cmd);
    if( pBuf == NULL)	//¹ß°ßÇÒ ¼ö ¾ø¾ú´Ù.
	{
		return iRejectCode ;
	}

	pBuf = pBuf + strlen( cmd );
	while(1) {			//½ºÆäÀÌ½º¸¦ ¾ø¾Ø´Ù.
		if(*pBuf == '\n') return 0;
		if(*pBuf != ' ') break;
		pBuf ++;
	}
	sscanf( pBuf , "%s", retBuf );

	return iRejectCode;
}

void CLANEntity::ParsingData(char* cmd, char* data, char* save)
{	
	char *pBuf;
	int nCount = 0;
	char szTemp[500];
	ZeroMemory(szTemp,sizeof(char)*500);
	pBuf = strstr(data,cmd);
	pBuf = pBuf + strlen( cmd );
	strcpy(szTemp,pBuf);
    if( pBuf == NULL)	//¹ß°ßÇÒ ¼ö ¾ø¾ú´Ù.
	{
		return;
	}

	pBuf = pBuf + strlen( cmd );
		while(1) {			//½ºÆäÀÌ½º¸¦ ¾ø¾Ø´Ù.		
		
		if(szTemp[nCount] == '\n') break;		
		save[nCount] = szTemp[nCount];
		nCount++;
		
	}
	
}






/*///////////////////////////////////////////////////////////////////////////
: ÇÔ¼ö¸í   : 
: ÇÏ ´Â ÀÏ : cldata ÆÄ½ÌÇÔ¼öµé
: ¹Ý È¯ °ª :
: ÆÄ¶ó¹ÌÅÍ :
///////////////////////////////////////////////////////////////////////////*/


//À¥ db¿¡¼­ ¿Â ½ºÆ®¸µ¿¡¼­ ¿øÇÏ´Â ½ºÆ®¸µÀ» »Ì¾Æ³¿.
int chk_readStrBuf(char *cmd, char *readbuf, char *retBuf)
{
	int iRejectCode = -1;			//1Àº Á¦´ë·Î µé¾î¿Â°ÍÀÓ.
	char *pBuf;

	pBuf = strstr(readbuf,cmd);
    if( pBuf == NULL)	//¹ß°ßÇÒ ¼ö ¾ø¾ú´Ù.
	{
		return iRejectCode ;
	}

	pBuf = pBuf + strlen( cmd );
	while(1) {			//½ºÆäÀÌ½º¸¦ ¾ø¾Ø´Ù.
		if(*pBuf == '\n') return 0;
		if(*pBuf != ' ') break;
		pBuf ++;
	}
	sscanf( pBuf , "%s", retBuf );

	return iRejectCode;
}

void ParsingData(char* cmd, char* data, char* save)
{	
	char *pBuf;
	int nCount = 0;
	char szTemp[500];
	ZeroMemory(szTemp,sizeof(char)*500);
	pBuf = strstr(data,cmd);
	pBuf = pBuf + strlen( cmd );
	strcpy(szTemp,pBuf);
    if( pBuf == NULL)	//¹ß°ßÇÒ ¼ö ¾ø¾ú´Ù.
	{
		return;
	}

	pBuf = pBuf + strlen( cmd );
		while(1) {			//½ºÆäÀÌ½º¸¦ ¾ø¾Ø´Ù.		
		
		if(szTemp[nCount] == '\n') break;		
		save[nCount] = szTemp[nCount];
		nCount++;
		
	}
	
}

void ParsingIsClanMember(char* data,int myposition)
{
	switch(myposition)
	{
	case 3:		
	case 0:
		cldata.myPosition = CLANNO_REGESTER;
		ZeroMemory(cldata.name, sizeof(cldata.name));

		ZeroMemory( cldata.ClanMark,   sizeof(cldata.ClanMark));
		cldata.intClanMark = 0;
		break;
	case 1:					
	case 2:
		if(myposition==1)   cldata.myPosition = CLANUSER;
		else				cldata.myPosition = CLANCHIP;
		chk_readStrBuf("CName=", data, cldata.name);
		chk_readStrBuf("CNote=", data, cldata.explanation);	
		chk_readStrBuf("CZang=", data, cldata.ClanZang_Name);
		chk_readStrBuf("CStats=", data, cldata.stats);	
		chk_readStrBuf("CSec=", data, cldata.sec);
		ParsingData("CRegiD=", data, cldata.szRegiD);
		ParsingData("CLimitD=", data, cldata.limitD);
		chk_readStrBuf("CRegiD=", data, cldata.szRegiD);
		chk_readStrBuf("CLimitD=", data, cldata.limitD);
		chk_readStrBuf("CDelActive=", data, cldata.DelActive);
		
		chk_readStrBuf("CIMG=", data, cldata.ClanMark);
		
		cldata.intClanMark = atoi(cldata.ClanMark);

		chk_readStrBuf("CPFlag=", data, cldata.PFlag);
		break;
	}
}

void CLANEntity::ParsingIsClanMember(char* data)
{
	
	chk_readStrBuf("CName=", data, cldata.name);
	chk_readStrBuf("CNote=", data, cldata.explanation);	
	chk_readStrBuf("CZang=", data, cldata.ClanZang_Name);
	chk_readStrBuf("CStats=", data, cldata.stats);	
	chk_readStrBuf("CSec=", data, cldata.sec);
	ParsingData("CRegiD=", data, cldata.szRegiD);
	ParsingData("CLimitD=", data, cldata.limitD);
	chk_readStrBuf("CRegiD=", data, cldata.szRegiD);
	chk_readStrBuf("CLimitD=", data, cldata.limitD);
	chk_readStrBuf("CDelActive=", data, cldata.DelActive);
	
	chk_readStrBuf("CIMG=", data, cldata.ClanMark);
	
	cldata.intClanMark = atoi(cldata.ClanMark);

	chk_readStrBuf("CPFlag=", data, cldata.PFlag);

	//WriteMyClanInfo();

}

void CLANEntity::ParsingIsClanMember2(char *data)
{
	chk_readStrBuf("CName=", data, cldata.name);
	chk_readStrBuf("CPFlag=", data, cldata.PFlag);
}

void CLANEntity::WriteMyClanInfo()
{
	char szTemp[100];
	HANDLE hFile;
	DWORD dwWrite;
	strcpy(MyClanInfo.ClanMark_Num,cldata.ClanMark);
	strcpy(MyClanInfo.ClanName,cldata.name);
	strcpy(MyClanInfo.ClanReport, cldata.explanation);
	MyClanInfo.nClanMark = atoi(MyClanInfo.ClanMark_Num);
	MyClanInfo.Gserver = cldata.ClanMark[0];
	strcpy(MyClanInfo.UniqueClaClanInfo_nNum,&cldata.ClanMark[3]);

	wsprintf(szTemp,"%s\\%s\\%c_%s.rep",SAVE_DIR,szConnServerName,MyClanInfo.Gserver,MyClanInfo.UniqueClaClanInfo_nNum);
	hFile = CreateFile( szTemp, GENERIC_WRITE, FILE_SHARE_WRITE,
                        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    if( hFile != INVALID_HANDLE_VALUE ) {        
        WriteFile( hFile , &MyClanInfo, sizeof(MYCLANINFOHEADER), &dwWrite , NULL );        
        CloseHandle( hFile );
    }
}



void ParsigClanWon(char* data)
{
	cBaseMenu.ParsigClanWon(data);
}


void CLANEntity::ParsigClanWon(char* data)
{
	int i = 0,j=0;
	int datastrlen;
	char szTemp[1024];
	char szClanWon[30];
	char *pBuf;
	int nPos,nClanWonlen;
	int k = 0;
	ZeroMemory(szTemp,sizeof(char)*1024);
	memcpy(szTemp,data,strlen(data));
	datastrlen = lstrlen(data);
	pBuf = strstr(szTemp,"CMem");
	nPos = pBuf - szTemp;
	nClanWonlen = datastrlen - nPos;
	k = nPos+5;
	
	
	
	//ktj : ´Ù½Ã »ì¸±°Í
	nClanWon_Num = 0;	
	//ZeroMemory(removeclick,sizeof(BUTTONCLICK)*30);


	ZeroMemory(szClanWon,sizeof(char)*30);
	ZeroMemory(cldata.clanWon,sizeof(cldata.clanWon));  //Å¬·£¿øÀ» ÀúÀåÇÒ º¯¼ö ÃÊ±âÈ­

	
	while(k<datastrlen )
	{
		if(szTemp[k] == 0) break;
		if(szTemp[k] =='\n')
		{
			k+=6;
			i+=1;
			j=0;			
			nClanWon_Num +=1;
		}
		else
		{
			//clanWons[i].clanWon[j] = szTemp[k];
			//clanWons[i].bFlag = 1;
			cldata.clanWon[i][j] = szTemp[k];
			k++;		
			j++;
		}
	}	
	/*
	//¿øº»
	for(i = 0; i<nClanWon_Num; i++)
	{
		if(strcmp(clanWons[i].clanWon,g_ClanZang.szClanZang_ChaName) == 0)
		{
			if(!clanWons[i+1].clanWon[0])
			{
				memset(&clanWons[i],0,sizeof(szClanWon));
				clanWons[i].bFlag = 0;
				nClanWon_Num-=1;
				
			}
			else
			{
				for(j=i+1; j<nClanWon_Num;j++,i++)
				{
				if(i == (nClanWon_Num-1)) break;
				memset(clanWons[i].clanWon,0,sizeof(char)*30);
				strcpy(szClanWon,clanWons[j].clanWon);
				strncpy(clanWons[i].clanWon,szClanWon,strlen(szClanWon));
				}
			}
			
			
			break;
		}
	}
	*/

	int ZangL = strlen( cldata.ChaName );
	for(i = 0; i<nClanWon_Num; i++)
	{
	//	int nameL = strlen(clanWons[i].clanWon);
		int nameL = strlen(cldata.clanWon[i]);
		if(ZangL == nameL && 
			strcmp(cldata.clanWon[i], cldata.ChaName) == 0 )
		{
			for(j=i; j<(nClanWon_Num); j++) {
				memcpy(&cldata.clanWon[j], &cldata.clanWon[j+1], sizeof(char)*30);
			}

			nClanWon_Num--;
			break;
		}
	}
	cldata.nTotalClanWonCount = nClanWon_Num;
	ParsingClanWonStrLen();
}

/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º ¸í: °¢ Å¬·£¿ø ¹®ÀÚ¿­ ±æÀÌ °®¾î¿À±â
: ÇÏ ´Â ÀÏ : 
: ¹Ý È¯ °ª :
///////////////////////////////////////////////////////////////////////////*/
void CLANEntity::ParsingClanWonStrLen()
{
	ZeroMemory(cldata.clanWonStrLen,sizeof(int)*41);
	int i,j;
	for(i=0; i < nClanWon_Num; i++){
		for(j=0; j<30; j++){
			if(cldata.clanWon[i][j] == 0) break;
			if((((cldata.clanWon[i][j]&0xffffff)>=0x65)&&((cldata.clanWon[i][j]&0xffffff)<=0x90))||(((cldata.clanWon[i][j]&0xffffff)>=0x97)&&((cldata.clanWon[i][j]&0xffffff)<=0x122))){
				cldata.clanWonStrLen[i]+=5;
			}
			else if(((cldata.clanWon[i][j]&0xffffff)>=0x255)){
				cldata.clanWonStrLen[i]+=8;
			}
			else cldata.clanWonStrLen[i]+=4;
			

		}

	}
}


//============ÇïÇÁ µµ¿ò¸» °ü·Ã ÇÔ¼ö ================================================start
/*///////////////////////////////////////////////////////////////////////////
: Å¬¶ó½º ¸í: ½ºÅ©¸³Æ® ÀÐ¾î¿À±â
: ÇÏ ´Â ÀÏ : 
: ¹Ý È¯ °ª :
///////////////////////////////////////////////////////////////////////////*/

BOOL CLANEntity::ReadClanHelpScr(char *filename)
{
	HANDLE hFile = NULL;
	DWORD dwRead;
	BOOL  bReturn;
	BOOL bFileEnd = FALSE;
	char szTemp[1];	
	int nCount = 0;
	g_nClanIconNum = 0;
	//g_RestClanHelpIcon = 0;
	
	
	
	ZeroMemory(ClanHelpIcon,sizeof(CLANHELPICON)*CLANHELPICON_NUM);	
	
	hFile = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE) return FALSE;
	while(1)
	{
		while(1)
		{
			memset(szTemp,0x00,sizeof(char));
			bReturn = ReadFile(hFile,szTemp,1,&dwRead,NULL);
			if(dwRead == 0) 
			{
				bFileEnd = TRUE;
			
				break;
			}
			if((bReturn != FALSE) && (dwRead == 0))
			{
				bFileEnd = TRUE;
				break;
			}
			if((szTemp[0] == 0x20))
			{
				break;
			}			
			if((szTemp[0]== 0x0D) || (szTemp[0] == 0x0A))
			{
				continue;
			}
			else
			{
				ClanHelpIcon[g_nClanIconNum].ImagePath[nCount] = szTemp[0];				
				nCount++;
			}
			
		}
		if(bFileEnd) break;
		nCount = 0;		
		memset(szTemp,0x00,sizeof(char));
		while(1)
		{
			bReturn = ReadFile(hFile,szTemp,1,&dwRead,NULL);
			if(dwRead == 0) 
			{				
				bFileEnd = TRUE;
			
				break;
			}
			if((bReturn != FALSE) && (dwRead == 0))
			{
				bFileEnd = TRUE;
				break;
			}
			if(szTemp[0] == 0x20)
			{
				break;
			}
			if((szTemp[0] == 0x0D) || (szTemp[0] == 0x0A))
			{
				break;
			}
			else
			{
				ClanHelpIcon[g_nClanIconNum].ImagePath2[nCount] = szTemp[0];
			}
			nCount++;
		}	
		if(bFileEnd) break;
		nCount = 0;		
		memset(szTemp,0x00,sizeof(char));
		while(1)
		{
			bReturn = ReadFile(hFile,szTemp,1,&dwRead,NULL);
			if(dwRead == 0) 
			{				
				bFileEnd = TRUE;
			
				break;
			}
			if((bReturn != FALSE) && (dwRead == 0))
			{
				bFileEnd = TRUE;
				break;
			}
			if(szTemp[0] == 0x20)
			{
				break;
			}
			if((szTemp[0] == 0x0D) || (szTemp[0] == 0x0A))
			{
				break;
			}
			else
			{
				ClanHelpIcon[g_nClanIconNum].ImagePath3[nCount] = szTemp[0];
			}
			nCount++;
		}	
		/*if(bFileEnd) break;
		nCount = 0;		
		memset(szTemp,0x00,sizeof(char));
		while(1)
		{
			bReturn = ReadFile(hFile,szTemp,1,&dwRead,NULL);
			if(dwRead == 0) 
			{				
				bFileEnd = TRUE;
			
				break;
			}
			if((bReturn != FALSE) && (dwRead == 0))
			{
				bFileEnd = TRUE;
				break;
			}
			if(szTemp[0] == 0x20)
			{
				break;
			}
			if((szTemp[0] == 0x0D) || (szTemp[0] == 0x0A))
			{
				break;
			}
			else
			{
				ClanHelpIcon[g_nClanIconNum].ImagePath3[nCount] = szTemp[0];
			}
			nCount++;
		}		*/	
		
		if(bFileEnd) break;
		nCount = 0;		
		memset(szTemp,0x00,sizeof(char));
		while(1)
		{
			bReturn = ReadFile(hFile,szTemp,1,&dwRead,NULL);
			if(dwRead == 0) 
			{				
				bFileEnd = TRUE;
			
				break;
			}
			if((bReturn != FALSE) && (dwRead == 0))
			{
				bFileEnd = TRUE;
				break;
			}
			if(szTemp[0] == 0x20)
			{
				continue;
			}
			if((szTemp[0] == 0x0D) || (szTemp[0] == 0x0A))
			{
				break;
			}
			else
			{
				ClanHelpIcon[g_nClanIconNum].TextFilePath[nCount] = szTemp[0];
			}
			nCount++;
		}	
		if(bFileEnd) break;
		g_nClanIconNum++;		
		nCount = 0;
	}
	//g_nClanIconNum-=1;
	//g_RestClanHelpIcon = g_nCla	nIconNum - CLANHELPICON_DISPLAY_MAX;
	
	CloseHandle(hFile);
	return TRUE;
}

void CLANEntity::LoadHelpImage()
{
	int i;	
	for(i=0; i < g_nClanIconNum; i++)
	{		
		#ifdef  USE_PROSTONTALE
		hClanHelpIcon[i][0] = LoadDibSurfaceOffscreen(ClanHelpIcon[i].ImagePath);
		hClanHelpIcon[i][1] = LoadDibSurfaceOffscreen(ClanHelpIcon[i].ImagePath2);								
		hClanHelpIcon[i][2] = LoadDibSurfaceOffscreen(ClanHelpIcon[i].ImagePath3);
		//hClanHelpIcon[i][3] = LoadDibSurfaceOffscreen(ClanHelpIcon[i].ImagePath4);
		#else
		hClanHelpIcon[i][0]
		= (HBITMAP)LoadImage(NULL,ClanHelpIcon[i].ImagePath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		hClanHelpIcon[i][1]
		= (HBITMAP)LoadImage(NULL,ClanHelpIcon[i].ImagePath2,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
		hClanHelpIcon[i][2]
		= (HBITMAP)LoadImage(NULL,ClanHelpIcon[i].ImagePath3,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
		//hClanHelpIcon[i][3]
		//= (HBITMAP)LoadImage(NULL,ClanHelpIcon[i].ImagePath4,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
		#endif
	}
}

void CLANEntity::DestroyClanHelpIcon()
{
	int i;	
	
	for(i=0; i < g_nClanIconNum; i++)
	{		
		
		#ifdef  USE_PROSTONTALE
		if(	hClanHelpIcon[i][0] ) {   hClanHelpIcon[i][0]->Release();  hClanHelpIcon[i][0] = NULL;}
		if(	hClanHelpIcon[i][1] ) {   hClanHelpIcon[i][1]->Release();  hClanHelpIcon[i][1] = NULL;}
		if(	hClanHelpIcon[i][2] ) {   hClanHelpIcon[i][2]->Release();  hClanHelpIcon[i][2] = NULL;}
		//if(	hClanHelpIcon[i][3] ) {   hClanHelpIcon[i][3]->Release();  hClanHelpIcon[i][3] = NULL;}
		
		#else
		if(hClanHelpIcon[i][0]) DeleteObject(hClanHelpIcon[i][0]);		
		if(hClanHelpIcon[i][1]) DeleteObject(hClanHelpIcon[i][1]);		
		if(hClanHelpIcon[i][2]) DeleteObject(hClanHelpIcon[i][2]);		
		//if(hClanHelpIcon[i][3]) DeleteObject(hClanHelpIcon[i][3]);		
		
		#endif
		
		
	}	
}


void CLANEntity::LoadHelp(char *help)
{
	g_HelpDisPlay = 0;

	HANDLE hFile = NULL;
	DWORD dwRead;
	BOOL  bReturn;
	BOOL bFileEnd = FALSE;
	BOOL bFlag = FALSE;
	int nCount = 0;
	char szTemp[1];	
	ZeroMemory(HelpContents,sizeof(HELPCONTENTS)*100);
	hFile = CreateFile(help,GENERIC_READ,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE) return;	
	while(1)
	{
		memset(szTemp,0x00,sizeof(char));
		bReturn = ReadFile(hFile,szTemp,1,&dwRead,NULL);		
		if(dwRead == 0) 
		{
			bFileEnd = TRUE;
		
			break;
		}
		if((bReturn != FALSE) && (dwRead == 0))
		{
			bFileEnd = TRUE;
			break;
		}		
		if(szTemp[0]== 0x0D) continue;
		if((szTemp[0] != 0x0A)&&(szTemp[0] != 0x0D)) bFlag = FALSE;
		if((szTemp[0] == 0x0A))
		{
			if(!bFlag)
			{
				g_HelpDisPlay++;
				nCount =0;
			}
			bFlag = TRUE;
			
		}
		else
		{
			HelpContents[g_HelpDisPlay].szHelpContent[nCount] = szTemp[0];
			//ClanHelpIcon[g_nClanIconNum].szName[nCount] = szTemp[0];	
			
			nCount++;

			if(nCount>=60)
				nCount=nCount;
		}		
	}
	/*int nLen;
	int nStrPos =0;
	int nCount = 0;
	nLen = lstrlen(help);
	g_HelpDisPlay =0;

	//for(g_HelpDisPlay = 0; g_HelpDisPlay< 100; g_HelpDisPlay++)
	while(1)
	{
		if(help[nStrPos] == 0) break;
		if(help[nStrPos] == 0x0D) 
		{
			nStrPos++;
			continue;
			
		}
		if(help[nStrPos] == 0x0A) 
		{
			if((help[nStrPos + 1] != 0x0D)&&(help[nStrPos + 2] != 0x0A))
			{
				if(help[nStrPos + 3] !=0)
					g_HelpDisPlay++;
			}
			nStrPos++;
			nCount=0;
			

		}
		else
		{
			HelpContents[g_HelpDisPlay].szHelpContent[nCount] = help[nStrPos] ;
			nCount++;
			nStrPos++;
		}
		
		//if(help[nStrPos] == 0) break;
		//memcpy(HelpContents[g_HelpDisPlay].szHelpContent,&help[nStrPos],HELPWIDTH);
		//nStrPos+=HELPWIDTH;
	}	*/
	CloseHandle(hFile);
	
	//g_HelpDisRest= g_HelpDisPlay  - HLEPDISPLAY_MAX;
}


//============ÇïÇÁ µµ¿ò¸» °ü·Ã ÇÔ¼ö ===============================================end

/*///////////////////////////////////////////////////////////////////////////
: ÇÔ ¼ö ¸í : ReMoveClanWon()
: ³í    Æò : Å¬·£¿ø Ãß¹æ½Ã »ç¿ë
: PROJECT  :
///////////////////////////////////////////////////////////////////////////*/
void CLANEntity::ReMoveClanWon(char *src)
{	
	int i = 0,j=0;
	int datastrlen;
	char szTemp[1024];
	char szClanWon[30];
	char *pBuf;
	int nPos,nClanWonlen;
	int k = 0;
	ZeroMemory(szTemp,sizeof(char)*1024);
	memcpy(szTemp,src,strlen(src));
	datastrlen = lstrlen(src);
	pBuf = strstr(szTemp,"=");
	nPos = pBuf - szTemp;
	nClanWonlen = datastrlen - nPos;
	k = nPos+1;
	//nClanWon_Num = 0;
	//ZeroMemory(removeclick,sizeof(BUTTONCLICK)*30);
	ZeroMemory(szClanWon,sizeof(char)*30);
	ZeroMemory(szRemoveClanWonResult,sizeof(szRemoveClanWonResult));  //Å¬·£¿øÀ» ÀúÀåÇÒ º¯¼ö ÃÊ±âÈ­
	
	while(k<datastrlen )
	{
		if(szTemp[k] == 0) break;
		if(szTemp[k] =='&')
		{
			k+=8;
			i+=1;
			j=0;			
			//nClanWon_Num +=1;
		}
		else
		{
			//clanWons[i].clanWon[j] = szTemp[k];
			//clanWons[i].bFlag = 1;
			//cldata.clanWon[i][j] = szTemp[k];
			szRemoveClanWonResult[i][j] = szTemp[k];
			k++;		
			j++;
		}
	}	
}

void CLANEntity::Draw()
{
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
	switch(C_menuN){
		case CLAN_READ_CLANMARK:
		case CLAN_NPC_START:			
		Draw_C_Text(szLoading, menuPos[0].left, menuPos[0].top);
			break;
	
	case CLAN_REAMMAKR_ERROR:
		DrawSprite(menuPos[2].left,menuPos[2].top, hButton_Box,  0, 0, 48, 23, 1);		
		if(menuMouseOver[0]>0) DrawSprite(menuPos[0].left,menuPos[0].top, hOk_Yellow,  0, 0, 32, 16, 1);
		else	DrawSprite(menuPos[0].left,menuPos[0].top, hOk_Gray,  0, 0, 32, 16, 1);
		
		Draw_C_Text(szErrorMsg, menuPos[1].left, menuPos[1].top);		
		break;
	case CLANSERVER_INSPECT:
		DrawSprite(menuPos[2].left,menuPos[2].top, hButton_Box,  0, 0, 48, 23, 1);		
		if(menuMouseOver[0]>0) DrawSprite(menuPos[0].left,menuPos[0].top, hOk_Yellow,  0, 0, 32, 16, 1);
		else	DrawSprite(menuPos[0].left,menuPos[0].top, hOk_Gray,  0, 0, 32, 16, 1);
		
		Draw_C_Text(ClanServerInspect, menuPos[1].left, menuPos[1].top);		
		break;
	
	}
#else
	HDC hdc;
	HDC sorDC;
	BITMAP bit;
	int bx,by;	

	switch(C_menuN){
		HDC destdc;		
		case CLAN_READ_CLANMARK:
		case CLAN_NPC_START:
			hdc = GetDC(g_hwnd);
			sorDC  = CreateCompatibleDC(hdc);

			destdc  = CreateCompatibleDC(hdc);
			SelectObject(destdc, Dback.hb );		//off½ºÅ©¸°¼³Á¤.				
			
			
			//TextOut(hdc,menuPos[1].left,menuPos[1].left,szMsg6,lstrlen(szMsg6));
			//Draw_Text(menuPos[1].left,menuPos[1].top,szMsg6);
			Text_Out(destdc,menuPos[0].left,menuPos[0].top,szLoading);

			DeleteDC(sorDC);
			DeleteDC(destdc);
			ReleaseDC(g_hwnd, hdc );
			break;
		case CLAN_REAMMAKR_ERROR:
		hdc = GetDC(g_hwnd);
		sorDC  = CreateCompatibleDC(hdc);		
		destdc=CreateCompatibleDC(hdc);

		SelectObject(destdc, Dback.hb );		//off½ºÅ©¸°¼³Á¤.
		
		GetObject(hButton_Box,sizeof(BITMAP),&bit);
		bx = bit.bmWidth;
		by = bit.bmHeight;	
		BitBlt(destdc,menuPos[2].left,menuPos[2].top,menuPos[2].right,menuPos[2].bottom,sorDC,0,0,SRCCOPY);

		if(menuMouseOver[0] > 0)
		{
			SelectObject(sorDC, hOk_Yellow);
			GetObject(hOk_Yellow,sizeof(BITMAP),&bit);
			bx = bit.bmWidth;
			by = bit.bmHeight;	
			BitBlt(destdc,menuPos[0].left,menuPos[0].top,menuPos[0].right,menuPos[0].bottom,sorDC,0,0,SRCCOPY);
		}
		else
		{
			SelectObject(sorDC, hOk_Gray);
			GetObject(hOk_Gray,sizeof(BITMAP),&bit);
			bx = bit.bmWidth;
			by = bit.bmHeight;	
			BitBlt(destdc,menuPos[0].left,menuPos[0].top,menuPos[0].right,menuPos[0].bottom,sorDC,0,0,SRCCOPY);
		}		
		
		//Draw_Text(menuPos[1].left,menuPos[1].top,szReport);
		Text_Out(destdc,menuPos[1].left,menuPos[1].top,szErrorMsg);

		SelectObject(sorDC, hButton_Box);
		
		DeleteDC(sorDC);
		DeleteDC(destdc);
		ReleaseDC(g_hwnd, hdc );
		break;
		case CLANSERVER_INSPECT:
			hdc = GetDC(g_hwnd);
			sorDC  = CreateCompatibleDC(hdc);		
			destdc=CreateCompatibleDC(hdc);

			SelectObject(destdc, Dback.hb );		//off½ºÅ©¸°¼³Á¤.
			
			GetObject(hButton_Box,sizeof(BITMAP),&bit);
			bx = bit.bmWidth;
			by = bit.bmHeight;	
			BitBlt(destdc,menuPos[2].left,menuPos[2].top,menuPos[2].right,menuPos[2].bottom,sorDC,0,0,SRCCOPY);

			if(menuMouseOver[0] > 0)
			{
				SelectObject(sorDC, hOk_Yellow);
				GetObject(hOk_Yellow,sizeof(BITMAP),&bit);
				bx = bit.bmWidth;
				by = bit.bmHeight;	
				BitBlt(destdc,menuPos[0].left,menuPos[0].top,menuPos[0].right,menuPos[0].bottom,sorDC,0,0,SRCCOPY);
			}
			else
			{
				SelectObject(sorDC, hOk_Gray);
				GetObject(hOk_Gray,sizeof(BITMAP),&bit);
				bx = bit.bmWidth;
				by = bit.bmHeight;	
				BitBlt(destdc,menuPos[0].left,menuPos[0].top,menuPos[0].right,menuPos[0].bottom,sorDC,0,0,SRCCOPY);
			}		
			
			//Draw_Text(menuPos[1].left,menuPos[1].top,szReport);
			Text_Out(destdc,menuPos[1].left,menuPos[1].top,ClanServerInspect);

			SelectObject(sorDC, hButton_Box);
			
			DeleteDC(sorDC);
			DeleteDC(destdc);
			ReleaseDC(g_hwnd, hdc );
			break;
	}
#endif
}

/*///////////////////////////////////////////////////////////////////////////
: ÇÔ ¼ö ¸í : menuInit()
: ³í    Æò :
: PROJECT  : ÀÌ¹ÌÁö °ü·Ã
///////////////////////////////////////////////////////////////////////////*/
void CLANEntity::menuInit(int num)
{
	int bx =0, by = 0;
#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
#else
	BITMAP	bit;
#endif
	switch(num){	
		case CLANSERVER_INSPECT:
			menuSu = 3;
			#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
			bx = 64;
			by = 16;
			#else
			GetObject(hOk_Gray,sizeof(BITMAP),&bit);
			bx = bit.bmWidth;
			by = bit.bmHeight;
			#endif
			menuPos[0].left = BackStartPos.x + BUTTON3_X;		//È®ÀÎ
			menuPos[0].top = BackStartPos.y + BUTTON3_Y;
			menuPos[0].right = menuPos[0].left + bx;
			menuPos[0].bottom = menuPos[0].top + by;

			//ÅØ½ºÆ® Ãâ·Â À§Ä¡
			menuPos[1].left = BackStartPos.x + 100;
			menuPos[1].top = BackStartPos.y + 90;

#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
			bx = 64;
			by = 16;
#else
			GetObject(hButton_Box,sizeof(BITMAP),&bit);
			bx = bit.bmWidth;
			by = bit.bmHeight;
#endif
			menuPos[2].left = BackStartPos.x + BUTTON3_BOX_X;		//È®ÀÎ
			menuPos[2].top = BackStartPos.y + BUTTON3_BOX_Y;
			menuPos[2].right = menuPos[2].left + bx;
			menuPos[2].bottom = menuPos[2].top + by;

			break;
		case CLAN_READ_CLANMARK:
			//ÅØ½ºÆ® Ãâ·Â À§Ä¡
			menuPos[0].left = BackStartPos.x + LOADTEXT_X;
			menuPos[0].top = BackStartPos.y + LOADTEXT_Y;
			menuPos[0].right = 0;
			menuPos[0].bottom = 0;
			break;
		case CLAN_NPC_START:
			//ÅØ½ºÆ® Ãâ·Â À§Ä¡
			menuPos[0].left = BackStartPos.x + LOADTEXT_X;
			menuPos[0].top = BackStartPos.y + LOADTEXT_Y;
			menuPos[0].right = 0;
			menuPos[0].bottom = 0;
			break;
	case CLAN_REAMMAKR_ERROR: //Å¬·£ ±¸¼º¿ø
		menuSu = 3;
		#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
		bx = 64;
		by = 16;
		#else
		GetObject(hOk_Gray,sizeof(BITMAP),&bit);
		bx = bit.bmWidth;
		by = bit.bmHeight;
		#endif
		menuPos[0].left = BackStartPos.x + BUTTON1_X;		//È®ÀÎ
		menuPos[0].top = BackStartPos.y + BUTTON1_Y;
		menuPos[0].right = menuPos[0].left + bx;
		menuPos[0].bottom = menuPos[0].top + by;

		//ÅØ½ºÆ® Ãâ·Â À§Ä¡
		menuPos[1].left = BackStartPos.x + 25;
		menuPos[1].top = BackStartPos.y + 70;

				#ifdef USE_PROSTONTALE		//=========== Æ÷¸®½ºÅÏ¼Ò½ºÀÎ°æ¿ì
		bx = 64;
		by = 16;
		#else
		GetObject(hButton_Box,sizeof(BITMAP),&bit);
		bx = bit.bmWidth;
		by = bit.bmHeight;
		#endif
		menuPos[2].left = BackStartPos.x + BUTTON1_BOX_X;		//È®ÀÎ
		menuPos[2].top = BackStartPos.y + BUTTON1_BOX_Y;
		menuPos[2].right = menuPos[2].left + bx;
		menuPos[2].bottom = menuPos[2].top + by;

		break;
	}
}

/*
void CLANEntity::main()
{
	int i;
	if(life ==0) return;

	switch(C_menuN)
	{
	
		//ycy
	//=========Å¬·£ ±¸¼º¿ø
	case CLAN_REAMMAKR_ERROR:
		
		i = chkeckMenuN();
		switch(i) {
		case 0:
			menuMouseOver[0] = 1;
			break;
		default:
			memset(menuMouseOver,-1,sizeof(int)*30);
			break;
		}
		if(MOUSE0())			
		{	
			MOUSE0()=0;
			i = i;
			switch(i) 
			{
			case 0:
				C_menuN = CLAN_AllEnd;
				//C_menuN = CLAN_ERROR_ret;		//³ª°¡±â : ³¡³»´Â°É·Î°£´Ù.
				//menuInit(C_menuN );

				C_menuN2 =-1;
				CLAN_ERROR_ret=-1;
				break;	
			default:
				i = i;
				break;

			}
		}
		break;
		
	
	}
}

*/






//sw==0 : ÀÏ¹Ý sw==1: Å°ÀÔ·ÂÀ» ¹Þ´Â°æ¿ì sw==2: Å¬·£ÀÌ¸§À» Å¸ÀÌÆ²¿¡ ¾µ°æ¿ì»ç¿ë(±×¸²ÀÚÃ³¸®)
void CLANEntity::Draw_C_Text(char *str, int x, int y, int sw)
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



void CLANEntity::LoadToFile()
{
	char szTemp[64];
	wsprintf(szTemp,"%c_%s.rep",MyClanInfo.Gserver,MyClanInfo.UniqueClaClanInfo_nNum);
	FILE *fp;
	fp = fopen(szTemp,"rb");
	fread(&MyClanInfo,sizeof(MYCLANINFOHEADER),1,fp);
	fclose(fp);
}