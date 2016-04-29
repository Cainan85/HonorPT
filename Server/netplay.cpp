#include "smlib3d\\smd3d.h"

#include "smwsock.h"
#include "character.h"
#include "playmain.h"
#include "fileread.h"

#include "particle.h"
#include "sinbaram\\sinlinkheader.h"
#include "record.h"
#include "netplay.h"
#include "playsub.h"
#include "field.h"
#include "cracker.h"
#include "language.h"
#include "srcLang\\jts.h"
#include "TextMessage.h"

#include "tjboy\\clanmenu\\tjclanDEF.h"
#include "tjboy\\clanmenu\\tjclan.h"
#include "tjboy\\clanmenu\\help.h"
#include "damage.h"
#include "skillsub.h"

#include "AreaServer.h"

#include "BellatraFontEffect.h"
#include "srcserver\\onserver.h"

//######################################################################################
//ÀÛ ¼º ÀÚ : ¿À ¿µ ¼®
#include "WinInt\\WinIntThread.h"
#include "WinInt\\WavIntHttp.h"
#include "WinInt\\ZipLib.h"
//######################################################################################

#include "CurseFilter.h"		//IsCurse ÇÔ¼ö(ÇØ¿Ü)

#ifdef _XTRAP_GUARD_4_
#include ".\\XTrapSrcD5\\Client\\XTrap4Client.h"	//XTrapD5
//#include ".\\XTrapSrc4\\XTrap.h"
#endif

//¹ÚÀç¿ø - XignCode
#ifdef _XIGNCODE_CLIENT
#include "Xigncode\Client\zwave_sdk_client.h"
#endif

extern int WinSizeX;
extern int WinSizeY;
extern HWND hwnd;

//½ºÅ³Ãë¼Ò ÇÔ¼ö´Ù!!
//Áö¼Ó ½ºÅ³ Ã¼Å©
//int cSKILL::CancelContinueSkill(DWORD CODE)


//»õ¹öÀü Àû¿ë½Ã (¼­¹öº° Ä³¸¯ÅÍ Á¤º¸ ³ª´²°¡Áü )
#define SERVER_CHAR_INFO

//ÀÚµ¿ ÀúÀå ½Ã°£ ( 5ºÐ °£°Ý )
#define RECORD_TIME_STEP		(5*60*1000)
//ÃÖÃÊ ·Îµù ±â´Ù¸®´Â ½Ã°£ ( 10ÃÊ )
#define NET_LOADING_WAIT_LIMIT		10000

//Å©¸®Æ¼ÄÃ ¼½¼Ç ¼±¾ð
extern CRITICAL_SECTION	cSection;
extern rsRECORD_DBASE	rsRecorder;					//¼­¹ö¿¡ °ÔÀÓµ¥ÀÌÅ¸ ±â·ÏÀåÄ¡
DWORD	dwLastRecordTime = 0;

CRITICAL_SECTION	cServSection;

int Debug_SendCount = 0;
int Debug_RecvCount1 = 0;
int Debug_RecvCount2 = 0;
int Debug_RecvCount3 = 0;

int NetWorkInitFlag = 0;
int	BellatraEffectInitFlag = 0;			//º§¶óÆ®¶ó ÀÌÆåÆ® ÃÊ±âÈ­ ÇÃ·¢
SBL_SetFontEffect SoD_SetFontEffect;	//SoD ¿ë ÆùÆ® ÀÌÆåÆ®


DWORD	dwTime_ServerT = 0;		//Á¢¼Ó½Ã ½Ã°£ (¼­¹öÃø Time_T )
DWORD	dwTime_ConnectMS = 0;	//Á¢¼Ó½Ã ½Ã°£ (Å¬¶óÀÌ¾ðÆ®ms)


//#define	CLIENT_VERSION_NUM		2235

//Áß±¹
#ifdef	_LANGUAGE_CHINESE
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		1031
#else
#define	CLIENT_VERSION_NUM		15031
#endif
#endif

//ÀÏº»
#ifdef	_LANGUAGE_JAPANESE

#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		4100
#else
#define	CLIENT_VERSION_NUM		15100
#endif
#endif

//´ë¸¸
#ifdef _LANGUAGE_TAIWAN
#define	CLIENT_VERSION_NUM		3003
#endif

#ifdef _LANGUAGE_ENGLISH
//#ifdef _LANGUAGE_PHILIPIN
//	#ifndef	_WINMODE_DEBUG
//	#define	CLIENT_VERSION_NUM		4060		//ÇÊ¸®ÇÉ
//	#else
//	#define	CLIENT_VERSION_NUM		15060		//ÇÊ¸®ÇÉGM
//	#endif
//#else
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		3122		//¿µ¹®
#else
#define	CLIENT_VERSION_NUM		15122		//¿µ¹®GM
#endif
//#endif

#endif

//º£Æ®³²
#ifdef _LANGUAGE_VEITNAM
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		10565
#else
#define	CLIENT_VERSION_NUM		15565
#endif
#endif

//ÅÂ±¹
#ifdef _LANGUAGE_THAI
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		3223
#else
#define	CLIENT_VERSION_NUM		15223
#endif
#endif

//ºê¶óÁú
#ifdef _LANGUAGE_BRAZIL
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		4135
#else
#define	CLIENT_VERSION_NUM		15135
#endif
#endif

//¾Æ¸£ÇîÆ¼³ª
#ifdef _LANGUAGE_ARGENTINA
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		4005
#else
#define	CLIENT_VERSION_NUM		15005
#endif
#endif

#ifdef _LANGUAGE_KOREAN
#define	CLIENT_VERSION_NUM		3000
#endif

#define CLIENT_VERSION_CHECK	(-CLIENT_VERSION_NUM*2)
/////////////////////////// ¹öÀü Á¤º¸ ///////////////////////////////////////////
int	Client_Version = CLIENT_VERSION_NUM;		//ÇöÀç ¹öÀü Å¬¶óÀÌ¾ðÆ® È®ÀÎ¿ë
int	Server_LimitVersion = 167;					//ÇöÀç ¹öÀü ¼­¹ö È®ÀÎ¿ë
/////////////////////////////////////////////////////////////////////////////////

int	Version_WareHouse = 1;						//Ã¢°í ÀúÀåµ¥ÀÌÅ¸ ¹öÀü
int	Version_CharInfo = 1;						//Ä³¸¯µ¥ÀÌÅ¸ ÀúÀåµ¥ÀÌÅ¸ ¹öÀü


smWINSOCK	*smWsockServer = 0;					//NPC-¸ó½ºÅÍ-¾ÆÀÌÅÛ
smWINSOCK	*smWsockDataServer = 0;				//µ¥ÀÌÅ¸ ÀúÀå ¼­¹ö
smWINSOCK	*smWsockUserServer = 0;				//À¯Àú ÇÃ·¹ÀÌ µ¥ÀÌÅ¸ ±³½Å ¼­¹ö
smWINSOCK	*smWsockExtendServer = 0;			//ÇÊµå È®Àå ¼­¹ö

//¼­¹ö Àç¿¬°á È½¼ö
int	ReconnDataServer = 0;
int	ReconnServer = 0;

char	szConnServerName[16];					//Á¢¼ÓÇÑ ¼­¹öÀÌ¸§

DWORD	dwRecvUserServerTime = 0;
DWORD	dwRecvDataServerTime = 0;
DWORD	dwRecvServerTime = 0;
DWORD	dwRecvExtendServerTime = 0;

//ÁÖ°í ¹Þ±â µ¿±âÈ­ º¯¼ö
int	rsRecvDataServer = TRUE;
int	rsRecvServer = TRUE;
int	rsRecvUserServer = TRUE;
int	rsRecvExtendServer = TRUE;

#define CHATBUFF_MAX		64
#define CHATBUFF_MASK		63

DWORD dwMyIP;

CHATBUFF ChatBuff[CHATBUFF_MAX];
CHATBUFF ChatBuff2[6][CHATBUFF_MAX];


int	ChatBuffCnt = 0;
int	ChatBuffCnt2[6] = { 0,0,0,0,0,0 };
int ChatDispCnt = 0;
int	ChatBuffCompCnt = 0;

char *szDefaultServIP = "211.50.44.170";

char *szNetLogFile = "debugnet.log";
FILE *fpNetLog = 0;

//¼­¹ö¿ÍÀÇ ¿¬°á ²÷¾îÁü
int	DisconnectFlag = 0;
int	DisconnectServerCode = 0;

//ÇØÅ· °æ°í
int	WarningHack = 0;

DWORD	dwQuestDelayTime = 0;		//Äù½ºÆ®Ã¢ µô·¹ÀÌ Å¸ÀÓ

DWORD	dwLoadingTime = 0;			//·Îµù ½Ãµµ ½Ã°£

char szServIP[32];
DWORD dwServPort;
char szDataServIP[32];
DWORD dwDataServPort;
char szUserServIP[32];
DWORD dwUserServPort;
char szExtendServIP[32];
DWORD dwExtendServPort;



DWORD	dwConnectedClientTime = 0;			//¼­¹ö¿Í ¿¬°á½Ã Å¬¶óÀÌ¾ðÆ® ½Ã°£
DWORD	dwConnectedServerTime = 0;			//¼­¹ö¿Í ¿¬°á½Ã ¼­¹ö ½Ã°£
DWORD	dwLastRecvGameServerTime = 0;		//ÃÖ±Ù ¼­¹ö·Î ºÎÅÍ ÆÐÅ¶À» ÀÔ¼öÇÑ ½Ã°£
DWORD	dwLastRecvGameServerTime2 = 0;		//ÃÖ±Ù ¼­¹ö·Î ºÎÅÍ ÆÐÅ¶À» ÀÔ¼öÇÑ ½Ã°£
DWORD	dwLastRecvGameServerTime3 = 0;		//ÃÖ±Ù ¼­¹ö·Î ºÎÅÍ ÆÐÅ¶À» ÀÔ¼öÇÑ ½Ã°£
DWORD	dwLastRecvGameServerTime4 = 0;		//ÃÖ±Ù ¼­¹ö·Î ºÎÅÍ ÆÐÅ¶À» ÀÔ¼öÇÑ ½Ã°£

typedef DWORD(*LPFN_CheckMem)(TRANS_FUNC_MEMORY *TransFuncMem, smTRANS_COMMAND	*lpTransCommand);
LPFN_CheckMem	fnChkMem2;

//°æÇèÄ¡¿Í µ· °Å·¡ ±Ý¾×À» ÅäÅ»¿¡ ±â·ÏÇÑ´Ù
static int	CompWareHouseMoney = 0;
int	WareHouseSubMoney = 0;					//Ã¢°í¿¡¼­ ³ª¿Â ´©Àû ±Ý¾×
int TotalSubMoney = 0;						//µ·ÀÌ ³ª°£ ±Ý¾×
int TotalAddMoney = 0;						//µ·ÀÌ µé¾î¿Â ±Ý¾×
int	TotalAddExp = 0;						//µé¾î¿Â °æÇèÄ¡
int	TotalSubExp = 0;						//ÁÙ¾îµç °æÇèÄ¡


smTRANS_COMMAND	*lpTransVirtualPotion = 0;


LPFN_CheckMem	fnChkMem;
DWORD	dwMemFunChkCode = 0;
//¸Þ¸ð¸® °Ë»ç ¸ðµâ ¹ÞÀ½
int	RecvMemFuncData(TRANS_FUNC_MEMORY *TransFuncMem);
int	RecvMemFuncData2(TRANS_FUNC_MEMORY *TransFuncMem);
//¼­¹ö¿¡¼­ ¸Þ¸ð¸® °ªÀ» Á¤ÇØ¼­ °¡Á®°£´Ù
DWORD funcCheckMemSum(DWORD FuncPoint, int count);
//¸ðµâ°Ë»ç ÇÏ¿© ¼­¹ö·Î º¸³»±â
int CheckProcessModule();


//char *szServIP =	"211.50.44.170";
smPLAYDATA	PlayData;
char	TransBuff[smSOCKBUFF_SIZE];
int		TransLen;
//smCHAR_INFO	CharInfo;

//¹ö¸± ¾ÆÀÌÅÛ ÀúÀå
TRANS_ITEMINFO	TransThrowItem;
//ÃÖ±Ù ¹ÞÀº ¾ÆÀÌÅÛ
TRANS_ITEMINFO	TransRecvItem;

//¾ÆÀÌÅÛ ±³È¯Å° Àç½Ãµµ¿ë
TRANS_TRADE_ITEMKEY	LastTransTradeItemKey;
DWORD				dwLastTransTradeKeyTime = 0;

int					InitClanMode = 0;			//Å¬·£ ÃÊ±âÈ­ ¸ðµå

DWORD				dwYahooTime = 0;			//¾ßÈ£ À¯Áö½Ã°£

int					ServerHideMode = 0;			//¼­¹ö Åõ¸í°ü¸®ÀÚ ¸ðµå


//////////////// PK ÇÊµå Á¤º¸ //////////////////
PK_FIELD_STATE	PK_FieldState;		//Pk ÇÊµå ±¸Á¶ Á¤º¸Ã¼


//Æ®·¹ÀÌµå ¾ÆÀÌÅÛ È®ÀÎ ±¸Á¶Ã¼
struct TRANS_TRADE_CHECKITEM
{
	int size, code;
	DWORD	dwSender;
	DWORD	dwRecver;
	DWORD	dwSum;

	DWORD	dwSendItemCode[MAX_TRADE_ITEM];
	DWORD	dwSendItemSum[MAX_TRADE_ITEM];

	DWORD	dwRecvItemCode[MAX_TRADE_ITEM];
	DWORD	dwRecvItemSum[MAX_TRADE_ITEM];
};


////////////////// ¾ÆÀÌÅÛ ±³È¯ °ü·Ã ///////////////
int		TradeItemSucessFlag = 0;
int		TradeRecvItemKeyFlag = 0;
int		TradeSendSucessFlag = 0;
DWORD	dwTradeMaskTime = 0;
int		TradeItemCancelCount = 0;

TRANS_TRADEITEMS	TransLastRecvTrade;
/*
//°³ÀÎ»óÁ¡ Á¤º¸
struct	TRANS_MYSHOP_ITEM {
	int		size,code;
	DWORD	dwSender;
	DWORD	dwRecver;
	DWORD	dwSum;
	DWORD	Temp[4];

	sMYSHOP	sMyShop;
};
*/
TRANS_TRADEITEMS	TransTradeItems_MyShop;			//°³ÀÎ»óÁ¡ ¾ÐÃà ±¸Á¶

/////////////////// Ã¢°í ///////////////////////////
DWORD	dwLastWareHouseChkSum = 0;

////////////// ¿¡ÀÌÂ¡ ¾ÆÀÌÅÛ ¼­¹ö¼Û¼ö½Å /////////////
int		TransAgingItemFlag;

smTRANS_COMMAND	TransServerConnectInfo;

//´Ù¸¥ À¯Àú¿Í ¿¬°á ½ÃÅ²´Ù
int ConnectOtherPlayer(DWORD dwIP);

int HoRecvMessage(DWORD dwCode, void *RecvBuff);

//¾ÆÀÌÅÛ ±³È¯ ÀÎÁõÅ° ¼ö½Å
int RecvTradeSucessKey(TRANS_TRADE_ITEMKEY *lpTransTradeItemKey, sTRADE *lpTrade);
//¾ÆÀÌÅÛ ±³È¯ ÀÎÁõÅ° Àü¼Û
int SendTradeSucessKey(sTRADE *lpTrade, DWORD dwSender);
//¾ÆÀÌÅÛ ±³È¯ ³»¿ë È®ÀÎ ¼ö½Å
int RecvTradeCheckItem(TRANS_TRADE_CHECKITEM *lpTransTradeCheckItem);
//Å©·¢ À©µµ¿ì ½Å°í
int SendCrackWindow(HWND hWnd);

//ÀÓ½Ã ÀúÀåµÈ ¹ö¸± ¾ÆÀÌÅÛÀ» È®ÀÎÇÏ¿© ÀÏÄ¡ÇÏ¸é ¼­¹ö·Î º¸³¿
int ThrowItemToServer(smTRANS_COMMAND_EX *lpTransCommand);
//¾ÆÀÌÅÛ È®ÀÎ °á°ú µµÂø
int RecvCheckItemFromServer(TRANS_ITEM_CODE *lpTransItemCode);
//Å¬¶óÀÌ¾ðÆ® Æã¼Ç À§Ä¡¿Í °ªÀ» ¼­¹ö¿¡ º¸°í
int	SendClientFuncPos();

//ÇÁ·Î¼¼½º ½Ã°£ °¨½ÃÇÏ´Â ¾²·¹µå °¡µ¿
int OpenTimeCheckThread();
//ÇÁ·Î¼¼½º ½Ã°£ °¨½ÃÁ¤º¸ ¼­¹ö·Î ¼ÛºÎ
int SendPlayTimerMax();


//ÀÓ½ÃÀúÀåµÈ ¾ÆÀÌÅÛ Å¥¿¡¼­ Ã£±â
TRANS_ITEMINFO	*FindRecvTransItemQue(DWORD dwCode, DWORD dwHead, DWORD dwChkSum);
//¾ÆÀÌÅÛ Å¥¿¡ ÀÓ½Ã ÀúÀå
int	PushRecvTransItemQue(TRANS_ITEMINFO *lpTransItemInfo);

//½ºÅ³ ½ÇÇà ÆÐÅ¶ ¼ö½Å
int RecvProcessSkill(smTRANS_COMMAND *lpTransCommand);
//ÆÄÆ¼ ½ºÅ³ ÆÐÅ¶ ¼ö½Å
int RecvPartySkillFromServer(TRANS_PARTY_SKILL *lpTransPartySkill);

//Äù½ºÆ® ¾ÆÀÌÅÛ ÀÔ¼ö ÇÊÅÍ¸µ
int FiltQuestItem(TRANS_ITEMINFO	*lpTransItemInfo, DWORD dwPacketCode);
//¸µÅ© ÄÚ¾î »ç¿ë ¼º°ø
int SucessLinkCore(smTRANS_COMMAND_EX *lpTransCommandEx);
//SoD ÁøÇà Á¤º¸ ¹ÞÀ½
int RecvSodGameInfomation(void *Info);

//Æ÷½º¿Àºê ¾ÆÀÌÅÛ ÀÔ¼ö
int RecvForceOrbItem(TRANS_ITEMINFO_GROUP2 *lpTransItemGroup2);

//¿¢½ºÆ®·¦ ÆÐÅ¶¼ö½Å
int XTrap_Recv(smTRANS_COMMAND *lpPacket, smWINSOCK *lpsmSock);

//ÀÌÀü ºí·¹½º Ä³½½ Á¤º¸
rsBLESS_CASTLE	rsBlessCastleOld;

//¼Ó¼ºÁ¤º¸ ¼Û½Å ½Ã°£
DWORD	dwResistance_SendingTime = 0;


//°ø°ÝÇÑ ¸ó½ºÅÍ ±¸º°ÄÚµå ±â·Ï
#define	ATT_MONSTER_MAX	128
#define	ATT_MONSTER_MASK	(ATT_MONSTER_MAX-1)

DWORD	dwAttMonsterCodeList[ATT_MONSTER_MAX];
int		AttMonsterCodeCount = 0;

//¸ó½ºÅÍ ÄÚµå Å¥¿¡ °ü¸®
int ClearAttMonsterCode()
{
	int cnt;
	for (cnt = 0; cnt < ATT_MONSTER_MAX; cnt++)
	{
		dwAttMonsterCodeList[cnt] = 0;
	}
	return TRUE;
}

int	AddAttMonsterCode(DWORD dwCode)
{
	int mcnt;
	if (dwCode)
	{
		mcnt = (AttMonsterCodeCount - 1)&ATT_MONSTER_MASK;
		if (dwAttMonsterCodeList[mcnt] == dwCode) return TRUE;
		mcnt = AttMonsterCodeCount&ATT_MONSTER_MASK;
		dwAttMonsterCodeList[mcnt] = dwCode;
		AttMonsterCodeCount++;
		return TRUE;
	}

	return FALSE;
}

int	CheckAttMonsterCode(DWORD dwCode)
{
	int cnt;
	for (cnt = 0; cnt < ATT_MONSTER_MAX; cnt++)
	{
		if (dwAttMonsterCodeList[cnt] == dwCode) return TRUE;
	}
	return FALSE;
}


///////////////////// º¸³»±â °ü·Ã ////////////////////

	//(smpacket.h / record.h / netplay.h)

	//smTRANSCODE_ID_GETUSERINFO: -> smTRANSCODE_ID_SETUSERINFO
	//ID·Î »ç¿ëÀÚ Ä³¸¯ÅÍ ¸®½ºÆ® ¹× Á¤º¸¸¦ ±¸ÇÔ

	//smTRANSCODE_INSRECORDDATA:
	//ID¿¡ »õ·Î¿î Ä³¸¯ÅÍ »ðÀÔ

	//smTRANSCODE_DELRECORDDATA:
	//ID¿¡ »õ·Î¿î Ä³¸¯ÅÍ »èÁ¦

	//smTRANSCODE_ISRECORDDATA: -> smTRANS_CHAR_COMMAND2
	//Ä³¸¯ÅÍ°¡ Á¸Àç ÇÏ´ÂÁö È®ÀÎ

//À¯Àú Á¤º¸ °ü·Ã Åë½Å ÄÚ¸àµå
//int	TransUserCommand ( DWORD dwCode , char *szID , char *szName )

///////////////////////////////////////////////////////

///////////////////// ¹Þ±â °ü·Ã ////////////////////
	//smTRANSCODE_ID_SETUSERINFO:
	//ID·Î »ç¿ëÀÚ Ä³¸¯ÅÍ ¸®½ºÆ® ¹× Á¤º¸¸¦ ±¸ÇÔ 
	//(TRANS_USERCHAR_INFO ±¸Á¶Ã¼·Î °á°ú µé¾î¿È )

	//smTRANSCODE_ISRECORDDATA:
	//Ä³¸¯ÅÍ°¡ Á¸Àç ÇÏ´ÂÁö È®ÀÎ 
	//( smTRANS_CHAR_COMMAND2 ->wParam ¿¡ °á°ú µé¾î¿È )
///////////////////////////////////////////////////////

//¼­¹ö Á¢¼Ó


//smWINSOCK *ConnectServer_Main();

//¼­¹ö Á¤º¸ È®ÀÎ
int CheckServerInfo(smTRANS_COMMAND *lpTransCommand)
{
	if ((-Client_Version * 2) != CLIENT_VERSION_CHECK)
	{
		//¹öÀüÀÌ ¸ÂÁö ¾ÊÀ½ ÀÔÀå ºÒ°¡
		SetGameError(1);
		return TRUE;
	}

	if (Client_Version < lpTransCommand->WParam)
	{
		//¹öÀüÀÌ ¸ÂÁö ¾ÊÀ½ ÀÔÀå ºÒ°¡
		SetGameError(1);
		return TRUE;
	}
	if (lpTransCommand->LParam>0)
	{
		//ÀÎ¿øÀÌ ¸¹¾Æ¼­ ÀÔÀå ºÒ°¡
		SetGameError(2);
		return TRUE;
	}
	SetGameError(0);
	return TRUE;
}

//ÇÁ·Î¼¼½º Á¤º¸ º¸³¿
int SendProcessInfo()
{
	TRANS_CHATMESSAGE	smTransChatMessage;

	if (smWsockDataServer)
	{
		smTransChatMessage.code = smTRANSCODE_PROCESSINFO;
		smTransChatMessage.dwIP = dwExeCheckSum;
		smTransChatMessage.dwObjectSerial = dwExeCheckSum;

		lstrcpy(smTransChatMessage.szMessage, szProcessPath);
		smTransChatMessage.size = 32 + lstrlen(szProcessPath);

		return smWsockDataServer->Send2((char *)&smTransChatMessage, smTransChatMessage.size, TRUE);
	}
	return FALSE;
}


//³Ý ÇÃ·¹ÀÌ ¸ÞÀÎ
smWINSOCK *ConnectServer_Main()
{
	/*
		//Å×½ºÆ® ¹öÀü
		smConfig.szServerIP[0] = 0;
		smConfig.szDataServerIP[0] = 0;
		smConfig.szUserServerIP[0] = 0;
	*/

	if (!smWsockServer)
	{
		//°ÔÀÓ ¼­¹ö ¿¬°á
		if (smConfig.szServerIP[0])
			lstrcpy(szServIP, smConfig.szServerIP);
		else
			lstrcpy(szServIP, szDefaultServIP);

		if (smConfig.dwServerPort)
			dwServPort = smConfig.dwServerPort;
		else
			dwServPort = TCP_SERVPORT;

		smWsockServer = smConnectSock(szServIP, (WORD)dwServPort);
	}

	if (smWsockServer && !smWsockDataServer)
	{
		//µ¥ÀÌÅ¸ ¼­¹ö ¿¬°á
		if (smConfig.szDataServerIP[0] && lstrcmpi(smConfig.szDataServerIP, smConfig.szServerIP) != 0)
		{
			lstrcpy(szDataServIP, smConfig.szDataServerIP);
			dwDataServPort = smConfig.dwDataServerPort;
			smWsockDataServer = smConnectSock(szDataServIP, (WORD)dwDataServPort);
		}
		else
		{
			//µ¥ÀÌÅ¸ ¼­¹ö¿Í °ÔÀÓ¼­¹ö¸¦ °°ÀÌ »ç¿ë
			lstrcpy(szDataServIP, szServIP);
			dwDataServPort = dwServPort;
			smWsockDataServer = smWsockServer;
		}
	}

	if (smWsockServer && smWsockDataServer && !smWsockUserServer)
	{
		//À¯Àú ¼­¹ö ¿¬°á
		if (smConfig.szUserServerIP[0] && lstrcmpi(smConfig.szUserServerIP, smConfig.szServerIP) != 0)
		{
			lstrcpy(szUserServIP, smConfig.szUserServerIP);
			dwUserServPort = smConfig.dwUserServerPort;
			smWsockUserServer = smConnectSock(szUserServIP, (WORD)dwUserServPort);
		}
		else
		{
			//À¯Àú ¼­¹ö¿Í °ÔÀÓ¼­¹ö¸¦ °°ÀÌ »ç¿ë
			lstrcpy(szUserServIP, szServIP);
			dwUserServPort = dwServPort;
			smWsockUserServer = smWsockServer;
		}
	}

	if (smWsockServer && smWsockDataServer && !smWsockExtendServer)
	{
		//È®Àå ¼­¹ö ¿¬°á
		if (smConfig.szExtendServerIP[0] && lstrcmpi(smConfig.szExtendServerIP, smConfig.szServerIP) != 0)
		{
			lstrcpy(szExtendServIP, smConfig.szExtendServerIP);
			dwExtendServPort = smConfig.dwExtendServerPort;
			smWsockExtendServer = smConnectSock(szExtendServIP, (WORD)dwExtendServPort);
		}
		else
		{
			//È®Àå ¼­¹ö¿Í °ÔÀÓ¼­¹ö¸¦ °°ÀÌ »ç¿ë
			lstrcpy(szExtendServIP, szServIP);
			dwExtendServPort = dwServPort;
			smWsockExtendServer = smWsockServer;
		}
	}


	if (smWsockServer && smWsockDataServer && smWsockUserServer)
	{

		if (smConfig.DebugMode)
		{
			//fpNetLog = fopen( szNetLogFile , "wb" );
		}

		return smWsockServer;
	}

	return NULL;
}

//¼­¹ö¿ÍÀÇ ¿¬°á ¸ðµÎ ²÷À½
int DisconnectServerFull()
{

	if (smWsockDataServer && smWsockDataServer != smWsockServer)
	{
		smWsockDataServer->CloseSocket();
		smWsockDataServer = 0;
	}
	if (smWsockUserServer && smWsockUserServer != smWsockServer)
	{
		smWsockUserServer->CloseSocket();
		smWsockUserServer = 0;
	}
	if (smWsockExtendServer && smWsockExtendServer != smWsockServer)
	{
		smWsockExtendServer->CloseSocket();
		smWsockExtendServer = 0;
	}
	if (smWsockServer)
	{
		smWsockServer->CloseSocket();
		smWsockServer = 0;
	}

	DisconnectFlag = 0;

	return TRUE;
}
/*
//Á¤º¸ ¼­¹ö Á¢¼Ó
smWINSOCK *ConnectServer_InfoMain();
//°ÔÀÓ ¼­¹ö Á¢¼Ó
smWINSOCK *ConnectServer_GameMain( char *szIP1, DWORD dwPort1, char *szIP2, DWORD dwPort2 );
//°ÔÀÓ ¼­¹ö ¿¬°á ²÷À½
int DisconnectServer_GameMain();

*/

//Á¤º¸ ¼­¹ö Á¢¼Ó
smWINSOCK *ConnectServer_InfoMain()
{
	if (!smWsockDataServer)
	{
		//µ¥ÀÌÅ¸ ¼­¹ö ¿¬°á
		if (smConfig.szDataServerIP[0])
		{
			dwDataServPort = TCP_SERVPORT;
			smWsockDataServer = smConnectSock(smConfig.szDataServerIP, (WORD)dwDataServPort);
			ZeroMemory(&TransServerConnectInfo, sizeof(smTRANS_COMMAND));
			return smWsockDataServer;
		}
	}


	return NULL;
}

//°ÔÀÓ ¼­¹ö Á¢¼Ó
smWINSOCK *ConnectServer_GameMain(char *szIP1, DWORD dwPort1, char *szIP2, DWORD dwPort2)
{
	int ServerInfoFlag = 0;

	lstrcpy(smConfig.szServerIP, szIP1);
	lstrcpy(smConfig.szUserServerIP, szIP2);

	if (smWsockServer && lstrcmp(smWsockServer->szIPAddr, szIP1) != 0)
	{
		DisconnectServer_GameMain();
	}
	if (smWsockUserServer && lstrcmp(smWsockUserServer->szIPAddr, szIP2) != 0)
	{
		DisconnectServer_GameMain();
	}

	if (!smWsockServer)
	{
		//°ÔÀÓ ¼­¹ö ¿¬°á
		if (smWsockDataServer &&
			lstrcmp(smWsockDataServer->szIPAddr, szIP1) == 0 && dwDataServPort == dwPort1)
		{

			smWsockServer = smWsockDataServer;
			dwServPort = dwDataServPort;

			ServerInfoFlag = TRUE;
		}
		else
		{
			smWsockServer = smConnectSock(szIP1, (WORD)dwPort1);
			dwServPort = dwPort1;
		}
		lstrcpy(smConfig.szServerIP, szIP1);
	}

	if (!smWsockUserServer)
	{
		if (smWsockServer &&
			lstrcmp(smWsockServer->szIPAddr, szIP2) == 0 && dwServPort == dwPort2)
		{

			smWsockUserServer = smWsockServer;
			dwUserServPort = dwServPort;
		}
		else
		{
			smWsockUserServer = smConnectSock(szIP2, (WORD)dwPort2);
			dwUserServPort = dwPort2;
		}
		lstrcpy(smConfig.szUserServerIP, szIP2);
	}


	if (smWsockServer && smWsockUserServer)
	{
		if (ServerInfoFlag)
		{
			//¼­¹ö Á¤º¸ È®ÀÎ
			CheckServerInfo(&TransServerConnectInfo);
		}

		return smWsockServer;
	}
	return NULL;
}

//°ÔÀÓ ¼­¹ö Á¢¼Ó
smWINSOCK *ConnectServer_GameMain(char *szIP1, DWORD dwPort1, char *szIP2, DWORD dwPort2, char *szIP3, DWORD dwPort3)
{
	int ServerInfoFlag = 0;

	lstrcpy(smConfig.szServerIP, szIP1);
	lstrcpy(smConfig.szUserServerIP, szIP2);
	lstrcpy(smConfig.szExtendServerIP, szIP3);


	if (smWsockServer && lstrcmp(smWsockServer->szIPAddr, szIP1) != 0)
	{
		DisconnectServer_GameMain();
	}
	if (smWsockUserServer && lstrcmp(smWsockUserServer->szIPAddr, szIP2) != 0)
	{
		DisconnectServer_GameMain();
	}
	if (smWsockExtendServer && lstrcmp(smWsockExtendServer->szIPAddr, szIP3) != 0)
	{
		DisconnectServer_GameMain();
	}

	if (!smWsockServer)
	{
		//°ÔÀÓ ¼­¹ö ¿¬°á
		if (smWsockDataServer &&
			lstrcmp(smWsockDataServer->szIPAddr, szIP1) == 0 && dwDataServPort == dwPort1)
		{

			smWsockServer = smWsockDataServer;
			dwServPort = dwDataServPort;

			ServerInfoFlag = TRUE;
		}
		else
		{
			smWsockServer = smConnectSock(szIP1, (WORD)dwPort1);
			dwServPort = dwPort1;
		}
		lstrcpy(smConfig.szServerIP, szIP1);
	}

	if (!smWsockUserServer)
	{
		if (smWsockServer &&
			lstrcmp(smWsockServer->szIPAddr, szIP2) == 0 && dwServPort == dwPort2)
		{

			smWsockUserServer = smWsockServer;
			dwUserServPort = dwServPort;
		}
		else
		{
			smWsockUserServer = smConnectSock(szIP2, (WORD)dwPort2);
			dwUserServPort = dwPort2;
		}
		lstrcpy(smConfig.szUserServerIP, szIP2);
	}

	if (!smWsockExtendServer)
	{
		if ((smWsockServer &&
			 lstrcmp(smWsockServer->szIPAddr, szIP3) == 0 && dwServPort == dwPort3) || !szIP3[0])
		{

			smWsockExtendServer = smWsockServer;
			dwExtendServPort = dwServPort;
		}
		else
		{
			smWsockExtendServer = smConnectSock(szIP3, (WORD)dwPort3);
			dwExtendServPort = dwPort3;
		}
		lstrcpy(smConfig.szExtendServerIP, szIP3);
	}

	if (smWsockServer && smWsockUserServer && smWsockExtendServer)
	{
		if (ServerInfoFlag)
		{
			//¼­¹ö Á¤º¸ È®ÀÎ
			CheckServerInfo(&TransServerConnectInfo);
		}

		return smWsockServer;
	}
	return NULL;
}


//°ÔÀÓ ¼­¹ö ¿¬°á ²÷À½
int DisconnectServer_GameMain()
{
	if (smWsockExtendServer && smWsockExtendServer != smWsockServer)
	{
		if (smWsockDataServer != smWsockExtendServer)
		{
			smWsockExtendServer->CloseSocket();
		}
	}
	smWsockExtendServer = 0;

	if (smWsockUserServer && smWsockUserServer != smWsockServer)
	{
		if (smWsockDataServer != smWsockUserServer)
		{
			smWsockUserServer->CloseSocket();
		}
	}
	smWsockUserServer = 0;


	if (smWsockServer && smWsockDataServer != smWsockServer)
	{
		if (smWsockDataServer != smWsockServer)
		{
			smWsockServer->CloseSocket();
		}
	}
	smWsockServer = 0;

	DisconnectFlag = 0;

	return TRUE;
}

//¼­¹ö ÄÚµå·Î ¼ÒÄÏÀ» Ã£À½
smWINSOCK *GetServerSock(int ServerCode)
{
	switch (ServerCode)
	{
		case PLAY_SERVER_CODE_EXTEND:
			if (smWsockExtendServer && smWsockServer != smWsockExtendServer)
				return smWsockExtendServer;
			else
				return smWsockUserServer;

		case PLAY_SERVER_CODE_USER:
			return smWsockUserServer;

		case PLAY_SERVER_CODE_MAIN:
			return smWsockServer;

		case PLAY_SERVER_CODE_AREA1:
			return lpWSockServer_DispArea[0];

		case PLAY_SERVER_CODE_AREA2:
			return lpWSockServer_DispArea[1];

	}

	return NULL;
}

//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
smWINSOCK *GetAreaServerSock()
{
	int ServerCode;

	if (lpCurPlayer->OnStageField >= 0)
	{
		if (AreaServerMode)
		{
			return lpWSockServer_DispArea[lpCurPlayer->OnStageField];
		}
		else
		{
			ServerCode = StageField[lpCurPlayer->OnStageField]->ServerCode;
			return GetServerSock(ServerCode);
		}
	}

	return NULL;
}


//¶³¾îÁø ¾ÆÀÌÅÛ Ã£±â
scITEM *FindScItem(int x, int z)
{
	int cnt;

	for (cnt = 0; cnt < DISP_ITEM_MAX; cnt++)
	{
		if (scItems[cnt].Flag &&
			scItems[cnt].pX == x && scItems[cnt].pZ == z)
		{
			return &scItems[cnt];
		}
	}

	return NULL;
}

//ºó ¾ÆÀÌÅÛ °ø°£Ã£±â
scITEM *FindEmptyScItem()
{
	int cnt;

	for (cnt = 0; cnt < DISP_ITEM_MAX; cnt++)
	{
		if (!scItems[cnt].Flag) return &scItems[cnt];
	}

	return NULL;
}

//±³È¯ °Å·¡ °Å¸® È®ÀÎ
int GetTradeDistance(smCHAR *lpChar)
{
	int	x, y, z;
	int dist;

	x = (lpCurPlayer->pX - lpChar->pX) >> FLOATNS;
	y = (lpCurPlayer->pY - lpChar->pY) >> FLOATNS;
	z = (lpCurPlayer->pZ - lpChar->pZ) >> FLOATNS;

	dist = x*x + y*y + z*z;

	if (dist < (256 * 256)) return TRUE;

	return FALSE;
}

//±³È¯ °Å·¡ °Å¸® È®ÀÎ
int GetTradeDistanceFromCode(DWORD dwObjectSerial)
{
	smCHAR *lpChar;

	lpChar = FindAutoPlayer(dwObjectSerial);

	if (lpChar && lpChar->DisplayFlag && lpChar->smCharInfo.szName[0])
	{

		return  GetTradeDistance(lpChar);

	}
	return FALSE;
}

//»õ·Î¸¸µç Ä³¸¯Á¤º¸ È®ÀÎ
int	CheckStartCharInfo()
{
	//º¸¾ÈÀ» À§ÇØ ÇÑ¹ø´õ È®ÀÎ
	smCHAR_INFO	smCharInfo;
	int cnt;

	memcpy(&smCharInfo, sinChar, sizeof(smCHAR_INFO));

	CheckCharForm();
	CodeXorExp = dwPlayTime + smCharInfo.Exp;						//°æÇèÄ¡ ¼û±è ÄÚµå
	CodeXorExp_High = dwPlayTime | (dwPlayTime*dwPlayTime);						//°æÇèÄ¡ ¼û±è ÄÚµå
	CodeXorLife = (dwPlayTime*smCharInfo.Life[0]) & 0xFFFF;		//»ý¸í·Â ¼û±è ÄÚµå
	cnt = smCharInfo.Dexterity + smCharInfo.Health + smCharInfo.Spirit + smCharInfo.Strength + smCharInfo.Talent;

	if (smCharInfo.Level >= 2 || cnt > 102 || smCharInfo.Exp || smCharInfo.ChangeJob || smCharInfo.Money)
	{
		//Ä³¸¯ÅÍ ¹®Á¦¹ß»ý Á¶ÀÛÀÇÈ¤
		SendSetHackUser2(6000, smCharInfo.Money);
		WarningHack = TRUE;
		return FALSE;
	}

	lpCurPlayer->smCharInfo.Exp ^= CodeXorExp;
	lpCurPlayer->smCharInfo.Exp_High ^= CodeXorExp_High;

	lpCurPlayer->smCharInfo.Life[0] ^= CodeXorLife;
	ReformCharForm();

	return TRUE;
}

//¸Þ¸ð¸® °Ë»ç ¸ðµâ ¹ÞÀ½2
int	RecvMemFuncData2(TRANS_FUNC_MEMORY *TransFuncMem)
{
	smTRANS_COMMAND	smTransCommand;


	smTransCommand.size = -1;
	fnChkMem2 = (LPFN_CheckMem)((void *)TransFuncMem->szData);
	fnChkMem2(TransFuncMem, &smTransCommand);

	ZeroMemory(TransFuncMem, 1500);

	if (smTransCommand.size > 0 && smWsockDataServer)
	{
		smTransCommand.code = smTRANSCODE_FUNCTION_MEM2;
		smWsockDataServer->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
	}

	return TRUE;
}



sSERVER_MONEY	sServerMoney[3] = { {0,0,0,0,0},{0,0,0,0,0} };
sSERVER_EXP		sServerExp[3] = { {0,0,0,0},{0,0,0,0} };
DWORD			dwLastSendTotalExpMoenyTime = 0;

//¹ÞÀº °æÇèÄ¡ µ·µ¥ÀÌÅ¸ ¼öÄ¡¸¦ ¼­¹ö¿¡ º¸³»¼­ È®ÀÎ
int	SendTotalExpMoney()
{
	TRANS_TOTAL_EXPMONEY	TransTotalExpMoney;

	if ((dwLastSendTotalExpMoenyTime + 30000) > dwPlayTime) return FALSE;

	TransTotalExpMoney.code = smTRANSCODE_CHECK_EXPMONEY;
	TransTotalExpMoney.size = sizeof(TRANS_TOTAL_EXPMONEY);

	if (smWsockServer)
	{
		memcpy(&TransTotalExpMoney.sServerExp, &sServerExp[0], sizeof(sSERVER_EXP));
		memcpy(&TransTotalExpMoney.sServerMoney, &sServerMoney[0], sizeof(sSERVER_MONEY));
		smWsockServer->Send2((char *)&TransTotalExpMoney, TransTotalExpMoney.size, TRUE);
	}
	if (smWsockUserServer && smWsockUserServer != smWsockServer)
	{
		memcpy(&TransTotalExpMoney.sServerExp, &sServerExp[1], sizeof(sSERVER_EXP));
		memcpy(&TransTotalExpMoney.sServerMoney, &sServerMoney[1], sizeof(sSERVER_MONEY));
		smWsockUserServer->Send2((char *)&TransTotalExpMoney, TransTotalExpMoney.size, TRUE);
	}
	if (smWsockExtendServer && smWsockExtendServer != smWsockServer)
	{
		memcpy(&TransTotalExpMoney.sServerExp, &sServerExp[2], sizeof(sSERVER_EXP));
		memcpy(&TransTotalExpMoney.sServerMoney, &sServerMoney[2], sizeof(sSERVER_MONEY));
		smWsockExtendServer->Send2((char *)&TransTotalExpMoney, TransTotalExpMoney.size, TRUE);
	}

	dwLastSendTotalExpMoenyTime = dwPlayTime;

	return TRUE;
}

//¼­¹ö¿¡¼­ µé¾î¿Â µ·ÀÇ ¾ç È®ÀÎ
int CheckServerMoney(smWINSOCK *lpsmSock, TRANS_ITEMINFO	*lpTransItemInfo)
{
	sSERVER_MONEY	*lpServerMoney;

	lpServerMoney = 0;

	if (lpsmSock == smWsockServer)
	{
		lpServerMoney = &sServerMoney[0];
	}
	else
	{
		if (lpsmSock == smWsockUserServer)
		{
			lpServerMoney = &sServerMoney[1];
		}
		else if (lpsmSock == smWsockExtendServer)
		{
			lpServerMoney = &sServerMoney[2];
		}
	}

	if (lpServerMoney)
	{
		lpServerMoney->InputMoney += lpTransItemInfo->Item.Money;
		lpServerMoney->Counter++;
		lpServerMoney->TotalX = lpTransItemInfo->x;
		lpServerMoney->TotalY = lpTransItemInfo->y;
		lpServerMoney->TotalZ = lpTransItemInfo->z;

		/*
							TransItemInfo.x = dwPlayServTime;											//ÇØµ¶ Å°
							TransItemInfo.y = TransItemInfo.Item.ItemHeader.dwChkSum;					//ÇØµ¶ Å°2
							TransItemInfo.z = rsPlayInfo[cnt].spMoney_Out^(TransItemInfo.x+TransItemInfo.y);	//ÅäÅ» ±â·Ï Àü¼Û
		*/

		int cnt;
		cnt = lpServerMoney->TotalZ ^ (lpServerMoney->TotalX + lpServerMoney->TotalY);
		cnt += 200;
		if (cnt < lpServerMoney->InputMoney)
		{
			//SendSetHackUser( 82 );
		}
	}

	return TRUE;
}


//¼­¹ö¿¡¼­ µé¾î¿Â °æÇèÄ¡ÀÇ ¾ç È®ÀÎ
int CheckServerExp(smWINSOCK *lpsmSock, smTRANS_COMMAND	*lpTransCommand)
{
	sSERVER_EXP	*lpServerExp;

	lpServerExp = 0;

	if (lpsmSock == smWsockServer)
	{
		lpServerExp = &sServerExp[0];
	}
	else
	{
		if (lpsmSock == smWsockUserServer)
		{
			lpServerExp = &sServerExp[1];
		}
		else if (lpsmSock == smWsockExtendServer)
		{
			lpServerExp = &sServerExp[2];
		}
	}

	if (lpServerExp)
	{
		lpServerExp->InputExp += lpTransCommand->WParam;
		lpServerExp->Counter++;
		lpServerExp->Total1 = lpTransCommand->SParam;
		lpServerExp->Total2 = lpTransCommand->EParam;

		int cnt;
		// Àåº° - °æÇèÄ¡ ¿¡·¯ À§ÇØ (800000 - > 1600000 = °æÇèÄ¡ 2¹è ÀÌº¥Æ®)
		cnt = (lpServerExp->Total1^lpServerExp->Total2) + 1600000;		//¿ÀÂ÷À² Àû¿ë
		if (cnt < lpServerExp->InputExp)
		{
			SendSetHackUser(83);
		}

	}

	return TRUE;
}



//´Ù¸¥ ÇÃ·¹ÀÌ¾î¿ÍÀÇ ¿¬°á ¾²·¹µå¸¦ »ý¼º
int CreateConnectThread(DWORD dwIP);
//´Ù¸¥ ÇÃ·¹ÀÌ¾î¿ÍÀÇ ¿¬°áÁ¾·á ¾²·¹µå »ý¼º
int CreateDisconnectThread(smWINSOCK *lpsmSock);

//´Ù¸¥ ´Ù¸¥ ÇÃ·¹ÀÌ¾îÀÇ ºó ¿µ¿ªÀ» Ã£À½
int FindNewOtherPlayer()
{
	int cnt;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (!chrOtherPlayer[cnt].Flag)
			return cnt;
	}

	return -1;
}

//ÀÚµ¿ ÇÃ·¹ÀÌ¾î¸¦ Ã£´Â´Ù
smCHAR *FindAutoPlayer(DWORD dwObjectSerial)
{
	int cnt;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].dwObjectSerial == dwObjectSerial)
		{
			return &chrOtherPlayer[cnt];
		}
	}

	return NULL;
}

// À¯Àú¹øÈ£·Î Ã£´Â´Ù
smCHAR *FindChrPlayer(DWORD dwObjectSerial)
{
	int cnt;

	if (lpCurPlayer->dwObjectSerial == dwObjectSerial)
		return lpCurPlayer;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].dwObjectSerial == dwObjectSerial)
		{
			return &chrOtherPlayer[cnt];
		}
	}

	return NULL;
}

//Á×ÀºÆÄÆ¼À¯Àú¸¦ Ã£´Â´Ù
smCHAR *FindDeadPartyUser()
{
	int cnt;
	smCHAR *lpChar;
	int dist, x, y, z;
	int	NearDist;

	lpChar = 0;
	NearDist = 300 * 300;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].PartyFlag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_USER)
		{
			if (chrOtherPlayer[cnt].MotionInfo->State == CHRMOTION_STATE_DEAD)
			{

				x = (lpCurPlayer->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
				y = (lpCurPlayer->pY - chrOtherPlayer[cnt].pY) >> FLOATNS;
				z = (lpCurPlayer->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;

				dist = x*x + z*z + y*y;

				if (dist < NearDist)
				{
					lpChar = &chrOtherPlayer[cnt];
					NearDist = dist;
				}
			}
		}
	}

	return lpChar;
}


//ÃÖ±ÙÁ¢ ¸ó½ºÅÍ¸¦ Ã£´Â´Ù
smCHAR *FindNearMonster(smCHAR *lpCurChar)
{
	int cnt;
	smCHAR *lpChar;
	int dist, x, y, z;
	int	NearDist;

	lpChar = 0;
	NearDist = 300 * 300;

	if (!lpCurChar) return NULL;

	if (lpCurChar != lpCurPlayer)
	{
		x = (lpCurChar->pX - lpCurPlayer->pX) >> FLOATNS;
		y = (lpCurChar->pY - lpCurPlayer->pY) >> FLOATNS;
		z = (lpCurChar->pZ - lpCurPlayer->pZ) >> FLOATNS;

		dist = x*x + z*z + y*y;
		if (dist < NearDist)
		{
			if (!lpCurChar->smCharInfo.ClassClan || lpCurChar->smCharInfo.ClassClan != lpCurPlayer->smCharInfo.ClassClan)
			{
				lpChar = lpCurPlayer;
				NearDist = dist;
			}
		}
	}

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY && lpCurChar != &chrOtherPlayer[cnt])
		{
			if (chrOtherPlayer[cnt].smCharInfo.Brood != smCHAR_MONSTER_USER && chrOtherPlayer[cnt].smCharInfo.Life[0]>0 &&
				(!chrOtherPlayer[cnt].smCharInfo.ClassClan || chrOtherPlayer[cnt].smCharInfo.ClassClan != lpCurChar->smCharInfo.ClassClan))
			{
				x = (lpCurChar->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
				y = (lpCurChar->pY - chrOtherPlayer[cnt].pY) >> FLOATNS;
				z = (lpCurChar->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;

				dist = x*x + z*z + y*y;

				if (dist < NearDist)
				{
					lpChar = &chrOtherPlayer[cnt];
					NearDist = dist;
				}
			}
		}
	}

	return lpChar;
}



//ºó ÇÃ·¹ÀÌ¾î¸¦ Ã£´Â´Ù
smCHAR *FindEmptyChar()
{
	int cnt;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (!chrOtherPlayer[cnt].Flag)
			return &chrOtherPlayer[cnt];
	}

	return NULL;
}

//Ã¤ÆÃÃ¢¿¡ ¹®ÀÚ »ðÀÔ
int AddChatBuff(char *szMsg, DWORD dwIP)
{
	Log::Debug("AddChatBuff : Requisitado porém foi removido.");
	return NULL;
}
int AddChatBuff(char *szMsg)
{
	Log::Debug("AddChatBuff : Requisitado porém foi removido.");
	return NULL;
}
int	ChatBuffFilter(char *szMsg, DWORD dwIP)
{
	Log::Debug("ChatBuffFilter : Requisitado porém foi removido.");
	return NULL;
}


//½Ã½ºÅÛ ½Ã°£À» °ÔÀÓ½Ã°£À¸·Î º¯È¯
DWORD	ConvSysTimeToGameTime(DWORD dwTime)
{
	return dwTime / GAME_WORLDTIME_MIN;
}

//DispLoading

#define RECV_DATA_QUE_COUNT		64
#define RECV_DATA_QUE_MASK		63


class	rsTRANS_SERVER
{
	char	TransBuff[smSOCKBUFF_SIZE];

	rsRECORD_DBASE	*lpRecorder;


	smTHREADSOCK	*RecvDataQue[RECV_DATA_QUE_COUNT];
	int				RecvDataQuePush;
	int				RecvDataQuePop;

	TRANS_PLAYPOS	TransPlayPosLast;
	int				TransPlayPosCount;

public:
	smWINSOCK	*lpsmSock;


	//Å¬·¡½º ÃÊ±âÈ­
	int	Init(smWINSOCK *lpsmsock);
	//¼­¹ö·Î ¸Þ¼¼Áö Àü¼Û
	int SendChat(char *szMessage);
	//¼­¹ö·Î ÇÃ·¹ÀÌ¾îÀÇ µ¥ÀÌÅ¸¸¦ À§Ä¡¸¦ Àü¼Û
	int SendPlayData(smCHAR *lpChar);

	//¼­¹ö·Î ºÎÅÍ ¹ÞÀº ¸Þ¼¼Áö Ã³¸®
	int RecvMessage(smTHREADSOCK *SockInfo);

	//¸Þ¼¼Áö Å¥ÀÇ ¸Þ¼¼Áö¸¦ Ã³¸®
	int RecvMessageQue();

};

/*
		case smTRANSCODE_ATTACKDATA:
		case smTRANSCODE_PLAYDATA1:
		case smTRANSCODE_PLAYDATA2:
		case smTRANSCODE_PLAYDATA3:
		case smTRANSCODE_TRANSPLAYDATA:
		case smTRANSCODE_PLAYDATAGROUP:
		case smTRANSCODE_PLAYITEM:
*/

//lpCurPlayer->MotionInfo->State==CHRMOTION_STATE_STAND
//lpCurPlayer->MotionInfo->State==CHRMOTION_STATE_WALK
//lpCurPlayer->MotionInfo->State==CHRMOTION_STATE_RUN


//¸Þ¼¼Áö Å¥ÀÇ ¸Þ¼¼Áö¸¦ Ã³¸®
int rsTRANS_SERVER::RecvMessageQue()
{
	int cnt;

	if (RecvDataQuePop >= RecvDataQuePush) return TRUE;
	if (MessageLoading) return FALSE;

	//Å©¸®Æ¼Ä® ¼½¼Ç ¼±¾ð
	EnterCriticalSection(&cSection);

	while (1)
	{
		if (RecvDataQuePop >= RecvDataQuePush) break;

		cnt = RecvDataQuePop & RECV_DATA_QUE_MASK;

		if (RecvDataQue[cnt])
		{
			RecvMessage(RecvDataQue[cnt]);

			delete RecvDataQue[cnt];
			RecvDataQue[cnt] = 0;
		}

		RecvDataQuePop++;
	}

	//Å©¸®Æ¼Ä® ¼½¼Ç ÇØÁ¦
	LeaveCriticalSection(&cSection);

	return TRUE;
}



DWORD	dwGetCharInfoTime = 0;

//¼­¹ö·Î ºÎÅÍ ¹ÞÀº ¸Þ¼¼Áö Ã³¸®
int rsTRANS_SERVER::RecvMessage(smTHREADSOCK *pData)
{
	Log::Debug("RecvMessage : Requisitado porém foi removido.");
	return NULL;
}



int rsTRANS_SERVER::Init(smWINSOCK *lpsmsock)
{

	lpsmSock = lpsmsock;
	lpRecorder = 0;

	//Å©¸®Æ¼Ä® ¼½¼Ç ÃÊ±âÈ­
	InitializeCriticalSection(&cServSection);

	RecvDataQuePush = 0;
	RecvDataQuePop = 0;

	ZeroMemory(RecvDataQue, RECV_DATA_QUE_COUNT * 4);

	return TRUE;
}


// °ø¹éÀÇ ¹®ÀÚ¿­À» Àß¶ó³½´Ù
extern int szSpaceSorting(char *lpString);

//Ã¤ÆÃ ¹®ÀÚ¸¦ ¼­¹ö·Î Àü¼Û
int rsTRANS_SERVER::SendChat(char *szMessage)
{
	int len;

	if (!szMessage || !szMessage[0]) return FALSE;

	if (szMessage[0] == '/' && (szMessage[1] == ';' || szMessage[1] == ':'))
	{
		// °ø¹éÀÇ ¹®ÀÚ¿­À» Àß¶ó³½´Ù
		szSpaceSorting(szMessage);
	}

	if (szMessage[0] == '`' && szMessage[1] == '/')
	{
		len = lstrlen(szMessage + 1) + 9;
		lstrcpy(TransBuff + 8, szMessage + 1);
		((int *)TransBuff)[0] = len;
		((int *)TransBuff)[1] = smTRANSCODE_CHATMESSAGE;
		if (smWsockDataServer) smWsockDataServer->Send(TransBuff, len, TRUE);
		return TRUE;
	}

	if (szMessage[0] == '~' && szMessage[1] == '/')
	{
		//¾ç¼­¹ö¿¡ µ¿½Ã¿¡ ¸Þ¼¼Áö º¸³¿ '~'Á¦°Å
		len = lstrlen(szMessage + 1) + 9;
		lstrcpy(TransBuff + 8, szMessage + 1);
		((int *)TransBuff)[0] = len;
		((int *)TransBuff)[1] = smTRANSCODE_CHATMESSAGE;
		if (smWsockServer) smWsockServer->Send(TransBuff, len, TRUE);
		if (smWsockServer != smWsockUserServer && smWsockUserServer) smWsockUserServer->Send(TransBuff, len, TRUE);
		if (smWsockServer != smWsockExtendServer && smWsockExtendServer) smWsockExtendServer->Send(TransBuff, len, TRUE);
		if (AreaServerMode)
		{
			if (lpWSockServer_Area[0]) lpWSockServer_Area[0]->Send(TransBuff, len, TRUE);
			if (lpWSockServer_Area[1]) lpWSockServer_Area[1]->Send(TransBuff, len, TRUE);
		}

	}
	else
	{

		if (szMessage[0] == '@')
		{
			//µ¿·á Ã¤ÆÃ

			wsprintf(TransBuff + 8, "%s: %s", PlayerName, szMessage + 1);
			len = lstrlen(TransBuff + 8) + 9;
			((int *)TransBuff)[0] = len;
			((int *)TransBuff)[1] = smTRANSCODE_PARTY_CHATMSG;

			if (smWsockServer) smWsockServer->Send(TransBuff, len, TRUE);
			AddChatBuff(TransBuff + 8, 4);
			return TRUE;
		}
		else
		{
			lstrcpy(TransBuff + 8, szMessage);
			len = lstrlen(szMessage) + 9;

			((int *)TransBuff)[0] = len;
			((int *)TransBuff)[1] = smTRANSCODE_CHATMESSAGE;
		}

		if (szMessage[0] == '/')
		{

			if (szMessage[1] == 'c' && szMessage[2] == 'o' && szMessage[3] == 'u' && szMessage[4] == 'p' &&
				szMessage[5] == 'l' && szMessage[6] == 'e')
			{
				if (smWsockUserServer && cInvenTory.SearchItemCode(sinSP1 | sin15)) smWsockUserServer->Send(TransBuff, len, TRUE);
				return TRUE;
			}

			if (szMessage[1] == '/')
			{
				if (smWsockUserServer) smWsockUserServer->Send(TransBuff, len, TRUE);
			}
			else
			{
				if (smWsockServer) smWsockServer->Send(TransBuff, len, TRUE);
			}

			return TRUE;
		}

		if (AreaServerMode)
		{
			if (lpCurPlayer->OnStageField >= 0 && lpWSockServer_DispArea[lpCurPlayer->OnStageField])
			{
				lpWSockServer_DispArea[lpCurPlayer->OnStageField]->Send(TransBuff, len, TRUE);
			}
		}
		else
			if (smWsockUserServer) smWsockUserServer->Send(TransBuff, len, TRUE);

	}
	return TRUE;
}
//¼­¹ö·Î ÇÃ·¹ÀÌ¾îÀÇ µ¥ÀÌÅ¸¸¦ À§Ä¡¸¦ Àü¼Û
int rsTRANS_SERVER::SendPlayData(smCHAR *lpChar)
{
	//	int cnt;
	//	int	*lpTransBuff;
	TRANS_PLAYPOS *lpTransPlayPos;
	int	dist, x, y, z;

	TransPlayPosCount++;

	if ((TransPlayPosCount & 1) == 0)
	{
		x = (TransPlayPosLast.rsPlayPos.x - lpChar->pX) >> FLOATNS;
		y = (TransPlayPosLast.rsPlayPos.y - lpChar->pY) >> FLOATNS;
		z = (TransPlayPosLast.rsPlayPos.z - lpChar->pZ) >> FLOATNS;
		dist = x*x + y*y + z*z;

		if (sinGetLife() > 0 && dist < (16 * 16 * 16))
		{
			//À§Ä¡°¡ º¯ÇÑ°Ô ¾ø´Â °æ¿ì ( 1¹ø¾¿ °É·¯¼­ º¸³¿ )
			return TRUE;
		}
	}

	lpTransPlayPos = (TRANS_PLAYPOS *)TransBuff;
	lpTransPlayPos->code = smTRANSCODE_POSITION;
	lpTransPlayPos->size = sizeof(TRANS_PLAYPOS);

	if (lpCurPlayer->OnStageField >= 0)
		lpTransPlayPos->rsPlayPos.Area = StageField[lpCurPlayer->OnStageField]->FieldCode;
	else
		lpTransPlayPos->rsPlayPos.Area = -1;

	lpTransPlayPos->rsPlayPos.x = lpChar->pX;
	lpTransPlayPos->rsPlayPos.y = lpChar->pY;
	lpTransPlayPos->rsPlayPos.z = lpChar->pZ;

	if (lpCurPlayer->MotionInfo && lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD)
		lpTransPlayPos->Hp[0] = 0;
	else
		lpTransPlayPos->Hp[0] = sinGetLife();

	lpTransPlayPos->Hp[1] = lpChar->smCharInfo.Life[1];

	if (smWsockServer)
		smWsockServer->Send2((char *)lpTransPlayPos, lpTransPlayPos->size, TRUE);

	if (smWsockExtendServer && smWsockExtendServer != smWsockServer)
		smWsockExtendServer->Send2((char *)lpTransPlayPos, lpTransPlayPos->size, TRUE);


	int	MainArea, SubArea;

	if (AreaServerMode)
	{
		if (lpCurPlayer->OnStageField >= 0)
		{
			MainArea = (lpCurPlayer->OnStageField) & 1;
			SubArea = (lpCurPlayer->OnStageField + 1) & 1;

			if (lpWSockServer_DispArea[SubArea] && lpWSockServer_DispArea[MainArea] != lpWSockServer_DispArea[SubArea])
			{
				lpWSockServer_DispArea[SubArea]->Send2((char *)lpTransPlayPos, lpTransPlayPos->size, TRUE);
			}
		}
	}


	return TRUE;
}




//¼­¹ö Åë½Å Å¬·¡½º ¼±¾ð
rsTRANS_SERVER	rsMainServer;







/*
//ÇÃ·¹ÀÌ¾î Á¤º¸¸¦ »ó´ë¹æ¿¡ Àü¼Û
int SendPlayerInfo( smWINSOCK *lpsmSock )
{

	smTRNAS_PLAYERINFO	*lpTransPlayerInfo;

	lpTransPlayerInfo = (smTRNAS_PLAYERINFO *)TransBuff;

	lstrcpy( lpTransPlayerInfo->smCharInfo.szName , PlayerName );
	lstrcpy( lpTransPlayerInfo->smCharInfo.szModelName , lpCurPlayer->lpDinaPattern->szPatName );

	lpTransPlayerInfo->size = sizeof( smTRNAS_PLAYERINFO );
	lpTransPlayerInfo->code = smTRANSCODE_PLAYERINFO;
	lpsmSock->Send( (char *)lpTransPlayerInfo , lpTransPlayerInfo->size );

	return TRUE;
}
*/

#define SAVE_CHAT_COMMAND_MAX		16
#define SAVE_CHAT_COMMAND_MASK		15

char szSaveChatCommands[SAVE_CHAT_COMMAND_MAX][256];
int ChatCommandSaveCnt = 0;
int ChatSaveRecallCnt = 0;

//Ã¤ÆÃ ÄÚ¸Çµå ÀúÀå
int SaveChatCommand(char *szMessage)
{
	int len, cnt;
	int mcnt;
	char szBuff[256];

	len = lstrlen(szMessage);
	lstrcpy(szBuff, szMessage);

	for (cnt = 0; cnt < len; cnt++)
	{
		if (szBuff[cnt] == ' ')
		{
			szBuff[cnt] = 0;
			break;
		}
	}
	lstrcat(szBuff, " ");

	//µ¿ÀÏ¹®ÀÚ È®ÀÎ
	for (cnt = 0; cnt < ChatCommandSaveCnt; cnt++)
	{
		if (cnt >= SAVE_CHAT_COMMAND_MAX) break;
		if (lstrcmp(szSaveChatCommands[cnt], szBuff) == 0)
			return TRUE;
	}

	mcnt = ChatCommandSaveCnt & SAVE_CHAT_COMMAND_MASK;
	lstrcpy(szSaveChatCommands[mcnt], szBuff);

	ChatCommandSaveCnt++;
	ChatSaveRecallCnt = ChatCommandSaveCnt;

	return TRUE;
}

//Ã¤ÆÃ ÀúÀå ÄÚ¸Çµå º¹±¸ 
int RecallSavedChatCommand(HWND hChatWnd, int Arrow)
{
	int mcnt;
	int len;

	if (ChatCommandSaveCnt <= 0) return FALSE;

	if (Arrow)
	{
		ChatSaveRecallCnt--;
		if (ChatSaveRecallCnt < 0)
		{
			ChatSaveRecallCnt = ChatCommandSaveCnt - 1;
		}
	}
	else
	{
		ChatSaveRecallCnt++;
		if (ChatSaveRecallCnt >= ChatCommandSaveCnt)
		{
			ChatSaveRecallCnt = ChatCommandSaveCnt - 1;
			SetWindowText(hChatWnd, "");
			return TRUE;
		}
	}

	mcnt = ChatSaveRecallCnt & SAVE_CHAT_COMMAND_MASK;
	SetWindowText(hChatWnd, szSaveChatCommands[mcnt]);

	len = lstrlen(szSaveChatCommands[mcnt]);
	SendMessage(hChatWnd, EM_SETSEL, len, len);

	return TRUE;
}

// ¹®ÀÚ¿­ ºñ±³ ÂªÀº ¹®ÀåÀÇ ±æÀÌ¸¸Å­ ºñ±³ÇÏ¿© ºñ±³ Á¾·á Æ÷ÀÎÅÍ ¹ÝÈ¯
int rsCompString(char *src1, char *src2)
{
	int cnt;
	int len1, len2;
	int len;

	len1 = lstrlen(src1);
	len2 = lstrlen(src2);

	if (len1 < len2) len = len1;
	else len = len2;

	if (!len) return NULL;

	for (cnt = 0; cnt < len1; cnt++)
	{
		if (src1[cnt] == 0) break;
		if (src1[cnt] != src2[cnt]) return NULL;
	}

	return cnt;
}



char szLastChatMessage[256];
DWORD	dwLastChatTime;
DWORD	dwContiueChatCount = 0;
#include "CurseFilter.h"		//¿å¼³ÇÊÅÍ

char *rsGetWord(char *q, char *p);		//¼­¹ö¿¡ ¼±¾ðµÈ °ª

extern int WaveCameraMode;

//Ã¤ÆÃ ¹®ÀÚ¸¦ ¼­¹ö·Î Àü¼Û
int SendChatMessageToServer(char *szChatMessage)
{
	Log::Debug("SendChatMessageToServer : Requisitado porém foi removido.");
	return NULL;
}

//Ã¤ÆÃ ½ºÅ©·Ñ Æ÷ÀÎÆ®
int ChatScrollPoint = 0;

//Ã¤ÆÃ Á¤º¸ È­¸é¿¡ Ç¥½Ã
int DisplayChatMessage(HDC hdc, int x, int y, int MaxLine)
{
	int cnt;
	int chatCnt;
	char *lpString;
	int start, end;

	if (ChatDispCnt == 0) return FALSE;

	if (ChatScrollPoint > CHATBUFF_MASK)
		ChatScrollPoint = CHATBUFF_MASK;
	if (ChatScrollPoint < 0)
		ChatScrollPoint = 0;

	start = ChatBuffCnt - MaxLine - ChatScrollPoint;
	end = ChatBuffCnt - ChatScrollPoint;

	for (cnt = start; cnt < end; cnt++)
	{
		if (cnt >= 0)
		{
			chatCnt = cnt & CHATBUFF_MASK;
			lpString = ChatBuff[chatCnt].szMessage;

			SetTextColor(hdc, RGB(0, 0, 0));
			dsTextLineOut(hdc, x + 1, y + 1, lpString, lstrlen(lpString));
			if (ChatBuff[chatCnt].dwIP == dwMyIP)
				SetTextColor(hdc, RGB(255, 128, 96));
			else
			{
				SetTextColor(hdc, RGB(255, 255, 128));

				switch (ChatBuff[chatCnt].dwIP)
				{
					case 1:
						SetTextColor(hdc, RGB(255, 128, 96));
						break;
					case 2:
						SetTextColor(hdc, RGB(96, 256, 255));
						break;
				}
			}
			dsTextLineOut(hdc, x, y, lpString, lstrlen(lpString));
		}
		y += 16;
	}

	return TRUE;
}







//250 , 60 ,342 , 70

//ÄÆÆ®ÇÒ ¶óÀÎ Ä«¿îÅÍ
int GetNextLineCount(char *szString, int LineMax)
{
	int cnt;
	int len;
	char *lp;

	len = lstrlen(szString);
	if (len <= LineMax) return 0;

	lp = szString;

	cnt = 0;

	while (1)
	{
		//2¹ÙÀÌÆ® ÄÚµå È®ÀÎ
		if (CheckCode_2Byte(lp) == 2)
		{
			cnt += 2;
			lp += 2;
		}
		else
		{
			cnt++;
			lp++;
		}

		if (cnt > (LineMax - 2)) break;
	}


	return cnt;
}

int SetIpColor(HDC hdc, DWORD dwIP)
{

	if (dwIP == dwMyIP)
	{
		SetTextColor(hdc, RGB(255, 128, 96));
		return TRUE;
	}

	switch (dwIP)
	{
		case 0:
			SetTextColor(hdc, RGB(255, 128, 96));				//¼­¹ö¸Þ¼¼Áö (ºÓÀº±Û¾¾)
			break;
		case 1:
			//		SetTextColor( hdc, RGB(255, 190, 150) );
			SetTextColor(hdc, RGB(150, 190, 255));			//±Ó¸»
			break;
		case 2:
			//		SetTextColor( hdc, RGB(255, 140, 120) );
			SetTextColor(hdc, RGB(150, 190, 255));			//
			break;
		case 3:
			SetTextColor(hdc, RGB(230, 160, 255));			//°øÁö
			break;
		case 4:
			SetTextColor(hdc, RGB(200, 255, 0));				//µ¿·áÃ¤ÆÃ
			break;
		case 5:
			SetTextColor(hdc, RGB(255, 255, 80));			//Å¬·£Ã¤ÆÃ
			break;
		case 6:
			SetTextColor(hdc, RGB(200, 255, 255));			//°Å·¡Ã¤ÆÃ
			break;

		case 9:
			SetTextColor(hdc, RGB(200, 128, 128));			//º¸Á¶ Á¤º¸Ã¢
			break;

		default:
			//		SetTextColor( hdc, RGB(180, 160, 255) );
			SetTextColor(hdc, RGB(255, 255, 192));
	}
	/*
		if ( dwIP>=0x010101 ) {
			//Custom Color
			SetTextColor( hdc, dwIP );
		}
	*/

	return TRUE;
}

static int	DisplayChatFilter = -1;
int	DispChatMsgHeight = 6;			//ÃÖ±Ù Ç¥½ÃµÈ Ã¤ÆÃÃ¢ ÁÙ ³ôÀÌ
int	DispChatMode = 0;

//Ã¤ÆÃÃ¢ ÇÊÅÍ¸µ
int	SetChatMsgFilter(int mode)
{

	DisplayChatFilter = -1;
	ChatScrollPoint = 0;

	switch (mode)
	{
		case 0:			//ÀüÃ¼
			DisplayChatFilter = -1;
			break;

		case 1:			//Å¬·£
			DisplayChatFilter = 5;
			break;

		case 2:			//µ¿·á
			DisplayChatFilter = 4;
			break;

		case 3:			//°Å·¡
			DisplayChatFilter = 6;
			break;

		case 4:			//±Ó¸»
			DisplayChatFilter = 1;
			break;
	}

	DispChatMode = mode;

	return TRUE;
}

int ChatBuffSort()
{
	int cnt, cnt2;
	int mCnt, nCnt;
	int	Col;

	if (ChatBuffCnt <= ChatBuffCompCnt) return FALSE;

	for (cnt = ChatBuffCompCnt; cnt < ChatBuffCnt; cnt++)
	{
		mCnt = cnt&CHATBUFF_MASK;
		Col = 0;
		switch (ChatBuff[mCnt].dwIP)
		{
			case 5:
				Col = 1;
				break;
			case 4:
				Col = 2;
				break;

			case 6:
				Col = 3;
				break;
			case 1:
				Col = 4;
				break;

			case 9:
				Col = 5;
				break;
		}
		nCnt = ChatBuffCnt2[Col] & CHATBUFF_MASK;
		memcpy(&ChatBuff2[Col][nCnt], &ChatBuff[mCnt], sizeof(CHATBUFF));
		ChatBuffCnt2[Col]++;

		if (Col > 0 && Col < 5)
		{
			nCnt = ChatBuffCnt2[0] & CHATBUFF_MASK;
			memcpy(&ChatBuff2[0][nCnt], &ChatBuff[mCnt], sizeof(CHATBUFF));
			ChatBuffCnt2[0]++;
		}

		if (Col == 0 && ChatBuff[mCnt].dwIP < 10)
		{
			for (cnt2 = 1; cnt2 < 5; cnt2++)
			{
				nCnt = ChatBuffCnt2[cnt2] & CHATBUFF_MASK;
				memcpy(&ChatBuff2[cnt2][nCnt], &ChatBuff[mCnt], sizeof(CHATBUFF));
				ChatBuffCnt2[cnt2]++;
			}
		}
	}

	ChatBuffCompCnt = ChatBuffCnt;

	return TRUE;
}
//Ã¤ÆÃ Á¤º¸ È­¸é¿¡ Ç¥½Ã ( º¸Á¶ )
int DisplayChatMessage3(HDC hdc, int x, int y, int StrMax, int LineMax, int Mode)
{
	int result;
	int ModeBackup = DispChatMode;
	int	ScrollBackup = ChatScrollPoint;

	DispChatMode = Mode;
	if (LineMax <= 6)
		ChatScrollPoint = 0;

	result = DisplayChatMessage2(hdc, x, y, StrMax, LineMax);

	DispChatMode = ModeBackup;
	ChatScrollPoint = ScrollBackup;
	return result;
}

//Ã¤ÆÃ Á¤º¸ È­¸é¿¡ Ç¥½Ã
int DisplayChatMessage2(HDC hdc, int x, int y, int StrMax, int LineMax)
{
	int cnt;
	int chatCnt;
	char *lpString;
	int LineCnt;
	int CutPos;
	int len;
	int start, end;
	int fl_Count;

	DWORD shColor = RGB(74, 74, 74);


	//if ( ChatDispCnt==0 ) return FALSE;
	ChatBuffSort();

	LineCnt = 0;

	if (ChatScrollPoint > (CHATBUFF_MAX - 32))
		ChatScrollPoint = (CHATBUFF_MAX - 32);
	if (ChatScrollPoint < 0)
		ChatScrollPoint = 0;

	fl_Count = DispChatMode;
	if (fl_Count < 0) fl_Count = 0;

	start = ChatBuffCnt2[fl_Count] - 1 - ChatScrollPoint;
	end = ChatBuffCnt2[fl_Count] - 32 - ChatScrollPoint;


	//	for( cnt=ChatBuffCnt-1;cnt>=ChatBuffCnt-32;cnt--) {

	DispChatMsgHeight = 0;

	for (cnt = start; cnt >= end; cnt--)
	{
		if (cnt >= 0)
		{
			chatCnt = cnt & CHATBUFF_MASK;

			//#ifdef _LANGUAGE_CHINESE //Áß±¹ Ã¤ÆÃÃ¢ Æ¯Á¤¾ð¾î »ç¿ë±ÝÁö
			//			lpString = ChatBuff2[fl_Count][chatCnt].szMessage;
			//			ConvertStringTHAI(lpString, lstrlen(lpString)+16);
			//#else
			lpString = ChatBuff2[fl_Count][chatCnt].szMessage;
			//#endif

						//if ( DisplayChatFilter<0 || DisplayChatFilter==ChatBuff[chatCnt].dwIP || ChatBuff[chatCnt].dwIP==0 || ChatBuff[chatCnt].dwIP==3 ) {


			DispChatMsgHeight++;			//³ôÀÌ ÃøÁ¤¿ë

			len = lstrlen(lpString);
			if (len > StrMax)
			{
				//ÄÆÆ®ÇÒ ¶óÀÎ Ä«¿îÅÍ
				CutPos = GetNextLineCount(lpString, StrMax);

				SetTextColor(hdc, shColor);
				dsTextLineOut(hdc, x - 1, y - 1, lpString + CutPos, len - CutPos);
				SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
				dsTextLineOut(hdc, x, y, lpString + CutPos, len - CutPos);

				y -= 12;
				LineCnt++;
				if (LineCnt >= LineMax) break;

#ifdef	_LANGUAGE_ENGLISH
				//¿µ¹® ÁÙ¹Ù²Þ½Ã '-' Ãß°¡
				char szStrBuff[256];

				memcpy(szStrBuff, lpString, CutPos);
				szStrBuff[CutPos] = 0;


				if (lpString[CutPos] > ' ' && lpString[CutPos + 1] > ' ')
				{
					szStrBuff[CutPos] = '-';
					szStrBuff[CutPos + 1] = 0;
				}

				len = lstrlen(szStrBuff);

				SetTextColor(hdc, shColor);
				dsTextLineOut(hdc, x - 1, y - 1, szStrBuff, len);
				SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
				dsTextLineOut(hdc, x, y, szStrBuff, len);
#else
#ifdef	_LANGUAGE_ARGENTINA
				// ¾Æ¸£ÇîÆ¼³ª ÁÙ¹Ù²Þ kyle
				char szStrBuff[256];

				memcpy(szStrBuff, lpString, CutPos);
				szStrBuff[CutPos] = 0;


				if (lpString[CutPos] > ' ' && lpString[CutPos + 1] > ' ')
				{
					szStrBuff[CutPos] = '-';
					szStrBuff[CutPos + 1] = 0;
				}

				len = lstrlen(szStrBuff);

				SetTextColor(hdc, shColor);
				dsTextLineOut(hdc, x - 1, y - 1, szStrBuff, len);
				SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
				dsTextLineOut(hdc, x, y, szStrBuff, len);
#else
				SetTextColor(hdc, shColor);
				dsTextLineOut(hdc, x - 1, y - 1, lpString, CutPos);
				SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
#ifdef _LANGUAGE_BRAZIL
				char semi[MAX_PATH], presemi[MAX_PATH], backsemi[MAX_PATH];
				ZeroMemory(presemi, MAX_PATH);
				ZeroMemory(backsemi, MAX_PATH);
				bool bsemi = 0;
				strcpy_s(semi, lpString);
				int k = 0, prelen = 0, lenth = 0;
				lenth = strlen(semi);
				//¹®ÀÚ¿­ Â÷·Ê´ë·Î °Ë»ç
				for (int i = 0; i < lenth; i++)
				{
					backsemi[k] = semi[i];
					k++;
					if ((bsemi == 0) && (semi[i] == ':'))
					{
						strcpy_s(presemi, backsemi);
						ZeroMemory(backsemi, MAX_PATH);
						bsemi++;
						k = 0;
					}
				}
				//:ÀÖ¾úÀ¸¸é »ö±òÀ» ¹Ù²ÙÀÚ
				if (bsemi)
				{
					SIZE size;
					//:¾Õ¹®ÀÚ¿­
					prelen = strlen(presemi);
					SetTextColor(hdc, RGB(100, 255, 50));
					dsTextLineOut(hdc, x, y, presemi, prelen);
					GetTextExtentPoint(hdc, presemi, prelen, &size);
					//µÚ¹®ÀÚ¿­
					int j = 0;
					j = x + size.cx;
					SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
					dsTextLineOut(hdc, j, y, backsemi, CutPos - prelen);
				}
				else dsTextLineOut(hdc, x, y, lpString, CutPos);
#else
				dsTextLineOut(hdc, x, y, lpString, CutPos);
#endif
#endif
#endif

				y -= 12;
				LineCnt++;
				if (LineCnt >= LineMax) break;

			}
			else
			{

				SetTextColor(hdc, shColor);
				dsTextLineOut(hdc, x - 1, y - 1, lpString, len);
				SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
#ifdef _LANGUAGE_BRAZIL
				char semi[MAX_PATH], presemi[MAX_PATH], backsemi[MAX_PATH];
				ZeroMemory(presemi, MAX_PATH);
				ZeroMemory(backsemi, MAX_PATH);
				bool bsemi = 0;
				strcpy_s(semi, lpString);
				int k = 0, prelen = 0, lenth = 0;
				lenth = strlen(semi);
				//¹®ÀÚ¿­ Â÷·Ê´ë·Î °Ë»ç
				for (int i = 0; i < lenth; i++)
				{
					backsemi[k] = semi[i];
					k++;
					if ((bsemi == 0) && (semi[i] == ':'))
					{
						strcpy_s(presemi, backsemi);
						ZeroMemory(backsemi, MAX_PATH);
						bsemi++;
						k = 0;
					}
				}
				//:ÀÖ¾úÀ¸¸é »ö±òÀ» ¹Ù²ÙÀÚ
				if (bsemi)
				{
					SIZE size;
					//:¾Õ¹®ÀÚ¿­
					prelen = strlen(presemi);
					SetTextColor(hdc, RGB(100, 255, 50));
					dsTextLineOut(hdc, x, y, presemi, prelen);
					GetTextExtentPoint(hdc, presemi, prelen, &size);
					//µÚ¹®ÀÚ¿­
					int j = 0;
					j = x + size.cx;
					SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
					dsTextLineOut(hdc, j, y, backsemi, strlen(backsemi));
				}
				else dsTextLineOut(hdc, x, y, lpString, len);
#else
				dsTextLineOut(hdc, x, y, lpString, len);
#endif

				y -= 12;
				LineCnt++;
				if (LineCnt >= LineMax) break;
			}
			//}
		}
	}

	return TRUE;
}


//»õ ÇÃ·¹ÀÌ¾î¿Í ¿¬°á
int ConnectPlayer(smWINSOCK *lpsmSock)
{
	int OthPlayerCnt;

	if (ServerMode)
		return Serv_ConnectPlayer(lpsmSock);


	//Å©¸®Æ¼Ä® ¼½¼Ç ¼±¾ð
	EnterCriticalSection(&cSection);

	OthPlayerCnt = FindNewOtherPlayer();
	if (OthPlayerCnt >= 0)
	{
		lpsmSock->ExtData1 = (void *)&chrOtherPlayer[OthPlayerCnt];
		chrOtherPlayer[OthPlayerCnt].Init();
		chrOtherPlayer[OthPlayerCnt].TransSendWait = 1;
		chrOtherPlayer[OthPlayerCnt].TransSock = lpsmSock;
		chrOtherPlayer[OthPlayerCnt].TransLastSendCnt = PlayCounter - 70;
		chrOtherPlayer[OthPlayerCnt].Flag = 1;
		chrOtherPlayer[OthPlayerCnt].smCharInfo.szName[0] = 0;
		chrOtherPlayer[OthPlayerCnt].ActionPattern = 99;
		chrOtherPlayer[OthPlayerCnt].Pattern = 0;
		chrOtherPlayer[OthPlayerCnt].AutoPlayer = 0;

		lpCurPlayer->SendCharInfo(lpsmSock);
		//		SendPlayerInfo( lpsmSock );

	}

	//Å©¸®Æ¼Ä® ¼½¼Ç ÇØÁ¦
	LeaveCriticalSection(&cSection);

	return TRUE;
}

//ÇÃ·¹ÀÌ¾î ¿¬°á ²ö¾îÁü
int DisconnectPlayer(smWINSOCK *lpsmSock)
{
	//	int cnt;
	smCHAR *lpPlayer;
	DWORD	dwTime;

	dwTime = GetCurrentTime();

	if (ServerMode)
		return Serv_DisconnectPlayer(lpsmSock);

	if (smWsockServer && lpsmSock->sock == smWsockServer->sock)
	{
		AddChatBuff("¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù");
		if (smWsockServer == smWsockDataServer)
			smWsockDataServer = 0;
		if (smWsockServer == smWsockUserServer)
			smWsockUserServer = 0;
		if (smWsockServer == smWsockExtendServer)
			smWsockExtendServer = 0;

		smWsockServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 1;
		//°Å·¡ÁßÀÌ¸é °Å·¡ Ãë¼Ò ½ÃÅ´
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);			//°Å·¡ Ãë¼Ò ¿ä±¸
			cTrade.CancelTradeItem();								//°Å·¡Ã¢ ´Ý±â
		}
		return TRUE;
	}

	if (smWsockDataServer && lpsmSock->sock == smWsockDataServer->sock)
	{
		AddChatBuff("µ¥ÀÌÅ¸ ¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù");
		smWsockDataServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 2;

		//°Å·¡ÁßÀÌ¸é °Å·¡ Ãë¼Ò ½ÃÅ´
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);			//°Å·¡ Ãë¼Ò ¿ä±¸
			cTrade.CancelTradeItem();								//°Å·¡Ã¢ ´Ý±â
		}
		return TRUE;
	}

	if (smWsockUserServer && lpsmSock->sock == smWsockUserServer->sock)
	{
		AddChatBuff("¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù");
		smWsockUserServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 3;
		//°Å·¡ÁßÀÌ¸é °Å·¡ Ãë¼Ò ½ÃÅ´
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);			//°Å·¡ Ãë¼Ò ¿ä±¸
			cTrade.CancelTradeItem();					//°Å·¡Ã¢ ´Ý±â
		}
		return TRUE;
	}

	if (smWsockExtendServer && lpsmSock->sock == smWsockExtendServer->sock)
	{
		AddChatBuff("¼­¹ö¿ÍÀÇ ¿¬°áÀÌ ²÷¾î Á³½À´Ï´Ù");
		smWsockExtendServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 3;
		//°Å·¡ÁßÀÌ¸é °Å·¡ Ãë¼Ò ½ÃÅ´
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);		//°Å·¡ Ãë¼Ò ¿ä±¸
			cTrade.CancelTradeItem();							//°Å·¡Ã¢ ´Ý±â
		}
		return TRUE;

	}

	char szBuff[256];

	//Áö¿¬ ¼­¹ö ¿¬°á ²ö¾îÁü
	if (AreaServerMode)
	{
		if (lpWSockServer_Area[0] && lpWSockServer_Area[0]->sock == lpsmSock->sock)
		{
			if (smConfig.DebugMode)
			{
				wsprintf(szBuff, "Disconnect Area Server (%s)", lpWSockServer_Area[0]->szIPAddr);
				AddChatBuff(szBuff, 0);
			}
			if (lpWSockServer_Area[0] == lpWSockServer_DispArea[0]) lpWSockServer_DispArea[0] = 0;
			if (lpWSockServer_Area[0] == lpWSockServer_DispArea[1]) lpWSockServer_DispArea[1] = 0;

			lpWSockServer_Area[0] = 0;
			return TRUE;
		}
		if (lpWSockServer_Area[1] && lpWSockServer_Area[1]->sock == lpsmSock->sock)
		{
			if (smConfig.DebugMode)
			{
				wsprintf(szBuff, "Disconnect Area Server (%s)", lpWSockServer_Area[1]->szIPAddr);
				AddChatBuff(szBuff, 0);
			}

			if (lpWSockServer_Area[1] == lpWSockServer_DispArea[0]) lpWSockServer_DispArea[0] = 0;
			if (lpWSockServer_Area[1] == lpWSockServer_DispArea[1]) lpWSockServer_DispArea[1] = 0;

			lpWSockServer_Area[1] = 0;
			return TRUE;
		}
	}



	//Å©¸®Æ¼Ä® ¼½¼Ç ¼±¾ð
	EnterCriticalSection(&cSection);

	lpPlayer = (smCHAR *)lpsmSock->ExtData1;
	if (lpPlayer)
	{
		lpPlayer->Close();
	}

	//Å©¸®Æ¼Ä® ¼½¼Ç ÇØÁ¦
	LeaveCriticalSection(&cSection);

	return TRUE;
}



//¸¶Áö¸·À¸·Î ¼­¹ö¿¡ µ¥ÀÌÅ¸¸¦ º¸³½ Ä«¿îÅÍ
int Ts_LastSendCounter;
DWORD	dwLastSendPosiTime = 0;		//¸¶Áö¸· Àü¼Û ½Ã°£ (À§Ä¡)
DWORD	dwLastSendPlayTime = 0;		//¸¶Áö¸· Àü¼Û ½Ã°£ (¸ð¼Çµ¥ÀÌÅ¸)

//¼­¹ö·Î ÇÃ·¹ÀÌ¾îÀÇ µ¥ÀÌÅ¸¸¦ À§Ä¡¸¦ Àü¼Û
int SendPlayDataToServer()
{
	Log::Debug("SendPlayDataToServer : Requisitado porém foi removido.");
	return NULL;
}

//´Ù¸¥ À¯Àú¿Í ¿¬°á ½ÃÅ²´Ù
int ConnectOtherPlayer(DWORD dwIP)
{
	smWINSOCK *lpsmSock;
	int OthPlayerCnt;

	OthPlayerCnt = FindNewOtherPlayer();
	if (OthPlayerCnt >= 0)
	{
		lpsmSock = smConnectSock2(ConvStrIP(dwIP), TCP_GAMEPORT);
		if (lpsmSock)
		{
			ConnectPlayer(lpsmSock);
			return TRUE;
		}
	}
	return FALSE;
}
//´Ù¸¥ ÇÃ·¹ÀÌ¾î¿ÍÀÇ ¿¬°á ¾²·¹µå
DWORD WINAPI ConnectPlayThreadProc(void *pInfo)
{

	ConnectOtherPlayer((DWORD)pInfo);

	ExitThread(TRUE);
	return TRUE;
}
//´Ù¸¥ ÇÃ·¹ÀÌ¾î¿ÍÀÇ ¿¬°á Á¾·á ¾²·¹µå
DWORD WINAPI DisconnectPlayThreadProc(void *pInfo)
{

	((smWINSOCK *)pInfo)->CloseSocket();

	ExitThread(TRUE);
	return TRUE;
}



//´Ù¸¥ ÇÃ·¹ÀÌ¾î¿ÍÀÇ ¿¬°á ¾²·¹µå¸¦ »ý¼º
int CreateConnectThread(DWORD dwIP)
{
	DWORD dwMsgId;
	CreateThread(NULL, 0, ConnectPlayThreadProc, (void *)dwIP, 0, &dwMsgId);

	return TRUE;
}

//´Ù¸¥ ÇÃ·¹ÀÌ¾î¿ÍÀÇ ¿¬°áÁ¾·á ¾²·¹µå »ý¼º
int CreateDisconnectThread(smWINSOCK *lpsmSock)
{
	DWORD dwMsgId;
	CreateThread(NULL, 0, DisconnectPlayThreadProc, (void *)lpsmSock, 0, &dwMsgId);

	return TRUE;
}


//°Å¸®°¡ ¸Õ ´Ù¸¥ À¯ÀúµéÀÇ ¿¬°áÀ» ²ö´Â´Ù
int DisconnectFarPlayer()
{
	int mx, mz, dDist;
	int cnt;
	int timeover;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		//°Å¸®¸¦ È®ÀÎÇÏ¿© ¸Õ °æ¿ì¿¡´Â ¿¬°áÀ» ²ö¾î¹ö¸°´Ù
		if (chrOtherPlayer[cnt].Flag)
		{

			if ((chrOtherPlayer[cnt].dwLastTransTime + DIS_TIME_OVER) < dwPlayTime)
				timeover = TRUE;
			else
				timeover = FALSE;

			//°Å¸®°è»ê
			mx = (lpCurPlayer->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
			mz = (lpCurPlayer->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;
			dDist = mx*mx + mz*mz;

			if ((chrOtherPlayer[cnt].DisplayFlag && dDist > DIST_TRANSLEVEL_DISCONNECT) || timeover)
			{
				//¿¬°á Á¾·á/ ¼ÒÄÏ ´ÝÀ½
				chrOtherPlayer[cnt].Flag = 0;

				if (chrOtherPlayer[cnt].TransSock)
				{
					chrOtherPlayer[cnt].TransSock->CloseSocket();
				}
				else
				{
					//Å©¸®Æ¼Ä® ¼½¼Ç ¼±¾ð
					EnterCriticalSection(&cSection);
					chrOtherPlayer[cnt].Close();
					//Å©¸®Æ¼Ä® ¼½¼Ç ÇØÁ¦
					LeaveCriticalSection(&cSection);
				}
			}
		}
	}

	return TRUE;
}

DWORD	dwExtendServ_RecvTime = 0;
DWORD	dwUserServ_RecvTime = 0;
DWORD	dwMainServ_RecvTime = 0;
DWORD	dwDataServ_RecvTime = 0;

//µ¥ÀÌÅ¸ ÀÔ¼ö
int RecvPlayData(smTHREADSOCK *pData)
{

	DWORD *IPData;
	//smCHAR *lpPlayer;

	DWORD	dwTime;

	//Å©¸®Æ¼Ä® ¼½¼Ç ¼±¾ð
	EnterCriticalSection(&cSection);


	IPData = (DWORD *)(pData->Buff);


	/*
		if ( pData->smMySock==smWsockServer ||
			 pData->smMySock==smWsockUserServer ||
			 pData->smMySock==smWsockDataServer ||
			  pData->smMySock==smWsockExtendServer ) {
	*/
	dwTime = GetCurrentTime();

	if (pData->smMySock == smWsockDataServer)
	{
		dwRecvDataServerTime = dwTime;
		dwDataServ_RecvTime = dwTime;
	}
	if (pData->smMySock == smWsockUserServer)
	{
		dwRecvUserServerTime = dwTime;
		dwUserServ_RecvTime = dwTime;
		Debug_RecvCount2++;
	}
	if (pData->smMySock == smWsockExtendServer)
	{
		dwRecvExtendServerTime = dwTime;
		dwExtendServ_RecvTime = dwTime;
		Debug_RecvCount3++;
	}
	if (pData->smMySock == smWsockServer)
	{
		Debug_RecvCount1++;
		dwRecvServerTime = dwTime;
		dwMainServ_RecvTime = dwTime;
	}

	//¼­¹öÃø µ¥ÀÌÅ¸ Ã³¸®
	rsMainServer.RecvMessage(pData);
	/*
		}
		else {
			//Å¬¶óÀÌ¾ðÆ®¿ÍÀÇ ±³½Å..

			lpPlayer = (smCHAR *)pData->smMySock->ExtData1;
			if ( lpPlayer ) lpPlayer->RecvPlayData( pData );
		}
	*/
	//Å©¸®Æ¼Ä® ¼½¼Ç ÇØÁ¦
	LeaveCriticalSection(&cSection);

	return TRUE;
}

//¸Þ¼¼Áö Å¥ÀÇ ¸Þ¼¼Áö¸¦ Ã³¸®
int PlayRecvMessageQue()
{
	return rsMainServer.RecvMessageQue();
}


int LastSendCnt;
/*
int SendPlayData( smCHAR *player )
{
	int cnt;

	for(cnt=0;cnt<OTHER_PLAYER_MAX;cnt++) {
		if ( chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].Pattern
			&& chrOtherPlayer[cnt].AutoPlayer==0 ) {
			chrOtherPlayer[cnt].SendPlayData( player );
		}
	}

	return TRUE;
}
*/
#define	NET_SENDING_WAIT_TIME		400
#define	NET_SENDING_WAIT_TIME2		1000

int NetStandCnt = 0;
extern int GameMode;

DWORD	NetScoopTime = 2000;

//³Ý ÇÃ·¹ÀÌ ¸ÞÀÎ
int NetWorkPlay()
{
	//	char *szIP;
	int cnt;
	DWORD	dwTime;
	DWORD	dwTime2;
	smWINSOCK	*lpsmSock;

	//¹ÚÀç¿ø - XignCode
#ifdef _XIGNCODE_CLIENT
	ZCWAVE_Init();
#endif

	if (NetWorkInitFlag == 0)
	{
		if (!smWsockServer || !smWsockDataServer || !smWsockUserServer || !smWsockExtendServer)
		{
			if (ConnectServer_Main() == NULL)
				return FALSE;
			//smWsockServer = smConnectSock( szServIP , dwServPort );
		}

		//Sleep( 60*1000*3 );		//3ºÐ ´ë±â

		NetWorkInitFlag = TRUE;

		rsMainServer.Init(smWsockServer);

		//¼­¹ö·Î ÇÃ·¹ÀÌ¾îÀÇ µ¥ÀÌÅ¸¸¦ À§Ä¡¸¦ Àü¼Û
		SendPlayDataToServer();

		//¼­¹ö¿¡ ±ÇÇÑ ¼³Á¤
		if (smConfig.DebugMode) SendAdminMode(TRUE);


		//		if ( smWsockServer )
		//			lpCurPlayer->SendCharInfo( smWsockServer );			//ÇÃ·¹ÀÌ¾î Á¤º¸ Àü¼Û

				//ÀúÀåµÈ Ä³¸¯ÅÍ µ¥ÀÌÅ¸ ºÒ·¯¿À±â ¿ä±¸
		if (smWsockDataServer)
		{

			SendGetRecordData(lpCurPlayer->smCharInfo.szName);

			//¼­¹ö IP Ã½Å© ( ±º¼­¹ö¿¡ Á¢¼ÓÇÑ IPÅëº¸ )
			if (smWsockServer && smWsockDataServer != smWsockServer)
			{
				SendCheckIP(1, smWsockServer);
			}
			if (smWsockUserServer && smWsockDataServer != smWsockUserServer)
			{
				SendCheckIP(2, smWsockUserServer);
			}
			if (smWsockExtendServer && smWsockExtendServer != smWsockServer && smWsockDataServer != smWsockExtendServer)
			{
				SendCheckIP(3, smWsockExtendServer);
			}
		}

		ChatBuffCnt = 0;
		ChatDispCnt = 0;

		Ts_LastSendCounter = PlayCounter;
		TransThrowItem.code = 0;			//¾ÆÀÌÅÛ ¹ö¸®±â ÃÊ±âÈ­

		RecordFailCount = 0;			//ÀúÀå ½ÇÆÐ Ä«¿îÅÍ
		dwLastWareHouseChkSum = 0;		//Ã¢°í Ã¼Å©¼¶ ÄÚµå

		TransAgingItemFlag = 0;			//¿¡ÀÌÂ¡ ¿Ï·á ¾ÆÀÌÅÛ Àü¼Û ÇÃ·¢


		ZeroMemory(&TransRecvItem, sizeof(TRANS_ITEMINFO));

		Trans_SplashCharList.code = 0;	//½ºÇÃ·¹½Ã °ø°Ý ¹öÆÛ ÃÊ±âÈ­

		ZeroMemory(&PK_FieldState, sizeof(PK_FIELD_STATE));	//PK ±¸Á¶ ÃÊ±âÈ­
		PK_FieldState.FieldCode = -1;


		//¸ó½ºÅÍ ÄÚµå Å¥¿¡ °ü¸®
		ClearAttMonsterCode();

		Init_RecordDamage();			//°ø°Ý·Â ±â·Ï ÃÊ±âÈ­

		switch (smConfig.NetworkQuality)
		{
			case 0:
				NetScoopTime = 3000;
				break;
			case 1:
				NetScoopTime = 2000;
				break;
			case 2:
				NetScoopTime = 1000;
				break;
			case 3:
				NetScoopTime = 5000;
				smTransTurbRcvMode = 50;
				break;
		}

	}
	else
	{

		if (cTrade.OpenFlag)
		{
			//¾ÆÀÌÅÛ ±³È¯½Ã ¿Ï·á È®ÀÎ
			if (!TradeSendSucessFlag && sTrade.CheckFlag && sTradeRecv.CheckFlag)
			{
				SendTradeCheckItem(cTrade.TradeCharCode);
				TradeSendSucessFlag = TRUE;
			}
		}
		if (dwTradeMaskTime)
		{
			//Æ®·¹ÀÌµå ¸¶½ºÅ© Å¸ÀÌ¸Ó ÃÊ±âÈ­
			if (dwTradeMaskTime<dwPlayTime) dwTradeMaskTime = 0;
		}

		dwTime = dwPlayTime - dwLastSendPosiTime;
		dwTime2 = dwPlayTime - dwRecvServerTime;

		//¼­¹ö·ÎÀÇ µ¥ÀÌÅ¸ Àü¼Û
		if ((PlayCounter - Ts_LastSendCounter)>PLAYSERVER_SENDCOUNTER)
		{//&& dwTime>NET_SENDING_WAIT_TIME &&
//			dwTime2<NET_SENDING_WAIT_TIME ) {
			//¼­¹ö·Î ÇÃ·¹ÀÌ¾îÀÇ µ¥ÀÌÅ¸¸¦ À§Ä¡¸¦ Àü¼Û
			SendPlayDataToServer();
			//°Å¸®°¡ ¸Õ ´Ù¸¥ À¯ÀúµéÀÇ ¿¬°áÀ» ²ö´Â´Ù
			DisconnectFarPlayer();

			//Ä«¿îÅÍ Áõ°¡
			Ts_LastSendCounter = PlayCounter;
			dwLastSendPosiTime = dwPlayTime;
		}

		//		if ( (xxcnt&0x3)==3 ) SendPlayData( lpCurPlayer );

				//´Ù¸¥ À¯Àúµé°úÀÇ µ¥ÀÌÅ¸ ±³È¯
				//SendPlayData( lpCurPlayer );

		if (smWsockUserServer)
		{// && rsRecvUserServer ) {
//ÇÃ·¹ÀÌ¾î µ¥ÀÌÅ¸¸¦ ¼­¹ö·Î Àü¼Û (40/70) ÃÊ´ÜÀ§ ±¸ºÐ 
//if ( (lpCurPlayer->PlayBuffCnt&0x3F)==0 ) {

			dwTime = dwPlayTime - dwLastSendPlayTime;
			dwTime2 = dwPlayTime - dwRecvUserServerTime;


			if ((lpCurPlayer->PlayBuffCnt & 0x3F) == 0 && lpCurPlayer->dwObjectSerial &&
				dwTime > NET_SENDING_WAIT_TIME)
			{//&& dwTime2<NET_SENDING_WAIT_TIME2 ) {

				if (smConfig.DebugMode && AdminCharDisable)
				{
					if (smWsockServer == smWsockUserServer)
					{
						rsMainServer.SendPlayData(lpCurPlayer);
					}
				}
				else
				{
					if (AreaServerMode)
					{
						if (lpCurPlayer->OnStageField >= 0)
						{
							lpsmSock = lpWSockServer_DispArea[lpCurPlayer->OnStageField];
						}

					}
					else
					{
						lpsmSock = smWsockUserServer;
					}

					if (lpsmSock)
					{
						lpCurPlayer->MakeTransPlayData(lpCurPlayer->srTransBuff, 0x40);
						if (((smTRANS_PLAYDATA *)lpCurPlayer->srTransBuff)->PlayBuffCnt > 1)
						{
							//for(int cnt=0;cnt<40;cnt++) 
							lpsmSock->Send(lpCurPlayer->srTransBuff, ((smTRANS_PLAYDATA *)lpCurPlayer->srTransBuff)->size, TRUE);
							NetStandCnt = 0;
						}
						else
						{
							//¼­ÀÖ´Â µ¿ÀÛ ³×Æ®¿÷ ºÎÇÏ ÃÖ¼Ò ( ÇÑ¹ø Àü¼ÛÈÄ ³×Æ®Ÿp ½½¸³ )
							//if ( NetStandCnt==0 )
							//for(int cnt=0;cnt<40;cnt++) 
							lpsmSock->Send(lpCurPlayer->srTransBuff, ((smTRANS_PLAYDATA *)lpCurPlayer->srTransBuff)->size, TRUE);
							NetStandCnt++;
						}
					}
				}

				Debug_SendCount++;

				rsRecvUserServer = FALSE;
				dwLastSendPlayTime = dwPlayTime;
			}
		}

		//ÇöÀç °ÔÀÓ ·Îµù Áß
		if (dwLoadingTime)
		{
			dwTime = GetCurrentTime();
			if (dwTime > (dwLoadingTime + NET_LOADING_WAIT_LIMIT))
			{
				//½Ã°£ ÃÊ°ú Á¾·á
				DisconnectFlag = GetCurrentTime();
				dwLoadingTime = 0;
			}
		}


		cnt = PlayCounter & 0x1FF;
		if (cnt == 0)
		{
			//µ¥µå¶ô Ã½Å© ( ¸Þ¼¼Áö°¡ µ¹¾Æ ¿ÀÁö ¾ÊÀ¸¸é ³×Æ®¿÷ ´Ù¿î )
			TransCheckNetwork();
		}
		else if (cnt == 0x80)
		{
			//ÇÁ·Î¼¼½º ½Ã°£ °¨½ÃÁ¤º¸ ¼­¹ö·Î ¼ÛºÎ
			SendPlayTimerMax();
		}



		if ((PlayCounter & 0x1F) == 0)
		{
			dwTime = GetCurrentTime();
			if (GameMode == 2 && dwMainServ_RecvTime && smWsockServer && (dwTime - dwMainServ_RecvTime) > 3000)
			{

				if (smWsockServer->WaitReadMessage >= 0)
				{
					smWsockServer->WaitReadMessage += 24;
					if (smConfig.DebugMode)
						AddChatBuff("Scoop Recv Route(Main)");
				}
				else
				{
					if (smConfig.DebugMode)
						AddChatBuff("Error Server(Main)");
				}
			}
			if (GameMode == 2 && smWsockUserServer && dwUserServ_RecvTime &&
				smWsockServer != smWsockUserServer && (dwTime - dwUserServ_RecvTime) > 3000)
			{
				//dwUserServ_RecvTime = dwTime;
				if (smWsockUserServer->WaitReadMessage >= 0)
				{
					smWsockUserServer->WaitReadMessage += 24;
					if (smConfig.DebugMode)
						AddChatBuff("Scoop Recv Route(User)");
				}
				else
				{
					if (smConfig.DebugMode)
						AddChatBuff("Error Server(User)");
				}
			}
			if (GameMode == 2 && smWsockExtendServer && dwExtendServ_RecvTime &&
				smWsockServer != smWsockExtendServer && (dwTime - dwExtendServ_RecvTime) > 3000)
			{
				//dwUserServ_RecvTime = dwTime;
				if (smWsockExtendServer->WaitReadMessage >= 0)
				{
					smWsockExtendServer->WaitReadMessage += 24;
					if (smConfig.DebugMode)
						AddChatBuff("Scoop Recv Route(Extend)");
				}
				else
				{
					if (smConfig.DebugMode)
						AddChatBuff("Error Server(Extend)");
				}
			}

			//			if ( GameMode==2 && dwLoadingTime && 
			if (GameMode == 2 && ((dwTime - dwDataServ_RecvTime) > 7000 || dwLoadingTime) &&
				smWsockDataServer && smWsockDataServer != smWsockServer)
			{
				smWsockDataServer->WaitReadMessage += 24;
				if (smConfig.DebugMode && DisplayDebug)
					AddChatBuff("Scoop Recv Route(Data)");
			}

			if (GameMode == 2 && AreaServerMode)
			{
				if (lpWSockServer_Area[0] && lpWSockServer_Area[0]->dwLastRecvTime)
				{
					if ((dwTime - lpWSockServer_Area[0]->dwLastRecvTime) > 2000)
					{
						lpWSockServer_Area[0]->WaitReadMessage += 24;
						smCheckWaitMessage(lpWSockServer_Area[0]);
					}
				}
				if (lpWSockServer_Area[1] && lpWSockServer_Area[1]->dwLastRecvTime)
				{
					if ((dwTime - lpWSockServer_Area[1]->dwLastRecvTime) > 2000)
					{
						lpWSockServer_Area[1]->WaitReadMessage += 24;
						smCheckWaitMessage(lpWSockServer_Area[1]);
					}
				}

				/*
					if ( AreaServerMode ) {
						if ( lpWSockServer_Area[0] ) lpWSockServer_Area[0]->Send2( (char *)&TransPlayerInfo , TransPlayerInfo.size , TRUE );
						if ( lpWSockServer_Area[1] ) lpWSockServer_Area[1]->Send2( (char *)&TransPlayerInfo , TransPlayerInfo.size , TRUE );
					}
				*/

			}

			//dwLastRecvTime


		}

		/*
			for(cnt=start;cnt<CONNECTMAX;cnt+=0x40 ) {
				if ( rsPlayInfo[cnt].lpsmSock && rsPlayInfo[cnt].dwObjectSerial &&//) {
					(dwPlayServTime-rsPlayInfo[cnt].dwLastRecvTime)<5000 ) {
					rsPlayInfo[cnt].lpsmSock->WaitReadMessage++;
				}
			}
		*/
	}
#ifndef _LANGUAGE_CHINESE	//À©µµ¿ì ¸ðµå
#ifndef _LANGUAGE_VEITNAM	//À©µµ¿ì ¸ðµå
#ifndef _LANGUAGE_PHILIPIN	//À©µµ¿ì ¸ðµå
#ifndef _LANGUAGE_ENGLISH	//À©µµ¿ì ¸ðµå
	HWND	hWnd;

	if ((PlayCounter & 0x1FF) == 0 && !smConfig.DebugMode)
	{
		hWnd = WinFocusCrack();
		//À©µµ¿ì Æ÷Ä¿½º¸¦ °¨½Ã
		if (hWnd)
		{
			//Å©·¢ À©µµ¿ì ½Å°í
			SendCrackWindow(hWnd);
		}
	}
#endif
#endif
#endif
#endif


	//°Å·¡ÈÄ È®ÀÎ»ï¾Æ Àç½Ãµµ ( 0.8ÃÊÈÄ )
	if (dwLastTransTradeKeyTime && (dwLastTransTradeKeyTime + 800) < dwPlayTime)
	{

		if (smWsockDataServer)
			smWsockDataServer->Send2((char *)&LastTransTradeItemKey, LastTransTradeItemKey.size, TRUE);

		dwLastTransTradeKeyTime = 0;
	}


	if (ChatDispCnt > 0) ChatDispCnt--;

	return TRUE;
}


//¾ÆÀÌÅÛ ¹ö¸®±â
int ThrowPutItem(sITEM *lpItem, int x, int y, int z)
{

	//¼­¹ö »óÅÂ¿¡ ¹®Á¦ ¹ß»ý½Ã ¾ÆÀÌÅÛ ¹ö¸®±â ±ÝÁö ( 4ÃÊ ÀÌ»ó ¼ö½ÅÀÌ ¾ø´Â »óÅÂ )
	if ((dwRecvServerTime + 4000) < dwPlayTime || QuitSave || TransThrowItem.code)
		return FALSE;

	if (AdminCharDisable || WarningHack || !smWsockDataServer || !smWsockServer) return FALSE;

	//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
	if (!SendSaveThrowItem2(&lpItem->sItemInfo))
		return FALSE;

	TransThrowItem.code = smTRANSCODE_THROWITEM;
	TransThrowItem.size = sizeof(TRANS_ITEMINFO);
	TransThrowItem.x = x;
	TransThrowItem.y = y;
	TransThrowItem.z = z;

	memcpy(&TransThrowItem.Item, &lpItem->sItemInfo, sizeof(sITEMINFO));

	return TRUE;
}
//¾ÆÀÌÅÛ ¹ö¸®±â
int ThrowPutItem2(sITEMINFO *lpItem, int x, int y, int z)
{

	//¼­¹ö »óÅÂ¿¡ ¹®Á¦ ¹ß»ý½Ã ¾ÆÀÌÅÛ ¹ö¸®±â ±ÝÁö ( 4ÃÊ ÀÌ»ó ¼ö½ÅÀÌ ¾ø´Â »óÅÂ )
	if ((dwRecvServerTime + 4000) < dwPlayTime || QuitSave || TransThrowItem.code)
		return FALSE;

	if (AdminCharDisable || WarningHack || !smWsockDataServer || !smWsockServer) return FALSE;

	//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
	if (!SendSaveThrowItem2(lpItem))
		return FALSE;

	TransThrowItem.code = smTRANSCODE_THROWITEM;
	TransThrowItem.size = sizeof(TRANS_ITEMINFO);
	TransThrowItem.x = x;
	TransThrowItem.y = y;
	TransThrowItem.z = z;

	memcpy(&TransThrowItem.Item, lpItem, sizeof(sITEMINFO));

	/*
		if ( lpItem->CODE==(sinGG1|sin01) ) {
			SendSaveMoney();
		}
	*/
	//return smWsockServer->Send( (char *)&TransItemInfo , TransItemInfo.size , TRUE );

	return TRUE;
}


//¹ö¸± ¾ÆÀÌÅÛ ÀúÀå
//TRANS_ITEMINFO	TransThrowItem;
//			ThrowItemToServer((TRANS_ITEMINFO *)pData->Buff );

int	ThrowPotionCount = 0;

//ÀÓ½Ã ÀúÀåµÈ ¹ö¸± ¾ÆÀÌÅÛÀ» È®ÀÎÇÏ¿© ÀÏÄ¡ÇÏ¸é ¼­¹ö·Î º¸³¿
int ThrowItemToServer(smTRANS_COMMAND_EX *lpTransCommand)
{
	Log::Debug("ThrowItemToServer : Requisitado porém foi removido.");
	return NULL;
}

//¸ó½ºÅÍ ½ÃÀÛ ÁöÁ¡ Ãß°¡
int SendAddStartPoint(int x, int z)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_ADD_STARTPOSI;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.LParam = x;
	TransCommand.WParam = z;
	TransCommand.SParam = TRUE;

	smWINSOCK *lpsmSock;
	lpsmSock = GetAreaServerSock();		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return NULL;
}

//¸ó½ºÅÍ ½ÃÀÛÁöÁ¡ Á¦°Å
int SendDeleteStartPoint(int x, int z)
{
	smTRANS_COMMAND	TransCommand;
	scITEM	*lpItem;
	smWINSOCK *lpsmSock;


	TransCommand.code = smTRANSCODE_DEL_STARTPOSI;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.LParam = x;
	TransCommand.WParam = z;
	TransCommand.SParam = TRUE;

	//¶³¾îÁø ¾ÆÀÌÅÛ Ã£±â
	lpItem = FindScItem(x, z);
	if (lpItem)
	{
		lpsmSock = GetServerSock(lpItem->ServerCode);		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
		lpItem->Close();
	}

	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return NULL;
}



//NPC Ä³¸¯ÅÍ Ãß°¡
int SendAdd_Npc(smCHAR *lpCharSample, int state)
{
	smTRNAS_PLAYERINFO	smTransPlayerInfo;

	smTransPlayerInfo.code = smTRANSCODE_ADD_NPC;
	smTransPlayerInfo.size = sizeof(smTRNAS_PLAYERINFO);
	memcpy(&smTransPlayerInfo.smCharInfo, &lpCharSample->smCharInfo, sizeof(smCHAR_INFO));
	smTransPlayerInfo.smCharInfo.State = 0;

	//NPC Á¤º¸ÆÄÀÏ À» ¸ðµ¨ÀÌ¸§2¿¡ ³Ö¾î¼­ º¸³¿
	lstrcpy(smTransPlayerInfo.smCharInfo.szModelName2, NpcSelectedName);

	smTransPlayerInfo.smCharInfo.Life[0] = 100;
	smTransPlayerInfo.smCharInfo.Mana[0] = 100;
	smTransPlayerInfo.smCharInfo.Life[1] = 100;

	smTransPlayerInfo.x = lpCharSample->pX;
	smTransPlayerInfo.y = lpCharSample->pY;
	smTransPlayerInfo.z = lpCharSample->pZ;

	smTransPlayerInfo.ax = lpCharSample->Angle.x;
	smTransPlayerInfo.ay = lpCharSample->Angle.y;
	smTransPlayerInfo.az = lpCharSample->Angle.z;

	smTransPlayerInfo.state = state;

	smWINSOCK *lpsmSock;
	lpsmSock = GetAreaServerSock();		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&smTransPlayerInfo, smTransPlayerInfo.size, TRUE);
	}

	return NULL;
}

//NPC Ä³¸¯ÅÍ Á¦°Å
int SendDelete_Npc(smCHAR *lpCharSample)
{
	smTRNAS_PLAYERINFO	smTransPlayerInfo;
	smWINSOCK *lpsmSock;

	smTransPlayerInfo.code = smTRANSCODE_DEL_NPC;
	smTransPlayerInfo.size = sizeof(smTRNAS_PLAYERINFO);
	memcpy(&smTransPlayerInfo.smCharInfo, &lpCharSample->smCharInfo, sizeof(smCHAR_INFO));

	smTransPlayerInfo.x = lpCharSample->pX;
	smTransPlayerInfo.y = lpCharSample->pY;
	smTransPlayerInfo.z = lpCharSample->pZ;

	smTransPlayerInfo.ax = lpCharSample->Angle.x;
	smTransPlayerInfo.ay = lpCharSample->Angle.y;
	smTransPlayerInfo.az = lpCharSample->Angle.z;

	smTransPlayerInfo.dwObjectSerial = lpCharSample->dwObjectSerial;

	lpsmSock = GetServerSock(lpCharSample->ServerCode);		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
	lpCharSample->Close();

	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&smTransPlayerInfo, smTransPlayerInfo.size, TRUE);
	}
	return NULL;
}

//ÀúÀåµÈ Ä³¸¯ÅÍ µ¥ÀÌÅ¸ ºÒ·¯¿À±â ¿ä±¸
int SendGetRecordData(char *szName)
{
	smTRANS_CHAR_COMMAND	smTransCharCommand;

	if (smWsockDataServer)
	{
		smTransCharCommand.code = smTRANSCODE_GETRECORDDATA;
		smTransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND);

		smTransCharCommand.WParam = dwExeCheckSum;					//½ÇÇàÆÄÀÏ Ã¼Å©¼¶
		smTransCharCommand.LParam = FALSE;
		smTransCharCommand.SParam = Client_Version;					//Å¬¶óÀÌ¾ðÆ® ¹öÀü

		lstrcpy(smTransCharCommand.szName, szName);

		dwLoadingTime = GetCurrentTime();			//·Îµù ½Ãµµ ½Ã°£ ±â·Ï

		return smWsockDataServer->Send((char *)&smTransCharCommand, smTransCharCommand.size, TRUE);
	}

	return FALSE;
}

char *szCmdOpenMonster[10] = {
	"È©°íºí¸°",
	"È©°íºí¸°",
	"È©°íºí¸°",
	"Å·È£ÇÇ",
	"Å¸ÀÌÅº",
	0,0,0,0,0
};


//¸ó½ºÅÍ »ý¼º
int SendOpenMonster(int State)
{

	smTRANS_CHAR_COMMAND	smTransCharCommand;
	char *szMonName;

	smTransCharCommand.code = smTRANSCODE_OPENMONSTER;
	smTransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND);
	smTransCharCommand.WParam = State;
	smTransCharCommand.LParam = 0;
	smTransCharCommand.SParam = 0;

	if (State >= 0)
	{
		szMonName = smConfig.szCmdOpenMonster[State];
		if (!szMonName[0]) szMonName = szCmdOpenMonster[State];
	}

	if (State >= 0 && szMonName)
		lstrcpy(smTransCharCommand.szName, szMonName);
	else
		smTransCharCommand.szName[0];


	smWINSOCK *lpsmSock;
	lpsmSock = GetAreaServerSock();		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&smTransCharCommand, smTransCharCommand.size, TRUE);
	}

	return FALSE;

}

//°¢ ÄÚµå µ¥ÀÌÅ¸¸¦ ¹ÝÀü
int	CodeXorCharInfo()
{
	lpCurPlayer->smCharInfo.Exp ^= CodeXorExp;
	lpCurPlayer->smCharInfo.Exp_High ^= CodeXorExp_High;
	lpCurPlayer->smCharInfo.Life[0] ^= CodeXorLife;
	return TRUE;
}

//½ºÅ³°ª ÀüºÎ È®ÀÎ
int SaveCheckSkill()
{

	ReformSkillInfo();

	for (int j = 1; j < SIN_MAX_USE_SKILL; j++)
	{
		if (sinSkill.UseSkill[j].Flag && sinSkill.UseSkill[j].Point)
		{
			if (cSkill.CheckSkillPointForm(&sinSkill.UseSkill[j]) == FALSE)
				return FALSE;
			if (cSkill.CheckSkillMasteryForm(&sinSkill.UseSkill[j]) == FALSE)
				return FALSE;
		}
	}

	return TRUE;
}

//½ºÅ³º¸È£°ª ÀüºÎ ÃÊ±âÈ­
int ReformSkillInfo()
{
	for (int j = 1; j < SIN_MAX_USE_SKILL; j++)
	{
		cSkill.ReformSkillMasteryForm(&sinSkill.UseSkill[j]);
		cSkill.ReformSkillPointForm(&sinSkill.UseSkill[j]);
	}
	return TRUE;
}

//°ÔÀÓÀ» ÀúÀå
int	SaveGameData()
{
	if (smWsockDataServer && !rsRecorder.dwRecordTime &&
		lpCurPlayer->smCharInfo.JOB_CODE && !AdminCharDisable && !cTrade.OpenFlag && !cWareHouse.OpenFlag &&
		!QuitSave)
	{

		//¹°¾à°¹¼ö ºñ±³
		//if ( ComparePotion()>0 ) return FALSE;		//¹°¾à ¿À·ù

		if (!WarningHack && CheckCharForm() != FALSE && SaveCheckSkill() == TRUE)
		{

			CodeXorCharInfo();
			ReformCharForm();

			rsRecorder.MakeRecordData(&lpCurPlayer->smCharInfo, cInvenTory.InvenItem, cInvenTory.InvenItemTemp, &MouseItem);
			rsRecorder.SendRecordDataToServer(smWsockDataServer);
			rsRecorder.dwRecordTime = dwPlayTime;					//°ÔÀÓ ÀúÀå ½Ã°£ È®ÀÎ

			if (smConfig.DebugMode) AddChatBuff("GameSave");

			CodeXorCharInfo();
			ReformCharForm();
			CheckPlaySkill();		//½ºÅ³°¨½Ã
			return TRUE;
		}
	}
	return FALSE;
}

//°ÔÀÓ ÀúÀå »óÅÂ ( FALSE ÀúÀåÁß ( ¼­¹ö·ÎºÎÅÍ °á°ú¸¦ ¹ÞÁö ¸øÇÔ ) )
int GetSaveResult()
{
	if (rsRecorder.dwRecordTime) return FALSE;

	return TRUE;
}



///////////////////// º¸³»±â °ü·Ã ////////////////////
	//smTRANSCODE_ID_GETUSERINFO:
	//ID·Î »ç¿ëÀÚ Ä³¸¯ÅÍ ¸®½ºÆ® ¹× Á¤º¸¸¦ ±¸ÇÔ

	//smTRANSCODE_INSRECORDDATA:
	//ID¿¡ »õ·Î¿î Ä³¸¯ÅÍ »ðÀÔ

	//smTRANSCODE_DELRECORDDATA:
	//ID¿¡ »õ·Î¿î Ä³¸¯ÅÍ »èÁ¦

	//smTRANSCODE_ISRECORDDATA:
	//Ä³¸¯ÅÍ°¡ Á¸Àç ÇÏ´ÂÁö È®ÀÎ

//À¯Àú Á¤º¸ °ü·Ã Åë½Å ÄÚ¸àµå
//int	TransUserCommand ( DWORD dwCode , char *szID , char *szName )

///////////////////////////////////////////////////////

///////////////////// ¹Þ±â °ü·Ã ////////////////////
	//smTRANSCODE_ID_SETUSERINFO:
	//ID·Î »ç¿ëÀÚ Ä³¸¯ÅÍ ¸®½ºÆ® ¹× Á¤º¸¸¦ ±¸ÇÔ 
	//(TRANS_USERCHAR_INFO ±¸Á¶Ã¼·Î °á°ú µé¾î¿È )

	//smTRANSCODE_ISRECORDDATA:
	//Ä³¸¯ÅÍ°¡ Á¸Àç ÇÏ´ÂÁö È®ÀÎ 
	//( smTRANS_CHAR_COMMAND2 ->wParam ¿¡ °á°ú µé¾î¿È )
///////////////////////////////////////////////////////

smTRANS_COMMAND	smMessageCommandBuff;

//À¯Àú Á¤º¸ °ü·Ã Åë½Å ÄÚ¸àµå ( µ¥ÀÌÅ¸ ¼­¹ö¿ë )
int	TransUserCommand(DWORD dwCode, char *szID, char *szName)
{
	smTRANS_CHAR_COMMAND2	TransCharCommand2;

	TransCharCommand2.code = dwCode;
	TransCharCommand2.size = sizeof(smTRANS_CHAR_COMMAND2);

#ifdef _LANGUAGE_KOREAN
	if (dwCode == smTRANSCODE_ID_GETUSERINFO)
	{
		int cnt, len;

		len = lstrlen(szName);
		for (cnt = 0; cnt < len; cnt++)
		{
			if ((szName[cnt] & 0x80) != 0)
			{
				smMessageCommandBuff.code = smTRANSCODE_FAILCONNECT;
				smMessageCommandBuff.size = sizeof(smTRANS_COMMAND);
				smMessageCommandBuff.WParam = -19;			//Æ¯¼ö¹®ÀÚ ¿À·ù
				smMessageCommandBuff.LParam = 0;
				smMessageCommandBuff.EParam = 2;
				HoRecvMessage(smTRANSCODE_FAILCONNECT, &smMessageCommandBuff);
				return FALSE;
			}
		}
	}
#endif

	if (szID)
		lstrcpy(TransCharCommand2.szId, szID);

	if (szName)
		lstrcpy(TransCharCommand2.szName, szName);

	return smWsockDataServer->Send2((char *)&TransCharCommand2, TransCharCommand2.size, TRUE);
}


//Ä³¸¯ÅÍ Á¤º¸ Àü¼Û À¯±¸
int	Send_GetCharInfo(DWORD	dwObjectSerial, int ServerCode)
{
	smTRANS_COMMAND			TransCommand;
	smWINSOCK	*lpsmSock;

	TransCommand.code = smTRANSCODE_GETPLAYERINFO;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectSerial;

	lpsmSock = GetServerSock(ServerCode);		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½

	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return FALSE;
}

#define DEADLOCK_CHECK_LIMIT_TIME		(30*1000)
#define DEADLOCK_CHECK_LIMIT_TIME2		(60*1000)
#define RECONNECT_LIMIT_TIME			(7*1000)

/*
#define DEADLOCK_CHECK_LIMIT_TIME		(30*1000)
#define DEADLOCK_CHECK_LIMIT_TIME2		(60*1000)
#define RECONNECT_LIMIT_TIME			(7*1000)
*/
static DWORD dwCheckNetCount = 0;
static DWORD dwLastTransCheckTime = 0;
static int	ReconnectCount = 0;

//µ¥µå¶ô Ã½Å© ( ¸Þ¼¼Áö°¡ µ¹¾Æ ¿ÀÁö ¾ÊÀ¸¸é ³×Æ®¿÷ ´Ù¿î )
int	TransCheckNetwork()
{
	smTRANS_COMMAND			TransCommand;
	DWORD	dwTime, dwTime2;
	smWINSOCK	*lpOldSock;

	//return TRUE;

	if (!dwConnectedServerTime) return FALSE;

	//ÇÁ·Î¼¼½º ½Ã°£ °¨½ÃÇÏ´Â ¾²·¹µå °¡µ¿

	OpenTimeCheckThread();

	//È®ÀÎ 4ÃÊ ÀÌÈÄ¿¡³ª È®ÀÎ ¸Þ¼¼Áö º¸³¿
	dwTime = GetCurrentTime();
	dwTime2 = dwTime;

	if ((dwTime - dwLastTransCheckTime) < 4000) return FALSE;

	dwLastTransCheckTime = dwTime;

	//if ( !smConfig.DebugMode ) {
	if ((dwMainServ_RecvTime && (dwMainServ_RecvTime + RECONNECT_LIMIT_TIME) < dwTime) ||
		(dwLastRecvGameServerTime + DEADLOCK_CHECK_LIMIT_TIME) < dwTime ||
		!smWsockServer || smWsockServer->WaitReadMessage < 0)
	{
		//¿¬°á ²ö¾î Áü ( ¼­¹ö¿ÍÀÇ ±³½Å ºÒ´É )

		dwLastRecvGameServerTime = dwTime;

		lpOldSock = smWsockServer;

		if (lpOldSock == smWsockDataServer)
			smWsockDataServer = 0;
		if (lpOldSock == smWsockUserServer)
			smWsockUserServer = 0;
		if (lpOldSock == smWsockExtendServer)
			smWsockExtendServer = 0;

		smWsockServer = 0;
		ConnectServer_Main();

		if (smWsockServer)
		{
			//Sleep(200);
			//¼­¹ö¿¡ ÀçÁ¢¼Ó ½Ãµµ
			SendContinueServer(lpCurPlayer->dwObjectSerial, smWsockServer);

			char szBuff[256];
			wsprintf(szBuff, "Reconnect Server(Main)");
			AddChatBuff(szBuff);

			if (dwLastRecvGameServerTime && dwLastRecvGameServerTime < dwTime2) dwLastRecvGameServerTime = dwTime2;
			if (dwLastRecvGameServerTime2 && dwLastRecvGameServerTime2 < dwTime2) dwLastRecvGameServerTime2 = dwTime2;
			if (dwLastRecvGameServerTime3 && dwLastRecvGameServerTime3 < dwTime2) dwLastRecvGameServerTime3 = dwTime2;
			if (dwLastRecvGameServerTime4 && dwLastRecvGameServerTime4 < dwTime2) dwLastRecvGameServerTime4 = dwTime2;
			ReconnServer++;		//Àç¿¬°á Ä«¿îÅÍ

			return TRUE;
		}

		DisconnectServerCode = 1;
		DisconnectFlag = GetCurrentTime();
		return FALSE;
	}
	//}


	//¿¹»ó ¼­¹ö ½Ã°£
	dwTime = (dwTime - dwConnectedClientTime) + dwConnectedServerTime;

	TransCommand.code = smTRANSCODE_CHECK_NETSTATE;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwTime;
	TransCommand.LParam = dwCheckNetCount;
	TransCommand.SParam = GetSafeCode2() + ((dwMemError^dwPlayTime) - 8080);
	TransCommand.EParam = dwMemFunChkCode;

	dwCheckNetCount++;

	if (smWsockServer) smWsockServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockDataServer && smWsockServer != smWsockDataServer)
	{

		if (dwLastRecvGameServerTime2 && (dwLastRecvGameServerTime2 + DEADLOCK_CHECK_LIMIT_TIME2) < dwTime2)
		{
			DisconnectFlag = GetCurrentTime();
			DisconnectServerCode = 2;
			return FALSE;
		}
		//		else
		//			dwLastRecvGameServerTime2=dwTime;


		TransCommand.code = smTRANSCODE_CHECK_NETSTATE2;
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
		if (!dwLastRecvGameServerTime2) dwLastRecvGameServerTime2 = dwTime2;
	}


	if (smWsockUserServer && smWsockServer != smWsockUserServer)
	{
		if ((dwLastRecvGameServerTime3 && (dwLastRecvGameServerTime3 + DEADLOCK_CHECK_LIMIT_TIME) < dwTime2) ||
			(dwUserServ_RecvTime && (dwUserServ_RecvTime + RECONNECT_LIMIT_TIME) < dwTime2) ||
			smWsockUserServer->WaitReadMessage < 0)
		{

			smWsockUserServer = 0;
			ConnectServer_Main();

			if (smWsockUserServer)
			{
				//¼­¹ö¿¡ ÀçÁ¢¼Ó ½Ãµµ
				//Sleep(200);
				SendContinueServer(lpCurPlayer->dwObjectSerial, smWsockUserServer);

				char szBuff[256];
				wsprintf(szBuff, "Reconnect Server(User)");
				AddChatBuff(szBuff);
				if (dwLastRecvGameServerTime && dwLastRecvGameServerTime < dwTime2) dwLastRecvGameServerTime = dwTime2;
				if (dwLastRecvGameServerTime2 && dwLastRecvGameServerTime2 < dwTime2) dwLastRecvGameServerTime2 = dwTime2;
				if (dwLastRecvGameServerTime3 && dwLastRecvGameServerTime3 < dwTime2) dwLastRecvGameServerTime3 = dwTime2;
				if (dwLastRecvGameServerTime4 && dwLastRecvGameServerTime4 < dwTime2) dwLastRecvGameServerTime4 = dwTime2;

				ReconnDataServer++;		//¼­¹ö Àç¿¬°á Ä«¿îÅÍ

				return TRUE;
			}

			DisconnectServerCode = 3;
			DisconnectFlag = GetCurrentTime();
			return FALSE;
		}

		TransCommand.code = smTRANSCODE_CHECK_NETSTATE3;
		smWsockUserServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
		if (!dwLastRecvGameServerTime3) dwLastRecvGameServerTime3 = dwTime2;

	}

	if (smWsockExtendServer && smWsockServer != smWsockExtendServer)
	{
		if ((dwLastRecvGameServerTime4 && (dwLastRecvGameServerTime4 + DEADLOCK_CHECK_LIMIT_TIME) < dwTime2) ||
			(dwExtendServ_RecvTime && (dwExtendServ_RecvTime + RECONNECT_LIMIT_TIME) < dwTime2) ||
			smWsockExtendServer->WaitReadMessage < 0)
		{

			smWsockExtendServer = 0;
			ConnectServer_Main();

			if (smWsockExtendServer)
			{
				SendContinueServer(lpCurPlayer->dwObjectSerial, smWsockExtendServer);

				char szBuff[256];
				wsprintf(szBuff, "Reconnect Server(Extend)");
				AddChatBuff(szBuff);
				if (dwLastRecvGameServerTime && dwLastRecvGameServerTime < dwTime2) dwLastRecvGameServerTime = dwTime2;
				if (dwLastRecvGameServerTime2 && dwLastRecvGameServerTime2 < dwTime2) dwLastRecvGameServerTime2 = dwTime2;
				if (dwLastRecvGameServerTime3 && dwLastRecvGameServerTime3 < dwTime2) dwLastRecvGameServerTime3 = dwTime2;
				if (dwLastRecvGameServerTime4 && dwLastRecvGameServerTime4 < dwTime2) dwLastRecvGameServerTime4 = dwTime2;

				//ReconnDataServer++;		//¼­¹ö Àç¿¬°á Ä«¿îÅÍ

				return TRUE;
			}

			DisconnectServerCode = 3;
			DisconnectFlag = GetCurrentTime();
			return FALSE;
		}

		TransCommand.code = smTRANSCODE_CHECK_NETSTATE4;
		smWsockExtendServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
		if (!dwLastRecvGameServerTime4) dwLastRecvGameServerTime4 = dwTime2;

	}
	if (AreaServerMode)
	{
		//Áö¿ª¼­¹ö µ¥µå¶ô Ã½Å©
		int cnt;
		for (cnt = 0; cnt < 2; cnt++)
		{
			if (lpWSockServer_Area[cnt])
			{
				if ((lpWSockServer_Area[cnt]->dwDeadLockTime + DEADLOCK_CHECK_LIMIT_TIME) < dwTime2 ||
					lpWSockServer_Area[cnt]->WaitReadMessage < 0)
				{

					lpOldSock = lpWSockServer_Area[cnt];
					lpWSockServer_Area[cnt] = smConnectSock(lpOldSock->szIPAddr, TCP_SERVPORT);

					if (lpWSockServer_Area[cnt])
					{
						SendContinueServer(lpCurPlayer->dwObjectSerial, lpWSockServer_Area[cnt]);

						char szBuff[256];
						wsprintf(szBuff, "Reconnect Server Area (%d)", cnt);
						AddChatBuff(szBuff);

					}
					else
					{
						lpOldSock->CloseSocket();
					}
				}

				if (lpWSockServer_Area[cnt])
				{
					TransCommand.code = smTRANSCODE_CHECK_NETSTATE5;
					TransCommand.EParam = (DWORD)lpWSockServer_Area[cnt];
					lpWSockServer_Area[cnt]->Send2((char *)&TransCommand, TransCommand.size, TRUE);
					//lpWSockServer_Area[cnt]->dwDeadLockTime = dwTime2;
				}
			}
		}
	}


	//¹ÞÀº °æÇèÄ¡ µ·µ¥ÀÌÅ¸ ¼öÄ¡¸¦ ¼­¹ö¿¡ º¸³»¼­ È®ÀÎ
	SendTotalExpMoney();

	CheckMaxDamage();			//ÃÖ´ë µ¥¹ÌÁö °Ë»ç


	return TRUE;
}

//¼­¹ö¿¡ °ü¸®ÀÚ ¸ðµå ¼³Á¤
int SendAdminMode(DWORD AdminNum)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_ADMINMODE;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = AdminNum;
	TransCommand.LParam = 0;
	TransCommand.SParam = ServerHideMode;

	if (smWsockDataServer && smWsockServer != smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockUserServer && smWsockServer != smWsockUserServer && smWsockDataServer != smWsockUserServer)
		smWsockUserServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockExtendServer && smWsockServer != smWsockExtendServer && smWsockDataServer != smWsockExtendServer)
		smWsockExtendServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);


	return smWsockServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}

//¼­¹ö¿¡ °ü¸®ÀÚ ¸ðµå ¼³Á¤
int SendAdminMode(DWORD AdminNum, smWINSOCK *lpsmSock)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_ADMINMODE;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = AdminNum;
	TransCommand.LParam = 0;
	TransCommand.SParam = ServerHideMode;

	return lpsmSock->Send3((char *)&TransCommand, TransCommand.size, TRUE);
}


//¼­¹ö¿¡ ³×Æ®¿öÅ© Ç®Áú ¸ðµå ¼³Á¤
int SendNetworkQuality(DWORD NetworkQuality)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_NETWORK_QUALITY;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = NetworkQuality;
	TransCommand.LParam = 0;
	TransCommand.SParam = 0;

	if (smWsockUserServer && smWsockServer != smWsockUserServer)
		smWsockUserServer->Send((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockExtendServer && smWsockServer != smWsockExtendServer)
		smWsockExtendServer->Send((char *)&TransCommand, TransCommand.size, TRUE);

	return smWsockServer->Send((char *)&TransCommand, TransCommand.size, TRUE);
}


//IPÁ¢¼Ó ÁÖ¼Ò¸¦ ±º ¼­¹ö¿¡ º¸³½´Ù
int SendCheckIP(int Code, smWINSOCK *lpsmSock)
{
	smTRANS_CHAR_COMMAND	TransCharCommand;

	TransCharCommand.code = smTRANSCODE_CHECKIP;
	TransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND);
	TransCharCommand.WParam = Code;
	TransCharCommand.LParam = 0;
	TransCharCommand.SParam = 0;

	lstrcpy(TransCharCommand.szName, lpsmSock->szIPAddr);

	return smWsockDataServer->Send2((char *)&TransCharCommand, TransCharCommand.size, TRUE);
}



//¼­¹ö¿¡ ¿ÀºêÁ§Æ® ¹øÈ£ ¼³Á¤
int SendSetObjectSerial(DWORD dwObjectSerial)
{

	//»õ¹öÀü 
	smTRNAS_PLAYERINFO	TransPlayerInfo;

	memcpy(&TransPlayerInfo.smCharInfo, &lpCurPlayer->smCharInfo, sizeof(smCHAR_INFO));
	TransPlayerInfo.size = sizeof(smTRNAS_PLAYERINFO);
	TransPlayerInfo.code = smTRANSCODE_PLAYERINFO2;
	TransPlayerInfo.dwObjectSerial = lpCurPlayer->dwObjectSerial;

	TransPlayerInfo.x = lpCurPlayer->pX;
	TransPlayerInfo.y = lpCurPlayer->pY;
	TransPlayerInfo.z = lpCurPlayer->pZ;

	smWsockServer->Send2((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);

	if (smWsockServer != smWsockUserServer)
		smWsockUserServer->Send2((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);

	if (smWsockServer != smWsockExtendServer)
		smWsockExtendServer->Send2((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);

	if (AreaServerMode)
	{
		if (lpWSockServer_Area[0]) lpWSockServer_Area[0]->Send2((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);
		if (lpWSockServer_Area[1]) lpWSockServer_Area[1]->Send2((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);
	}

	return TRUE;
}

//¼­¹ö¿¡ ¿ÀºêÁ§Æ® ¹øÈ£ ¼³Á¤ ( Ã³À½ ÇÑ¹ø¸¸ )
int SendSetObjectSerial_First(DWORD dwObjectSerial)
{

	smTRNAS_PLAYERINFO	TransPlayerInfo;

	memcpy(&TransPlayerInfo.smCharInfo, &lpCurPlayer->smCharInfo, sizeof(smCHAR_INFO));
	TransPlayerInfo.size = sizeof(smTRNAS_PLAYERINFO);
	TransPlayerInfo.code = smTRANSCODE_PLAYERINFO2;
	TransPlayerInfo.dwObjectSerial = lpCurPlayer->dwObjectSerial;

	TransPlayerInfo.x = lpCurPlayer->pX;
	TransPlayerInfo.y = lpCurPlayer->pY;
	TransPlayerInfo.z = lpCurPlayer->pZ;

	//smWsockServer->Send2( (char *)&TransPlayerInfo , TransPlayerInfo.size , TRUE );

	if (smWsockServer != smWsockUserServer)
		smWsockUserServer->Send3((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);

	if (smWsockServer != smWsockExtendServer)
		smWsockExtendServer->Send3((char *)&TransPlayerInfo, TransPlayerInfo.size, TRUE);

	return TRUE;
}


//¼­¹ö¿¡ ÀçÁ¢¼Ó ½Ãµµ
int SendContinueServer(DWORD dwObjectSerial, smWINSOCK *lpsmSock)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_CONT_OBJSERIAL;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectSerial;
	TransCommand.LParam = 0;
	TransCommand.SParam = 0;

	return lpsmSock->Send((char *)&TransCommand, TransCommand.size, TRUE);
}

//ÇØÅ· ½ÃµµÇÑ À¯Àú ÀÚµ¿ ½Å°í
int SendSetHackUser3(int Type, int LParam, int SParam)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_SET_BLACKLIST2;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = Type;
	TransCommand.LParam = LParam;
	TransCommand.SParam = SParam;

	if (smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	return TRUE;
}


//Å©·¢ Ã½Å©
int CheckCracker()
{
	char *szName;
	char TransBuff[1024];
	int len;

	szName = FindCracker();

	if (szName)
	{
		//Å©·¢ ¹ß°ß
		if (smWsockDataServer)
		{

			len = lstrlen(szName) + 9;
			lstrcpy(TransBuff + 8, szName);
			((int *)TransBuff)[0] = len;
			((int *)TransBuff)[1] = smTRANSCODE_FINDCRACK;

			smWsockDataServer->Send2(TransBuff, len, TRUE);
		}
		return TRUE;
	}
	return FALSE;
}

//Å©·¢ Ã½Å© ( ¸Þ¼¼Áö Åëº¸µµ °°ÀÌ »ç¿ë )
int CheckCracker(TRANS_FIND_CRACKER *lpTransFindCrack)
{
	char *szName;
	char TransBuff[1024];
	int len;
	char	*lpBuff;

	if (lpTransFindCrack)
		szName = FindCracker(lpTransFindCrack);
	else
		szName = FindCrackCont();

	if (szName)
	{
		//Å©·¢ ¹ß°ß
		if (smWsockDataServer)
		{

			len = lstrlen(szName) + 9;
			lstrcpy(TransBuff + 8, szName);
			((int *)TransBuff)[0] = len;
			((int *)TransBuff)[1] = smTRANSCODE_FINDCRACK;

			smWsockDataServer->Send2(TransBuff, len, TRUE);
		}
		return TRUE;
	}

	if (lpTransFindCrack)
	{
		if (lpTransFindCrack->size > (lpTransFindCrack->FindCounter*((int)sizeof(_CRACK_NAME)) + 16))
		{
			lpBuff = (char *)&lpTransFindCrack->CrackName[lpTransFindCrack->FindCounter];
			if (lpBuff[0])
			{
				//³»¿ë ÀÖÀ½ , °øÁö Ãâ·Â
				GetNoticeUrl(lpBuff);
			}
		}
	}

	return FALSE;
}

//Å©·¢ À©µµ¿ì ½Å°í
int SendCrackWindow(HWND hWnd)
{
	char *szName;
	char TransBuff[1024];
	int len;

	//Å©·¢ ¹ß°ß
	if (smWsockServer)
	{

		szName = TransBuff + 8;
		GetWindowText(hWnd, szName, 32);
		len = lstrlen(szName) + 9;
		((int *)TransBuff)[0] = len;
		((int *)TransBuff)[1] = smTRANSCODE_FINDCRACK2;

		smWsockServer->Send2(TransBuff, len, TRUE);
	}
	return TRUE;
}



//ÇØÅ· ½ÃµµÇÑ À¯Àú ÀÚµ¿ ½Å°í
int SendSetHackUser(int StopGame)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_SET_BLACKLIST;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = StopGame;
	TransCommand.LParam = 0;
	TransCommand.SParam = 0;

	WarningHack = StopGame;		//ÇØÅ· °æ°í

	if (smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (WarningHack)
	{
		DisconnectServerCode = 4;
		DisconnectFlag = GetCurrentTime();
	}

	return TRUE;
}

//ÇØÅ· ½ÃµµÇÑ À¯Àú ÀÚµ¿ ½Å°í
int SendSetHackUser2(int Type, int LParam)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_SET_BLACKLIST;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = Type;
	TransCommand.LParam = LParam;
	TransCommand.SParam = 0;

	if (smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	return TRUE;
}


//¼­¹ö¿¡ °ü¸®ÀÚ ¸ðµå ¼³Á¤
int SendAdminMode2(DWORD AdminNum)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_ADMINMODE2;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = AdminNum;
	TransCommand.LParam = 1;
	TransCommand.SParam = 0;

	if (smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	return TRUE;

}



//º¹»ç ¾ÆÀÌÅÛÀ» ¼ÒÁöÇÑ »ç¶÷ ½Å°í 
int SendCopyItemUser(int CopyItemCount)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_SET_ITEMCOPYLIST;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = CopyItemCount;
	TransCommand.LParam = 0;
	TransCommand.SParam = 0;

	if (smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	return TRUE;
}


//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
int SendSaveThrowItem(sITEMINFO *lpItemInfo)
{
	smTRANS_COMMAND	TransCommand;

	//¹°¾àÀÎ °æ¿ì¸¸ ÀúÀåÇÏµµ·Ï ¼öÁ¤
	if ((lpItemInfo->CODE&sinITEM_MASK1) != (sinPM1&sinITEM_MASK1))	return FALSE;


	TransCommand.code = smTRANSCODE_SAVETHROWITEM;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = lpItemInfo->CODE;
	TransCommand.LParam = lpItemInfo->ItemHeader.Head;
	TransCommand.SParam = lpItemInfo->ItemHeader.dwChkSum;

	if ((lpItemInfo->CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		ComparePotion();	//¹°¾à°¹¼ö ºñ±³

	return smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}
/*
//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ÃÖ´ë 32°³ µ¿½Ã ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
int	SendSaveThrowItems( TRANS_THROW_ITEMS	*lpTransThrowItems )
{
	if ( lpTransThrowItems->ThrowItemCount<0 ) return FALSE;
	if ( lpTransThrowItems->ThrowItemCount>THROW_ITEMS_MAX )
		lpTransThrowItems->ThrowItemCount = THROW_ITEMS_MAX;

	lpTransThrowItems->size = TRANS_THROW_ITEMS_DEFSIZE + lpTransThrowItems->ThrowItemCount * sizeof(_TRANS_THROW_ITEM);
	lpTransThrowItems->code = smTRANSCODE_SAVETHROWITEMS;

	return smWsockDataServer->Send2( (char *)lpTransThrowItems , lpTransThrowItems->size , TRUE );
}
*/

//¾ÆÀÌÅÛ °Å·¡³»¿ª ¼­¹ö¿¡ º¸³»±â
int	SendRecordTradeItems(sTRADE *lpTrade)
{

	TRANS_RECORD_TRADE_ITEMS	TransRecordTradeItems;
	int cnt;

	ZeroMemory(&TransRecordTradeItems, sizeof(TRANS_RECORD_TRADE_ITEMS));

	TransRecordTradeItems.code = smTRANSCODE_RECORD_TRADE;
	TransRecordTradeItems.Money = lpTrade->Money - 193;
	lstrcpy(TransRecordTradeItems.szTradeUserName, cTrade.szTradeCharName);

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (lpTrade->TradeItem[cnt].Flag && TransRecordTradeItems.ThrowItemCount < TRADE_RECV_ITEMS_MAX)
		{
			TransRecordTradeItems.Items[TransRecordTradeItems.ThrowItemCount].dwCode = lpTrade->TradeItem[cnt].sItemInfo.CODE;
			TransRecordTradeItems.Items[TransRecordTradeItems.ThrowItemCount].dwHead = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head;
			TransRecordTradeItems.Items[TransRecordTradeItems.ThrowItemCount].dwChkSum = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;
			TransRecordTradeItems.ThrowItemCount++;
		}
	}


	TransRecordTradeItems.size = TRANS_RECORD_TRADE_DEFSIZE + TransRecordTradeItems.ThrowItemCount * sizeof(_TRANS_RECORD_TRADE_ITEM);

	return smWsockDataServer->Send2((char *)&TransRecordTradeItems, TransRecordTradeItems.size, TRUE);
}


//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
int SendSaveThrowItem2(sITEMINFO *lpItemInfo)
{
	smTRANS_COMMAND_EX	TransCommand;


	TransCommand.code = smTRANSCODE_SAVETHROWITEM2;
	TransCommand.size = sizeof(smTRANS_COMMAND_EX);
	TransCommand.WParam = lpItemInfo->CODE;
	TransCommand.LParam = lpItemInfo->ItemHeader.Head;
	TransCommand.SParam = lpItemInfo->ItemHeader.dwChkSum;
	TransCommand.EParam = lpItemInfo->PotionCount^TransCommand.SParam^TransCommand.code;	//¹°¾à °¹¼ö

	TransCommand.WxParam = lpCurPlayer->smCharInfo.Money;
	TransCommand.LxParam = EncodeParam(TransCommand.WxParam);
	TransCommand.SxParam = GetTotalMoney() ^ TransCommand.LxParam;
	TransCommand.ExParam = lpItemInfo->Money;

	if ((lpItemInfo->CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		ComparePotion();	//¹°¾à°¹¼ö ºñ±³

	//ÀÌº¥Åä¸® µ· °Ë»ç ¼³Á¤
	AddInvenMoney(-lpItemInfo->Money);

	return smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}

//µ¥ÀÌÅ¸ ¼­¹ö¿¡ µ· ÀúÀå ¿ä±¸ ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
int SendSaveMoney()
{
	smTRANS_COMMAND	TransCommand;

	if (cWareHouse.OpenFlag && CompWareHouseMoney) return FALSE;

	TransCommand.code = smTRANSCODE_SAVETHROWMONEY;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = lpCurPlayer->smCharInfo.Money;
	TransCommand.LParam = EncodeParam(TransCommand.WParam);
	TransCommand.SParam = GetTotalMoney() ^ TransCommand.LParam;

	return smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}

//´Ù¸¥ Ä³¸¯ÅÍ Ä«ÇÇ Àü¼Û ¿ä±¸
int SendCopyOtherUser(char *szName, int Mode)
{
	smTRANS_CHAR_COMMAND	TransCharCommand;
	int cnt, len;

	TransCharCommand.code = smTRANSCODE_GETRECORDDATA;
	TransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND);
	TransCharCommand.WParam = dwExeCheckSum;
	TransCharCommand.LParam = Mode;
	TransCharCommand.SParam = 0;

	//20020912
	if (Mode == 2)
	{
		len = lstrlen(szName);
		for (cnt = 0; cnt < len; cnt++)
		{
			if (szName[cnt] == ' ')
			{
				TransCharCommand.LParam = atoi(&szName[cnt]);
				szName[cnt] = 0;
				if (TransCharCommand.LParam < 1000) TransCharCommand.LParam = Mode;
				break;
			}
		}
	}

	lstrcpy(TransCharCommand.szName, szName);

	AdminCharDisable = TRUE;

	if (smWsockDataServer)
		return smWsockDataServer->Send((char *)&TransCharCommand, TransCharCommand.size, TRUE);

	return NULL;
}



//¼­¹ö¿¡ º¯°æµÈ Ä³¸¯ÅÍ Á¤º¸ º¸³¿
int SendPlayUpdateInfo()
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_UPDATE_CINFO;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = 0;
	TransCommand.LParam = lpCurPlayer->smCharInfo.Level;			//·¹º§ Á¤º¸
	TransCommand.SParam = 0;
	TransCommand.EParam = 0;

	return smWsockServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}

//¼­¹ö¿¡ º¯°æµÈ Å¬·£ Á¤º¸ º¸³¿
int SendUpdateClanInfo()
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_UPDATE_CINFO;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = 1;
	TransCommand.LParam = lpCurPlayer->smCharInfo.Level;			//·¹º§ Á¤º¸
	TransCommand.SParam = lpCurPlayer->smCharInfo.ClassClan;
	TransCommand.EParam = lpCurPlayer->smCharInfo.bUpdateInfo[0];

	if (smWsockServer)
		smWsockServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockUserServer && smWsockServer != smWsockUserServer)
		smWsockUserServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockExtendServer && smWsockServer != smWsockExtendServer)
		smWsockExtendServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (smWsockDataServer && smWsockServer != smWsockDataServer && rsBlessCastle.dwMasterClan == GetClanCode(lpCurPlayer->smCharInfo.ClassClan))
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (AreaServerMode)
	{
		if (lpWSockServer_Area[0]) lpWSockServer_Area[0]->Send2((char *)&TransCommand, TransCommand.size, TRUE);
		if (lpWSockServer_Area[1]) lpWSockServer_Area[1]->Send2((char *)&TransCommand, TransCommand.size, TRUE);
	}

	return TRUE;
}






/*
smTRANSCODE_SKIL_ATTACKDATA
#define SKIL_ATTACK_CHAR_MAX		32

struct TRANS_SKIL_ATTACKDATA {
	int size,code;

	DWORD	dwDestObjectSerial;	//°ø°ÝÇÏ´Â Ä³¸¯ÅÍ °íÀ¯¹øÈ£

	int x,y,z;			// ÁÂÇ¥
	int AttackState;	// ¼Ó¼º
	int AttackSize;		// °ø°Ý ¹üÀ§
	int Power;			// °ø°Ý ÆÄ¿ö

	int	TargetCount;
	DWORD	dwTarObjectSerial[SKIL_ATTACK_CHAR_MAX];	//°ø°Ý´ë»ó Ä³¸¯ÅÍ °íÀ¯¹øÈ£
};
*/


TRANS_SKIL_ATTACKDATA	Back_TransSkilAttackData;

//¹üÀ§Çü °ø°Ý
int Skil_RangeAttack(int x, int y, int z, int range, int power, int TargetState)
{

	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	int cnt, mCnt;
	//int	accuracy;
	int	dRange;
	int dx, dy, dz, dist;
	smCHAR	*lpChar;
	smWINSOCK *lpsmSendSock = 0;


	//Å¬¶óÀÌ¾ðÆ® ·Î±× ÆÄÀÏ ±â·Ï
	Record_ClinetLogFile("±¸¹öÀü °ø°ÝÆÐÅ¶ - Skil_RangeAttack ");
	quit = 1;
	return TRUE;


	mCnt = 0;

	dRange = range >> FLOATNS;
	dRange *= dRange;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (mCnt >= SKIL_ATTACK_CHAR_MAX) break;
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY)
		{
			lpChar = &chrOtherPlayer[cnt];

			dx = (x - lpChar->pX) >> FLOATNS;
			dy = (y - lpChar->pY) >> FLOATNS;
			dz = (z - lpChar->pZ) >> FLOATNS;
			dist = dx*dx + dy*dy + dz*dz;

			if (dist < dRange)
			{
				//accuracy = sinGetAccuracy( lpChar->smCharInfo.Level , lpChar->smCharInfo.Defence );
				//if ( (rand()%100)<accuracy ) {

				TransSkilAttackData.dwTarObjectSerial[mCnt++] = chrOtherPlayer[cnt].dwObjectSerial;

				//Ã³À½ °ø°ÝÇÏ´Â Ä³¸¯ÇÑÅ×¸¸ Ã¼·Â¹Ù Ãâ·Â
				if (lpChar->smCharInfo.Life[1] && lpChar->smCharInfo.Life[0] == lpChar->smCharInfo.Life[1])
				{
					lpChar->EnableStateBar = TRUE;
				}
				//}
			}
		}
	}

	TransSkilAttackData.TargetCount = mCnt;

	TransSkilAttackData.code = smTRANSCODE_SKIL_ATTACKDATA;
	TransSkilAttackData.size = (sizeof(TRANS_SKIL_ATTACKDATA) - sizeof(DWORD)*SKIL_ATTACK_CHAR_MAX);
	TransSkilAttackData.size += sizeof(DWORD)*mCnt + 16;

	TransSkilAttackData.x = x;
	TransSkilAttackData.y = y;
	TransSkilAttackData.z = z;

	TransSkilAttackData.AttackState = TargetState;
	TransSkilAttackData.AttackSize = range;
	TransSkilAttackData.Power = power;

	TransSkilAttackData.dwChkSum = TransSkilAttackData.Power * 2002 +
		(TransSkilAttackData.x * TransSkilAttackData.y * TransSkilAttackData.z);// Á¶ÀÛ ¹æÁö ÄÚµå

	LastAttackDamage = power;

	memcpy(&Back_TransSkilAttackData, &TransSkilAttackData, sizeof(TRANS_SKIL_ATTACKDATA));		//¹é¾÷

	lpsmSendSock = GetAreaServerSock();		//º¸³¾ ¼­¹ö±¸ÇÏ±â

	if (lpsmSendSock && mCnt > 0)
	{
		//º¸³½ °ø°Ý·Â ±â·Ï
		Record_SendDamage(lpsmSendSock, TransSkilAttackData.Power);

		lpsmSendSock->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}

	if (PkMode)
	{
		//¹üÀ§Çü °ø°Ý( À¯Àú¸¸ °ø°Ý )
		Skil_RangeAttack_User(x, y, z, range, power, TargetState);
	}

	return TRUE;
}


//¹üÀ§Çü °ø°Ý( À¯Àú¸¸ °ø°Ý )
int Skil_RangeAttack_User(int x, int y, int z, int range, int power, int TargetState)
{
	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	int cnt, mCnt;
	//int	accuracy;
	int	dRange;
	int dx, dy, dz, dist;
	smCHAR	*lpChar;
	smWINSOCK *lpsmSendSock = 0;

	mCnt = 0;

	dRange = range >> FLOATNS;
	dRange *= dRange;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (mCnt >= SKIL_ATTACK_CHAR_MAX) break;
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_USER)
		{
			lpChar = &chrOtherPlayer[cnt];

			dx = (x - lpChar->pX) >> FLOATNS;
			dy = (y - lpChar->pY) >> FLOATNS;
			dz = (z - lpChar->pZ) >> FLOATNS;
			dist = dx*dx + dy*dy + dz*dz;

			if (dist < dRange)
			{

				TransSkilAttackData.dwTarObjectSerial[mCnt++] = chrOtherPlayer[cnt].dwObjectSerial;

			}
		}
	}

	TransSkilAttackData.TargetCount = mCnt;

	TransSkilAttackData.code = smTRANSCODE_SKIL_ATTACKUSER;
	TransSkilAttackData.size = (sizeof(TRANS_SKIL_ATTACKDATA) - sizeof(DWORD)*SKIL_ATTACK_CHAR_MAX);
	TransSkilAttackData.size += sizeof(DWORD)*mCnt + 16;

	TransSkilAttackData.x = x;
	TransSkilAttackData.y = y;
	TransSkilAttackData.z = z;

	TransSkilAttackData.AttackState = TargetState;
	TransSkilAttackData.AttackSize = range;
	TransSkilAttackData.Power = power;

	TransSkilAttackData.dwChkSum = TransSkilAttackData.Power * 2002 +
		(TransSkilAttackData.x * TransSkilAttackData.y * TransSkilAttackData.z);// Á¶ÀÛ ¹æÁö ÄÚµå


	if (mCnt > 0)
	{
		//º¸³½ °ø°Ý·Â ±â·Ï
		Record_SendDamage(smWsockUserServer, TransSkilAttackData.Power);

		smWsockUserServer->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}


	//	lpsmSendSock = GetAreaServerSock();		//º¸³¾ ¼­¹ö±¸ÇÏ±â

	//	if ( lpsmSendSock ) {
	//		lpsmSendSock->Send2( (char *)&TransSkilAttackData , TransSkilAttackData.size , TRUE );
	//	}
	return TRUE;
}

//¹üÀ§Çü °ø°Ý
int Skil_RangeBoxAttack(smCHAR *lpCurChar, RECT *lpRect, int power, int TargetState, int UseAttackRating)
{

	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	int cnt, mCnt;
	int dx, dy, dz;
	int Attack, accuracy;
	smCHAR	*lpChar;
	smWINSOCK *lpsmSendSock = 0;


	//Å¬¶óÀÌ¾ðÆ® ·Î±× ÆÄÀÏ ±â·Ï
	Record_ClinetLogFile("±¸¹öÀü °ø°ÝÆÐÅ¶ - Skil_RangeBoxAttack ");
	quit = 1;
	return TRUE;


	mCnt = 0;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (mCnt >= SKIL_ATTACK_CHAR_MAX) break;
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY)
		{
			lpChar = &chrOtherPlayer[cnt];

			dx = (lpChar->pX - lpCurChar->pX) >> FLOATNS;
			dy = (lpChar->pY - lpCurChar->pY) >> FLOATNS;
			dz = (lpChar->pZ - lpCurChar->pZ) >> FLOATNS;

			// ÀÌµ¿µÈ Áö¿ª ÁÂÇ¥°ªÀ» ±¸ÇÏ´Â °ÍÀÌ±¸³ª! ³î¶ø´Ù.
			GetMoveLocation(dx, dy, dz, 0, (-lpCurChar->Angle.y)&ANGCLIP, 0);

			if (GeoResult_X > lpRect->left && GeoResult_X<lpRect->right  &&
				GeoResult_Z>lpRect->top && GeoResult_Z < lpRect->bottom && abs(dy) < 256)
			{

				Attack = TRUE;

				if (UseAttackRating)
				{
					accuracy = sinGetAccuracy(chrOtherPlayer[cnt].smCharInfo.Level, chrOtherPlayer[cnt].smCharInfo.Defence);
					if ((rand() % 100) > accuracy)
						Attack = FALSE;
				}

				if (Attack)
				{
					TransSkilAttackData.dwTarObjectSerial[mCnt++] = chrOtherPlayer[cnt].dwObjectSerial;

					//Ã³À½ °ø°ÝÇÏ´Â Ä³¸¯ÇÑÅ×¸¸ Ã¼·Â¹Ù Ãâ·Â
					if (lpChar->smCharInfo.Life[1] && lpChar->smCharInfo.Life[0] == lpChar->smCharInfo.Life[1])
					{
						lpChar->EnableStateBar = TRUE;
					}
				}
			}

		}
	}

	TransSkilAttackData.TargetCount = mCnt;

	TransSkilAttackData.code = smTRANSCODE_SKIL_ATTACKDATA;
	TransSkilAttackData.size = (sizeof(TRANS_SKIL_ATTACKDATA) - sizeof(DWORD)*SKIL_ATTACK_CHAR_MAX);
	TransSkilAttackData.size += sizeof(DWORD)*mCnt + 16;

	TransSkilAttackData.x = lpCurChar->pX;
	TransSkilAttackData.y = lpCurChar->pY;
	TransSkilAttackData.z = lpCurChar->pZ;

	TransSkilAttackData.AttackState = TargetState;
	TransSkilAttackData.AttackSize = 0;
	TransSkilAttackData.Power = power;

	LastAttackDamage = power;

	TransSkilAttackData.dwChkSum = TransSkilAttackData.Power * 2002 +
		(TransSkilAttackData.x * TransSkilAttackData.y * TransSkilAttackData.z);// Á¶ÀÛ ¹æÁö ÄÚµå


	memcpy(&Back_TransSkilAttackData, &TransSkilAttackData, sizeof(TRANS_SKIL_ATTACKDATA));		//¹é¾÷

	lpsmSendSock = GetAreaServerSock();		//º¸³¾ ¼­¹ö±¸ÇÏ±â

	if (lpsmSendSock && mCnt > 0)
	{
		//º¸³½ °ø°Ý·Â ±â·Ï
		Record_SendDamage(lpsmSendSock, TransSkilAttackData.Power);

		lpsmSendSock->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}
	return TRUE;
}


int Skill_CharRangeEffect(DWORD dwSkillCode, smCHAR *lpCurChar, smCHAR *lpCharTarget, int point)
{
	int time, rs;

	switch (dwSkillCode)
	{
		case SKILL_PLAY_ROAR:
			time = Roar_Time[point - 1];

			rs = lpCharTarget->smCharInfo.Resistance[sITEMINFO_BIONIC];
			if (rs < 0) rs = 0;
			if (rs >= 100) rs = 100;
			rs = 100 - rs;
			time = (time*rs) / 100;

			if (time)
			{
				if (time > 2) time = 2;
				AssaParticle_Stune(lpCharTarget, time);
				return TRUE;
			}
			break;

		case SKILL_PLAY_SOUL_SUCKER:
			if (lpCharTarget->smCharInfo.Brood != smCHAR_MONSTER_UNDEAD &&
				((lpCharTarget->smCharInfo.Life[1] * 50) / 100) < lpCharTarget->smCharInfo.Life[0])
			{

				sinSkillEffect_Soul_Sucker(lpCurChar, lpCharTarget);
				return TRUE;
			}
			break;

	}
	return FALSE;
}


//¹üÀ§Çü °ø°Ý ÀÌÆåÆ® ¿¬Ãâ
int Skil_RangeBoxEffect(smCHAR *lpCurChar, RECT *lpRect, DWORD dwSkillCode, int TargetState)
{

	int cnt, mCnt;
	int dx, dy, dz;
	smCHAR	*lpChar;

	mCnt = 0;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (mCnt >= SKIL_ATTACK_CHAR_MAX) break;
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY &&
			lpCurChar != &chrOtherPlayer[cnt])
		{

			lpChar = &chrOtherPlayer[cnt];

			dx = (lpChar->pX - lpCurChar->pX) >> FLOATNS;
			dy = (lpChar->pY - lpCurChar->pY) >> FLOATNS;
			dz = (lpChar->pZ - lpCurChar->pZ) >> FLOATNS;

			// ÀÌµ¿µÈ Áö¿ª ÁÂÇ¥°ªÀ» ±¸ÇÏ´Â °ÍÀÌ±¸³ª! ³î¶ø´Ù.
			GetMoveLocation(dx, dy, dz, 0, (-lpCurChar->Angle.y)&ANGCLIP, 0);

			if (GeoResult_X > lpRect->left && GeoResult_X<lpRect->right  &&
				GeoResult_Z>lpRect->top && GeoResult_Z < lpRect->bottom && abs(dy) < 256)
			{

				if (Skill_CharRangeEffect(dwSkillCode, lpCurChar, &chrOtherPlayer[cnt], 0) == TRUE)
				{
					mCnt++;
				}
			}

		}
	}

	return mCnt;
}

//¹üÀ§Çü °ø°Ý ÀÌÆåÆ® ¿¬Ãâ
int Skil_RangeEffect(smCHAR *lpCurChar, int Range, DWORD dwSkillCode, int TargetState, int point)
{

	int cnt, mCnt;
	int dx, dy, dz;
	smCHAR	*lpChar;
	int	dRange;

	dRange = Range*Range;

	mCnt = 0;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (mCnt >= SKIL_ATTACK_CHAR_MAX) break;
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY &&
			lpCurChar != &chrOtherPlayer[cnt])
		{

			lpChar = &chrOtherPlayer[cnt];

			dx = (lpChar->pX - lpCurChar->pX) >> FLOATNS;
			dy = (lpChar->pY - lpCurChar->pY) >> FLOATNS;
			dz = (lpChar->pZ - lpCurChar->pZ) >> FLOATNS;

			if ((dx*dx + dy*dy + dz*dz) < dRange)
			{
				if (Skill_CharRangeEffect(dwSkillCode, lpCurChar, &chrOtherPlayer[cnt], point) == TRUE)
				{
					mCnt++;
				}
			}

		}
	}

	return mCnt;
}




//ÄÚµå ¾ÏÈ£ Ã½Å©¼¶ ¸¸µé±â
DWORD EncodeParam(DWORD Param)
{
	return (Param + (Param << 4)) * 2002;
}



//»ó´ë Ä³¸¯ÅÍ¿¡ °ø°Ý °¡ÇÔ 
int QuickSendTransAttack(smCHAR *lpChar, int power, int AttackState, int Resistance)
{
	TRANS_ATTACKDATA	TransAttackData;
	smWINSOCK *lpsmSendSock = 0;
	int pow;

	//Å¬¶óÀÌ¾ðÆ® ·Î±× ÆÄÀÏ ±â·Ï
	Record_ClinetLogFile("±¸¹öÀü °ø°ÝÆÐÅ¶ - QuickSendTransAttack ");
	quit = 1;
	return TRUE;


	if (RestartPlayCount != 0) return FALSE;

	TransAttackData.code = smTRANSCODE_ATTACKDATA;
	TransAttackData.size = sizeof(TRANS_ATTACKDATA);

	TransAttackData.x = lpChar->pX;
	TransAttackData.y = lpChar->pY;
	TransAttackData.z = lpChar->pZ;

	TransAttackData.AttackState = AttackState;
	TransAttackData.AttackSize = 48 * fONE;
	TransAttackData.Power = power;

	if (Resistance)
	{
		TransAttackData.AttackState |= (Resistance << 16);
		pow = Resistance >> 4;
		if (pow > 0 && pow < 512)
		{
			TransAttackData.Power += pow;			//¼Ó¼º °ø°Ý·Â Ãß°¡ ( ¼­¹ö¿¡¼­ Ãß°¡µÈ °ø°Ý·Â µû·Î °è»ê )
		}
	}

	TransAttackData.dwDestObjectSerial = lpCurPlayer->dwObjectSerial;
	TransAttackData.dwTarObjectSerial = lpChar->dwObjectSerial;
	TransAttackData.dwChkSum = TransAttackData.Power * 2002 +
		(TransAttackData.x * TransAttackData.y * TransAttackData.z);// Á¶ÀÛ ¹æÁö ÄÚµå

	LastAttackDamage = TransAttackData.Power;

	//Ã³À½ °ø°ÝÇÏ´Â Ä³¸¯ÇÑÅ×¸¸ Ã¼·Â¹Ù Ãâ·Â
	if (lpChar->smCharInfo.Life[1] && lpChar->smCharInfo.Life[0] == lpChar->smCharInfo.Life[1])
	{
		lpChar->EnableStateBar = TRUE;
	}

	if (lpChar->ServerCode)
	{
		lpsmSendSock = GetServerSock(lpChar->ServerCode);		//¼­¹ö ÄÚµå ±¸ÇÏ±â
	}


	if (lpsmSendSock)
	{
		//º¸³½ °ø°Ý·Â ±â·Ï
		Record_SendDamage(lpsmSendSock, TransAttackData.Power);

		return lpsmSendSock->Send2((char *)&TransAttackData, TransAttackData.size, TRUE);
	}

	return NULL;
}

//»ó´ë Ä³¸¯ÅÍ¿¡ °ø°Ý °¡ÇÔ 
int QuickSendTransAttack(smCHAR *lpChar, int power)
{
	return QuickSendTransAttack(lpChar, power, 1, 0);
}
//»ó´ë Ä³¸¯ÅÍ¿¡ °ø°Ý °¡ÇÔ 
int QuickSendTransAttack(smCHAR *lpChar, int power, int Resistance)
{
	return QuickSendTransAttack(lpChar, power, 1, Resistance);
}

///////////////////////// Æ®·¹ÀÌµå °ü·Ã //////////////////////////////
/*
sTRADE sTrade;	   //º¸³¾ Á¤º¸
sTRADE sTradeRecv; //¹ÞÀº Á¤º¸
*/

//¾ÆÀÌÅÛ ±³È¯ ½ÅÃ»
int SendRequestTrade(int dwObjectSerial, int Flag)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_TRADE_REQUST;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectSerial;
	TransCommand.LParam = Flag;
	TransCommand.SParam = lpCurPlayer->dwObjectSerial;

	if (smWsockUserServer)
		return smWsockUserServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	return FALSE;
}

//¾ÆÀÌÅÛ ±³È¯ Á¤º¸ Àü¼Û
int SendTradeItem(sTRADE *lpTrade, DWORD dwSender)
{
	int cnt;
	int len;
	int	PotionError = 0;
	TRANS_TRADEITEMS TransTradeItems;
	sTRADE	sTradeBuff;

	len = 0;

	//Æ®·¹ÀÌµå Á¤º¸ ÀÎÁõ È®ÀÎ
	if (CheckTrade((void *)lpTrade) == FALSE)
	{
		SendRequestTrade(dwSender, 3);			//°Å·¡ Ãë¼Ò ¿ä±¸
		cTrade.CancelTradeItem();					//°Å·¡Ã¢ ´Ý±â
		return FALSE;
	}

	memcpy(&sTradeBuff, lpTrade, sizeof(sTRADE));

	//¹°¾à°¹¼ö ºñ±³
	PotionError = ComparePotion();

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (!sTradeBuff.TradeItem[cnt].Flag)
		{
			ZeroMemory(&sTradeBuff.TradeItem[cnt], sizeof(sITEM));
		}
		else
		{
			sTradeBuff.TradeItem[cnt].sItemInfo.ItemHeader.Head = 0;		//Å°°ª Áö¿ò
			if (PotionError)
			{
				sTradeBuff.TradeItem[cnt].sItemInfo.PotionCount = sTradeBuff.TradeItem[cnt].sItemInfo.ItemHeader.Head + 1;	//¹°¾à°¹¼ö ¿À·ù½Ã ¸ðµç Æ÷¼ÇÄ«¿îÆ®¸¦ 1·Î ¹Ù²Û´Ù
			}
			len++;
		}
	}

	//µ¥ÀÌÅ¸ ¾ÐÃà ( Z/NZ ¹æ½Ä )
	len = EecodeCompress((BYTE *)&sTradeBuff, TransTradeItems.TradeBuff, sizeof(sTRADE));
	if (len > TRANS_TRADE_BUFF_SIZE) return FALSE;

	TransTradeItems.code = smTRANSCODE_TRADEITEMS;
	TransTradeItems.size = len + 48;
	TransTradeItems.dwSum = 0;
	TransTradeItems.dwSender = lpCurPlayer->dwObjectSerial;
	TransTradeItems.dwRecver = dwSender;
	TransTradeItems.Temp[0] = 0;
	TransTradeItems.Temp[1] = 0;
	TransTradeItems.Temp[2] = 0;
	TransTradeItems.Temp[3] = 0;

	if (smWsockUserServer)
		return smWsockServer->Send2((char *)&TransTradeItems, TransTradeItems.size, TRUE);

	return FALSE;
}

//¾ÆÀÌÅÛ ±³È¯ Á¤º¸ ¼ö½Å 
int RecvTradeItems(TRANS_TRADEITEMS *lpTransTradeItems, sTRADE *lpTrade)
{
	//¾ÐÃà µ¥ÀÌÅ¸ ÇØµ¶ ( Z/NZ ¹æ½Ä )
	DecodeCompress(lpTransTradeItems->TradeBuff, (BYTE *)lpTrade);
	memcpy(&TransLastRecvTrade, lpTransTradeItems, lpTransTradeItems->size);		//¹ÞÀº Æ®·¹ÀÌµå µ¥ÀÌÅ¸ ÀÓ½Ã º¹»ç
	return TRUE;
}

//±³È¯Ã¢ ¾ÆÀÌÅÛ ¼Û½Å
int SendTradeItem(DWORD dwSender)
{
	return SendTradeItem(&sTrade, dwSender);
}
//±³È¯Ã¢ ¾ÆÀÌÅÛ ¼ö½Å
int RecvTradeItems(TRANS_TRADEITEMS *lpTransTradeItems)
{
	int res;

	res = RecvTradeItems(lpTransTradeItems, &sTradeRecv);
	cTrade.LoadTradeItemIamge();
	cTrade.CheckTradeButtonOk();

	ItemPickUpFlag = 1;

	//Æ®·¹ÀÌµå Á¤º¸ ÀÎÁõ È®ÀÎ
	if (CheckTrade(&sTradeRecv) == FALSE)
	{
		SendRequestTrade(lpTransTradeItems->dwSender, 3);	//±³È¯ Ãë¼Ò
		cTrade.CancelTradeItem();
		return FALSE;
	}

	if (sTrade.CheckFlag && !sTradeRecv.CheckFlag)
	{
		sTrade.CheckFlag = 0;
		TradeItemCancelCount++;
		if (TradeItemCancelCount >= 3)
		{
			//3¹ø Ãë¼Ò ´çÇÔ
			SendRequestTrade(lpTransTradeItems->dwSender, 3);	//±³È¯ Ãë¼Ò
			cTrade.CancelTradeItem();
			return FALSE;
		}
		else
		{
			cMessageBox.ShowMessage(MESSAGE_MOVE_TRADE_ITEM);
			SendTradeItem(lpTransTradeItems->dwSender);
		}
	}

	return res;
}

//¾ÆÀÌÅÛ ±³È¯ ÀÎÁõÅ° Àü¼Û
int SendTradeSucessKey(DWORD dwSender)
{
	//¾ÆÀÌÅÛ ±³È¯ ÀÎÁõÅ° Àü¼Û
	return SendTradeSucessKey(&sTrade, dwSender);
}



//-------> ÀÌÇÔ¼ö°¡ °ÅÀÇ ÅëÃ¤·Î ¸¹ÀÌ ¹Ù²åÀ¸´Ï ±¸¹öÀü Àû¿ë½Ã È®ÀÎ //////////////

//¾ÆÀÌÅÛ ±³È¯ ÀÎÁõÅ° Àü¼Û
int SendTradeSucessKey(sTRADE *lpTrade, DWORD dwSender)
{

	TRANS_TRADE_ITEMKEY	TransTradeItemKey;
	int cnt;
	int	TradeOpenFlag, CancelFlag;

	if (DisconnectFlag) return FALSE;

	TransTradeItemKey.code = smTRANSCODE_TRADE_ITEMKEY;
	TransTradeItemKey.size = sizeof(TRANS_TRADE_ITEMKEY);
	TransTradeItemKey.dwSender = lpCurPlayer->dwObjectSerial;;
	TransTradeItemKey.dwRecver = dwSender;
	TransTradeItemKey.Money = lpTrade->Money;


	TradeOpenFlag = cTrade.OpenFlag;
	cTrade.OpenFlag = 0;
	CancelFlag = 0;

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (lpTrade->TradeItem[cnt].Flag)
		{
			TransTradeItemKey.dwItemCode[cnt] = lpTrade->TradeItem[cnt].sItemInfo.CODE;
			TransTradeItemKey.dwItemKey[cnt] = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head;
			TransTradeItemKey.dwItemSum[cnt] = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;

			if (FindInvenItem(lpTrade->TradeItem[cnt].sItemInfo.CODE,
							  lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head,
							  lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum) != 0)
			{
				//º¸³¾ ¾ÆÀÌÅÛÀÌ ÀÎº¥¿¡ ³²¾Æ ÀÖÀ½À» ¹ß°ß
				CancelFlag = TRUE;

				//ÀÎº¥Åä¸® ¾ÆÀÌÅÛ º¹»ç ¿À·ù
				SendInvenItemError(1, lpTrade->TradeItem[cnt].sItemInfo.CODE,
								   lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head,
								   lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum);
			}

			if ((TransTradeItemKey.dwItemCode[cnt] & sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
			{
				//¹°¾àÀÌ´Ù! ( ¾ÆÀÌÅÛ Ã¼Å©¼¶¿¡ ¹°¾à°¹¼ö¸¦ ¼³Á¤ )
				TransTradeItemKey.dwItemSum[cnt] = lpTrade->TradeItem[cnt].sItemInfo.PotionCount & 0xFFFF;
				TransTradeItemKey.dwItemSum[cnt] |= (lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum) << 16;
				TransTradeItemKey.dwItemSum[cnt] ^= (TransTradeItemKey.dwItemSum[cnt] >> 16) ^ 0x5cbc;
			}

		}
		else
		{
			TransTradeItemKey.dwItemCode[cnt] = 0;
			TransTradeItemKey.dwItemKey[cnt] = 0;
			TransTradeItemKey.dwItemSum[cnt] = 0;
		}
	}

	cTrade.OpenFlag = TradeOpenFlag;

	if (!CancelFlag)
	{

		/*
				TRANS_THROW_ITEMS	TransThrowItems;

				TransThrowItems.ThrowItemCount = 0;

				for( cnt=0;cnt<MAX_TRADE_ITEM;cnt++) {
					if ( lpTrade->TradeItem[cnt].Flag ) {
						TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwCode   = lpTrade->TradeItem[cnt].sItemInfo.CODE;
						TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwHead   = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head;
						TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwChkSum = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;
						TransThrowItems.ThrowItemCount++;
						//SendSaveThrowItem( &lpTrade->TradeItem[cnt].sItemInfo );
					}
				}
				//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ÃÖ´ë 32°³ µ¿½Ã ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
				SendSaveThrowItems( &TransThrowItems );

				if ( smWsockServer )
					smWsockServer->Send2( (char *)&TransTradeItemKey , TransTradeItemKey.size , TRUE ) ;

				if ( smWsockUserServer && smWsockServer!=smWsockUserServer )
					smWsockUserServer->Send2( (char *)&TransTradeItemKey , TransTradeItemKey.size , TRUE ) ;
		*/
		if (smWsockDataServer)
			smWsockDataServer->Send2((char *)&TransTradeItemKey, TransTradeItemKey.size, TRUE);


		//¾ÆÀÌÅÛ ±³È¯Å°
		memcpy(&LastTransTradeItemKey, &TransTradeItemKey, sizeof(TRANS_TRADE_ITEMKEY));
		dwLastTransTradeKeyTime = dwPlayTime;
	}

	dwTradeMaskTime = dwPlayTime + 3000;

	return TRUE;
}


//¾ÆÀÌÅÛ ±³È¯ ÀÎÁõÅ° ¼ö½Å
int RecvTradeSucessKey(TRANS_TRADE_ITEMKEY *lpTransTradeItemKey, sTRADE *lpTrade)
{
	int cnt;
	sTRADE sChkTrade;
	DWORD	dwChkSum1, dwChkSum2;
	int	ItemFlag;

	dwTradeMaskTime = dwPlayTime + 3000;

	if (TransLastRecvTrade.code)
	{
		//¸¶Áö¸· ¹ÞÀº µ¥ÀÌÅ¸·Î ¾ÐÃà µ¥ÀÌÅ¸ ´Ù½Ã ÇØµ¶ ( Z/NZ ¹æ½Ä )
		DecodeCompress(TransLastRecvTrade.TradeBuff, (BYTE *)&sChkTrade);
		for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
		{
			memcpy(&lpTrade->TradeItem[cnt].sItemInfo, &sChkTrade.TradeItem[cnt].sItemInfo, sizeof(sITEMINFO));
		}
	}
	//cTrade.LoadTradeItemIamge();

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (lpTrade->TradeItem[cnt].Flag)
		{

			dwChkSum1 = lpTransTradeItemKey->dwItemSum[cnt];
			dwChkSum2 = lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;
			if ((lpTrade->TradeItem[cnt].sItemInfo.CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
			{
				//¹°¾àÀÎ°æ¿ì
				ItemFlag = 1;
				dwChkSum1 = dwChkSum1 >> 16;
				dwChkSum2 = dwChkSum2 & 0xFFFF;
			}
			else
				ItemFlag = 0;

			if (lpTransTradeItemKey->dwItemCode[cnt] == lpTrade->TradeItem[cnt].sItemInfo.CODE && dwChkSum1 == dwChkSum2)
			{
				//			lpTransTradeItemKey->dwItemSum[cnt]==lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum ) {

				lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head = lpTransTradeItemKey->dwItemKey[cnt];

				//Àß¸øµÈ ¾ÆÀÌÅÛ ¼ö½Å
				if (!lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head)
					lpTrade->TradeItem[cnt].Flag = 0;
				else
				{
					if (ItemFlag == 1)
					{
						//¹°¾à ÀçÀÎÁõ
						ReformItem(&lpTrade->TradeItem[cnt].sItemInfo);
					}
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if (lpTransTradeItemKey->dwItemCode[cnt])
				return FALSE;

		}
	}

	//SendRecordTradeItems( lpTrade );			//±³È¯ ³»¿ë ¼­¹ö¿¡ ±â·Ï

	return TRUE;
}



//¾ÆÀÌÅÛ ±³È¯ ³»¿ë È®ÀÎ
int SendTradeCheckItem(DWORD dwSender)
{

	TRANS_TRADE_CHECKITEM	TransTradeCheckItem;
	int cnt;

	if (DisconnectFlag) return FALSE;

	TransTradeCheckItem.code = smTRANSCODE_TRADE_READYITEM;
	TransTradeCheckItem.size = sizeof(TRANS_TRADE_CHECKITEM);
	TransTradeCheckItem.dwSender = lpCurPlayer->dwObjectSerial;
	TransTradeCheckItem.dwRecver = dwSender;

	if (sTrade.Money)
	{
		SendSaveMoney();			//µ· ÀúÀå ( º¹»ç¹æÁö )
	}

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (sTrade.TradeItem[cnt].Flag)
		{
			TransTradeCheckItem.dwSendItemCode[cnt] = sTrade.TradeItem[cnt].sItemInfo.CODE;
			TransTradeCheckItem.dwSendItemSum[cnt] = sTrade.TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;

			//SendSaveThrowItem( &sTrade.TradeItem[cnt].sItemInfo );		//¾ÆÀÌÅÛ º¹»ç ¹æÁö

			//¾ÆÀÌÅÛ ÀÎÁõ È®ÀÎ
			if (CheckItemForm(&sTrade.TradeItem[cnt].sItemInfo) == FALSE)
				return FALSE;		//Àß¸øµÈ ¾ÆÀÌÅÛ 

		}
		else
		{
			TransTradeCheckItem.dwSendItemCode[cnt] = 0;
			TransTradeCheckItem.dwSendItemSum[cnt] = 0;
		}
	}


	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (sTradeRecv.TradeItem[cnt].Flag)
		{
			TransTradeCheckItem.dwRecvItemCode[cnt] = sTradeRecv.TradeItem[cnt].sItemInfo.CODE;
			TransTradeCheckItem.dwRecvItemSum[cnt] = sTradeRecv.TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;
		}
		else
		{
			TransTradeCheckItem.dwRecvItemCode[cnt] = 0;
			TransTradeCheckItem.dwRecvItemSum[cnt] = 0;
		}
	}
	/*
		TRANS_THROW_ITEMS	TransThrowItems;
		TransThrowItems.ThrowItemCount = 0;
		for( cnt=0;cnt<MAX_TRADE_ITEM;cnt++) {
			if ( sTrade.TradeItem[cnt].Flag ) {
				TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwCode   = sTrade.TradeItem[cnt].sItemInfo.CODE;
				TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwHead   = sTrade.TradeItem[cnt].sItemInfo.ItemHeader.Head;
				TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwChkSum = sTrade.TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;
				TransThrowItems.ThrowItemCount++;
			}
		}
		//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ÃÖ´ë 32°³ µ¿½Ã ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
		SendSaveThrowItems( &TransThrowItems );
	*/

	if (smWsockServer)
		smWsockServer->Send2((char *)&TransTradeCheckItem, TransTradeCheckItem.size, TRUE);

	if (smWsockUserServer && smWsockServer != smWsockUserServer)
		smWsockUserServer->Send2((char *)&TransTradeCheckItem, TransTradeCheckItem.size, TRUE);

	dwTradeMaskTime = dwPlayTime + 3000;

	return TRUE;
}

//¾ÆÀÌÅÛ ±³È¯ ³»¿ë È®ÀÎ ¼ö½Å
int RecvTradeCheckItem(TRANS_TRADE_CHECKITEM *lpTransTradeCheckItem)
{
	int cnt;

	dwTradeMaskTime = dwPlayTime + 3000;

	if (DisconnectFlag) return FALSE;

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (sTrade.TradeItem[cnt].Flag)
		{
			if (lpTransTradeCheckItem->dwRecvItemCode[cnt] != sTrade.TradeItem[cnt].sItemInfo.CODE ||
				((lpTransTradeCheckItem->dwRecvItemCode[cnt] & sinITEM_MASK1) != (sinPM1&sinITEM_MASK1) &&
				 lpTransTradeCheckItem->dwRecvItemSum[cnt] != sTrade.TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum))
			{

				return FALSE;

			}
		}
		else
		{
			if (lpTransTradeCheckItem->dwRecvItemCode[cnt])
				return FALSE;
		}

		if (sTradeRecv.TradeItem[cnt].Flag)
		{
			if (lpTransTradeCheckItem->dwSendItemCode[cnt] != sTradeRecv.TradeItem[cnt].sItemInfo.CODE ||
				((lpTransTradeCheckItem->dwSendItemCode[cnt] & sinITEM_MASK1) != (sinPM1&sinITEM_MASK1) &&
				 lpTransTradeCheckItem->dwSendItemSum[cnt] != sTradeRecv.TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum))
			{

				return FALSE;

			}
		}
		else
		{
			if (lpTransTradeCheckItem->dwSendItemCode[cnt])
				return FALSE;
		}
	}

	return TRUE;
}







//ÆÄÆ¼ Çã¶ô 
int SendJoinPartyUser(DWORD dwObjectCode)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_PARTY_JOIN;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectCode;
	TransCommand.LParam = 0;
	TransCommand.SParam = 0;

	return smWsockUserServer->Send((char *)&TransCommand, TransCommand.size, TRUE);
}

//ÆÄÆ¼ ÀÎÅÍÆäÀÌ½º ¹öÆ°ÀÔ·Â 
int	PartyButton(int Num, int Button)
{
	// Num ÆÄÆ¼¿ø ¹øÈ£
	// Button  0-ÀÌÀÓ 1-ÇØ»ê 2-Ãß¹æ 3-Å»Åð
	DWORD	dwCode;
	smTRANS_COMMAND	smTransCommand;

	if (!InterfaceParty.PartyMemberCount) return FALSE;

	dwCode = 0;

	if (Num >= 0)
		dwCode = InterfaceParty.PartyMember[Num].ChrCode;
	else
		dwCode = 0;

	smTransCommand.size = sizeof(smTRANS_COMMAND);
	smTransCommand.code = smTRANSCODE_PARTY_COMMAND;
	smTransCommand.WParam = dwCode;
	smTransCommand.LParam = Button;
	smTransCommand.SParam = 0;

	if (Button < 3 && InterfaceParty.PartyMember[0].ChrCode != lpCurPlayer->dwObjectSerial)
	{
		//ÆÄÆ¼Àå ±ÇÇÑ
		return FALSE;
	}

	return smWsockUserServer->Send((char *)&smTransCommand, smTransCommand.size, TRUE);
}





////////////////////////// µ·°ú °æÇèÄ¡ ÀÌµ¿ ±â·Ï ///////////////

int	SetTotalSubMoney(int Money)
{
	TotalSubMoney += Money;

	return TRUE;
}

int	SetTotalAddMoney(int Money)
{
	TotalAddMoney += Money;

	return TRUE;
}

int	SetTotalAddExp(int Exp)
{
	TotalAddExp += Exp;

	return TRUE;
}

int	SetTotalSubExp(int Exp)
{
	TotalSubExp += Exp;

	return TRUE;
}

int	GetTotalMoney()
{
	int m;

	m = sServerMoney[0].InputMoney + sServerMoney[1].InputMoney + sServerMoney[2].InputMoney + WareHouseSubMoney + TotalAddMoney - TotalSubMoney;

	return m;
}

int GetTotalSubExp()
{
	int m;

	m = (sServerExp[0].InputExp + sServerExp[1].InputExp + sServerExp[2].InputExp) - (TotalAddExp - TotalSubExp);

	return m;
}

int GetTotalExp()
{
	int m;

	m = sServerExp[0].InputExp + sServerExp[1].InputExp + sServerExp[2].InputExp + TotalAddExp - TotalSubExp;

	return m;
}

//Ã¢°í ÀúÀå
int	SaveWareHouse(sWAREHOUSE *lpWareHouse, TRANS_WAREHOUSE *lpTransWareHouse)
{
	TRANS_WAREHOUSE	TransWareHouse;
	//TRANS_THROW_ITEMS	TransThrowItems;
	sWAREHOUSE	WareHouseCheck;
	int	CompSize;
	int cnt;
	DWORD	dwChkSum;
	char	*szComp1, *szComp2;
	int	flag;

	if (lpTransWareHouse) flag = 1;
	else flag = 0;

	if (!flag && QuitSave)	return FALSE;

	for (cnt = 0; cnt < 100; cnt++)
	{
		if (!lpWareHouse->WareHouseItem[cnt].Flag)
		{
			ZeroMemory(&lpWareHouse->WareHouseItem[cnt], sizeof(sITEM));
		}
	}


	CompSize = EecodeCompress((BYTE *)lpWareHouse, (BYTE *)TransWareHouse.Data, sizeof(sWAREHOUSE), sizeof(sITEM) * 100);


	//¿ë·® Ã½Å©
	if (!flag && CompSize > (smSOCKBUFF_SIZE - 256))
		return FALSE;

	if (flag && CompSize > (smSOCKBUFF_SIZE - 140))
		return FALSE;


	DecodeCompress((BYTE *)TransWareHouse.Data, (BYTE *)&WareHouseCheck, sizeof(sWAREHOUSE));

	szComp1 = (char *)lpWareHouse;
	szComp2 = (char *)&WareHouseCheck;

	dwChkSum = 0;

	for (cnt = 0; cnt < sizeof(sWAREHOUSE); cnt++)
	{
		if (szComp1[cnt] != szComp2[cnt])
		{
			return FALSE;
		}
		else
		{
			dwChkSum += szComp1[cnt] * (cnt + 1);
		}
	}


	//Ã¢°í°¡ ¿­·ÈÀ»¶§¿Í ´ÝÇûÀ»¶§ÀÇ µ·ÀÇ Â÷¸¦ °Ë»çÇÏ±â À§ÇÑ º¯¼ö
	WareHouseSubMoney += (CompWareHouseMoney - lpWareHouse->Money);
	CompWareHouseMoney = 0;
	/*
		if ( !flag ) {
			//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
			SendSaveMoney();				//µ·

			TransThrowItems.ThrowItemCount = 0;
			for( cnt=0;cnt<THROW_ITEMS_MAX;cnt++) {
				if ( lpWareHouse->WareHouseItem[cnt].Flag ) {
					TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwCode   = lpWareHouse->WareHouseItem[cnt].sItemInfo.CODE;
					TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwHead   = lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.Head;
					TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwChkSum = lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwChkSum;
					TransThrowItems.ThrowItemCount++;
				}
			}
			//µ¥ÀÌÅ¸ ¼­¹ö¿¡ ¾ÆÀÌÅÛ ÀúÀå ¿ä±¸ ÃÖ´ë 32°³ µ¿½Ã ( º¹»ç ¾ÆÀÌÅÛ ¹æÁö )
			SendSaveThrowItems( &TransThrowItems );
		}
	*/

	TransWareHouse.code = smTRANSCODE_WAREHOUSE;
	TransWareHouse.size = sizeof(TRANS_WAREHOUSE) - (sizeof(sWAREHOUSE) - CompSize);
	TransWareHouse.DataSize = CompSize;
	TransWareHouse.dwChkSum = dwChkSum;
	TransWareHouse.wVersion[0] = Version_WareHouse;
	TransWareHouse.wVersion[1] = 0;

	if (!flag)
	{
		//µ· ÀúÀå ( ¼­¹ö¿¡¼­ »ç¿ëÇÒ ³»¿ë / ºÒ·¯¿Ã¶§´Â ¼­¹ö¿¡¼­ ÃÊ±âÈ­ ÇØÁà¾ß ÇÔ )
		TransWareHouse.WareHouseMoney = lpWareHouse->Money ^ (dwChkSum^smTRANSCODE_WAREHOUSE);
		TransWareHouse.UserMoney = lpCurPlayer->smCharInfo.Money ^ (dwChkSum^smTRANSCODE_WAREHOUSE);
	}

	TransWareHouse.dwTemp[0] = 0;
	TransWareHouse.dwTemp[1] = 0;
	TransWareHouse.dwTemp[2] = 0;
	TransWareHouse.dwTemp[3] = 0;
	TransWareHouse.dwTemp[4] = 0;

	dwLastWareHouseChkSum = dwChkSum;				//ÃÖÁ¾ ÀúÀå Ã¢°í ÄÚµå

	if (flag)
	{
		memcpy(lpTransWareHouse, &TransWareHouse, sizeof(TRANS_WAREHOUSE));
		return TRUE;
	}

	if (TransWareHouse.size > smSOCKBUFF_SIZE) TransWareHouse.size = smSOCKBUFF_SIZE;

	if (smWsockDataServer)
	{
		smWsockDataServer->Send2((char *)&TransWareHouse, TransWareHouse.size, TRUE);

		SaveGameData();						//°ÔÀÓÀ» ÀúÀå
		return TRUE;
	}

	return FALSE;
}
//Ã¢°í ÀúÀå
int	SaveWareHouse(sWAREHOUSE *lpWareHouse)
{
	return SaveWareHouse(lpWareHouse, 0);
}

//Ã¢°í °¡Á®¿À±â
int	LoadWareHouse(TRANS_WAREHOUSE *lpTransWareHouse, sWAREHOUSE *lpWareHouse, int flag)
{
	sWAREHOUSE	WareHouseCheck;
	int cnt;
	DWORD	dwChkSum;
	char	*szComp;
	char	szMsgBuff[128];

	if (lpTransWareHouse->DataSize == 0)
	{
		//ÃÊ±â°ª ¾Æ¹«°Íµµ ¾øÀ½
		ZeroMemory(lpWareHouse, sizeof(sWAREHOUSE));
		lpWareHouse->Money = 2023;
		lpWareHouse->Weight[0] = 197;
		CompWareHouseMoney = lpWareHouse->Money;	//Ã¢°í°¡ ¿­·ÈÀ»¶§¿Í ´ÝÇûÀ»¶§ÀÇ µ·ÀÇ Â÷¸¦ °Ë»çÇÏ±â À§ÇÑ º¯¼ö
		return TRUE;
	}

	DecodeCompress((BYTE *)lpTransWareHouse->Data, (BYTE *)&WareHouseCheck);

	dwChkSum = 0;

	szComp = (char *)&WareHouseCheck;

	for (cnt = 0; cnt < sizeof(sWAREHOUSE); cnt++)
	{
		dwChkSum += szComp[cnt] * (cnt + 1);
	}
	if (dwChkSum == lpTransWareHouse->dwChkSum)
	{

		if (!flag && dwLastWareHouseChkSum && dwLastWareHouseChkSum != lpTransWareHouse->dwChkSum && !smConfig.DebugMode)
		{
			//¸¶Áö¸· ÀúÀåÇÑ Ã¢°í µ¥ÀÌÅ¸¿Í ½Å±Ô ·Îµå µ¥ÀÌÅ¸ÀÇ ÄÚµå°¡ Æ²¸²
			return FALSE;
		}

		memcpy(lpWareHouse, &WareHouseCheck, sizeof(sWAREHOUSE));

		CompWareHouseMoney = lpWareHouse->Money;	//Ã¢°í°¡ ¿­·ÈÀ»¶§¿Í ´ÝÇûÀ»¶§ÀÇ µ·ÀÇ Â÷¸¦ °Ë»çÇÏ±â À§ÇÑ º¯¼ö

		for (cnt = 0; cnt < 100; cnt++)
		{
			if (lpWareHouse->WareHouseItem[cnt].Flag)
			{

				//2Â÷ º¸¾È °ª
				if (!lpWareHouse->WareHouseItem[cnt].sItemInfo.Temp0)
					lpWareHouse->WareHouseItem[cnt].sItemInfo.Temp0 = lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwChkSum + lpWareHouse->WareHouseItem[cnt].sItemInfo.CODE;

				//CheckCopyItem( &lpWareHouse->WareHouseItem[cnt].sItemInfo );		//º¹»ç ¶Ç´Â ºÒ·®¾ÆÀÌÅÛ °Ë»ç

				if (!lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwTime)
				{
					//ÄÚµå À§Àå
					lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwTime =
						(lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.Head >> 2) ^ (lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwChkSum << 2);
				}

				//¼ÛÆí È®ÀÎ
				if ((lpWareHouse->WareHouseItem[cnt].sItemInfo.CODE&sinITEM_MASK2) == sinSP1)
				{
					//¼ÛÆí ( Æ÷¼ÇÄ«¿îÅÍ°¡ ÀÖÀ¸¸é ¹®Á¦ÀÖ´Â ¼ÛÆí )
					if (lpWareHouse->WareHouseItem[cnt].sItemInfo.PotionCount > 1)	lpWareHouse->WareHouseItem[cnt].Flag = 0;

					// Àåº° - SP1 ¾ÆÀÌÅÛ Ã¢°í¿¡¼­ »ç¶óÁö´Â ¹®Á¦
					/*
					//»ý¼º±â°£ È®ÀÎ
					if ( lpWareHouse->WareHouseItem[cnt].sItemInfo.dwCreateTime<(sinItemTime-(60*60*24*12)) ||
						lpWareHouse->WareHouseItem[cnt].sItemInfo.dwCreateTime>(sinItemTime+(60*60*24*7)) ) {
							lpWareHouse->WareHouseItem[cnt].Flag =0;
						}
						*/
				}

				if (DeleteEventItem_TimeOut(&lpWareHouse->WareHouseItem[cnt].sItemInfo) == TRUE)
				{
					//ÀÌº¥Æ® ¾ÆÀÌÅÛ ³¯Â¥¸ÂÃç Á¦°Å
					lpWareHouse->WareHouseItem[cnt].Flag = 0;
					wsprintf(szMsgBuff, mgItemTimeOut, lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemName);
					AddChatBuff(szMsgBuff, 0);
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

//Ã¢°í °¡Á®¿À±â
int	LoadWareHouse(TRANS_WAREHOUSE *lpTransWareHouse, sWAREHOUSE *lpWareHouse)
{
	return LoadWareHouse(lpTransWareHouse, lpWareHouse, 0);
}

//ÀÌº¥Æ® Âü°¡ºñ º¸³¿
int SendCollectMoney(int Money)
{
	/*
		smTRANS_COMMAND	TransCommand;

		TransCommand.code =	smTRANSCODE_COLLECTMONEY;
		TransCommand.size = sizeof(smTRANS_COMMAND);
		TransCommand.WParam = Money;							//±âºÎÇÑ µ·
		TransCommand.LParam = lpCurPlayer->smCharInfo.Money;		//º¸À¯ÇÑ µ·
		TransCommand.SParam = 0;

		AddInvenMoney( -Money );				//ÀÎº¥ µ· È®ÀÎ

		if ( smWsockServer ) {
			return smWsockServer->Send2( (char *)&TransCommand , TransCommand.size , TRUE );
		}
	*/

	sITEM	sItemMoney;
	int j;


	memset(&sItemMoney, 0, sizeof(sITEM));
	sItemMoney.sItemInfo.CODE = sinGG1 | sin01;
	for (j = 0; j < MAX_ITEM; j++)
	{
		if (sItem[j].CODE == sItemMoney.sItemInfo.CODE)
		{ //ÄÚÀÎÀÌ¸é 
			memcpy(&sItemMoney, &sItem[j], sizeof(sITEM));
			sItemMoney.sItemInfo.Money = Money;			//±Ý¾×À» ³Ñ°ÜÁØ´Ù 
			sItemMoney.sItemInfo.ItemKindCode = smTRANSCODE_COLLECTMONEY;
			ReformItem(&sItemMoney.sItemInfo);		//ÀÎÁõ 
			if (sinThrowItemToFeild(&sItemMoney))
			{
				AddInvenMoney(-Money);				//ÀÎº¥ µ· È®ÀÎ
				return TRUE;
			}
		}
	}

	return FALSE;
}

//PK´çÇØ¼­ Á×À½
int PlayerKilling(DWORD dwObjectSerial)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_PLAYERKILLING;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectSerial;				//»ó´ë¹æ ÄÚµå
	TransCommand.LParam = lpCurPlayer->dwObjectSerial;		//³» ÄÚµå
	TransCommand.SParam = 0;

	if (smWsockUserServer)
	{
		return smWsockUserServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return FALSE;
}



//¹Í½ºÃÄ µ¥ÀÌÅ¸¸¦ ¼­¹ö·Î º¸³¿
int SendCraftItemToServer(void *lpsCraftItem_Send)
{
	Log::Debug("SendCraftItemToServer : Requisitado porém foi removido.");
	return NULL;
}
//À§ÇÁ¿ë À® ¾ÆÀÌÅÛ µ¥ÀÌÅ¸¸¦ ¼­¹ö·Î º¸³¿
int SendWingItemToServer(void *lpsCraftItem_Send)
{
	Log::Debug("SendWingItemToServer : Requisitado porém foi removido.");
	return NULL;
}

//¿¡ÀÌÂ¡ µ¥ÀÌÅ¸¸¦ ¼­¹ö·Î º¸³¿
int SendAgingItemToServer(void *lpsCraftItem_Send)
{
	Log::Debug("SendAgingItemToServer : Requisitado porém foi removido.");
	return NULL;
}
//¸µÅ©ÄÚ¾î µî·Ï º¸³¿
int SendLinkCoreToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendLinkCoreToServer : Requisitado porém foi removido.");
	return NULL;
}
int SucessLinkCore(smTRANS_COMMAND_EX *lpTransCommandEx)
{
	int x, z, cnt;

	sITEMINFO	*lpItem;

	//ÀÎº¥Åä¸®ÀÇ ÁöÁ¤ÇÑ ¾ÆÀÌÅÛÀ» Ã£¾Æ °Ë»öÇÏ¿© Ã£´Â´Ù ( ¸µÅ© ÄÚ¾î Á¦°Å )
	lpItem = FindInvenItem(lpTransCommandEx->WxParam, lpTransCommandEx->LxParam, lpTransCommandEx->SxParam);

	if (lpItem)
	{
		SendUseItemCodeToServer(lpItem);
		cInvenTory.DeleteInvenItemToServer(lpTransCommandEx->WxParam, lpTransCommandEx->LxParam, lpTransCommandEx->SxParam);
		ResetInvenItemCode();				//ÀÌº¥ ¾ÆÀÌÅÛ ÄÚµå ÃÊ±âÈ­

		if (FindInvenItem(lpTransCommandEx->WxParam, lpTransCommandEx->LxParam, lpTransCommandEx->SxParam) != 0) return FALSE;

	}
	else
		return FALSE;


	CloseEachPlayer();

	WarpFieldNearPos(lpTransCommandEx->EParam, lpTransCommandEx->WParam, lpTransCommandEx->SParam, &x, &z);

	lpCurPlayer->SetPosi(lpTransCommandEx->WParam, lpTransCommandEx->LParam, lpTransCommandEx->SParam, 0, 0, 0);
	TraceCameraPosi.x = lpCurPlayer->pX;
	TraceCameraPosi.y = lpCurPlayer->pY;
	TraceCameraPosi.z = lpCurPlayer->pZ;
	TraceTargetPosi.x = lpCurPlayer->pX;
	TraceTargetPosi.y = lpCurPlayer->pY;
	TraceTargetPosi.z = lpCurPlayer->pZ;

	//¹Ù´Ú È®ÀÎ
	lpCurPlayer->OnStageField = -1;
	if (smGameStage[0])
	{
		cnt = smGameStage[0]->GetFloorHeight(lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ, lpCurPlayer->Pattern->SizeHeight);
		lpCurPlayer->OnStageField = 0;
	}
	if (cnt == CLIP_OUT && smGameStage[1])
	{
		lpCurPlayer->OnStageField = 1;
	}

	StartEffect(lpCurPlayer->pX, lpCurPlayer->pY + 48 * fONE, lpCurPlayer->pZ, EFFECT_RETURN1);
	SkillPlaySound(SKILL_SOUND_LEARN, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);			//½ºÅ³ È¿°úÀ½
	RestartPlayCount = 350;		//5ÃÊ µ¿¾È ¹«Àû

	return TRUE;
}


//¿¡ÀÌÂ¡ ¿Ï·áµÈ ¾ÆÀÌÅÛÀ» ¼­¹ö·Î º¸³¿
int SendAgingUpgradeItemToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendAgingUpgradeItemToServer : Requisitado porém foi removido.");
	return NULL;
}
/*
//¾ÆÀÌÅÛ È®ÀÎ
#define smTRANSCODE_CHECKITEM			0x50320300
//¾ÆÀÌÅÛ ¿À·ù È®ÀÎ
#define smTRANSCODE_ERRORITEM			0x50320310

struct TRANS_ITEMINFO {
	int	size,code;

	//char	szItemName[32];
	//DWORD	ItemCode;

	sITEMINFO	Item;

	int x,y,z;

};

struct	TRANS_ITEM_CODE {
	int size,code;

	DWORD	dwFlag;

	DWORD	dwItemCode;
	DWORD	dwHead;
	DWORD	dwChkSum;
	DWORD	dwTime;
}


*/


//ÀÎº¥Åä¸®ÀÇ ¹°¾à°¹¼ö¸¦ ÀüºÎ 1·Î ¼¼ÆÃ
int ClearInvenPotionCount()
{
	int cnt;

	for (cnt = 0; cnt < INVENTORY_MAXITEM; cnt++)
	{
		if (cInvenTory.InvenItem[cnt].Flag && (cInvenTory.InvenItem[cnt].sItemInfo.CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		{
			cInvenTory.InvenItem[cnt].sItemInfo.PotionCount = 1;
		}
	}

	for (cnt = 0; cnt < INVENTORY_MAXITEM; cnt++)
	{
		if (cInvenTory.InvenItemTemp[cnt].Flag && (cInvenTory.InvenItemTemp[cnt].sItemInfo.CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		{
			cInvenTory.InvenItemTemp[cnt].sItemInfo.PotionCount = 1;
		}
	}

	if (cTrade.OpenFlag)
	{
		for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
		{
			if (sTrade.TradeItem[cnt].Flag && (sTrade.TradeItem[cnt].sItemInfo.CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
			{
				sTrade.TradeItem[cnt].sItemInfo.PotionCount = 1;
			}
		}
	}

	if (MouseItem.Flag && (MouseItem.sItemInfo.CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		MouseItem.sItemInfo.PotionCount = 1;

	cInvenTory.ReFormPotionNum();

	SaveGameData();

	return TRUE;
}

//ÀÎº¥Åä¸®ÀÇ ¾ÆÀÌÅÛ Ã£±â (ÄÚµå)
sITEMINFO *FindItemFromCode(DWORD dwItemCode)
{
	int cnt;

	for (cnt = 0; cnt < INVENTORY_MAXITEM; cnt++)
	{
		if (cInvenTory.InvenItem[cnt].Flag && cInvenTory.InvenItem[cnt].sItemInfo.CODE == dwItemCode)
		{
			return &cInvenTory.InvenItem[cnt].sItemInfo;
		}
	}
	for (cnt = 0; cnt < INVENTORY_MAXITEM; cnt++)
	{
		if (cInvenTory.InvenItemTemp[cnt].Flag && cInvenTory.InvenItemTemp[cnt].sItemInfo.CODE == dwItemCode)
		{
			return &cInvenTory.InvenItemTemp[cnt].sItemInfo;
		}
	}

	return NULL;
}



DWORD	dwLastCheckItemTime = 0;

//¾ÆÀÌÅÛÀ» ¼­¹ö¿¡ º¸³»¼­ È®ÀÎ
int	SendCheckItemToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendCheckItemToServer : Requisitado porém foi removido.");
	return NULL;
}
//¾ÆÀÌÅÛ È®ÀÎ °á°ú µµÂø
int RecvCheckItemFromServer(TRANS_ITEM_CODE *lpTransItemCode)
{
	Log::Debug("RecvCheckItemFromServer : Requisitado porém foi removido.");
	return NULL;
}
//ÆÇ¸ÅÇÒ ¾ÆÀÌÅÛÀ» ¼­¹ö·Î º¸³¿
int	SellItemToServer(sITEMINFO *lpItem, int Count)
{
	Log::Debug("SellItemToServer : Requisitado porém foi removido.");
	return NULL;
}



int Chk_InventPosCount = 1;
DWORD	dwCheckInvenItemTime = 0;

//ÀÎº¥Åä¸®ÀÇ ÀåÂøµÈ ¾ÆÀÌÅÛÀ» ¼­¹ö·Î º¸³»¼­ °Ë»ç
int CheckInvenItemToServer()
{
	Log::Debug("CheckInvenItemToServer : Requisitado porém foi removido.");
	return NULL;
}

////////////////////////////// ¸Þ¸ð¸® Å¸ÀÓ Ã½Å© ¾²·¹µå ///////////////////////////////

HANDLE	hTimeCheckThread = 0;
int		TimeCheckCounter;

DWORD WINAPI TimeCheckThread(void *pInfo)
{
	HANDLE	hThread;
	DWORD	dwTime;
	DWORD	dwLastTime;
	int		cnt;

	hThread = GetCurrentThread();
	dwTime = GetCurrentTime();
	dwLastTime = dwTime;
	TimeCheckCounter = 0;

	while (1)
	{
		dwTime = GetCurrentTime();
		cnt = dwTime - dwLastTime;
		if (cnt > PlayTimerMax) PlayTimerMax = cnt;
		dwLastTime = dwTime;
		TimeCheckCounter++;
		Sleep(800);
	}

	ExitThread(TRUE);
	return TRUE;
}


//·ÎµùÁß È­¸é Ç¥½Ã
int OpenTimeCheckThread()
{
	DWORD	dwTimeCheckThreadID;

	if (!hTimeCheckThread)
	{
		hTimeCheckThread = CreateThread(NULL, 0, TimeCheckThread, 0, 0, &dwTimeCheckThreadID);
	}

	return TRUE;
}

int	PlayTimerMax_SendingCount = 0;

int SendPlayTimerMax()
{

	smTRANS_COMMAND	smTransCommand;

	if (PlayTimerMax >= 10000 || (PlayTimerMax_SendingCount & 0x7) == 0)
	{

		smTransCommand.size = sizeof(smTRANS_COMMAND);
		smTransCommand.code = smTRANSCODE_PROCESS_TIMEMAX;
		smTransCommand.WParam = PlayTimerMax;
		smTransCommand.LParam = TimeCheckCounter;
		smTransCommand.SParam = 0;
		smTransCommand.EParam = 0;

		if (smWsockDataServer)
			smWsockDataServer->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

		PlayTimerMax = 0;
	}

	PlayTimerMax_SendingCount++;

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////




//¼­¹ö¿¡¼­ ¸Þ¸ð¸® °ªÀ» Á¤ÇØ¼­ °¡Á®°£´Ù
DWORD funcCheckMemSum(DWORD FuncPoint, int count)
{
	int cnt;
	DWORD dwSum;
	int	xcnt;
	DWORD *lpGetPrtectPoint;


	lpGetPrtectPoint = (DWORD *)FuncPoint;

	dwSum = 0;
	xcnt = 1;

	for (cnt = 0; cnt < count; cnt++)
	{
		xcnt += cnt;
		dwSum += lpGetPrtectPoint[cnt] * xcnt;
	}

	return dwSum;
}

/*
		dwResult = fnChkMem( TransFuncMem->Param[0] , TransFuncMem->Param[1] );

		smTransCommand.code = smTRANSCODE_FUNCTON_RESULT;
		smTransCommand.size = sizeof(smTRANS_COMMAND);
		smTransCommand.WParam = TransFuncMem->Param[0];
		smTransCommand.LParam = TransFuncMem->Param[1];
		smTransCommand.SParam = TransFuncMem->Param[2];
		smTransCommand.EParam = dwResult;

*/
//¼­¹ö¿¡¼­ ¸Þ¸ð¸® °ªÀ» Á¤ÇØ¼­ °¡Á®°£´Ù
//DWORD funcCheckMemSum2( DWORD FuncPoint , int count )

///////////////////////////////////////////////////////////////////////////////

DWORD	dwFuncList[][2] = {
	{ (DWORD)funcCheckMemSum << 1				,	(DWORD)512		},
	{ (DWORD)Check_PlaySubReleaseFunc << 1	,	(DWORD)512		},
	{ (DWORD)PlayPat3D << 1					,	(DWORD)2048	},
	{ (DWORD)Check_CodeSafe << 1				,	(DWORD)1024	},
	{ (DWORD)AddExp << 1						,	(DWORD)512		},
	{ (DWORD)sinSetLife << 1					,	(DWORD)512		},
	{ (DWORD)NetWorkPlay << 1					,	(DWORD)2800	},

	{ (DWORD)E_Shield_BlockRate << 1			,	(DWORD)((Meteo_UseMana - E_Shield_BlockRate) + 10) },		//½ºÅ³ Å×ÀÌºí º¸¾È

	{ (DWORD)Code_VRamBuffOpen << 1			,	(DWORD)3600	},			//¸¶Áö¸· ²¨´Â º°µµ Ã½Å© ÇÑ´Ù
	{ (DWORD)0								,	(DWORD)0 }
};



//Å¬¶óÀÌ¾ðÆ® Æã¼Ç À§Ä¡¿Í °ªÀ» ¼­¹ö¿¡ º¸°í
int	SendClientFuncPos()
{

	TRANS_CLIENT_FUNPOS	TransClientFuncPos;
	int cnt;


	ZeroMemory(&TransClientFuncPos, sizeof(TRANS_CLIENT_FUNPOS));

	TransClientFuncPos.code = smTRANSCODE_CLIENT_FUNCPOS;
	TransClientFuncPos.size = sizeof(TRANS_CLIENT_FUNPOS);
	TransClientFuncPos.ClientVersion = Client_Version;

	cnt = 0;
	TransClientFuncPos.dwFunc_CheckMemSum = dwFuncList[cnt][0] >> 1;
	TransClientFuncPos.dwFuncValue[cnt].dwFunc = dwFuncList[cnt][0] >> 1;
	TransClientFuncPos.dwFuncValue[cnt].dwLen = dwFuncList[cnt][1];
	TransClientFuncPos.dwFuncValue[cnt].dwChkSum = funcCheckMemSum(TransClientFuncPos.dwFuncValue[cnt].dwFunc, TransClientFuncPos.dwFuncValue[cnt].dwLen);
	//TransClientFuncPos.dwFuncValue[cnt].dwChkSum = fnChkMem( TransClientFuncPos.dwFuncValue[cnt].dwFunc , TransClientFuncPos.dwFuncValue[cnt].dwLen );

	while (1)
	{

		if (dwFuncList[cnt][0] == 0) break;
		if (cnt >= dwFUNC_VALUE_MAX)  break;

		TransClientFuncPos.dwFuncValue[cnt].dwFunc = dwFuncList[cnt][0] >> 1;
		TransClientFuncPos.dwFuncValue[cnt].dwLen = dwFuncList[cnt][1];
		TransClientFuncPos.dwFuncValue[cnt].dwChkSum = funcCheckMemSum(TransClientFuncPos.dwFuncValue[cnt].dwFunc, TransClientFuncPos.dwFuncValue[cnt].dwLen);

		cnt++;
	}

	TransClientFuncPos.FuncCount = cnt;

	//¼­¹ö¿¡ º¸°í
	if (smWsockDataServer)
	{
		return smWsockDataServer->Send2((char *)&TransClientFuncPos, TransClientFuncPos.size, TRUE);
	}

	return FALSE;
}


//¸Þ¸ð¸® °Ë»ç ¸ðµâ ¹ÞÀ½
int	RecvMemFuncData(TRANS_FUNC_MEMORY *TransFuncMem)
{
	smTRANS_COMMAND	smTransCommand;

	fnChkMem = (LPFN_CheckMem)((void *)TransFuncMem->szData);

	smTransCommand.size = 0;

	fnChkMem(TransFuncMem, &smTransCommand);

	if (smWsockDataServer && smTransCommand.size > 0)
		smWsockDataServer->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

	return TRUE;
}


////////////////////////////// ¾ÆÀÌÅÛ ÀÓÁö ÀúÀå Å¥Ã³¸® //////////////////////////

#define	RECV_TRANSITEM_QUE_MAX			256
#define	RECV_TRANSITEM_QUE_MASK			255

TRANS_ITEMINFO	Recv_TransItemInfo_Que[RECV_TRANSITEM_QUE_MAX];

int	RecvTransItemQue_Push = 0;
int	RecvTransItemQue_Pop = 0;

//¾ÆÀÌÅÛ Å¥¿¡ ÀÓ½Ã ÀúÀå
int	PushRecvTransItemQue(TRANS_ITEMINFO *lpTransItemInfo)
{
	int mCnt;

	mCnt = RecvTransItemQue_Push & RECV_TRANSITEM_QUE_MASK;

	memcpy(&Recv_TransItemInfo_Que[mCnt], lpTransItemInfo, sizeof(TRANS_ITEMINFO));
	RecvTransItemQue_Push++;
	RecvTransItemQue_Pop = RecvTransItemQue_Push - RECV_TRANSITEM_QUE_MASK;
	if (RecvTransItemQue_Pop < 0) RecvTransItemQue_Pop = 0;

	return TRUE;
}

//ÀÓ½ÃÀúÀåµÈ ¾ÆÀÌÅÛ Å¥¿¡¼­ Ã£±â
TRANS_ITEMINFO	*FindRecvTransItemQue(DWORD dwCode, DWORD dwHead, DWORD dwChkSum)
{
	int cnt, mCnt;

	for (cnt = RecvTransItemQue_Pop; cnt < RecvTransItemQue_Push; cnt++)
	{
		mCnt = cnt&RECV_TRANSITEM_QUE_MASK;

		if (Recv_TransItemInfo_Que[mCnt].Item.CODE == dwCode &&
			Recv_TransItemInfo_Que[mCnt].Item.ItemHeader.Head == dwHead &&
			Recv_TransItemInfo_Que[mCnt].Item.ItemHeader.dwChkSum == dwChkSum)
		{

			return &Recv_TransItemInfo_Que[mCnt];
		}
	}

	return NULL;
}

//ÀÓ½ÃÀúÀåµÈ ¾ÆÀÌÅÛ Å¥¿¡ ¾ÆÀÌÅÛ ´ë±âÁßÀÎÁö °Ë»ç
int CheckRecvTrnsItemQue()
{
	int cnt, mCnt;

	for (cnt = RecvTransItemQue_Pop; cnt < RecvTransItemQue_Push; cnt++)
	{
		mCnt = cnt&RECV_TRANSITEM_QUE_MASK;
		if (Recv_TransItemInfo_Que[mCnt].Item.CODE) return FALSE;
	}

	return TRUE;
}

//»ç¿ëÇÑ ¾ÆÀÌÅÛ ÄÚµå ¼­¹ö·Î º¸³¿
int SendUseItemCodeToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendUseItemCodeToServer : Requisitado porém foi removido.");
	return NULL;
}
//ÁÖº¯ Ä³¸¯ÅÍ¿ë ÀÌº¥Æ® ¸í·É ÄÚµå
int RecvCommandUser(smTRANS_COMMAND_EX *lpTransCommandEx)
{
	smCHAR *lpChar;
	smCHAR *lpChar2;

	switch (lpTransCommandEx->WParam)
	{
		case smCOMMNAD_USER_WARP:
			//¿öÇÁ ÀÌÆåÆ® Ç¥½Ã
			StartEffect(lpTransCommandEx->LParam, lpTransCommandEx->SParam, lpTransCommandEx->EParam, EFFECT_RETURN1);
			SkillPlaySound(SKILL_SOUND_LEARN, lpTransCommandEx->LParam, lpTransCommandEx->SParam, lpTransCommandEx->EParam);			//½ºÅ³ È¿°úÀ½
			break;


		case smCOMMNAD_USER_AGINGUP:
			//ÀÚµ¿ ÇÃ·¹ÀÌ¾î¸¦ Ã£´Â´Ù
			lpChar = FindChrPlayer(lpTransCommandEx->WxParam);
			if (lpChar)
			{
				StartEffect(lpChar->pX, lpChar->pY, lpChar->pZ, EFFECT_AGING);
				esPlaySound(7, GetDistVolume(lpChar->pX, lpChar->pY, lpChar->pZ), 1600);	//»ç¿îµå ¿¬ÁÖ
			}
			break;

		case smCOMMNAD_USER_HOLY_INCANTATION:
			lpChar = FindChrPlayer(lpTransCommandEx->LParam);
			if (lpChar)
			{
				AssaParticle_HolyIncantationPart(lpChar, lpTransCommandEx->SParam);
			}
			break;

		case smCOMMNAD_USER_RESURRECTION:
			lpChar = FindChrPlayer(lpTransCommandEx->LParam);
			if (lpChar)
			{
				AssaParticle_ResurrectionPart(lpChar);
			}
			break;

		case smCOMMNAD_USER_FORCEORB:		//Æ÷½º »ç¿ë
			//ÀÚµ¿ ÇÃ·¹ÀÌ¾î¸¦ Ã£´Â´Ù
			lpChar = FindChrPlayer(lpTransCommandEx->WxParam);
			if (lpChar)
			{
				StartSkill(lpChar->pX, lpChar->pY, lpChar->pZ, 0, 0, 0, SKILL_UP1);
				SkillPlaySound(SKILL_SOUND_LEARN, lpChar->pX, lpChar->pY, lpChar->pZ);			//½ºÅ³ È¿°úÀ½
			}
			break;

		case smCOMMNAD_USER_VALENTO:
			//¹ß·»Åä ¼Ò¿ï¼­Ä¿ ÀÌÆåÆ®
			lpChar = FindChrPlayer(lpTransCommandEx->LParam);
			lpChar2 = FindChrPlayer(lpTransCommandEx->SParam);
			if (lpChar && lpChar2)
			{
				sinSkillEffect_Soul_Sucker(lpChar, lpChar2);
			}
			break;

		case smCOMMNAD_USER_FIREFLOWER:
			lpChar = FindChrPlayer(lpTransCommandEx->WxParam);
			if (lpChar && lpChar != lpCurPlayer)
			{
				DWORD	dwTable = GetTableNum(lpTransCommandEx->LParam);
				AddTimeEffect(SKILL_EFFECT_FIREFLOWER, dwPlayTime + 1000, (dwTable >> 24) & 0xFF, 0, 0, lpChar);
				AddTimeEffect(SKILL_EFFECT_FIREFLOWER, dwPlayTime + 1500, (dwTable >> 16) & 0xFF, 0, 0, lpChar);
				AddTimeEffect(SKILL_EFFECT_FIREFLOWER, dwPlayTime + 2000, (dwTable >> 8) & 0xFF, 0, 0, lpChar);
				AddTimeEffect(SKILL_EFFECT_FIREFLOWER, dwPlayTime + 2500, dwTable & 0xFF, 0, 0, lpChar);
			}
			break;

	}

	return TRUE;
}

//ÁÖº¯ Ä³¸¯ÅÍ¿¡ ÀÌº¥Æ® ¸í·É ÄÚµå º¸³»±â
int	SendCommandUser(int wParam, int lParam, int sParam, int eParam)
{
	smTRANS_COMMAND_EX	smTransCommandEx;

	smTransCommandEx.code = smTRANSCODE_COMMAND_USER;
	smTransCommandEx.size = sizeof(smTRANS_COMMAND_EX);
	smTransCommandEx.WParam = wParam;
	smTransCommandEx.LParam = lParam;
	smTransCommandEx.SParam = sParam;
	smTransCommandEx.EParam = eParam;

	smTransCommandEx.WxParam = lpCurPlayer->dwObjectSerial;
	smTransCommandEx.LxParam = 0;
	smTransCommandEx.SxParam = 0;
	smTransCommandEx.ExParam = 0;

	if (smWsockUserServer)
	{
		return smWsockUserServer->Send((char *)&smTransCommandEx, smTransCommandEx.size, TRUE);
	}
	return FALSE;
}

//Áö¿ª¼­¹ö¿¡ ¸í·É ÄÚµå Àü¼Û
int	SendCommand_AreaServer(DWORD dwCode, int wParam, int lParam, int sParam, int eParam)
{
	Log::Debug("SendCommand_AreaServer : Requisitado porém foi removido.");
	return NULL;
}

//Áö¿ª¼­¹ö¿¡ ¸í·É ÄÚµå Àü¼Û
int	SendCommand_DataServer(DWORD dwCode, int wParam, int lParam, int sParam, int eParam)
{
	Log::Debug("SendCommand_DataServer : Requisitado porém foi removido.");
	return NULL;
}

struct	sDAMAGE_RECORD
{
	int	Damage;
	int	Count;
};

sDAMAGE_RECORD	sRecvDamage[3];
sDAMAGE_RECORD	sSendDamage[3];

////////////////////////////////////////////////////////////////////////////
//°ø°Ý·Â ±â·Ï ÃÊ±âÈ­
int	Init_RecordDamage()
{
	ZeroMemory(sRecvDamage, sizeof(sDAMAGE_RECORD) * 3);
	ZeroMemory(sSendDamage, sizeof(sDAMAGE_RECORD) * 3);

	return TRUE;
}

//¹ÞÀº °ø°Ý·Â ±â·Ï
int	Record_RecvDamage(smWINSOCK *lpsmSock, int Damage)
{
	smTRANS_COMMAND	smTransCommand;

	if (smWsockServer == lpsmSock)
	{
		sRecvDamage[0].Damage -= Damage;
		sRecvDamage[0].Count--;

		if ((sRecvDamage[0].Count & 0x1FF) == 0)
		{
			smTransCommand.code = smTRANSCODE_REC_DAMAGEDATA;
			smTransCommand.size = sizeof(smTRANS_COMMAND);
			smTransCommand.WParam = sRecvDamage[0].Count;
			smTransCommand.LParam = sRecvDamage[0].Damage;
			smTransCommand.SParam = sSendDamage[0].Count;
			smTransCommand.EParam = sSendDamage[0].Damage;
			if (lpsmSock) lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
		}
	}
	else if (smWsockUserServer == lpsmSock)
	{
		sRecvDamage[1].Damage -= Damage;
		sRecvDamage[1].Count--;

		if ((sRecvDamage[1].Count & 0x1FF) == 0)
		{
			smTransCommand.code = smTRANSCODE_REC_DAMAGEDATA;
			smTransCommand.size = sizeof(smTRANS_COMMAND);
			smTransCommand.WParam = sRecvDamage[1].Count;
			smTransCommand.LParam = sRecvDamage[1].Damage;
			smTransCommand.SParam = sSendDamage[1].Count;
			smTransCommand.EParam = sSendDamage[1].Damage;
			if (lpsmSock) lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
		}

	}
	else if (smWsockExtendServer == lpsmSock)
	{
		sRecvDamage[2].Damage -= Damage;
		sRecvDamage[2].Count--;

		if ((sRecvDamage[2].Count & 0x1FF) == 0)
		{
			smTransCommand.code = smTRANSCODE_REC_DAMAGEDATA;
			smTransCommand.size = sizeof(smTRANS_COMMAND);
			smTransCommand.WParam = sRecvDamage[2].Count;
			smTransCommand.LParam = sRecvDamage[2].Damage;
			smTransCommand.SParam = sSendDamage[2].Count;
			smTransCommand.EParam = sSendDamage[2].Damage;
			if (lpsmSock) lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
		}
	}

	return TRUE;
}

//º¸³½ °ø°Ý·Â ±â·Ï
int	Record_SendDamage(smWINSOCK *lpsmSock, int Damage)
{
	smTRANS_COMMAND	smTransCommand;

	if (smWsockServer == lpsmSock)
	{
		sSendDamage[0].Damage -= Damage;
		sSendDamage[0].Count--;

		if ((sRecvDamage[0].Count & 0x1FF) == 0)
		{
			smTransCommand.code = smTRANSCODE_REC_DAMAGEDATA;
			smTransCommand.size = sizeof(smTRANS_COMMAND);
			smTransCommand.WParam = sRecvDamage[0].Count;
			smTransCommand.LParam = sRecvDamage[0].Damage;
			smTransCommand.SParam = sSendDamage[0].Count;
			smTransCommand.EParam = sSendDamage[0].Damage;
			if (lpsmSock) lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
		}
	}
	else if (smWsockUserServer == lpsmSock)
	{
		sSendDamage[1].Damage -= Damage;
		sSendDamage[1].Count--;

		if ((sRecvDamage[1].Count & 0x1FF) == 0)
		{
			smTransCommand.code = smTRANSCODE_REC_DAMAGEDATA;
			smTransCommand.size = sizeof(smTRANS_COMMAND);
			smTransCommand.WParam = sRecvDamage[1].Count;
			smTransCommand.LParam = sRecvDamage[1].Damage;
			smTransCommand.SParam = sSendDamage[1].Count;
			smTransCommand.EParam = sSendDamage[1].Damage;
			if (lpsmSock) lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
		}

	}
	else if (smWsockExtendServer == lpsmSock)
	{
		sSendDamage[2].Damage -= Damage;
		sSendDamage[2].Count--;

		if ((sRecvDamage[2].Count & 0x1FF) == 0)
		{
			smTransCommand.code = smTRANSCODE_REC_DAMAGEDATA;
			smTransCommand.size = sizeof(smTRANS_COMMAND);
			smTransCommand.WParam = sRecvDamage[2].Count;
			smTransCommand.LParam = sRecvDamage[2].Damage;
			smTransCommand.SParam = sSendDamage[2].Count;
			smTransCommand.EParam = sSendDamage[2].Damage;
			if (lpsmSock) lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
		}
	}

	return TRUE;
}

//ÆÄÆ¼¿ø¿¡°Ô ½ºÅ³ »ç¿ë Á¤º¸ º¸³»±â
int	SendPartySkillToServer(DWORD dwSkillCode, int SkillLevel, int Around, int wParam, int lParam, int sParam, int eParam)
{
	Log::Debug("SendPartySkillToServer : Requisitado porém foi removido.");
	return NULL;
}
//½ºÅ³ Ãë¼Ò
int SendCancelSkillToServer(DWORD dwSkillCode, int lParam, int sParam, int eParam)
{
	Log::Debug("SendCancelSkillToServer : Requisitado porém foi removido.");
	return NULL;
}
//½ºÅ³ º¯°æ
int SendUpdateSkillToServer(DWORD dwSkillCode, int lParam, int sParam, int eParam)
{
	Log::Debug("SendUpdateSkillToServer : Requisitado porém foi removido.");
	return NULL;
}
//¼­¹ö¿¡ ½ºÅ³¼öÇà º¸³¿
int SendProcessSKillToServer(DWORD dwSkillCode, int point, int Param1, int Param2)
{
	Log::Debug("SendProcessSKillToServer : Requisitado porém foi removido.");
	return NULL;
}
//ÆÄÆ¼ ½ºÅ³ ÆÐÅ¶ ¼ö½Å
int RecvPartySkillFromServer(TRANS_PARTY_SKILL *lpTransPartySkill)
{
	Log::Debug("RecvPartySkillFromServer : Requisitado porém foi removido.");
	return NULL;
}
DWORD	dwLastClanUserCode = 0;

//Å¬·£ ¸Þ¼¼Áö º¸³»±â
int SendClanJoinService(DWORD dwMsgCode, smCHAR *lpChar)
{
	smTRANS_CHAR_COMMAND2	smTransCharCommand;

#ifdef  __CLANSUBCHIP__
	if (cldata.myPosition != 101 && cldata.myPosition != 104) return FALSE;
#else
	if (cldata.myPosition != 101) return FALSE;
#endif

	smTransCharCommand.code = smTRANSCODE_CLAN_SERVICE;
	smTransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND2);
	smTransCharCommand.WParam = dwMsgCode;
	smTransCharCommand.LParam = lpChar->dwObjectSerial;
	smTransCharCommand.SParam = lpCurPlayer->dwObjectSerial;
	lstrcpy(smTransCharCommand.szName, cldata.name);
	lstrcpy(smTransCharCommand.szId, UserAccount);

	if (smWsockUserServer)
		smWsockUserServer->Send2((char *)&smTransCharCommand, smTransCharCommand.size, TRUE);

	return TRUE;
}

//Å¬·£ °¡ÀÔ ½ÂÀÎ ¸Þ¼¼Áö º¸³»±â
int SendJoinClan()
{
	smTRANS_CHAR_COMMAND2	smTransCharCommand;

	if (!dwLastClanUserCode) return FALSE;

	smTransCharCommand.code = smTRANSCODE_CLAN_SERVICE;
	smTransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND2);
	smTransCharCommand.WParam = 2;
	smTransCharCommand.LParam = dwLastClanUserCode;
	smTransCharCommand.SParam = lpCurPlayer->dwObjectSerial;
	lstrcpy(smTransCharCommand.szName, lpCurPlayer->smCharInfo.szName);
	lstrcpy(smTransCharCommand.szId, UserAccount);

	if (smWsockUserServer)
		smWsockUserServer->Send2((char *)&smTransCharCommand, smTransCharCommand.size, TRUE);

	return TRUE;
}

//Å¬·£ ¸Þ¼¼Áö ¹Þ±â
int RecvClanJoinService(smTRANS_CHAR_COMMAND2 *lpTransCharCommand)
{
	smCHAR *lpChar;

	switch (lpTransCharCommand->WParam)
	{
		case 1:		//°¡ÀÔ ±ÇÀ¯
			if (cldata.myPosition == 102 || cldata.myPosition == 103)
			{

				lpChar = FindAutoPlayer(lpTransCharCommand->SParam);
				if (lpChar)
				{
					ClanJoin(lpTransCharCommand->szName, lpChar->smCharInfo.szName);
					OpenEachPlayer(lpChar);
					DispEachMode = TRUE;
					dwLastClanUserCode = lpTransCharCommand->SParam;
				}
			}
			break;

		case 2:
#ifdef  __CLANSUBCHIP__
			if (cldata.myPosition == 101 || cldata.myPosition == 104)
			{
#else
			if (cldata.myPosition == 101)
			{
#endif
				lpChar = FindAutoPlayer(lpTransCharCommand->SParam);
				if (lpChar)
					menu_joinOpen_Chip(1, lpTransCharCommand->szId, lpTransCharCommand->szName, lpChar->smCharInfo.JOB_CODE, lpChar->smCharInfo.Level);
				else
					menu_joinOpen_Chip(1, lpTransCharCommand->szId, lpTransCharCommand->szName, 0, 0);
			}
			break;
	}

	return TRUE;
}


/////////// DivineLightning ½ºÅ³ ¼­ºê ÇÔ¼ö ///////////////////

DWORD	dwSkill_DivineLightning_Target[8];
int		DivineLightning_Target_Count = 0;
int		DivineLightning_FindCount = 0;

//µð¹ÙÀÎ ¶óÀÌÆ®´× ½ºÅ³ ¿¬Ãâ ( ÁØºñÇÔ¼ö - ÁÖÀÎ°ø¸¸ )
int	SkillPlay_DivineLightning_Select(smCHAR *lpChar, int SkillPoint)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int	LightningCount;
	int cCount;
	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	smWINSOCK *lpsmSendSock = 0;

	//Å¬¶óÀÌ¾ðÆ® ·Î±× ÆÄÀÏ ±â·Ï
	Record_ClinetLogFile("±¸¹öÀü °ø°ÝÆÐÅ¶ - SkillPlay_DivineLightning_Select ");
	quit = 1;
	return TRUE;

	if (lpChar != lpCurPlayer) return FALSE;

	TargetCount = 0;

	LightningCount = Divine_Lightning_Num[SkillPoint - 1];

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{

		cCount = (DivineLightning_FindCount + cnt)&OTHER_PLAYER_MASK;

		if (chrOtherPlayer[cCount].Flag && chrOtherPlayer[cCount].dwObjectSerial &&
			lpChar != &chrOtherPlayer[cCount] && chrOtherPlayer[cCount].smCharInfo.State == smCHAR_STATE_ENEMY &&
			chrOtherPlayer[cCount].smCharInfo.Brood != smCHAR_MONSTER_USER &&
			chrOtherPlayer[cCount].smCharInfo.Life[0]>0)
		{

			x = (lpChar->pX - chrOtherPlayer[cCount].pX) >> FLOATNS;
			y = (lpChar->pY - chrOtherPlayer[cCount].pY) >> FLOATNS;
			z = (lpChar->pZ - chrOtherPlayer[cCount].pZ) >> FLOATNS;

			dist = x*x + y*y + z*z;

			if (dist < (160 * 160))
			{
				dwSkill_DivineLightning_Target[TargetCount] = chrOtherPlayer[cCount].dwObjectSerial;
				TransSkilAttackData.dwTarObjectSerial[TargetCount] = chrOtherPlayer[cCount].dwObjectSerial;
				TargetCount++;

				if (chrOtherPlayer[cCount].smCharInfo.Life[1] && chrOtherPlayer[cCount].smCharInfo.Life[0] == chrOtherPlayer[cCount].smCharInfo.Life[1])
				{
					chrOtherPlayer[cCount].EnableStateBar = TRUE;
				}
				if (TargetCount >= LightningCount)
				{
					DivineLightning_FindCount = cCount;
					break;
				}
			}
		}
	}

	TransSkilAttackData.TargetCount = TargetCount;
	DivineLightning_Target_Count = TargetCount;

	TransSkilAttackData.code = smTRANSCODE_SKIL_ATTACKDATA;
	TransSkilAttackData.size = (sizeof(TRANS_SKIL_ATTACKDATA) - sizeof(DWORD)*SKIL_ATTACK_CHAR_MAX);
	TransSkilAttackData.size += sizeof(DWORD)*TargetCount + 16;

	TransSkilAttackData.x = lpChar->pX;
	TransSkilAttackData.y = lpChar->pY;
	TransSkilAttackData.z = lpChar->pZ;

	TransSkilAttackData.AttackState = 103;			//¾ðµ¥µå °¡Áß
	TransSkilAttackData.AttackSize = 160 * 160;
	TransSkilAttackData.Power = GetRandomPos(Divine_Lightning_Damage[SkillPoint - 1][0], Divine_Lightning_Damage[SkillPoint - 1][1]);

	lpsmSendSock = GetAreaServerSock();		//º¸³¾ ¼­¹ö±¸ÇÏ±â

	if (lpsmSendSock && TargetCount > 0)
	{
		//º¸³½ °ø°Ý·Â ±â·Ï
		Record_SendDamage(lpsmSendSock, TransSkilAttackData.Power);

		lpsmSendSock->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}

	return TRUE;
}

//µð¹ÙÀÎ ¶óÀÌÆ®´× ½ºÅ³ ¿¬Ãâ ( ÀÌÆåÆ® ½ÇÇö )
int SkillPlay_DivineLightning_Effect(smCHAR *lpChar, int SkillPoint)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int	LightningCount;
	int cCount;
	smCHAR *lpTarChar;

	TargetCount = 0;

	if (lpChar == lpCurPlayer)
	{
		for (cnt = 0; cnt < DivineLightning_Target_Count; cnt++)
		{
			lpTarChar = FindAutoPlayer(dwSkill_DivineLightning_Target[cnt]);
			if (lpTarChar)
			{
				AssaParticle_DivineLighting(lpTarChar);
				TargetCount++;
			}
		}
	}
	else
	{
		LightningCount = Divine_Lightning_Num[SkillPoint - 1];

		for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
		{
			cCount = (DivineLightning_FindCount + cnt)&OTHER_PLAYER_MASK;
			if (chrOtherPlayer[cCount].Flag && chrOtherPlayer[cCount].dwObjectSerial &&
				lpChar != &chrOtherPlayer[cCount] && chrOtherPlayer[cCount].smCharInfo.State == smCHAR_STATE_ENEMY &&
				chrOtherPlayer[cCount].smCharInfo.Brood != smCHAR_MONSTER_USER &&
				chrOtherPlayer[cCount].smCharInfo.Life[0]>0)
			{

				x = (lpChar->pX - chrOtherPlayer[cCount].pX) >> FLOATNS;
				y = (lpChar->pY - chrOtherPlayer[cCount].pY) >> FLOATNS;
				z = (lpChar->pZ - chrOtherPlayer[cCount].pZ) >> FLOATNS;

				dist = x*x + y*y + z*z;

				if (dist < (160 * 160))
				{
					AssaParticle_DivineLighting(&chrOtherPlayer[cCount]);
					TargetCount++;
					if (TargetCount >= LightningCount)
					{
						DivineLightning_FindCount = cCount;
						break;
					}
				}
			}
		}

		if (TargetCount < LightningCount && lpChar->smCharInfo.State == smCHAR_STATE_ENEMY)
		{
			x = (lpChar->pX - lpCurPlayer->pX) >> FLOATNS;
			y = (lpChar->pY - lpCurPlayer->pY) >> FLOATNS;
			z = (lpChar->pZ - lpCurPlayer->pZ) >> FLOATNS;

			dist = x*x + y*y + z*z;

			if (dist < (160 * 160))
			{
				AssaParticle_DivineLighting(lpCurPlayer);
				TargetCount++;
			}
		}

	}


	return TargetCount;
}


//µð¹ÙÀÎ ¶óÀÌÆ®´× ½ºÅ³ ¿¬Ãâ ( ¸Ó¹Ì·Îµå )
int SkillPlay_MummyLord_Effect(smCHAR *lpChar, int Range)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int	dRange = Range*Range;

	TargetCount = 0;

	if (lpChar->smCharInfo.Brood != smCHAR_MONSTER_USER)
	{
		//¸ó½ºÅÍ°¡ À¯Àú °ø°Ý
		for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
		{
			if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].dwObjectSerial &&
				lpChar != &chrOtherPlayer[cnt] && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_USER &&
				chrOtherPlayer[cnt].smCharInfo.Life[0]>0)
			{

				x = (lpChar->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
				y = (lpChar->pY - chrOtherPlayer[cnt].pY) >> FLOATNS;
				z = (lpChar->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;

				dist = x*x + y*y + z*z;

				if (dist < dRange)
				{
					ParkAssaParticle_DivineLighting(&chrOtherPlayer[cnt]);
					TargetCount++;
				}

			}

		}

		if (lpChar->smCharInfo.State == smCHAR_STATE_ENEMY)
		{
			x = (lpChar->pX - lpCurPlayer->pX) >> FLOATNS;
			y = (lpChar->pY - lpCurPlayer->pY) >> FLOATNS;
			z = (lpChar->pZ - lpCurPlayer->pZ) >> FLOATNS;

			dist = x*x + y*y + z*z;

			if (dist < dRange)
			{
				ParkAssaParticle_DivineLighting(lpCurPlayer);
				TargetCount++;
			}
		}
	}
	else
	{
		//À¯Àú¶û °°Àº Æí ( ¸ó½ºÅÍ °ø°Ý )
		for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
		{
			if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].dwObjectSerial &&
				lpChar != &chrOtherPlayer[cnt] && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_ENEMY &&
				chrOtherPlayer[cnt].smCharInfo.Life[0]>0)
			{

				x = (lpChar->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
				y = (lpChar->pY - chrOtherPlayer[cnt].pY) >> FLOATNS;
				z = (lpChar->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;

				dist = x*x + y*y + z*z;

				if (dist < dRange)
				{
					ParkAssaParticle_DivineLighting(&chrOtherPlayer[cnt]);
					TargetCount++;
				}

			}
		}
	}


	return TargetCount;
}


//º£³ð ½ºÇÇ¾î ½ºÅ³ ¿¬Ãâ ( ÀÌÆåÆ® ½ÇÇö )
int SkillPlay_VenomSpear_Effect(smCHAR *lpChar, int SkillPoint)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int	LightningCount;
	int cCount;
	smCHAR *lpTarChar;
	int range;

	range = VenomSpear_Range[SkillPoint - 1];
	range *= range;

	TargetCount = 0;

	if (lpChar == lpCurPlayer)
	{
		for (cnt = 0; cnt < DivineLightning_Target_Count; cnt++)
		{
			lpTarChar = FindAutoPlayer(dwSkill_DivineLightning_Target[cnt]);
			if (lpTarChar)
			{
				AssaParticle_VeonmSpear(lpTarChar);
				TargetCount++;
			}
		}
	}
	else
	{
		LightningCount = Divine_Lightning_Num[SkillPoint - 1];

		for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
		{
			cCount = (DivineLightning_FindCount + cnt)&OTHER_PLAYER_MASK;
			if (chrOtherPlayer[cCount].Flag && chrOtherPlayer[cCount].dwObjectSerial &&
				lpChar != &chrOtherPlayer[cCount] && chrOtherPlayer[cCount].smCharInfo.State == smCHAR_STATE_ENEMY &&
				chrOtherPlayer[cCount].smCharInfo.Brood != smCHAR_MONSTER_USER &&
				chrOtherPlayer[cCount].smCharInfo.Life[0]>0)
			{

				x = (lpChar->pX - chrOtherPlayer[cCount].pX) >> FLOATNS;
				y = (lpChar->pY - chrOtherPlayer[cCount].pY) >> FLOATNS;
				z = (lpChar->pZ - chrOtherPlayer[cCount].pZ) >> FLOATNS;

				dist = x*x + y*y + z*z;

				if (dist < range)
				{
					AssaParticle_VeonmSpear(&chrOtherPlayer[cCount]);
					TargetCount++;
					if (TargetCount >= LightningCount)
					{
						DivineLightning_FindCount = cCount;
						break;
					}
				}
			}
		}
	}


	return TargetCount;
}

//¸ó½ºÅÍ ½ºÅ³ ÀÌÆåÆ® ½ÇÇö ( ÀÌÆåÆ® ½ÇÇö )
int SkillPlay_Monster_Effect(smCHAR *lpChar, int EffectKind, int Range)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int dDist = Range*Range;


	TargetCount = 0;

	x = (lpChar->pX - lpCurPlayer->pX) >> FLOATNS;
	y = (lpChar->pY - lpCurPlayer->pY) >> FLOATNS;
	z = (lpChar->pZ - lpCurPlayer->pZ) >> FLOATNS;

	dist = x*x + y*y + z*z;

	if (dist < dDist)
	{
		ParkAssaParticle_ChaosKaraSkill_User(lpCurPlayer);	// Ä«¿À½ºÄ«¶ó ¹ìÆÄÀÌ¾î¸¯ (À¯Àú)
		TargetCount++;
	}


	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		if (chrOtherPlayer[cnt].Flag && chrOtherPlayer[cnt].dwObjectSerial &&
			lpChar != &chrOtherPlayer[cnt] && chrOtherPlayer[cnt].smCharInfo.State == smCHAR_STATE_USER &&
			chrOtherPlayer[cnt].smCharInfo.Life[0]>0)
		{
			x = (lpChar->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
			y = (lpChar->pY - chrOtherPlayer[cnt].pY) >> FLOATNS;
			z = (lpChar->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;

			dist = x*x + y*y + z*z;

			if (dist < dDist)
			{
				ParkAssaParticle_ChaosKaraSkill_User(&chrOtherPlayer[cnt]);	// Ä«¿À½ºÄ«¶ó ¹ìÆÄÀÌ¾î¸¯ (À¯Àú)
				TargetCount++;
			}
		}
	}

	return TargetCount;
}




//½ºÅ³ ½ÇÇà ÆÐÅ¶ ¼ö½Å
int RecvProcessSkill(smTRANS_COMMAND *lpTransCommand)
{
	int cnt;
	int dwSkillCode, point;
	smCHAR	*lpChar, *lpChar2;
	POINT3D	Pos1, Pos2;

	dwSkillCode = lpTransCommand->WParam & 0xFF;
	point = (lpTransCommand->WParam >> 8) & 0xF;

	switch (dwSkillCode)
	{
		case SKILL_PLAY_HOLY_REFLECTION:
			//È¦¸® ¸®ÇÃ·º¼Ç Å¸°Ý ÀÌÆåÆ®
			sinSkillEffect_Holy_Reflection_Defense(lpCurPlayer);
			break;

		case SKILL_PLAY_ENCHANT_WEAPON:
			//ÀÎÃ¾Æ® ¿þÆù
			cnt = Enchant_Weapon_Time[point - 1];				//lpTransCommand->LParam-1];
			lpCurPlayer->EnchantEffect_Point = point;		//lpTransCommand->LParam;

			cSkill.SetEnchant_Weapon(cnt, point);

			switch (lpTransCommand->EParam)
			{
				case 0:
					AssaParticle_EnchantWeaponIceJang(lpCurPlayer, cnt);
					lpCurPlayer->WeaponEffect = sITEMINFO_ICE + 1;
					lpCurPlayer->dwWeaponEffectTime = dwPlayTime + cnt * 1000;
					break;
				case 1:
					AssaParticle_EnchantWeaponLightJang(lpCurPlayer, cnt);
					lpCurPlayer->WeaponEffect = sITEMINFO_LIGHTING + 1;
					lpCurPlayer->dwWeaponEffectTime = dwPlayTime + cnt * 1000;
					break;
				case 2:
					AssaParticle_EnchantWeaponFireJang(lpCurPlayer, cnt);
					lpCurPlayer->WeaponEffect = sITEMINFO_FIRE + 1;
					lpCurPlayer->dwWeaponEffectTime = dwPlayTime + cnt * 1000;
					break;
			}
			break;

		case SKILL_PLAY_RESURRECTION:
			//Á×ÀºÀÚ »ì¸°±â
			if (lpCurPlayer->MotionInfo && lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD)
			{
				SendProcessSKillToServer(SKILL_PLAY_RESURRECTION, point, 0, 0);

				lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_RESTART);		//½ÃÀÛ¸ð¼Ç
				StartEffect(lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ, EFFECT_GAME_START1);
				SetDynLight(lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ, 100, 100, 100, 0, 200);
				CharPlaySound(lpCurPlayer);

				sinSetLife(lpCurPlayer->smCharInfo.Life[1] / 2);
				ResetEnergyGraph(4);								//¿¡³ÊÁö ±×·¡ÇÁ Ã½Å© ÃÊ±âÈ­
				lpCurPlayer->MoveFlag = FALSE;
				RestartPlayCount = 350;								//5ÃÊ µ¿¾È ¹«Àû
				ReStartFlag = 0;
			}
			break;

		case SKILL_PLAY_VIRTUAL_LIFE:
			//¹öÃò¾ó ¶óÀÌÇÁ Àû¿ë
			cSkill.SetVirtualLife(Virtual_Life_Time[point - 1], point);
			AssaParticle_VirtualLife(lpCurPlayer, Virtual_Life_Time[point - 1]);
			break;

		case SKILL_PLAY_LOVELY_LIFE:
			//»ç¶ûÀÇ ÃÊÄÝ·¿
			if (lpTransCommand->LParam == lpCurPlayer->dwObjectSerial)
			{
				cSkill.SetVirtualLife(Virtual_Life_Time[point - 1], point);
				AssaParticle_VirtualLife(lpCurPlayer, Virtual_Life_Time[point - 1]);
				SkillPlaySound(SKILL_SOUND_SKILL_VIRTUAL_LIFE, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
			}
			else
			{
				lpChar = FindChrPlayer(lpTransCommand->LParam);
				if (lpChar)
				{
					AssaParticle_VirtualLife(lpChar, Virtual_Life_Time[point - 1]);
					SkillPlaySound(SKILL_SOUND_SKILL_VIRTUAL_LIFE, lpChar->pX, lpChar->pY, lpChar->pZ);
				}
			}
			break;

		case SKILL_PLAY_VANISH:
			//¹è´Ï½¬ ½ºÅ³ ÇØÁ¦
			lpCurPlayer->PlayVanish = 0;
			cSkill.CancelContinueSkill(SKILL_VANISH);

			if (dwM_BlurTime) dwM_BlurTime = dwPlayTime;
			break;

		case SKILL_PLAY_TRIUMPH_OF_VALHALLA:
			//Æ®¶óÀÌ¾öÇÁ
			cSkill.SetT_Of_Valhalla(point, lpTransCommand->EParam);
			AssaParticle_TriumphOfValhalla(lpCurPlayer, T_Of_Valhalla_Time[point - 1]);
			break;

		case SKILL_PLAY_SPARK_SHIELD:
			//½ºÆÄÅ©½¯µå ÀÌÆåÆ®

			// À¯Àú¹øÈ£·Î Ã£´Â´Ù
			lpChar = FindChrPlayer(lpTransCommand->LParam);
			lpChar2 = FindChrPlayer(lpTransCommand->SParam);
			if (lpChar && lpChar2)
			{
				//½ºÆÄÅ© ½¯µå
				if (lpChar->HvLeftHand.PatTool)
				{
					if (lpChar->GetToolBipPoint(&lpChar->HvLeftHand, &Pos1.x, &Pos1.y, &Pos1.z))
					{

						Pos2.x = lpChar2->pX;
						Pos2.y = lpChar2->pY + 24 * fONE;
						Pos2.z = lpChar2->pZ;

						AssaParticle_SparkShieldDefence(&Pos1, &Pos2);
						SkillPlaySound(SKILL_SOUND_SKILL_SPARK2, lpChar->pX, lpChar->pY, lpChar->pZ);
					}
				}
			}
			break;

		case SKILL_PLAY_HALL_OF_VALHALLA:
			//È¦¿Àºê¹ßÇÒ¶ó
			cSkill.HellOfValhalla(Hall_Of_Valhalla_Time[point - 1], point, lpTransCommand->SParam, lpTransCommand->EParam, 1);
			SkillValhallaHallOfValhallaHandEffect(lpCurPlayer, (float)lpTransCommand->EParam);
			lpCurPlayer->dwHallOfValhallaTime = dwPlayTime + Hall_Of_Valhalla_Time[point - 1] * 1000;	//È¦¿Àºê¹ßÇÒ¶ó À¯Áö½Ã°£
			break;

		case SKILL_PLAY_FORCE_OF_NATURE:
			//Æ÷½º¿Àºê ³×ÀÌÃÄ
			cSkill.ForeceOfNature(Force_Of_Nature_Time[point - 1], point, 1);
			SkillSagittarionForceOfNature1(lpCurPlayer, (float)Force_Of_Nature_Time[point - 1]);
			lpCurPlayer->dwForceOfNatureTime = dwPlayTime + Force_Of_Nature_Time[point - 1] * 1000;	//Æ÷½º¿Àºê ³×ÀÌÃÄ À¯Áö½Ã°£
			break;


		case SKILL_PLAY_SOD_ITEM:
			//SOD ±ôÂ¦ ¾ÆÀÌÅÛ ÀÌÆåÆ®

			switch (lpTransCommand->SParam)
			{
				case 1:	//ÆøÅº
					SetBellatraFontEffect(E_BL_QUAKE_SEAL);
					SetBellatraFontEffect(E_BL_FONT_QUAKE_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 2:	//½Ã°£
					SetBellatraFontEffect(E_BL_STUN_SEAL);
					SetBellatraFontEffect(E_BL_FONT_STUN_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 3:	//¾óÀ½
					SetBellatraFontEffect(E_BL_FREEZE_SEAL);
					SetBellatraFontEffect(E_BL_FONT_FREEZE_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 4:	//Åä³¢
					SetBellatraFontEffect(E_BL_RABBIE_SEAL);
					SetBellatraFontEffect(E_BL_FONT_RABBIE_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 5:	//´Þ¼öÁ¤
					SetBellatraFontEffect(E_BL_STYGIAN_SEAL);
					SetBellatraFontEffect(E_BL_FONT_STYGIAN_SEAL);
					break;

				case 6:	//ÅÂ¾ç¼öÁ¤
					SetBellatraFontEffect(E_BL_GUARDIAN_SEAL);
					SetBellatraFontEffect(E_BL_FONT_GUARDIAN_SEAL);
					break;

				case 7:	//Ãµ°ø¼öÁ¤
					SetBellatraFontEffect(E_BL_POINT_SEAL);
					SetBellatraFontEffect(E_BL_FONT_POINT_SEAL);
					break;
			}
			esPlaySound(19, 360);		//SOD ¾ÆÀÌÅÛ ¼Ò¸®
			break;

		case SKILL_PLAY_HEALING:
			//¸ÅÁ÷ ¸¶½ºÅÍ Èú¸µ»ç¿ë
			lpChar = FindChrPlayer(lpTransCommand->LParam);
			if (lpChar)
			{
				sinEffect_Healing2(lpChar);		//Èú¸µ ÀÌÆåÆ®
				SkillPlaySound(SKILL_SOUND_SKILL_HEALING, lpChar->pX, lpChar->pY, lpChar->pZ);		//Èú¸µ »ç¿îµå
			}
			break;

		case SKILL_PLAY_BLESS_SIEGE_ITEM:
			//ºí·¡½º Ä³½½ °ø¼º ¾ÆÀÌÅÛ »ç¿ë

			switch (lpTransCommand->SParam)
			{

				////////////// ºô¸µ¿ë /////////////////
				case (sinBI1 | sin05) :	//¹«Àû½ºÅ©·Ñ
					lpCurPlayer->PlayInvincible = 70 * 30;		//30ÃÊ
					break;
				case (sinBI1 | sin06) :	//Å©¸®Æ¼ÄÃ ½ºÅ©·Ñ
					break;
				case (sinBI1 | sin07) :	//Ãß°¡ È¸ÇÇ ½ºÅ©·Ñ
					break;

					//////////////// °ø¼º¿ë //////////////////

				case (sinBC1 | sin01) :	//¹«Àû½ºÅ©·Ñ
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_S_INVU);
					lpCurPlayer->PlayInvincible = 70 * 30;		//30ÃÊ
					break;
				case (sinBC1 | sin02) :	//Å©¸®Æ¼ÄÃ ½ºÅ©·Ñ
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_S_CRITICAL);
					break;
				case (sinBC1 | sin03) :	//Ãß°¡ È¸ÇÇ ½ºÅ©·Ñ
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_S_EVASION);
					break;
					/*
						case (sinBC1|sin04):	//ÆÄÆ¼¿ø Àü¿ø È¸º¹
						case (sinBC1|sin05):	//ºÎÈ° ½ºÅ©·Ñ
							return FALSE;
					*/
				case (sinBC1 | sin06) :	//ÆÄÀÌ¾î Å©¸®½ºÅ» ÀúÇ×¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_R_FIRE_C);
					break;
				case (sinBC1 | sin07) :	//¾ÆÀÌ½º Å©¸®½ºÅ» ÀúÇ×¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_R_ICE_C);
					break;
				case (sinBC1 | sin08) :	//¶óÀÌÆ®´× Å©¸®½ºÅ» ÀúÇ×¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_R_LIGHTING_C);
					break;
				case (sinBC1 | sin09) :	//´ë ÆÄÀÌÅÍ °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_FIGHTER);
					break;
				case (sinBC1 | sin10) :	//´ë ¸ÞÄ«´Ï¼Ç °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_MECHANICIAN);
					break;
				case (sinBC1 | sin11) :	//´ëÆÄÀÌÅ©¸Ç °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_PIKEMAN);
					break;
				case (sinBC1 | sin12) :	//´ë ÀÌÃÄ °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_ARCHER);
					break;
				case (sinBC1 | sin13) :	//´ë ³ªÀÌÆ® °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_KNIGHT);
					break;
				case (sinBC1 | sin14) :	//´ë ¾ÆÅ»¶õÅ¸ °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_ATALANTA);
					break;
				case (sinBC1 | sin15) :	//´ë ¸ÞÁö¼Ç °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_MAGICIAN);
					break;
				case (sinBC1 | sin16) :	//´ë ÇÁ¸®½ºÆ¼½º °ø°Ý·Â °­È­¼®
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_PRIESTESS);
					break;
			}
			break;

	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////

//int ReadClanInfo(DWORD dwClanNum)
//CLANINFO ClanInfo[CLAN_MAX]; //Å¬·£ Á¤º¸ ÀúÀå
//
//cldata ¾øµ¥ÀÌÆ®¿ë
//void Updatecldata();

//Å¬·£ ¾÷µ¥ÀÌÆ® ¿ä±¸ ¸Þ¼¼Áö
//#define	smTRANSCODE_CLAN_UPDATE			0x50320C10


//Å¬·£¿ø Á¤º¸ Àü¼Û¿ë ÇÔ¼ö
int SendClanCommandUser(smWINSOCK *lpsmSock, DWORD	dwCode, int Param1, int Param2, CLANWONLIST *lpClanUserList, int ClanUserCount)
{

	TRANS_CLAN_COMMAND_USER	TransClanUser;
	int	cnt, len;
	char *lpBuff;
	DWORD	dwSpeedSum;
	int	BuffSize;

	TransClanUser.code = dwCode;

	TransClanUser.Param[0] = Param1;
	TransClanUser.Param[1] = Param2;
	TransClanUser.Param[2] = 0;
	TransClanUser.Param[3] = 0;
	TransClanUser.UserCount = ClanUserCount;

	lpBuff = TransClanUser.szUserBuff;

	for (cnt = 0; cnt < ClanUserCount; cnt++)
	{
		dwSpeedSum = GetSpeedSum(lpClanUserList->clanWon[cnt]);
		((DWORD *)lpBuff)[0] = dwSpeedSum;
		lpBuff += sizeof(DWORD);
		lstrcpy(lpBuff, lpClanUserList->clanWon[cnt]);
		len = lstrlen(lpClanUserList->clanWon[cnt]);
		lpBuff += len + 1;
	}

	BuffSize = lpBuff - TransClanUser.szUserBuff;
	BuffSize += 16;

	TransClanUser.size = 32 + BuffSize;

	if (lpsmSock && ClanUserCount > 0)
	{
		lpsmSock->Send2((char *)&TransClanUser, TransClanUser.size, TRUE);
	}

	return TRUE;
}

//Å¬·£¿ø Á¤º¸ ÇØµ¶
int	RecvClanCommand(TRANS_CLAN_COMMAND_USER *lpTransClanUser, _CLAN_USER_INFO *ClanUserInfo)// char **lpClanUserName , DWORD *dwUserSpeedSum )
{
	int cnt;
	int cnt2;
	char *lpBuff;
	char	ch;

	lpBuff = lpTransClanUser->szUserBuff;

	if (lpTransClanUser->UserCount >= CLAN_USER_MAX) return FALSE;

	for (cnt = 0; cnt < lpTransClanUser->UserCount; cnt++)
	{
		ClanUserInfo[cnt].dwSpeedSum = ((DWORD *)lpBuff)[0];
		lpBuff += sizeof(DWORD);
		for (cnt2 = 0; cnt2 < 32; cnt2++)
		{
			ch = *lpBuff;
			ClanUserInfo[cnt].szName[cnt2] = ch;
			lpBuff++;
			if (!ch) break;
		}
		ClanUserInfo[cnt].szName[31] = 0;
	}

	return TRUE;
}

//Äù½ºÆ® ¾ÆÀÌÅÛ ÀÔ¼ö ÇÊÅÍ¸µ
int FiltQuestItem(TRANS_ITEMINFO	*lpTransItemInfo, DWORD dwPacketCode)
{
	int cnt;

	if (chaQuest.sHaQuestElementary[6].CODE != HAQUEST_CODE_ELEMENTARY_G)
	{

		cnt = cInvenTory.SearchItemCode(sinQT1 | sin16);
		if (cnt >= 1) { DeleteQuestItem(sinQT1 | sin16); }
		if (lpTransItemInfo->Item.CODE == (sinQT1 | sin16)) { return FALSE; }		//¼º±Ù¼öÁ¤

	}

	switch (sinQuest_ChangeJob2.CODE)
	{
		case SIN_QUEST_CODE_CHANGEJOB2_NPC_M:
			//¹ú²Ü ¾ÆÀÌÅÛ (ÃÖ´ë °¹¼ö È®ÀÎ )
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin01))
			{
				if (!cInvenTory.SearchItemCode(sinMA1 | sin01)) return FALSE;		//À¯¸®º´ ¾ø´Â°æ¿ì
				if (cInvenTory.SearchItemCode(sinQT1 | sin04)) return FALSE;		//·Î¾âÁ¦¸® ÀÖ´Â °æ¿ì
				cnt = cInvenTory.SearchItemCode(sinMA2 | sin01);
				if (cnt >= 9) return FALSE;
			}
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin02)) return FALSE;
			return TRUE;

		case SIN_QUEST_CODE_CHANGEJOB2_NPC_D:
			//±â¸§ ¾ÆÀÌÅÛ (ÃÖ´ë °¹¼ö È®ÀÎ )
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin02))
			{
				if (!cInvenTory.SearchItemCode(sinMA1 | sin01)) return FALSE;		//À¯¸®º´ ¾ø´Â°æ¿ì
				if (cInvenTory.SearchItemCode(sinQT1 | sin05)) return FALSE;		//¹ß¸ðÁ¦ ÀÖ´Â °æ¿ì
				cnt = cInvenTory.SearchItemCode(sinMA2 | sin02);
				if (cnt >= 7) return FALSE;
			}
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin01)) return FALSE;
			return TRUE;
	}

	//3Â÷ ÀüÁ÷ Äù½ºÆ®
	//¹ìÇÁ
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin06))
	{
		if (sinQuest_ChangeJob3.CODE == SIN_QUEST_CODE_CHANGEJOB3)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin06);
			if (cnt >= 3) return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	//·¹º§ Äù½ºÆ®¿ë ¹ìÇÁ 1
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin09))
	{
		if (sinQuest_Level80_2.CODE == SIN_QUEST_CODE_LEVEL80_2)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin09);
			if (cnt >= 1) return FALSE;
			return TRUE;
		}
		return FALSE;
	}
	//·¹º§ Äù½ºÆ®¿ë ¹ìÇÁ 2
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin10))
	{
		if (sinQuest_Level80_2.CODE == SIN_QUEST_CODE_LEVEL80_2)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin10);
			if (cnt >= 1) return FALSE;
			return TRUE;
		}
		return FALSE;
	}
	//·¹º§ Äù½ºÆ®¿ë ¹ìÇÁ 3
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin11))
	{
		if (sinQuest_Level80_2.CODE == SIN_QUEST_CODE_LEVEL80_2)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin11);
			if (cnt >= 1) return FALSE;
			return TRUE;
		}
		return FALSE;
	}


	//3Â÷ Àü¾÷ Äù½ºÆ®¿ë 
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin13))
	{

		if (sinQuest_ChangeJob4.CODE == SIN_QUEST_CODE_CHANGEJOB4 && sinQuest_ChangeJob4.State == 5)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin13);
			if (cnt >= 1 || CheckAttMonsterCode(sinQuest_ChangeJob4.Kind) == FALSE)
				return FALSE;

			return TRUE;
		}
		return FALSE;
	}



	//¾öÇÁÀÇ ¸ÁÄ¡ Äù½ºÆ® 
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin16))
	{

		if (chaQuest.sHaQuestElementary[6].CODE == HAQUEST_CODE_ELEMENTARY_G)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin16);
			if (cnt >= 1)
				return FALSE;

			if (dwPacketCode == smTRANSCODE_GETITEM)
			{
				//¾ÆÀÌÅÛ È¹µæ ¸Þ¼¼Áö
				char szBuff[256];
				//wsprintf( szBuff, mgGetQuestItem , lpTransItemInfo->Item.ItemName );//_ignore_//ÇØ¿Ü
				AddChatBuff(szBuff, 6);
			}
			return TRUE;
		}
		return FALSE;
	}


	if ((lpTransItemInfo->Item.CODE&sinITEM_MASK2) == sinMA1 ||
		(lpTransItemInfo->Item.CODE&sinITEM_MASK2) == sinMA2)
	{
		//Äù½ºÆ®°¡ ¾ø´Â°æ¿ì Äù½ºÆ® ¾ÆÀÌÅÛ µé¾î¿Â °æ¿ì

		return FALSE;
	}

	return TRUE;
}

POINT GetProcessModule();

//¸ðµâ°Ë»ç ÇÏ¿© ¼­¹ö·Î º¸³»±â
int CheckProcessModule()
{
	POINT p;
	OSVERSIONINFO VersionInfo;
	BOOL Result;
	smTRANS_COMMAND	smTransCommand;


	return TRUE;

	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	ZeroMemory(&smTransCommand, sizeof(smTRANS_COMMAND));
	smTransCommand.size = sizeof(smTRANS_COMMAND);
	smTransCommand.code = smTRANSCODE_CLIENTINFO;

	Result = GetVersionEx(&VersionInfo);
	p = GetProcessModule();

	if (Result != FALSE)
	{
		smTransCommand.WParam = VersionInfo.dwPlatformId;
		smTransCommand.LParam = (VersionInfo.dwMajorVersion * 100) + VersionInfo.dwMinorVersion;
		smTransCommand.LParam = (smTransCommand.LParam << 16) | (VersionInfo.dwBuildNumber & 0xFFFF);
		smTransCommand.SParam = p.x;
		smTransCommand.EParam = p.y;

		smTransCommand.WParam ^= ((DWORD *)(UserAccount + 0))[0];
		smTransCommand.LParam ^= ((DWORD *)(UserAccount + 1))[0];
		smTransCommand.SParam ^= ((DWORD *)(UserAccount + 2))[0];
		smTransCommand.EParam ^= ((DWORD *)(UserAccount + 3))[0];

		if (smWsockDataServer) smWsockDataServer->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);
	}

	return TRUE;
}

//ÆÈ°ï µ¥¹ÌÁö º¸³»±â
int SendFalconDamage(smCHAR *lpChar)
{
	DWORD point;
	int cnt;

	if (lpChar == lpCurPlayer && lpChar->chrAttackTarget)
	{
		point = ((SkillFalconPoint ^ ((DWORD)chrOtherPlayer^lpCurPlayer->dwObjectSerial)) + 1) << 8;
		point |= SKILL_PLAY_FALCON;
		cnt = dm_SendTransDamage(lpChar->chrAttackTarget, 0, 0, 0, 0, point, FALSE);
		DamageExp(lpChar->chrAttackTarget, cnt);
		return cnt;
	}
	return NULL;
}

//´í½Ì¼Òµå µ¥¹ÌÁö º¸³»±â
int SendDancingSwordDamage(smCHAR *lpChar)
{
	DWORD point;
	int cnt;
	int wpDamage[2];

	if (lpChar)
	{

		wpDamage[0] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[0];
		wpDamage[1] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[1];

		point = 1 << 8;
		point |= SKILL_PLAY_DANCING_SWORD;
		cnt = dm_SendTransDamage(lpChar, wpDamage[0], wpDamage[1], 0, 0, point, FALSE);
		DamageExp(lpChar, cnt);

		if ((rand() % 2) == 0)
			SkillPlaySound(SKILL_SOUND_SKILL_DANCING_SWORD_ATK1, lpChar->pX, lpChar->pY, lpChar->pZ);
		else
			SkillPlaySound(SKILL_SOUND_SKILL_DANCING_SWORD_ATK2, lpChar->pX, lpChar->pY, lpChar->pZ);

		return cnt;
	}
	return FALSE;
}


//¸Þ±×³×Æ½ ½ºÇÇ¾î µ¥¹ÌÁö º¸³»±â
int SendMegneticSphereDamage(smCHAR *lpChar)
{
	DWORD point;
	int cnt;
	int wpDamage[2];

	if (lpChar)
	{

		wpDamage[0] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[0];
		wpDamage[1] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[1];

		point = 1 << 8;
		point |= SKILL_PLAY_MAGNETIC_SPHERE;
		cnt = dm_SendTransDamage(lpChar, wpDamage[0], wpDamage[1], 0, 0, point, FALSE);
		DamageExp(lpChar, cnt);

		if ((rand() % 2) == 0)
			SkillPlaySound(SKILL_SOUND_SKILL_DANCING_SWORD_ATK1, lpChar->pX, lpChar->pY, lpChar->pZ);
		else
			SkillPlaySound(SKILL_SOUND_SKILL_DANCING_SWORD_ATK2, lpChar->pX, lpChar->pY, lpChar->pZ);

		return cnt;
	}
	return FALSE;
}

//¸Ó½ºÆç µ¥¹ÌÁö º¸³»±â
int SendMuspellDamage(smCHAR *lpChar)
{
	DWORD point;
	int cnt;
	int wpDamage[2];

	if (lpChar)
	{

		wpDamage[0] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[0];
		wpDamage[1] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[1];

		point = 1 << 8;
		point |= SKILL_PLAY_SUMMON_MUSPELL;
		cnt = dm_SendTransDamage(lpChar, wpDamage[0], wpDamage[1], 0, 0, point, FALSE);
		DamageExp(lpChar, cnt);

		if ((rand() % 2) == 0)
			SkillPlaySound(SKILL_SOUND_SKILL_DANCING_SWORD_ATK1, lpChar->pX, lpChar->pY, lpChar->pZ);
		else
			SkillPlaySound(SKILL_SOUND_SKILL_DANCING_SWORD_ATK2, lpChar->pX, lpChar->pY, lpChar->pZ);

		return cnt;
	}
	return FALSE;
}



//Àú·¦¿ë Æê µ¥¹ÌÁö
int SendLowLevelPetDamage(smCHAR *lpChar, int petType)
{
	DWORD point;
	int cnt;
	int wpDamage[2];

	if (lpChar)
	{

		wpDamage[0] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[0];
		wpDamage[1] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[1];

		point = (1 + petType) << 8;
		point |= SKILL_PLAY_PET_ATTACK;
		cnt = dm_SendTransDamage(lpChar, wpDamage[0], wpDamage[1], 0, 0, point, FALSE);
		DamageExp(lpChar, cnt);

		return cnt;
	}
	return FALSE;
}


//PC¹æ¿ë Æê µ¥¹ÌÁö
int SendPCBangPetDamage(smCHAR *lpChar, int petType)
{
	DWORD point;
	int cnt;
	int wpDamage[2];

	if (lpChar)
	{

		wpDamage[0] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[0];
		wpDamage[1] = cInvenTory.InvenItem[sInven[0].ItemIndex - 1].sItemInfo.Damage[1];

		point = (1 + petType) << 8;
		point |= SKILL_PLAY_PET_ATTACK2;
		cnt = dm_SendTransDamage(lpChar, wpDamage[0], wpDamage[1], 0, 0, point, FALSE);
		DamageExp(lpChar, cnt);

		return cnt;
	}
	return FALSE;
}



//¾ÆÀÌÅÛ ÅÃ¹è ¼­ºñ½º ¼ö½Å ¿ä±¸
int	SendItemExpress(DWORD	dwItemCode, char *szPassCode)
{
	smTRANS_COMMAND	smTransCommand;

	ZeroMemory((char *)&smTransCommand, sizeof(smTRANS_COMMAND));
	smTransCommand.code = smTRANSCODE_ITEM_EXPRESS;
	smTransCommand.size = sizeof(smTRANS_COMMAND);
	smTransCommand.WParam = dwItemCode;

	if (szPassCode)
		smTransCommand.LParam = GetSpeedSum(szPassCode);

	if (smWsockDataServer)
		return smWsockDataServer->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

	return FALSE;
}

//¾ÆÀÌÅÛ ÅÃ¹è ¼­ºñ½º ¼ö½Å ¿ä±¸
int	SendItemExpress(DWORD	dwItemCode)
{
	return	SendItemExpress(dwItemCode, 0);
}


//¾ßÈ£ ¸ð¼Ç ¼­¹ö·Î º¸³»±â
int SendClanYahooMotion()
{
	smTRANS_COMMAND	smTransCommand;
	smWINSOCK *lpsmSock;

	if (lpCurPlayer->smCharInfo.ClassClan)
	{
		smTransCommand.size = sizeof(smTRANS_COMMAND);
		smTransCommand.code = smTRANSCODE_YAHOO_MOTION;
		smTransCommand.WParam = dwPlayTime;
		smTransCommand.LParam = 0;
		smTransCommand.SParam = 0;
		smTransCommand.EParam = 0;

		lpsmSock = GetAreaServerSock();		//Áö¿ª¼­¹ö ¼ÒÄÏÀ» Ã£À½
		if (lpsmSock)
			lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

		return TRUE;

	}
	return FALSE;
}

//º° Æ÷ÀÎÆ® Á¤º¸¸¦ ¼­¹ö¿¡ º¸³½´Ù
int	SendStarPointToServer(int Price, int Cash)
{
	Log::Debug("SendStarPointToServer : Requisitado porém foi removido.");
	return NULL;
}
//±âºÎÇÑµ·À» ¼­¹ö¿¡ º¸³½´Ù
int	SendGiveMoneyToServer(int Money)
{
	Log::Debug("SendGiveMoneyToServer : Requisitado porém foi removido.");
	return NULL;
}
//Å¬·£¸Ó´Ï Á¤º¸¸¦ ¼­¹ö¿¡ º¸³½´Ù
int	SendClanMoneyToServer(int Money, int Flag, int Kind)
{
	Log::Debug("SendClanMoneyToServer : Requisitado porém foi removido.");
	return NULL;
}
int	SendClanMoneyToServer(int Money, int Flag)
{
	Log::Debug("SendClanMoneyToServer : Requisitado porém foi removido.");
	return NULL;
}
//ÀÌ¿ë¿ä±Ý Á¤º¸¸¦ ¼­¹ö¿¡ º¸³½´Ù
int	SendPaymentMoneyToServer(int Money, int Flag)
{
	Log::Debug("SendPaymentMoneyToServer : Requisitado porém foi removido.");
	return NULL;
}


/*
//°³ÀÎ»óÁ¡ ¿ÀÇÂ
#define smTRANSCODE_OPEN_PERTRADE	0x48478A90
//°³ÀÎ»óÁ¡ ºÎ¸£±â
#define smTRANSCODE_CALL_PERTRADE	0x48478AA0
//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ
#define	smTRANSCODE_MYSHOP_ITEM		0x48478AB0

int	Send_ShowMyShopItem( smCHAR *lpChar );							//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ ³»¿ë ¿ä±¸
int	Send_MyShopItemList( smTRANS_COMMAND	*lpTransCommand );		//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ ³»¿ë »ó´ë¹æ¿¡ º¸³»±â
int	Recv_MyShopItemList( TRANS_TRADEITEMS *lpTransTradeItems );		//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ ³»¿ë ¹ÞÀ½
*/


//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ ³»¿ë ¿ä±¸
int	Send_ShowMyShopItem(smCHAR *lpChar)
{
	smTRANS_COMMAND	smTransCommand;
	int	result;
	smWINSOCK	*lpsmSock;

	smTransCommand.size = sizeof(smTRANS_COMMAND);
	smTransCommand.code = smTRANSCODE_CALL_MYSHOP;
	smTransCommand.WParam = 0;
	smTransCommand.LParam = lpChar->dwObjectSerial;
	smTransCommand.SParam = lpCurPlayer->dwObjectSerial;
	smTransCommand.EParam = 0;

	lpsmSock = GetAreaServerSock();
	if (lpsmSock)
		result = lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

	return result;
}

//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ ³»¿ë »ó´ë¹æ¿¡ º¸³»±â
int	Send_MyShopItemList(smTRANS_COMMAND	*lpTransCommand)
{
	int	result;
	smWINSOCK	*lpsmSock;

	TransTradeItems_MyShop.dwRecver = lpTransCommand->SParam;

	lpsmSock = GetAreaServerSock();
	if (lpsmSock)
		result = lpsmSock->Send2((char *)&TransTradeItems_MyShop, TransTradeItems_MyShop.size, TRUE);

	return result;
}


//°³ÀÎ»óÁ¡ ¾ÆÀÌÅÛ ³»¿ë ¹ÞÀ½
int	Recv_MyShopItemList(TRANS_TRADEITEMS *lpTransTradeItems)
{
	sMYSHOP MyShopItem;
	smCHAR	*lpChar;

	//¾ÐÃà µ¥ÀÌÅ¸ ÇØµ¶ ( Z/NZ ¹æ½Ä )
	DecodeCompress(lpTransTradeItems->TradeBuff, (BYTE *)&MyShopItem);

	//»ó·Ä¾¾ ÇÔ¼ö È£Ãâ ¿¹Á¤
	cCharShop.RecvShopItem(&MyShopItem);


	lpChar = FindAutoPlayer(lpTransTradeItems->dwSender);

	if (lpChar && lpChar->DisplayFlag && lpChar->smCharInfo.szName[0])
	{
		OpenEachPlayer(lpChar);
		DispEachMode = TRUE;
	}

	return TRUE;
}


//°³ÀÎ »óÁ¡ ¹°°Ç ±¸ÀÔ
int	Send_PersonalShopItem(DWORD dwCharCode, void *lpShopItem)
{
	smTRANS_COMMAND_BUFF	smtBuff;


	smtBuff.smTransCommand.code = smTRANSCODE_MYSHOP_TRADE;
	smtBuff.smTransCommand.size = sizeof(smTRANS_COMMAND) + sizeof(sMYSHOP_ITEM_SERVER);
	smtBuff.smTransCommand.WParam = 0;
	smtBuff.smTransCommand.LParam = dwCharCode;
	smtBuff.smTransCommand.SParam = lpCurPlayer->dwObjectSerial;
	smtBuff.smTransCommand.EParam = 0;

	memcpy(smtBuff.Buff, lpShopItem, sizeof(sMYSHOP_ITEM_SERVER));

	if (smWsockDataServer)
		return smWsockDataServer->Send2((char *)&smtBuff, smtBuff.smTransCommand.size, TRUE);

	return FALSE;
}

//°³ÀÎ »óÁ¡ ¾ÆÀÌÅÛ Á¤º¸ ¾÷µ¥ÀÌÆ®
int	UpdateMyShopList(void *lpMyShop)
{
	sMYSHOP *lpMyShopItem = (sMYSHOP *)lpMyShop;
	int	result = 0;
	int		len;

	ZeroMemory(&TransTradeItems_MyShop, sizeof(TRANS_TRADEITEMS));

	//µ¥ÀÌÅ¸ ¾ÐÃà ( Z/NZ ¹æ½Ä )
	len = EecodeCompress((BYTE *)lpMyShopItem, TransTradeItems_MyShop.TradeBuff, sizeof(sMYSHOP));
	if (len > TRANS_TRADE_BUFF_SIZE) return FALSE;

	TransTradeItems_MyShop.code = smTRANSCODE_MYSHOP_ITEM;
	TransTradeItems_MyShop.size = len + 48;
	TransTradeItems_MyShop.dwSum = 0;
	TransTradeItems_MyShop.dwSender = lpCurPlayer->dwObjectSerial;
	TransTradeItems_MyShop.dwRecver = 0;
	TransTradeItems_MyShop.Temp[0] = 0;
	TransTradeItems_MyShop.Temp[1] = 0;
	TransTradeItems_MyShop.Temp[2] = 0;
	TransTradeItems_MyShop.Temp[3] = 0;

	return TRUE;
}

//°³ÀÎ »óÁ¡ ¿ÀÇÂ
int SendOpenPersonalTrade(char *szTradeMsg, void *lpPersTrade)
{
	TRANS_CHATMESSAGE	TransChatMessage;
	smWINSOCK	*lpsmSock;
	int	result = 0;
	DWORD	dwCode;
	sMYSHOP *lpMyShopItem = (sMYSHOP *)lpPersTrade;
	int		len;

	//smTRANSCODE_OPEN_PERTRADE
	//lpMyShopItem->

	ZeroMemory(&TransTradeItems_MyShop, sizeof(TRANS_TRADEITEMS));
	//memcpy( &TransMyShop.sMyShop , lpMyShopItem , sizeof(sMYSHOP) );

	ZeroMemory(&TransChatMessage, sizeof(TRANS_CHATMESSAGE));

	//µ¥ÀÌÅ¸ ¾ÐÃà ( Z/NZ ¹æ½Ä )
	len = EecodeCompress((BYTE *)lpMyShopItem, TransTradeItems_MyShop.TradeBuff, sizeof(sMYSHOP));
	if (len > TRANS_TRADE_BUFF_SIZE) return FALSE;

	TransTradeItems_MyShop.code = smTRANSCODE_MYSHOP_ITEM;
	TransTradeItems_MyShop.size = len + 48;
	TransTradeItems_MyShop.dwSum = 0;
	TransTradeItems_MyShop.dwSender = lpCurPlayer->dwObjectSerial;
	TransTradeItems_MyShop.dwRecver = 0;
	TransTradeItems_MyShop.Temp[0] = 0;
	TransTradeItems_MyShop.Temp[1] = 0;
	TransTradeItems_MyShop.Temp[2] = 0;
	TransTradeItems_MyShop.Temp[3] = 0;


	TransChatMessage.code = smTRANSCODE_OPEN_MYSHOP;
	TransChatMessage.size = sizeof(TRANS_CHATMESSAGE);
	lstrcpy(TransChatMessage.szMessage, szTradeMsg);

	dwCode = GetSpeedSum(szTradeMsg);
	TransChatMessage.dwIP = dwCode;

	lpsmSock = GetAreaServerSock();
	if (lpsmSock)
		//ÇØ¿Ü Áß±¹(°³ÀÎ»óÁ¡ÀÌ¸§°ü·Ã)
		//#ifdef _LANGUAGE_CHINESE
		//		ConvertStringTHAI((char *)&TransChatMessage, lstrlen((char *)&TransChatMessage)+16);
		//#endif
		result = lpsmSock->Send2((char *)&TransChatMessage, TransChatMessage.size, TRUE);

	if (result)
	{
		/*
		//ÇØ¿Ü Áß±¹(°³ÀÎ»óÁ¡ÀÌ¸§°ü·Ã)
		#ifdef _LANGUAGE_CHINESE
				ConvertStringTHAI(szTradeMsg, lstrlen(szTradeMsg)+16);
		#endif
		*/
		wsprintf(lpCurPlayer->szTradeMessage, "%s: %s", lpCurPlayer->smCharInfo.szName, szTradeMsg);
		lpCurPlayer->dwTradeMsgCode = dwCode;
	}

	return result;
}

//»óÁ¡ ´Ý±â
int SendClosePersonalTrade()
{
	TRANS_CHATMESSAGE	TransChatMessage;
	smWINSOCK	*lpsmSock;
	int	result = 0;

	ZeroMemory(&TransChatMessage, sizeof(TRANS_CHATMESSAGE));

	TransChatMessage.code = smTRANSCODE_OPEN_MYSHOP;
	TransChatMessage.size = sizeof(TRANS_CHATMESSAGE);
	lpsmSock = GetAreaServerSock();
	if (lpsmSock)
		result = lpsmSock->Send2((char *)&TransChatMessage, TransChatMessage.size, TRUE);

	lpCurPlayer->dwTradeMsgCode = 0;
	lpCurPlayer->szTradeMessage[0] = 0;

	return result;
}

// Àåº° - ÀÔ·ÂÃ¢
int SendsServerDoc(char *szTradeMsg)
{
	TRANS_CHATMESSAGE	TransChatMessage;
	smWINSOCK	*lpsmSock;
	int	result = 0;


	ZeroMemory(&TransChatMessage, sizeof(TRANS_CHATMESSAGE));

	TransChatMessage.code = smTRANSCODE_ITEMDOC;
	TransChatMessage.size = sizeof(TRANS_CHATMESSAGE);
	lstrcpy(TransChatMessage.szMessage, szTradeMsg);

	lpsmSock = GetAreaServerSock();
	if (lpsmSock)
		result = lpsmSock->Send2((char *)&TransChatMessage, TransChatMessage.size, TRUE);

	return result;
}


//³¯Â¥ È®ÀÎ Á¦°Å ¾ÆÀÌÅÛ 
int DeleteEventItem_TimeOut(sITEMINFO	*lpsItem)
{
	// ¹ÚÀç¿ø - ¿¡ÀÌÂ¡ ¸¶½ºÅÍ, ½ºÅ³ ¸¶½ºÅÍ Ãß°¡
	if (lpsItem->CODE == (sinBI1 | sin32) || lpsItem->CODE == (sinBI1 | sin33) || lpsItem->CODE == (sinBI1 | sin34) || lpsItem->CODE == (sinBI1 | sin51) ||// pluto Æê(ÇØ¿Ü)
	//	lpsItem->CODE == (sinBI1|sin35) || lpsItem->CODE == (sinBI1|sin36) || lpsItem->CODE == (sinBI1|sin37) || lpsItem->CODE == (sinBI1|sin38) )	//¼öÇ¥
		lpsItem->CODE == (sinBI1 | sin36) || lpsItem->CODE == (sinBI1 | sin37) ||
		lpsItem->CODE == (sinBI1 | sin38) || lpsItem->CODE == (sinBI1 | sin39) ||
		lpsItem->CODE == (sinBI1 | sin40) || lpsItem->CODE == (sinBI1 | sin41) ||
		lpsItem->CODE == (sinBI1 | sin42) || lpsItem->CODE == (sinBI1 | sin43) ||
		lpsItem->CODE == (sinBI1 | sin44) || lpsItem->CODE == (sinBI1 | sin45) ||
		lpsItem->CODE == (sinBI1 | sin46) || lpsItem->CODE == (sinBI1 | sin47) ||
		lpsItem->CODE == (sinBI1 | sin48) || lpsItem->CODE == (sinBI1 | sin49) ||
		lpsItem->CODE == (sinBI1 | sin50) || lpsItem->CODE == (sinBI1 | sin52) || // ¹ÚÀç¿ø - ºô¸µ µµ¿ì¹Ì Æê Ãß°¡
		lpsItem->CODE == (sinBI1 | sin53) || lpsItem->CODE == (sinBI1 | sin54) ||

		lpsItem->CODE == (sinBI1 | sin65) || lpsItem->CODE == (sinBI1 | sin66) || // Àåº° - ºô¸µ µµ¿ì¹Ì Æê (7ÀÏ, 1ÀÏ)
		lpsItem->CODE == (sinBI1 | sin67) || lpsItem->CODE == (sinBI1 | sin68) ||
		lpsItem->CODE == (sinBI1 | sin69) || lpsItem->CODE == (sinBI1 | sin70) ||
		lpsItem->CODE == (sinBI1 | sin71) || lpsItem->CODE == (sinBI1 | sin72) ||
		lpsItem->CODE == (sinBI1 | sin55) || lpsItem->CODE == (sinBI1 | sin77) ||	// Àåº° - ÇÇ´Ð½ºÆê(1½Ã°£)
		lpsItem->CODE == (sinBI1 | sin62) || lpsItem->CODE == (sinBI1 | sin63) ||	// ¹ÚÀç¿ø - ¿¡ÀÌÂ¡ ¸¶½ºÅÍ(2Â÷) ¾ÆÀÌÅÛ Ãß°¡ D, E
		lpsItem->CODE == (sinBI1 | sin64) ||										// ¹ÚÀç¿ø - ¿¡ÀÌÂ¡ ¸¶½ºÅÍ(2Â÷) ¾ÆÀÌÅÛ Ãß°¡ F
		lpsItem->CODE == (sinBI1 | sin73) || lpsItem->CODE == (sinBI1 | sin74) || // Àåº° - ºô¸µ µµ¿ì¹Ì Æê Ãß°¡(Å×¸® 1½Ã°£ ¿Ü 4Á¾)
		lpsItem->CODE == (sinBI1 | sin75) || lpsItem->CODE == (sinBI1 | sin76)) // Àåº° - ºô¸µ µµ¿ì¹Ì Æê (1½Ã°£)
	{
		return FALSE;
	}

	struct	tm	tm;
	time_t		ttm;//,ttm2;
	DWORD		dwCreateTime;

	DWORD		dwItemCode;

	dwItemCode = lpsItem->CODE&sinITEM_MASK2;

	DWORD NowTime = GetPlayTime_T(); // ÇöÀç ½Ã°£(second´ÜÀ§)

	//ÇÑº¹ 14ÀÏ
	if (lpsItem->CODE == (sinDA1 | sin48) || lpsItem->CODE == (sinDA2 | sin48))
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 14) < NowTime)
			return true;
	}
	//ÄÚ½ºÆ¬ 7ÀÏ
	if (lpsItem->CODE == (sinDA1 | sin31) || lpsItem->CODE == (sinDA1 | sin33) ||
		lpsItem->CODE == (sinDA1 | sin35) || lpsItem->CODE == (sinDA1 | sin37) ||
		lpsItem->CODE == (sinDA1 | sin39) || lpsItem->CODE == (sinDA1 | sin41) ||
		lpsItem->CODE == (sinDA1 | sin43) || lpsItem->CODE == (sinDA1 | sin45) ||
		lpsItem->CODE == (sinDA2 | sin31) || lpsItem->CODE == (sinDA2 | sin33) ||
		lpsItem->CODE == (sinDA2 | sin35) || lpsItem->CODE == (sinDA2 | sin37) ||
		lpsItem->CODE == (sinDA2 | sin39) || lpsItem->CODE == (sinDA2 | sin41) ||
		lpsItem->CODE == (sinDA2 | sin43) || lpsItem->CODE == (sinDA2 | sin45) ||
		lpsItem->CODE == (sinDB1 | sin31) || // ¹ÚÀç¿ø - ½ºÇÇµå ºÎÃ÷(7ÀÏ) Ãß°¡
		lpsItem->CODE == (sinOA2 | sin31) || // ¹ÚÀç¿ø - ½´ÆÛ ¾Ï¸´(7ÀÏ) Ãß°¡
		lpsItem->CODE == (sinOR2 | sin31) || // ¹ÚÀç¿ø - º¸½º ¸ó½ºÅÍ ¸µ Ãß°¡(¹Ùº§)
		lpsItem->CODE == (sinOR2 | sin32))  // ¹ÚÀç¿ø - º¸½º ¸ó½ºÅÍ ¸µ Ãß°¡(Ç»¸®)
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return true;
	}
	//ÄÚ½ºÆ¬ 30ÀÏ
	if (lpsItem->CODE == (sinDA1 | sin32) || lpsItem->CODE == (sinDA1 | sin34) ||
		lpsItem->CODE == (sinDA1 | sin36) || lpsItem->CODE == (sinDA1 | sin38) ||
		lpsItem->CODE == (sinDA1 | sin40) || lpsItem->CODE == (sinDA1 | sin42) ||
		lpsItem->CODE == (sinDA1 | sin44) || lpsItem->CODE == (sinDA1 | sin46) ||
		lpsItem->CODE == (sinDA1 | sin54) || lpsItem->CODE == (sinDA1 | sin55) || // ¹ÚÀç¿ø - ¼ö¿µº¹ º¹Àå Ãß°¡
		lpsItem->CODE == (sinDA2 | sin32) || lpsItem->CODE == (sinDA2 | sin34) ||
		lpsItem->CODE == (sinDA2 | sin36) || lpsItem->CODE == (sinDA2 | sin38) ||
		lpsItem->CODE == (sinDA2 | sin40) || lpsItem->CODE == (sinDA2 | sin42) ||
		lpsItem->CODE == (sinDA2 | sin44) || lpsItem->CODE == (sinDA2 | sin46) ||
		lpsItem->CODE == (sinDA2 | sin54) || lpsItem->CODE == (sinDA2 | sin55) || // ¹ÚÀç¿ø - ¼ö¿µº¹ º¹Àå Ãß°¡
		lpsItem->CODE == (sinDB1 | sin32) || // ¹ÚÀç¿ø - ½ºÇÇµå ºÎÃ÷(30ÀÏ) Ãß°¡
		lpsItem->CODE == (sinOA2 | sin32) || // ¹ÚÀç¿ø - ½´ÆÛ ¾Ï¸´(30ÀÏ) Ãß°¡
		lpsItem->CODE == (sinSP1 | sin34))  // ¹ÚÀç¿ø - È£¶ûÀÌ Ä¸½¶(30ÀÏ) Ãß°¡

	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 30) < NowTime)
			return true;
	}

	// 1ÀÏ
	if (lpsItem->CODE == (sinDB1 | sin33) || lpsItem->CODE == (sinOA2 | sin33)) // Àåº° - ½ºÇÇµå ºÎÃ÷(1ÀÏ) Ãß°¡ // Àåº° - ½´ÆÛ ¾Ï¸´(1ÀÏ)
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 1) < NowTime)
			return true;
	}

	// 1½Ã°£
	if (lpsItem->CODE == (sinDB1 | sin34) || lpsItem->CODE == (sinOA2 | sin34)) // Àåº° - ½ºÇÇµå ºÎÃ÷(1½Ã°£) Ãß°¡ // Àåº° - ½´ÆÛ ¾Ï¸´(1½Ã°£)
	{
		if (lpsItem->dwCreateTime + (60 * 60) < NowTime)
			return true;
	}

	//ÀÛ¼ºÀÚ : ¹ÚÀç¿ø, ÀÛ¼ºÀÏ : 08.04.07
	//³»  ¿ë : Å¬·£Ä¡ÇÁ ¸µ »ý¼ºÈÄ 5ÀÏÈÄ¿¡ »èÁ¦
	if (lpsItem->CODE == (sinOR2 | sin06) || lpsItem->CODE == (sinOR2 | sin07) ||
		lpsItem->CODE == (sinOR2 | sin08) || lpsItem->CODE == (sinOR2 | sin09) ||
		lpsItem->CODE == (sinOR2 | sin10) || lpsItem->CODE == (sinOR2 | sin11) ||
		lpsItem->CODE == (sinOR2 | sin12) || lpsItem->CODE == (sinOR2 | sin13) ||
		lpsItem->CODE == (sinOR2 | sin14) || lpsItem->CODE == (sinOR2 | sin15) ||
		lpsItem->CODE == (sinOR2 | sin16) || lpsItem->CODE == (sinOR2 | sin17) ||
		lpsItem->CODE == (sinOR2 | sin18) || lpsItem->CODE == (sinOR2 | sin19) ||
		lpsItem->CODE == (sinOR2 | sin20) || lpsItem->CODE == (sinOR2 | sin21) ||
		lpsItem->CODE == (sinOR2 | sin22) || lpsItem->CODE == (sinOR2 | sin23) ||
		lpsItem->CODE == (sinOR2 | sin24) || lpsItem->CODE == (sinOR2 | sin25))
	{
		// »ý¼º½Ã°£ + 5ÀÏ < ÇöÀç½Ã°£
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 5) < NowTime)
			return TRUE;
	}

	//ÀÛ¼ºÀÚ : ¹ÚÀç¿ø, ÀÛ¼ºÀÏ : 09.12.08
	//³»  ¿ë : »êÅ¸ ¸µ, »êÅ¸ ¾Æ¹Ä·¿ »ý¼ºÈÄ 3ÀÏ ÈÄ¿¡ »èÁ¦
	if (lpsItem->CODE == (sinOR2 | sin27) || lpsItem->CODE == (sinOA1 | sin32))  // »êÅ¸ ¸µ, »êÅ¸ ¾Æ¹Ä·¿ Ãß°¡
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // ¹ÚÀç¿ø : »êÅ¸¸µ, »êÅ¸¾Æ¹Ä·¿ÀÌ ¹Í½ºÃÄµÇ¸é¼­ ½Ã°£ÀÌ ÃÊ±âÈ­ µÇ¾î °­Á¦·Î »èÁ¦½ÃÅ°±â À§ÇÔ.
			// »ý¼º½Ã°£ + 3ÀÏ < ÇöÀç½Ã°£
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 3) < NowTime)
			return TRUE;
	}

	//ÀÛ¼ºÀÚ : ¹ÚÀç¿ø, ÀÛ¼ºÀÏ : 09.07.31
	//³»  ¿ë : ÀÌº¥Æ® ¸µ, ÀÌº¥Æ® ¾Æ¹Ä·¿ »ý¼ºÈÄ 7ÀÏ ÈÄ¿¡ »èÁ¦
	if (lpsItem->CODE == (sinOR2 | sin28) || lpsItem->CODE == (sinOA1 | sin33))  // ÀÌº¥Æ® ¸µ, ÀÌº¥Æ® ¾Æ¹Ä·¿ Ãß°¡
	{
		// »ý¼º½Ã°£ + 7ÀÏ < ÇöÀç½Ã°£
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return TRUE;
	}

	//ÀÛ¼ºÀÚ : ¹ÚÀç¿ø, ÀÛ¼ºÀÏ : 09.12.17
	//³»  ¿ë : ÀÌº¥Æ® ¸µ, ÀÌº¥Æ® ¾Æ¹Ä·¿ »ý¼ºÈÄ 1½Ã°£ ÈÄ¿¡ »èÁ¦
	if (lpsItem->CODE == (sinOR2 | sin29) || lpsItem->CODE == (sinOA1 | sin34))  // ÀÌº¥Æ® ¸µ, ÀÌº¥Æ® ¾Æ¹Ä·¿ Ãß°¡
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // ¹ÚÀç¿ø - ÀÌº¥Æ®¸µ, ÀÌº¥Æ®¾Æ¹Ä·¿ °­Á¦·Î »èÁ¦½ÃÅ°±â À§ÇÔ.
		//		return TRUE;
		// »ý¼º½Ã°£ + 1½Ã°£ < ÇöÀç½Ã°£
		if (lpsItem->dwCreateTime + (60 * 60) < NowTime)
			return TRUE;
	}

	//ÀÛ¼ºÀÚ : ¹ÚÀç¿ø, ÀÛ¼ºÀÏ : 09.12.17
	//³»  ¿ë : ÀÌº¥Æ® ¸µ, ÀÌº¥Æ® ¾Æ¹Ä·¿ »ý¼ºÈÄ 1ÀÏ ÈÄ¿¡ »èÁ¦
	if (lpsItem->CODE == (sinOR2 | sin30) || lpsItem->CODE == (sinOA1 | sin35))  // ÀÌº¥Æ® ¸µ, ÀÌº¥Æ® ¾Æ¹Ä·¿ Ãß°¡
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // ¹ÚÀç¿ø - ÀÌº¥Æ®¸µ, ÀÌº¥Æ®¾Æ¹Ä·¿ °­Á¦·Î »èÁ¦½ÃÅ°±â À§ÇÔ.
		//		return TRUE;
		// »ý¼º½Ã°£ + 1ÀÏ < ÇöÀç½Ã°£
		if (lpsItem->dwCreateTime + (60 * 60 * 24) < NowTime)
			return TRUE;
	}

	//ÀÛ¼ºÀÚ : Àåº°, ÀÛ¼ºÀÏ : 10.02.02
	//³»  ¿ë : ÇÏÆ®¸µ »ý¼ºÈÄ 7ÀÏ ÈÄ¿¡ »èÁ¦
	if (lpsItem->CODE == (sinOR2 | sin33))  // ÇÏÆ®¸µ Ãß°¡
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // ¹ÚÀç¿ø - ÀÌº¥Æ®¸µ, ÀÌº¥Æ®¾Æ¹Ä·¿ °­Á¦·Î »èÁ¦½ÃÅ°±â À§ÇÔ.
		//		return TRUE;
		// »ý¼º½Ã°£ + 7ÀÏ < ÇöÀç½Ã°£
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return TRUE;
	}


	// 7ÀÏ
	if (lpsItem->CODE == (sinOA1 | sin36) || lpsItem->CODE == (sinOA1 | sin37) || // Àåº° - ´«²É ¾Æ¹Ä·¿, ÇÏÆ® ¾Æ¹Ä·¿
		lpsItem->CODE == (sinOR2 | sin36) || lpsItem->CODE == (sinOR2 | sin37) || // Àåº° - ¼Ò¿ï½ºÅæ
		lpsItem->CODE == (sinOR2 | sin38) || lpsItem->CODE == (sinOR2 | sin39) ||
		lpsItem->CODE == (sinOR2 | sin40))
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return TRUE;
	}

	// Àåº° - ¼Ò¿ï½ºÅæ 14ÀÏ // Àåº° - º¹³¯ ÀÌº¥Æ® ¾Æ¹Ä·¿, ¸µ
	if (lpsItem->CODE == (sinOA1 | sin39) ||
		lpsItem->CODE == (sinOA1 | sin40) || lpsItem->CODE == (sinOA1 | sin41) ||
		lpsItem->CODE == (sinOA1 | sin42) ||
		lpsItem->CODE == (sinOA1 | sin38) || lpsItem->CODE == (sinOR2 | sin34) || lpsItem->CODE == (sinOR2 | sin35))
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 14) < NowTime)
			return TRUE;
	}



	//(ÁöÁ¸¹ÝÁö , ÆÛÁñµîÀº ³¯Â¥ È®ÀÎÇÏ¿© Á¦°Å)
	if (lpsItem->CODE != (sinOR2 | sin01) &&
		dwItemCode != sinPZ1 && dwItemCode != sinPZ2)
		return FALSE;

#ifdef _LANGUAGE_THAI_DELETE_RING
	if (lpsItem->CODE == (sinOR2 | sin01)) return TRUE;
#endif

	tm.tm_year = 2004 - 1900;	// 2004³â
	tm.tm_mon = 4 - 1;			// 4¿ù
	tm.tm_mday = 1;			// 1ÀÏ
	tm.tm_hour = 15;			// 15½Ã
	tm.tm_min = 30;				// 30ºÐ
	tm.tm_sec = 0;				// 0ÃÊ
	ttm = mktime(&tm);		//Á¦ÇÑ ½Ã°£

	dwCreateTime = (DWORD)ttm;

	if (lpsItem->dwCreateTime < dwCreateTime) return TRUE;

	return FALSE;
}

int RecvSodGameInfomation(void *Info)
{
	smTRANS_COMMAND_SOD	*lpTransCommand = (smTRANS_COMMAND_SOD *)Info;

	if (!BellatraEffectInitFlag)
	{
		CreateBellatraFontEffect();
		BellatraEffectInitFlag = TRUE;
	}

	switch (lpTransCommand->smTransCommand.WParam)
	{
		case 1:
			if (SoD_SetFontEffect.eBL_Type == E_BL_LODING)
			{
				CheckBellatraFontEffect(&SoD_SetFontEffect, TRUE);	//·Îµù Á¾·á
			}

			if (lpTransCommand->smTransCommand.SParam < 0)
			{
				//SoD ¶ó¿îµå ÀÌÆåÆ®
				//Sod_SodEffect( lpTransCommand->smTransCommand.LParam+1 );
				sSodScore.EffectStartCount = 70 * 3;
				sSodScore.EffectCode = lpTransCommand->smTransCommand.LParam + 1;
			}
			else
			{
				if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) break;		//Á×Àº°æ¿ì ¹«½Ã

				sSodScore.SodNextStageNum = lpTransCommand->smTransCommand.SParam;
				sSodScore.dwSoD_NextStageTime = dwPlayTime + 9000;
				sSodScore.dwSoD_CloseStageTime = dwPlayTime + 5000;
				sSodScore.NextRound = lpTransCommand->smTransCommand.LParam + 1;

				StopBGM();
				esPlayContSound(14);
				lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_YAHOO);

				SetBellatraFontEffect(E_BL_FONT_STAGE);
				SetBellatraFontEffect(E_BL_FONT_COMPLETE);
				SoDGateFlag = TRUE;
			}
			break;

		case 3:
			//½ÇÆÐ µ¹¾Æ°¡±â
			if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) break;		//Á×Àº°æ¿ì ¹«½Ã

			sSodScore.SodNextStageNum = -1;
			sSodScore.dwSoD_NextStageTime = dwPlayTime + 8000;
			sSodScore.dwSoD_CloseStageTime = dwPlayTime + 6000;
			sSodScore.NextRound = -1;

			StopBGM();
			esPlayContSound(15);

			SetBellatraFontEffect(E_BL_FONT_STAGE);
			SetBellatraFontEffect(E_BL_FONT_FAIL);
			SoDGateFlag = TRUE;
			break;

		case 4:
			//¸ðµÎ Å¬¸®¾î
			if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) break;		//Á×Àº°æ¿ì ¹«½Ã

			sSodScore.SodNextStageNum = lpTransCommand->smTransCommand.SParam;
			sSodScore.dwSoD_NextStageTime = dwPlayTime + 7000;
			sSodScore.NextRound = lpTransCommand->smTransCommand.LParam + 1;

			StopBGM();
			esPlayContSound(16);
			lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_YAHOO);

			//SetBellatraFontEffect( E_BL_FONT_STAGE );
			SetBellatraFontEffect(E_BL_FONT_CON);
			SoDGateFlag = FALSE;

			//È¹µæÇÑ ±Ý¾× Ãâ·Â
			char szBuff[128];
			wsprintf(szBuff, mgSOD_Clear, lpTransCommand->smTransCommand.EParam);
			cMessageBox.ShowMessageEvent(szBuff);

			break;

		case 2:
			//Á¡¼ö ¸ñ·Ï ¹ÞÀ½
			if (lpTransCommand->smTransCommand.LParam)
			{
				//³»Á¡¼ö È¹µæ¿¡ ÀÇÇÑ Á¡¼ö ¹ÞÀ½
				sSodScore.ScoreEffectCount = 255;

			}
			else
			{
				//ÁÖ±âÀûÀ¸·Î º¸³»Áö´Â ÀüÃ¼ Á¡¼ö


			}

			if (!sSodScore.dwSoD_NextStageTime)
			{
				if (sSodScore.Round != lpTransCommand->smTransCommand.SParam)
				{
					switch (lpTransCommand->smTransCommand.SParam)
					{
						case 1:		//1¶ó¿îµå ½ÃÀÛ
							sSodScore.dwPlayTime = dwPlayTime;
							PlayBGM_Direct(BGM_CODE_SOD1);
							break;

						case 4:
							PlayBGM_Direct(BGM_CODE_SOD2);
							break;

						case 7:
							PlayBGM_Direct(BGM_CODE_SOD3);
							break;
					}
				}
				sSodScore.Round = lpTransCommand->smTransCommand.SParam;
			}

			sSodScore.dwDispTime = dwPlayTime + 10 * 1000;
			sSodScore.MyTeam = lpTransCommand->MyTeam;
			sSodScore.Score = lpTransCommand->MyScore;
			sSodScore.TeamScore[0] = lpTransCommand->TeamScore[0];
			sSodScore.TeamScore[1] = lpTransCommand->TeamScore[1];
			sSodScore.TeamScore[2] = lpTransCommand->TeamScore[2];
			sSodScore.TeamScore[3] = lpTransCommand->TeamScore[3];

			break;

		case smCODE_SOD_EFFECT:					//ÁÖ¹® ½Ã½ºÅÛ Ãß°¡ - SOD ÀÌÆåÆ® Àç»ç¿ë
			if (!BellatraEffectInitFlag)
			{
				CreateBellatraFontEffect();
				BellatraEffectInitFlag = TRUE;
			}
			SetBellatraFontEffect((EBL_FontEffectType)lpTransCommand->smTransCommand.LParam);
			break;

	}

	return TRUE;
}

//Æ÷½º¿Àºê ¾ÆÀÌÅÛ ÀÔ¼ö
int RecvForceOrbItem(TRANS_ITEMINFO_GROUP2 *lpTransItemGroup2)
{
	TRANS_ITEMINFO_GROUP	TransItemGroup;

	ZeroMemory(&TransItemGroup, sizeof(TRANS_ITEMINFO_GROUP));
	memcpy(&TransItemGroup, lpTransItemGroup2, TRANS_GROUPITEM_HEADER_SIZE);

	DecodeCompress((BYTE *)lpTransItemGroup2->szBuff, (BYTE *)TransItemGroup.sItemInfo, sizeof(sITEMINFO)*TRANS_GROUPITEM_MAX);

	//¹Ú»ó·Ä ÇÔ¼ö È£Ãâ
	//Æ÷½º ¾ÆÀÌÅÛÀ» ¹Þ´Â´Ù
	sinRecvForceOrb(TransItemGroup.sItemInfo, TransItemGroup.ItemCount);


	return TRUE;
}

//Äù½ºÆ® ¸í·É º¸³»±â
int SendQuestCommandToServer(DWORD dwQuestCode, int Param1, int Param2, int Param3)
{
	Log::Debug("SendQuestCommandToServer : Requisitado porém foi removido.");
	return NULL;
}
//´ëÀü Äù½ºÆ® ½ÇÇà
int Start_QuestArena(DWORD dwQuestCode, int Param1, int Param2)
{
	smTRANS_COMMAND	smTransCommand;
	int	result = 0;
	smWINSOCK *lpsmSock;

	//if ( dwQuestCode==SIN_QUEST_CODE_CHANGEJOB4 ) {

	smTransCommand.size = sizeof(smTRANS_COMMAND);
	smTransCommand.code = smTRANSCODE_QUEST_COMMAND;
	smTransCommand.WParam = dwQuestCode;
	smTransCommand.LParam = QUEST_ARENA_FIELD;
	smTransCommand.SParam = Param1;
	smTransCommand.EParam = Param2;

	lpsmSock = GetAreaServerSock();
	if (lpsmSock)
		result = lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

	if (result)
		WarpField2(QUEST_ARENA_FIELD);

	//}

	return FALSE;
}

//ºí·¡½ºÄ³½½ ¼³Á¤Á¤º¸ ¼­¹ö¿¡ º¸³¿
int	SendBlessCastleToServer(smTRANS_BLESSCASTLE *lpBlessCastleSetup, int Mode)
{
	Log::Debug("SendBlessCastleToServer : Requisitado porém foi removido.");
	return NULL;
}
int	Send_GetBlessCastleTax()
{
	smTRANS_COMMAND	smTransCommand;

	ZeroMemory(&smTransCommand, sizeof(smTRANS_COMMAND));

	smTransCommand.code = smTRANSCODE_BLESSCASTLE_INFO;
	smTransCommand.size = sizeof(smTRANS_COMMAND);
	smTransCommand.SParam = cSinSiege.GetTaxRate();

	if (smWsockDataServer)
		return smWsockDataServer->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

	return FALSE;
}
//ºí·¡½ºÄ³½½ ¼³Á¤Á¤º¸ ¼ö½Å
int RecvBlessCastInfo(void *lpPacket)
{
	smTRANS_BLESSCASTLE *lpBlessCastleSetup = (smTRANS_BLESSCASTLE *)lpPacket;
	rsUSER_LIST_TOP10	*lpClanListTop10;

	switch (lpBlessCastleSetup->smTransCommand.WParam)
	{
		case 1:		//¼³Á¤Á¤º¸ ¼ö½Å
			chaSiege.ShowSiegeMenu(lpBlessCastleSetup);
			break;

		case 2:		//¼øÀ§ Á¤º¸ ¼ö½Å
			lpClanListTop10 = (rsUSER_LIST_TOP10 *)lpPacket;
			//¼øÀ§ Ãâ·Â ÇÔ¼ö È£Ãâ
			chaSiege.ShowSiegeScore(lpClanListTop10);
			break;
	}

	return TRUE;
}

//ºí·¹½º Ä³½½ ¸¶½ºÅÍ Å¬·£ ¼³Á¤
int SetBlessCastleMaster(DWORD dwClanCode, int Mode)
{

	if (!Mode && lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField]->FieldCode == rsCASTLE_FIELD)
	{

	}
	else
	{
		if (rsBlessCastle.dwMasterClan != dwClanCode)
		{
			//Ä³½½ ¸¶½ºÅÍ ¾÷µ¥ÀÌÆ®
			UpdateCastleMasterClan(rsBlessCastle.dwMasterClan);
		}

		rsBlessCastle.dwMasterClan = dwClanCode;
	}

	return TRUE;
}

//¼Ó¼ºÁ¤º¸ ¼­¹ö·Î Àü¼Û
int	SendResistanceToServer()
{
	Log::Debug("SendResistanceToServer : Requisitado porém foi removido.");
	return NULL;
}
//¼³¹®Á¶»ç °á°ú ¼­¹ö¿¡ º¸³»±â
int	SendPublicPollingToServer(int PollCode, int PollCnt, BYTE *bPollingData)
{
	Log::Debug("SendPublicPollingToServer : Requisitado porém foi removido.");
	return NULL;
}
int XTrap_Recv(smTRANS_COMMAND *lpPacket, smWINSOCK *lpsmSock)
{
	return TRUE;
}

int	HackTrap_OpenFlagMask = 0;

int HackTrap_CheckOpenFlag()
{
	Log::Debug("HackTrap_CheckOpenFlag : Requisitado porém foi removido.");
	return NULL;
}

int	SendHackTrapToServer(DWORD dwTrapCode, int Param)
{
	Log::Debug("SendHackTrapToServer : Requisitado porém foi removido.");
	return NULL;
}

int SendSmeltingItemToServer(void *lpsSmeltingItem_Send)
{
	Log::Debug("SendSmeltingItemToServer : Requisitado porém foi removido.");
	return NULL;
}

int SendManufactureItemToServer(void *lpsManufactureItem_Send)
{
	Log::Debug("SendManufactureItemToServer : Requisitado porém foi removido.");
	return NULL;
}

int SendMixtureResetItemToServer(void *lpsMixtureResetItem_Send)
{
	Log::Debug("SendMixtureResetItemToServer : Requisitado porém foi removido.");
	return NULL;
}