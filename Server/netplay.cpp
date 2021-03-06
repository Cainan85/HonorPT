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
//작 성 자 : 오 영 석
#include "WinInt\\WinIntThread.h"
#include "WinInt\\WavIntHttp.h"
#include "WinInt\\ZipLib.h"
//######################################################################################

#include "CurseFilter.h"		//IsCurse 함수(해외)

#ifdef _XTRAP_GUARD_4_
#include ".\\XTrapSrcD5\\Client\\XTrap4Client.h"	//XTrapD5
//#include ".\\XTrapSrc4\\XTrap.h"
#endif

//박재원 - XignCode
#ifdef _XIGNCODE_CLIENT
#include "Xigncode\Client\zwave_sdk_client.h"
#endif

extern int WinSizeX;
extern int WinSizeY;
extern HWND hwnd;

//스킬취소 함수다!!
//지속 스킬 체크
//int cSKILL::CancelContinueSkill(DWORD CODE)


//새버전 적용시 (서버별 캐릭터 정보 나눠가짐 )
#define SERVER_CHAR_INFO

//자동 저장 시간 ( 5분 간격 )
#define RECORD_TIME_STEP		(5*60*1000)
//최초 로딩 기다리는 시간 ( 10초 )
#define NET_LOADING_WAIT_LIMIT		10000

//크리티컬 섹션 선언
extern CRITICAL_SECTION	cSection;
extern rsRECORD_DBASE	rsRecorder;					//서버에 게임데이타 기록장치
DWORD	dwLastRecordTime = 0;

CRITICAL_SECTION	cServSection;

int Debug_SendCount = 0;
int Debug_RecvCount1 = 0;
int Debug_RecvCount2 = 0;
int Debug_RecvCount3 = 0;

int NetWorkInitFlag = 0;
int	BellatraEffectInitFlag = 0;			//벨라트라 이펙트 초기화 플랙
SBL_SetFontEffect SoD_SetFontEffect;	//SoD 용 폰트 이펙트


DWORD	dwTime_ServerT = 0;		//접속시 시간 (서버측 Time_T )
DWORD	dwTime_ConnectMS = 0;	//접속시 시간 (클라이언트ms)


//#define	CLIENT_VERSION_NUM		2235

//중국
#ifdef	_LANGUAGE_CHINESE
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		1031
#else
#define	CLIENT_VERSION_NUM		15031
#endif
#endif

//일본
#ifdef	_LANGUAGE_JAPANESE

#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		4100
#else
#define	CLIENT_VERSION_NUM		15100
#endif
#endif

//대만
#ifdef _LANGUAGE_TAIWAN
#define	CLIENT_VERSION_NUM		3003
#endif

#ifdef _LANGUAGE_ENGLISH
//#ifdef _LANGUAGE_PHILIPIN
//	#ifndef	_WINMODE_DEBUG
//	#define	CLIENT_VERSION_NUM		4060		//필리핀
//	#else
//	#define	CLIENT_VERSION_NUM		15060		//필리핀GM
//	#endif
//#else
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		3122		//영문
#else
#define	CLIENT_VERSION_NUM		15122		//영문GM
#endif
//#endif

#endif

//베트남
#ifdef _LANGUAGE_VEITNAM
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		10565
#else
#define	CLIENT_VERSION_NUM		15565
#endif
#endif

//태국
#ifdef _LANGUAGE_THAI
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		3223
#else
#define	CLIENT_VERSION_NUM		15223
#endif
#endif

//브라질
#ifdef _LANGUAGE_BRAZIL
#ifndef	_WINMODE_DEBUG
#define	CLIENT_VERSION_NUM		4135
#else
#define	CLIENT_VERSION_NUM		15135
#endif
#endif

//아르헨티나
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
/////////////////////////// 버전 정보 ///////////////////////////////////////////
int	Client_Version = CLIENT_VERSION_NUM;		//현재 버전 클라이언트 확인용
int	Server_LimitVersion = 167;					//현재 버전 서버 확인용
/////////////////////////////////////////////////////////////////////////////////

int	Version_WareHouse = 1;						//창고 저장데이타 버전
int	Version_CharInfo = 1;						//캐릭데이타 저장데이타 버전


smWINSOCK	*smWsockServer = 0;					//NPC-몬스터-아이템
smWINSOCK	*smWsockDataServer = 0;				//데이타 저장 서버
smWINSOCK	*smWsockUserServer = 0;				//유저 플레이 데이타 교신 서버
smWINSOCK	*smWsockExtendServer = 0;			//필드 확장 서버

//서버 재연결 횟수
int	ReconnDataServer = 0;
int	ReconnServer = 0;

char	szConnServerName[16];					//접속한 서버이름

DWORD	dwRecvUserServerTime = 0;
DWORD	dwRecvDataServerTime = 0;
DWORD	dwRecvServerTime = 0;
DWORD	dwRecvExtendServerTime = 0;

//주고 받기 동기화 변수
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

//서버와의 연결 끊어짐
int	DisconnectFlag = 0;
int	DisconnectServerCode = 0;

//해킹 경고
int	WarningHack = 0;

DWORD	dwQuestDelayTime = 0;		//퀘스트창 딜레이 타임

DWORD	dwLoadingTime = 0;			//로딩 시도 시간

char szServIP[32];
DWORD dwServPort;
char szDataServIP[32];
DWORD dwDataServPort;
char szUserServIP[32];
DWORD dwUserServPort;
char szExtendServIP[32];
DWORD dwExtendServPort;



DWORD	dwConnectedClientTime = 0;			//서버와 연결시 클라이언트 시간
DWORD	dwConnectedServerTime = 0;			//서버와 연결시 서버 시간
DWORD	dwLastRecvGameServerTime = 0;		//최근 서버로 부터 패킷을 입수한 시간
DWORD	dwLastRecvGameServerTime2 = 0;		//최근 서버로 부터 패킷을 입수한 시간
DWORD	dwLastRecvGameServerTime3 = 0;		//최근 서버로 부터 패킷을 입수한 시간
DWORD	dwLastRecvGameServerTime4 = 0;		//최근 서버로 부터 패킷을 입수한 시간

typedef DWORD(*LPFN_CheckMem)(TRANS_FUNC_MEMORY *TransFuncMem, smTRANS_COMMAND	*lpTransCommand);
LPFN_CheckMem	fnChkMem2;

//경험치와 돈 거래 금액을 토탈에 기록한다
static int	CompWareHouseMoney = 0;
int	WareHouseSubMoney = 0;					//창고에서 나온 누적 금액
int TotalSubMoney = 0;						//돈이 나간 금액
int TotalAddMoney = 0;						//돈이 들어온 금액
int	TotalAddExp = 0;						//들어온 경험치
int	TotalSubExp = 0;						//줄어든 경험치


smTRANS_COMMAND	*lpTransVirtualPotion = 0;


LPFN_CheckMem	fnChkMem;
DWORD	dwMemFunChkCode = 0;
//메모리 검사 모듈 받음
int	RecvMemFuncData(TRANS_FUNC_MEMORY *TransFuncMem);
int	RecvMemFuncData2(TRANS_FUNC_MEMORY *TransFuncMem);
//서버에서 메모리 값을 정해서 가져간다
DWORD funcCheckMemSum(DWORD FuncPoint, int count);
//모듈검사 하여 서버로 보내기
int CheckProcessModule();


//char *szServIP =	"211.50.44.170";
smPLAYDATA	PlayData;
char	TransBuff[smSOCKBUFF_SIZE];
int		TransLen;
//smCHAR_INFO	CharInfo;

//버릴 아이템 저장
TRANS_ITEMINFO	TransThrowItem;
//최근 받은 아이템
TRANS_ITEMINFO	TransRecvItem;

//아이템 교환키 재시도용
TRANS_TRADE_ITEMKEY	LastTransTradeItemKey;
DWORD				dwLastTransTradeKeyTime = 0;

int					InitClanMode = 0;			//클랜 초기화 모드

DWORD				dwYahooTime = 0;			//야호 유지시간

int					ServerHideMode = 0;			//서버 투명관리자 모드


//////////////// PK 필드 정보 //////////////////
PK_FIELD_STATE	PK_FieldState;		//Pk 필드 구조 정보체


//트레이드 아이템 확인 구조체
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


////////////////// 아이템 교환 관련 ///////////////
int		TradeItemSucessFlag = 0;
int		TradeRecvItemKeyFlag = 0;
int		TradeSendSucessFlag = 0;
DWORD	dwTradeMaskTime = 0;
int		TradeItemCancelCount = 0;

TRANS_TRADEITEMS	TransLastRecvTrade;
/*
//개인상점 정보
struct	TRANS_MYSHOP_ITEM {
	int		size,code;
	DWORD	dwSender;
	DWORD	dwRecver;
	DWORD	dwSum;
	DWORD	Temp[4];

	sMYSHOP	sMyShop;
};
*/
TRANS_TRADEITEMS	TransTradeItems_MyShop;			//개인상점 압축 구조

/////////////////// 창고 ///////////////////////////
DWORD	dwLastWareHouseChkSum = 0;

////////////// 에이징 아이템 서버송수신 /////////////
int		TransAgingItemFlag;

smTRANS_COMMAND	TransServerConnectInfo;

//다른 유저와 연결 시킨다
int ConnectOtherPlayer(DWORD dwIP);

int HoRecvMessage(DWORD dwCode, void *RecvBuff);

//아이템 교환 인증키 수신
int RecvTradeSucessKey(TRANS_TRADE_ITEMKEY *lpTransTradeItemKey, sTRADE *lpTrade);
//아이템 교환 인증키 전송
int SendTradeSucessKey(sTRADE *lpTrade, DWORD dwSender);
//아이템 교환 내용 확인 수신
int RecvTradeCheckItem(TRANS_TRADE_CHECKITEM *lpTransTradeCheckItem);
//크랙 윈도우 신고
int SendCrackWindow(HWND hWnd);

//임시 저장된 버릴 아이템을 확인하여 일치하면 서버로 보냄
int ThrowItemToServer(smTRANS_COMMAND_EX *lpTransCommand);
//아이템 확인 결과 도착
int RecvCheckItemFromServer(TRANS_ITEM_CODE *lpTransItemCode);
//클라이언트 펑션 위치와 값을 서버에 보고
int	SendClientFuncPos();

//프로세스 시간 감시하는 쓰레드 가동
int OpenTimeCheckThread();
//프로세스 시간 감시정보 서버로 송부
int SendPlayTimerMax();


//임시저장된 아이템 큐에서 찾기
TRANS_ITEMINFO	*FindRecvTransItemQue(DWORD dwCode, DWORD dwHead, DWORD dwChkSum);
//아이템 큐에 임시 저장
int	PushRecvTransItemQue(TRANS_ITEMINFO *lpTransItemInfo);

//스킬 실행 패킷 수신
int RecvProcessSkill(smTRANS_COMMAND *lpTransCommand);
//파티 스킬 패킷 수신
int RecvPartySkillFromServer(TRANS_PARTY_SKILL *lpTransPartySkill);

//퀘스트 아이템 입수 필터링
int FiltQuestItem(TRANS_ITEMINFO	*lpTransItemInfo, DWORD dwPacketCode);
//링크 코어 사용 성공
int SucessLinkCore(smTRANS_COMMAND_EX *lpTransCommandEx);
//SoD 진행 정보 받음
int RecvSodGameInfomation(void *Info);

//포스오브 아이템 입수
int RecvForceOrbItem(TRANS_ITEMINFO_GROUP2 *lpTransItemGroup2);

//엑스트랩 패킷수신
int XTrap_Recv(smTRANS_COMMAND *lpPacket, smWINSOCK *lpsmSock);

//이전 블레스 캐슬 정보
rsBLESS_CASTLE	rsBlessCastleOld;

//속성정보 송신 시간
DWORD	dwResistance_SendingTime = 0;


//공격한 몬스터 구별코드 기록
#define	ATT_MONSTER_MAX	128
#define	ATT_MONSTER_MASK	(ATT_MONSTER_MAX-1)

DWORD	dwAttMonsterCodeList[ATT_MONSTER_MAX];
int		AttMonsterCodeCount = 0;

//몬스터 코드 큐에 관리
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


///////////////////// 보내기 관련 ////////////////////

	//(smpacket.h / record.h / netplay.h)

	//smTRANSCODE_ID_GETUSERINFO: -> smTRANSCODE_ID_SETUSERINFO
	//ID로 사용자 캐릭터 리스트 및 정보를 구함

	//smTRANSCODE_INSRECORDDATA:
	//ID에 새로운 캐릭터 삽입

	//smTRANSCODE_DELRECORDDATA:
	//ID에 새로운 캐릭터 삭제

	//smTRANSCODE_ISRECORDDATA: -> smTRANS_CHAR_COMMAND2
	//캐릭터가 존재 하는지 확인

//유저 정보 관련 통신 코멘드
//int	TransUserCommand ( DWORD dwCode , char *szID , char *szName )

///////////////////////////////////////////////////////

///////////////////// 받기 관련 ////////////////////
	//smTRANSCODE_ID_SETUSERINFO:
	//ID로 사용자 캐릭터 리스트 및 정보를 구함 
	//(TRANS_USERCHAR_INFO 구조체로 결과 들어옴 )

	//smTRANSCODE_ISRECORDDATA:
	//캐릭터가 존재 하는지 확인 
	//( smTRANS_CHAR_COMMAND2 ->wParam 에 결과 들어옴 )
///////////////////////////////////////////////////////

//서버 접속


//smWINSOCK *ConnectServer_Main();

//서버 정보 확인
int CheckServerInfo(smTRANS_COMMAND *lpTransCommand)
{
	if ((-Client_Version * 2) != CLIENT_VERSION_CHECK)
	{
		//버전이 맞지 않음 입장 불가
		SetGameError(1);
		return TRUE;
	}

	if (Client_Version < lpTransCommand->WParam)
	{
		//버전이 맞지 않음 입장 불가
		SetGameError(1);
		return TRUE;
	}
	if (lpTransCommand->LParam>0)
	{
		//인원이 많아서 입장 불가
		SetGameError(2);
		return TRUE;
	}
	SetGameError(0);
	return TRUE;
}

//프로세스 정보 보냄
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


//넷 플레이 메인
smWINSOCK *ConnectServer_Main()
{
	/*
		//테스트 버전
		smConfig.szServerIP[0] = 0;
		smConfig.szDataServerIP[0] = 0;
		smConfig.szUserServerIP[0] = 0;
	*/

	if (!smWsockServer)
	{
		//게임 서버 연결
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
		//데이타 서버 연결
		if (smConfig.szDataServerIP[0] && lstrcmpi(smConfig.szDataServerIP, smConfig.szServerIP) != 0)
		{
			lstrcpy(szDataServIP, smConfig.szDataServerIP);
			dwDataServPort = smConfig.dwDataServerPort;
			smWsockDataServer = smConnectSock(szDataServIP, (WORD)dwDataServPort);
		}
		else
		{
			//데이타 서버와 게임서버를 같이 사용
			lstrcpy(szDataServIP, szServIP);
			dwDataServPort = dwServPort;
			smWsockDataServer = smWsockServer;
		}
	}

	if (smWsockServer && smWsockDataServer && !smWsockUserServer)
	{
		//유저 서버 연결
		if (smConfig.szUserServerIP[0] && lstrcmpi(smConfig.szUserServerIP, smConfig.szServerIP) != 0)
		{
			lstrcpy(szUserServIP, smConfig.szUserServerIP);
			dwUserServPort = smConfig.dwUserServerPort;
			smWsockUserServer = smConnectSock(szUserServIP, (WORD)dwUserServPort);
		}
		else
		{
			//유저 서버와 게임서버를 같이 사용
			lstrcpy(szUserServIP, szServIP);
			dwUserServPort = dwServPort;
			smWsockUserServer = smWsockServer;
		}
	}

	if (smWsockServer && smWsockDataServer && !smWsockExtendServer)
	{
		//확장 서버 연결
		if (smConfig.szExtendServerIP[0] && lstrcmpi(smConfig.szExtendServerIP, smConfig.szServerIP) != 0)
		{
			lstrcpy(szExtendServIP, smConfig.szExtendServerIP);
			dwExtendServPort = smConfig.dwExtendServerPort;
			smWsockExtendServer = smConnectSock(szExtendServIP, (WORD)dwExtendServPort);
		}
		else
		{
			//확장 서버와 게임서버를 같이 사용
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

//서버와의 연결 모두 끊음
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
//정보 서버 접속
smWINSOCK *ConnectServer_InfoMain();
//게임 서버 접속
smWINSOCK *ConnectServer_GameMain( char *szIP1, DWORD dwPort1, char *szIP2, DWORD dwPort2 );
//게임 서버 연결 끊음
int DisconnectServer_GameMain();

*/

//정보 서버 접속
smWINSOCK *ConnectServer_InfoMain()
{
	if (!smWsockDataServer)
	{
		//데이타 서버 연결
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

//게임 서버 접속
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
		//게임 서버 연결
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
			//서버 정보 확인
			CheckServerInfo(&TransServerConnectInfo);
		}

		return smWsockServer;
	}
	return NULL;
}

//게임 서버 접속
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
		//게임 서버 연결
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
			//서버 정보 확인
			CheckServerInfo(&TransServerConnectInfo);
		}

		return smWsockServer;
	}
	return NULL;
}


//게임 서버 연결 끊음
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

//서버 코드로 소켓을 찾음
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

//지역서버 소켓을 찾음
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


//떨어진 아이템 찾기
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

//빈 아이템 공간찾기
scITEM *FindEmptyScItem()
{
	int cnt;

	for (cnt = 0; cnt < DISP_ITEM_MAX; cnt++)
	{
		if (!scItems[cnt].Flag) return &scItems[cnt];
	}

	return NULL;
}

//교환 거래 거리 확인
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

//교환 거래 거리 확인
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

//새로만든 캐릭정보 확인
int	CheckStartCharInfo()
{
	//보안을 위해 한번더 확인
	smCHAR_INFO	smCharInfo;
	int cnt;

	memcpy(&smCharInfo, sinChar, sizeof(smCHAR_INFO));

	CheckCharForm();
	CodeXorExp = dwPlayTime + smCharInfo.Exp;						//경험치 숨김 코드
	CodeXorExp_High = dwPlayTime | (dwPlayTime*dwPlayTime);						//경험치 숨김 코드
	CodeXorLife = (dwPlayTime*smCharInfo.Life[0]) & 0xFFFF;		//생명력 숨김 코드
	cnt = smCharInfo.Dexterity + smCharInfo.Health + smCharInfo.Spirit + smCharInfo.Strength + smCharInfo.Talent;

	if (smCharInfo.Level >= 2 || cnt > 102 || smCharInfo.Exp || smCharInfo.ChangeJob || smCharInfo.Money)
	{
		//캐릭터 문제발생 조작의혹
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

//메모리 검사 모듈 받음2
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

//받은 경험치 돈데이타 수치를 서버에 보내서 확인
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

//서버에서 들어온 돈의 양 확인
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
							TransItemInfo.x = dwPlayServTime;											//해독 키
							TransItemInfo.y = TransItemInfo.Item.ItemHeader.dwChkSum;					//해독 키2
							TransItemInfo.z = rsPlayInfo[cnt].spMoney_Out^(TransItemInfo.x+TransItemInfo.y);	//토탈 기록 전송
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


//서버에서 들어온 경험치의 양 확인
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
		// 장별 - 경험치 에러 위해 (800000 - > 1600000 = 경험치 2배 이벤트)
		cnt = (lpServerExp->Total1^lpServerExp->Total2) + 1600000;		//오차율 적용
		if (cnt < lpServerExp->InputExp)
		{
			SendSetHackUser(83);
		}

	}

	return TRUE;
}



//다른 플레이어와의 연결 쓰레드를 생성
int CreateConnectThread(DWORD dwIP);
//다른 플레이어와의 연결종료 쓰레드 생성
int CreateDisconnectThread(smWINSOCK *lpsmSock);

//다른 다른 플레이어의 빈 영역을 찾음
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

//자동 플레이어를 찾는다
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

// 유저번호로 찾는다
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

//죽은파티유저를 찾는다
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


//최근접 몬스터를 찾는다
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



//빈 플레이어를 찾는다
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

//채팅창에 문자 삽입
int AddChatBuff(char *szMsg, DWORD dwIP)
{
	Log::Debug("AddChatBuff : Requisitado por�m foi removido.");
	return NULL;
}
int AddChatBuff(char *szMsg)
{
	Log::Debug("AddChatBuff : Requisitado por�m foi removido.");
	return NULL;
}
int	ChatBuffFilter(char *szMsg, DWORD dwIP)
{
	Log::Debug("ChatBuffFilter : Requisitado por�m foi removido.");
	return NULL;
}


//시스템 시간을 게임시간으로 변환
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


	//클래스 초기화
	int	Init(smWINSOCK *lpsmsock);
	//서버로 메세지 전송
	int SendChat(char *szMessage);
	//서버로 플레이어의 데이타를 위치를 전송
	int SendPlayData(smCHAR *lpChar);

	//서버로 부터 받은 메세지 처리
	int RecvMessage(smTHREADSOCK *SockInfo);

	//메세지 큐의 메세지를 처리
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


//메세지 큐의 메세지를 처리
int rsTRANS_SERVER::RecvMessageQue()
{
	int cnt;

	if (RecvDataQuePop >= RecvDataQuePush) return TRUE;
	if (MessageLoading) return FALSE;

	//크리티칼 섹션 선언
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

	//크리티칼 섹션 해제
	LeaveCriticalSection(&cSection);

	return TRUE;
}



DWORD	dwGetCharInfoTime = 0;

//서버로 부터 받은 메세지 처리
int rsTRANS_SERVER::RecvMessage(smTHREADSOCK *pData)
{
	Log::Debug("RecvMessage : Requisitado por�m foi removido.");
	return NULL;
}



int rsTRANS_SERVER::Init(smWINSOCK *lpsmsock)
{

	lpsmSock = lpsmsock;
	lpRecorder = 0;

	//크리티칼 섹션 초기화
	InitializeCriticalSection(&cServSection);

	RecvDataQuePush = 0;
	RecvDataQuePop = 0;

	ZeroMemory(RecvDataQue, RECV_DATA_QUE_COUNT * 4);

	return TRUE;
}


// 공백의 문자열을 잘라낸다
extern int szSpaceSorting(char *lpString);

//채팅 문자를 서버로 전송
int rsTRANS_SERVER::SendChat(char *szMessage)
{
	int len;

	if (!szMessage || !szMessage[0]) return FALSE;

	if (szMessage[0] == '/' && (szMessage[1] == ';' || szMessage[1] == ':'))
	{
		// 공백의 문자열을 잘라낸다
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
		//양서버에 동시에 메세지 보냄 '~'제거
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
			//동료 채팅

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
//서버로 플레이어의 데이타를 위치를 전송
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
			//위치가 변한게 없는 경우 ( 1번씩 걸러서 보냄 )
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




//서버 통신 클래스 선언
rsTRANS_SERVER	rsMainServer;







/*
//플레이어 정보를 상대방에 전송
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

//채팅 코맨드 저장
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

	//동일문자 확인
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

//채팅 저장 코맨드 복구 
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

// 문자열 비교 짧은 문장의 길이만큼 비교하여 비교 종료 포인터 반환
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
#include "CurseFilter.h"		//욕설필터

char *rsGetWord(char *q, char *p);		//서버에 선언된 값

extern int WaveCameraMode;

//채팅 문자를 서버로 전송
int SendChatMessageToServer(char *szChatMessage)
{
	Log::Debug("SendChatMessageToServer : Requisitado por�m foi removido.");
	return NULL;
}

//채팅 스크롤 포인트
int ChatScrollPoint = 0;

//채팅 정보 화면에 표시
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

//컷트할 라인 카운터
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
		//2바이트 코드 확인
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
			SetTextColor(hdc, RGB(255, 128, 96));				//서버메세지 (붉은글씨)
			break;
		case 1:
			//		SetTextColor( hdc, RGB(255, 190, 150) );
			SetTextColor(hdc, RGB(150, 190, 255));			//귓말
			break;
		case 2:
			//		SetTextColor( hdc, RGB(255, 140, 120) );
			SetTextColor(hdc, RGB(150, 190, 255));			//
			break;
		case 3:
			SetTextColor(hdc, RGB(230, 160, 255));			//공지
			break;
		case 4:
			SetTextColor(hdc, RGB(200, 255, 0));				//동료채팅
			break;
		case 5:
			SetTextColor(hdc, RGB(255, 255, 80));			//클랜채팅
			break;
		case 6:
			SetTextColor(hdc, RGB(200, 255, 255));			//거래채팅
			break;

		case 9:
			SetTextColor(hdc, RGB(200, 128, 128));			//보조 정보창
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
int	DispChatMsgHeight = 6;			//최근 표시된 채팅창 줄 높이
int	DispChatMode = 0;

//채팅창 필터링
int	SetChatMsgFilter(int mode)
{

	DisplayChatFilter = -1;
	ChatScrollPoint = 0;

	switch (mode)
	{
		case 0:			//전체
			DisplayChatFilter = -1;
			break;

		case 1:			//클랜
			DisplayChatFilter = 5;
			break;

		case 2:			//동료
			DisplayChatFilter = 4;
			break;

		case 3:			//거래
			DisplayChatFilter = 6;
			break;

		case 4:			//귓말
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
//채팅 정보 화면에 표시 ( 보조 )
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

//채팅 정보 화면에 표시
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

			//#ifdef _LANGUAGE_CHINESE //중국 채팅창 특정언어 사용금지
			//			lpString = ChatBuff2[fl_Count][chatCnt].szMessage;
			//			ConvertStringTHAI(lpString, lstrlen(lpString)+16);
			//#else
			lpString = ChatBuff2[fl_Count][chatCnt].szMessage;
			//#endif

						//if ( DisplayChatFilter<0 || DisplayChatFilter==ChatBuff[chatCnt].dwIP || ChatBuff[chatCnt].dwIP==0 || ChatBuff[chatCnt].dwIP==3 ) {


			DispChatMsgHeight++;			//높이 측정용

			len = lstrlen(lpString);
			if (len > StrMax)
			{
				//컷트할 라인 카운터
				CutPos = GetNextLineCount(lpString, StrMax);

				SetTextColor(hdc, shColor);
				dsTextLineOut(hdc, x - 1, y - 1, lpString + CutPos, len - CutPos);
				SetIpColor(hdc, ChatBuff2[fl_Count][chatCnt].dwIP);
				dsTextLineOut(hdc, x, y, lpString + CutPos, len - CutPos);

				y -= 12;
				LineCnt++;
				if (LineCnt >= LineMax) break;

#ifdef	_LANGUAGE_ENGLISH
				//영문 줄바꿈시 '-' 추가
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
				// 아르헨티나 줄바꿈 kyle
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
				//문자열 차례대로 검사
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
				//:있었으면 색깔을 바꾸자
				if (bsemi)
				{
					SIZE size;
					//:앞문자열
					prelen = strlen(presemi);
					SetTextColor(hdc, RGB(100, 255, 50));
					dsTextLineOut(hdc, x, y, presemi, prelen);
					GetTextExtentPoint(hdc, presemi, prelen, &size);
					//뒤문자열
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
				//문자열 차례대로 검사
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
				//:있었으면 색깔을 바꾸자
				if (bsemi)
				{
					SIZE size;
					//:앞문자열
					prelen = strlen(presemi);
					SetTextColor(hdc, RGB(100, 255, 50));
					dsTextLineOut(hdc, x, y, presemi, prelen);
					GetTextExtentPoint(hdc, presemi, prelen, &size);
					//뒤문자열
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


//새 플레이어와 연결
int ConnectPlayer(smWINSOCK *lpsmSock)
{
	int OthPlayerCnt;

	if (ServerMode)
		return Serv_ConnectPlayer(lpsmSock);


	//크리티칼 섹션 선언
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

	//크리티칼 섹션 해제
	LeaveCriticalSection(&cSection);

	return TRUE;
}

//플레이어 연결 끈어짐
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
		AddChatBuff("서버와의 연결이 끊어 졌습니다");
		if (smWsockServer == smWsockDataServer)
			smWsockDataServer = 0;
		if (smWsockServer == smWsockUserServer)
			smWsockUserServer = 0;
		if (smWsockServer == smWsockExtendServer)
			smWsockExtendServer = 0;

		smWsockServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 1;
		//거래중이면 거래 취소 시킴
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);			//거래 취소 요구
			cTrade.CancelTradeItem();								//거래창 닫기
		}
		return TRUE;
	}

	if (smWsockDataServer && lpsmSock->sock == smWsockDataServer->sock)
	{
		AddChatBuff("데이타 서버와의 연결이 끊어 졌습니다");
		smWsockDataServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 2;

		//거래중이면 거래 취소 시킴
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);			//거래 취소 요구
			cTrade.CancelTradeItem();								//거래창 닫기
		}
		return TRUE;
	}

	if (smWsockUserServer && lpsmSock->sock == smWsockUserServer->sock)
	{
		AddChatBuff("서버와의 연결이 끊어 졌습니다");
		smWsockUserServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 3;
		//거래중이면 거래 취소 시킴
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);			//거래 취소 요구
			cTrade.CancelTradeItem();					//거래창 닫기
		}
		return TRUE;
	}

	if (smWsockExtendServer && lpsmSock->sock == smWsockExtendServer->sock)
	{
		AddChatBuff("서버와의 연결이 끊어 졌습니다");
		smWsockExtendServer = 0;
		DisconnectFlag = dwTime;
		//DisconnectServerCode = 3;
		//거래중이면 거래 취소 시킴
		if (cTrade.OpenFlag)
		{
			SendRequestTrade(cTrade.TradeCharCode, 3);		//거래 취소 요구
			cTrade.CancelTradeItem();							//거래창 닫기
		}
		return TRUE;

	}

	char szBuff[256];

	//지연 서버 연결 끈어짐
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



	//크리티칼 섹션 선언
	EnterCriticalSection(&cSection);

	lpPlayer = (smCHAR *)lpsmSock->ExtData1;
	if (lpPlayer)
	{
		lpPlayer->Close();
	}

	//크리티칼 섹션 해제
	LeaveCriticalSection(&cSection);

	return TRUE;
}



//마지막으로 서버에 데이타를 보낸 카운터
int Ts_LastSendCounter;
DWORD	dwLastSendPosiTime = 0;		//마지막 전송 시간 (위치)
DWORD	dwLastSendPlayTime = 0;		//마지막 전송 시간 (모션데이타)

//서버로 플레이어의 데이타를 위치를 전송
int SendPlayDataToServer()
{
	Log::Debug("SendPlayDataToServer : Requisitado por�m foi removido.");
	return NULL;
}

//다른 유저와 연결 시킨다
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
//다른 플레이어와의 연결 쓰레드
DWORD WINAPI ConnectPlayThreadProc(void *pInfo)
{

	ConnectOtherPlayer((DWORD)pInfo);

	ExitThread(TRUE);
	return TRUE;
}
//다른 플레이어와의 연결 종료 쓰레드
DWORD WINAPI DisconnectPlayThreadProc(void *pInfo)
{

	((smWINSOCK *)pInfo)->CloseSocket();

	ExitThread(TRUE);
	return TRUE;
}



//다른 플레이어와의 연결 쓰레드를 생성
int CreateConnectThread(DWORD dwIP)
{
	DWORD dwMsgId;
	CreateThread(NULL, 0, ConnectPlayThreadProc, (void *)dwIP, 0, &dwMsgId);

	return TRUE;
}

//다른 플레이어와의 연결종료 쓰레드 생성
int CreateDisconnectThread(smWINSOCK *lpsmSock)
{
	DWORD dwMsgId;
	CreateThread(NULL, 0, DisconnectPlayThreadProc, (void *)lpsmSock, 0, &dwMsgId);

	return TRUE;
}


//거리가 먼 다른 유저들의 연결을 끈는다
int DisconnectFarPlayer()
{
	int mx, mz, dDist;
	int cnt;
	int timeover;

	for (cnt = 0; cnt < OTHER_PLAYER_MAX; cnt++)
	{
		//거리를 확인하여 먼 경우에는 연결을 끈어버린다
		if (chrOtherPlayer[cnt].Flag)
		{

			if ((chrOtherPlayer[cnt].dwLastTransTime + DIS_TIME_OVER) < dwPlayTime)
				timeover = TRUE;
			else
				timeover = FALSE;

			//거리계산
			mx = (lpCurPlayer->pX - chrOtherPlayer[cnt].pX) >> FLOATNS;
			mz = (lpCurPlayer->pZ - chrOtherPlayer[cnt].pZ) >> FLOATNS;
			dDist = mx*mx + mz*mz;

			if ((chrOtherPlayer[cnt].DisplayFlag && dDist > DIST_TRANSLEVEL_DISCONNECT) || timeover)
			{
				//연결 종료/ 소켓 닫음
				chrOtherPlayer[cnt].Flag = 0;

				if (chrOtherPlayer[cnt].TransSock)
				{
					chrOtherPlayer[cnt].TransSock->CloseSocket();
				}
				else
				{
					//크리티칼 섹션 선언
					EnterCriticalSection(&cSection);
					chrOtherPlayer[cnt].Close();
					//크리티칼 섹션 해제
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

//데이타 입수
int RecvPlayData(smTHREADSOCK *pData)
{

	DWORD *IPData;
	//smCHAR *lpPlayer;

	DWORD	dwTime;

	//크리티칼 섹션 선언
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

	//서버측 데이타 처리
	rsMainServer.RecvMessage(pData);
	/*
		}
		else {
			//클라이언트와의 교신..

			lpPlayer = (smCHAR *)pData->smMySock->ExtData1;
			if ( lpPlayer ) lpPlayer->RecvPlayData( pData );
		}
	*/
	//크리티칼 섹션 해제
	LeaveCriticalSection(&cSection);

	return TRUE;
}

//메세지 큐의 메세지를 처리
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

//넷 플레이 메인
int NetWorkPlay()
{
	//	char *szIP;
	int cnt;
	DWORD	dwTime;
	DWORD	dwTime2;
	smWINSOCK	*lpsmSock;

	//박재원 - XignCode
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

		//Sleep( 60*1000*3 );		//3분 대기

		NetWorkInitFlag = TRUE;

		rsMainServer.Init(smWsockServer);

		//서버로 플레이어의 데이타를 위치를 전송
		SendPlayDataToServer();

		//서버에 권한 설정
		if (smConfig.DebugMode) SendAdminMode(TRUE);


		//		if ( smWsockServer )
		//			lpCurPlayer->SendCharInfo( smWsockServer );			//플레이어 정보 전송

				//저장된 캐릭터 데이타 불러오기 요구
		if (smWsockDataServer)
		{

			SendGetRecordData(lpCurPlayer->smCharInfo.szName);

			//서버 IP 첵크 ( 군서버에 접속한 IP통보 )
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
		TransThrowItem.code = 0;			//아이템 버리기 초기화

		RecordFailCount = 0;			//저장 실패 카운터
		dwLastWareHouseChkSum = 0;		//창고 체크섬 코드

		TransAgingItemFlag = 0;			//에이징 완료 아이템 전송 플랙


		ZeroMemory(&TransRecvItem, sizeof(TRANS_ITEMINFO));

		Trans_SplashCharList.code = 0;	//스플레시 공격 버퍼 초기화

		ZeroMemory(&PK_FieldState, sizeof(PK_FIELD_STATE));	//PK 구조 초기화
		PK_FieldState.FieldCode = -1;


		//몬스터 코드 큐에 관리
		ClearAttMonsterCode();

		Init_RecordDamage();			//공격력 기록 초기화

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
			//아이템 교환시 완료 확인
			if (!TradeSendSucessFlag && sTrade.CheckFlag && sTradeRecv.CheckFlag)
			{
				SendTradeCheckItem(cTrade.TradeCharCode);
				TradeSendSucessFlag = TRUE;
			}
		}
		if (dwTradeMaskTime)
		{
			//트레이드 마스크 타이머 초기화
			if (dwTradeMaskTime<dwPlayTime) dwTradeMaskTime = 0;
		}

		dwTime = dwPlayTime - dwLastSendPosiTime;
		dwTime2 = dwPlayTime - dwRecvServerTime;

		//서버로의 데이타 전송
		if ((PlayCounter - Ts_LastSendCounter)>PLAYSERVER_SENDCOUNTER)
		{//&& dwTime>NET_SENDING_WAIT_TIME &&
//			dwTime2<NET_SENDING_WAIT_TIME ) {
			//서버로 플레이어의 데이타를 위치를 전송
			SendPlayDataToServer();
			//거리가 먼 다른 유저들의 연결을 끈는다
			DisconnectFarPlayer();

			//카운터 증가
			Ts_LastSendCounter = PlayCounter;
			dwLastSendPosiTime = dwPlayTime;
		}

		//		if ( (xxcnt&0x3)==3 ) SendPlayData( lpCurPlayer );

				//다른 유저들과의 데이타 교환
				//SendPlayData( lpCurPlayer );

		if (smWsockUserServer)
		{// && rsRecvUserServer ) {
//플레이어 데이타를 서버로 전송 (40/70) 초단위 구분 
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
							//서있는 동작 네트웍 부하 최소 ( 한번 전송후 네트웤 슬립 )
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

		//현재 게임 로딩 중
		if (dwLoadingTime)
		{
			dwTime = GetCurrentTime();
			if (dwTime > (dwLoadingTime + NET_LOADING_WAIT_LIMIT))
			{
				//시간 초과 종료
				DisconnectFlag = GetCurrentTime();
				dwLoadingTime = 0;
			}
		}


		cnt = PlayCounter & 0x1FF;
		if (cnt == 0)
		{
			//데드락 첵크 ( 메세지가 돌아 오지 않으면 네트웍 다운 )
			TransCheckNetwork();
		}
		else if (cnt == 0x80)
		{
			//프로세스 시간 감시정보 서버로 송부
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
#ifndef _LANGUAGE_CHINESE	//윈도우 모드
#ifndef _LANGUAGE_VEITNAM	//윈도우 모드
#ifndef _LANGUAGE_PHILIPIN	//윈도우 모드
#ifndef _LANGUAGE_ENGLISH	//윈도우 모드
	HWND	hWnd;

	if ((PlayCounter & 0x1FF) == 0 && !smConfig.DebugMode)
	{
		hWnd = WinFocusCrack();
		//윈도우 포커스를 감시
		if (hWnd)
		{
			//크랙 윈도우 신고
			SendCrackWindow(hWnd);
		}
	}
#endif
#endif
#endif
#endif


	//거래후 확인삼아 재시도 ( 0.8초후 )
	if (dwLastTransTradeKeyTime && (dwLastTransTradeKeyTime + 800) < dwPlayTime)
	{

		if (smWsockDataServer)
			smWsockDataServer->Send2((char *)&LastTransTradeItemKey, LastTransTradeItemKey.size, TRUE);

		dwLastTransTradeKeyTime = 0;
	}


	if (ChatDispCnt > 0) ChatDispCnt--;

	return TRUE;
}


//아이템 버리기
int ThrowPutItem(sITEM *lpItem, int x, int y, int z)
{

	//서버 상태에 문제 발생시 아이템 버리기 금지 ( 4초 이상 수신이 없는 상태 )
	if ((dwRecvServerTime + 4000) < dwPlayTime || QuitSave || TransThrowItem.code)
		return FALSE;

	if (AdminCharDisable || WarningHack || !smWsockDataServer || !smWsockServer) return FALSE;

	//데이타 서버에 아이템 저장 요구 ( 복사 아이템 방지 )
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
//아이템 버리기
int ThrowPutItem2(sITEMINFO *lpItem, int x, int y, int z)
{

	//서버 상태에 문제 발생시 아이템 버리기 금지 ( 4초 이상 수신이 없는 상태 )
	if ((dwRecvServerTime + 4000) < dwPlayTime || QuitSave || TransThrowItem.code)
		return FALSE;

	if (AdminCharDisable || WarningHack || !smWsockDataServer || !smWsockServer) return FALSE;

	//데이타 서버에 아이템 저장 요구 ( 복사 아이템 방지 )
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


//버릴 아이템 저장
//TRANS_ITEMINFO	TransThrowItem;
//			ThrowItemToServer((TRANS_ITEMINFO *)pData->Buff );

int	ThrowPotionCount = 0;

//임시 저장된 버릴 아이템을 확인하여 일치하면 서버로 보냄
int ThrowItemToServer(smTRANS_COMMAND_EX *lpTransCommand)
{
	Log::Debug("ThrowItemToServer : Requisitado por�m foi removido.");
	return NULL;
}

//몬스터 시작 지점 추가
int SendAddStartPoint(int x, int z)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_ADD_STARTPOSI;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.LParam = x;
	TransCommand.WParam = z;
	TransCommand.SParam = TRUE;

	smWINSOCK *lpsmSock;
	lpsmSock = GetAreaServerSock();		//지역서버 소켓을 찾음
	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return NULL;
}

//몬스터 시작지점 제거
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

	//떨어진 아이템 찾기
	lpItem = FindScItem(x, z);
	if (lpItem)
	{
		lpsmSock = GetServerSock(lpItem->ServerCode);		//지역서버 소켓을 찾음
		lpItem->Close();
	}

	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return NULL;
}



//NPC 캐릭터 추가
int SendAdd_Npc(smCHAR *lpCharSample, int state)
{
	smTRNAS_PLAYERINFO	smTransPlayerInfo;

	smTransPlayerInfo.code = smTRANSCODE_ADD_NPC;
	smTransPlayerInfo.size = sizeof(smTRNAS_PLAYERINFO);
	memcpy(&smTransPlayerInfo.smCharInfo, &lpCharSample->smCharInfo, sizeof(smCHAR_INFO));
	smTransPlayerInfo.smCharInfo.State = 0;

	//NPC 정보파일 을 모델이름2에 넣어서 보냄
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
	lpsmSock = GetAreaServerSock();		//지역서버 소켓을 찾음
	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&smTransPlayerInfo, smTransPlayerInfo.size, TRUE);
	}

	return NULL;
}

//NPC 캐릭터 제거
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

	lpsmSock = GetServerSock(lpCharSample->ServerCode);		//지역서버 소켓을 찾음
	lpCharSample->Close();

	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&smTransPlayerInfo, smTransPlayerInfo.size, TRUE);
	}
	return NULL;
}

//저장된 캐릭터 데이타 불러오기 요구
int SendGetRecordData(char *szName)
{
	smTRANS_CHAR_COMMAND	smTransCharCommand;

	if (smWsockDataServer)
	{
		smTransCharCommand.code = smTRANSCODE_GETRECORDDATA;
		smTransCharCommand.size = sizeof(smTRANS_CHAR_COMMAND);

		smTransCharCommand.WParam = dwExeCheckSum;					//실행파일 체크섬
		smTransCharCommand.LParam = FALSE;
		smTransCharCommand.SParam = Client_Version;					//클라이언트 버전

		lstrcpy(smTransCharCommand.szName, szName);

		dwLoadingTime = GetCurrentTime();			//로딩 시도 시간 기록

		return smWsockDataServer->Send((char *)&smTransCharCommand, smTransCharCommand.size, TRUE);
	}

	return FALSE;
}

char *szCmdOpenMonster[10] = {
	"홉고블린",
	"홉고블린",
	"홉고블린",
	"킹호피",
	"타이탄",
	0,0,0,0,0
};


//몬스터 생성
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
	lpsmSock = GetAreaServerSock();		//지역서버 소켓을 찾음
	if (lpsmSock)
	{
		return lpsmSock->Send((char *)&smTransCharCommand, smTransCharCommand.size, TRUE);
	}

	return FALSE;

}

//각 코드 데이타를 반전
int	CodeXorCharInfo()
{
	lpCurPlayer->smCharInfo.Exp ^= CodeXorExp;
	lpCurPlayer->smCharInfo.Exp_High ^= CodeXorExp_High;
	lpCurPlayer->smCharInfo.Life[0] ^= CodeXorLife;
	return TRUE;
}

//스킬값 전부 확인
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

//스킬보호값 전부 초기화
int ReformSkillInfo()
{
	for (int j = 1; j < SIN_MAX_USE_SKILL; j++)
	{
		cSkill.ReformSkillMasteryForm(&sinSkill.UseSkill[j]);
		cSkill.ReformSkillPointForm(&sinSkill.UseSkill[j]);
	}
	return TRUE;
}

//게임을 저장
int	SaveGameData()
{
	if (smWsockDataServer && !rsRecorder.dwRecordTime &&
		lpCurPlayer->smCharInfo.JOB_CODE && !AdminCharDisable && !cTrade.OpenFlag && !cWareHouse.OpenFlag &&
		!QuitSave)
	{

		//물약갯수 비교
		//if ( ComparePotion()>0 ) return FALSE;		//물약 오류

		if (!WarningHack && CheckCharForm() != FALSE && SaveCheckSkill() == TRUE)
		{

			CodeXorCharInfo();
			ReformCharForm();

			rsRecorder.MakeRecordData(&lpCurPlayer->smCharInfo, cInvenTory.InvenItem, cInvenTory.InvenItemTemp, &MouseItem);
			rsRecorder.SendRecordDataToServer(smWsockDataServer);
			rsRecorder.dwRecordTime = dwPlayTime;					//게임 저장 시간 확인

			if (smConfig.DebugMode) AddChatBuff("GameSave");

			CodeXorCharInfo();
			ReformCharForm();
			CheckPlaySkill();		//스킬감시
			return TRUE;
		}
	}
	return FALSE;
}

//게임 저장 상태 ( FALSE 저장중 ( 서버로부터 결과를 받지 못함 ) )
int GetSaveResult()
{
	if (rsRecorder.dwRecordTime) return FALSE;

	return TRUE;
}



///////////////////// 보내기 관련 ////////////////////
	//smTRANSCODE_ID_GETUSERINFO:
	//ID로 사용자 캐릭터 리스트 및 정보를 구함

	//smTRANSCODE_INSRECORDDATA:
	//ID에 새로운 캐릭터 삽입

	//smTRANSCODE_DELRECORDDATA:
	//ID에 새로운 캐릭터 삭제

	//smTRANSCODE_ISRECORDDATA:
	//캐릭터가 존재 하는지 확인

//유저 정보 관련 통신 코멘드
//int	TransUserCommand ( DWORD dwCode , char *szID , char *szName )

///////////////////////////////////////////////////////

///////////////////// 받기 관련 ////////////////////
	//smTRANSCODE_ID_SETUSERINFO:
	//ID로 사용자 캐릭터 리스트 및 정보를 구함 
	//(TRANS_USERCHAR_INFO 구조체로 결과 들어옴 )

	//smTRANSCODE_ISRECORDDATA:
	//캐릭터가 존재 하는지 확인 
	//( smTRANS_CHAR_COMMAND2 ->wParam 에 결과 들어옴 )
///////////////////////////////////////////////////////

smTRANS_COMMAND	smMessageCommandBuff;

//유저 정보 관련 통신 코멘드 ( 데이타 서버용 )
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
				smMessageCommandBuff.WParam = -19;			//특수문자 오류
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


//캐릭터 정보 전송 유구
int	Send_GetCharInfo(DWORD	dwObjectSerial, int ServerCode)
{
	smTRANS_COMMAND			TransCommand;
	smWINSOCK	*lpsmSock;

	TransCommand.code = smTRANSCODE_GETPLAYERINFO;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectSerial;

	lpsmSock = GetServerSock(ServerCode);		//지역서버 소켓을 찾음

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

//데드락 첵크 ( 메세지가 돌아 오지 않으면 네트웍 다운 )
int	TransCheckNetwork()
{
	smTRANS_COMMAND			TransCommand;
	DWORD	dwTime, dwTime2;
	smWINSOCK	*lpOldSock;

	//return TRUE;

	if (!dwConnectedServerTime) return FALSE;

	//프로세스 시간 감시하는 쓰레드 가동

	OpenTimeCheckThread();

	//확인 4초 이후에나 확인 메세지 보냄
	dwTime = GetCurrentTime();
	dwTime2 = dwTime;

	if ((dwTime - dwLastTransCheckTime) < 4000) return FALSE;

	dwLastTransCheckTime = dwTime;

	//if ( !smConfig.DebugMode ) {
	if ((dwMainServ_RecvTime && (dwMainServ_RecvTime + RECONNECT_LIMIT_TIME) < dwTime) ||
		(dwLastRecvGameServerTime + DEADLOCK_CHECK_LIMIT_TIME) < dwTime ||
		!smWsockServer || smWsockServer->WaitReadMessage < 0)
	{
		//연결 끈어 짐 ( 서버와의 교신 불능 )

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
			//서버에 재접속 시도
			SendContinueServer(lpCurPlayer->dwObjectSerial, smWsockServer);

			char szBuff[256];
			wsprintf(szBuff, "Reconnect Server(Main)");
			AddChatBuff(szBuff);

			if (dwLastRecvGameServerTime && dwLastRecvGameServerTime < dwTime2) dwLastRecvGameServerTime = dwTime2;
			if (dwLastRecvGameServerTime2 && dwLastRecvGameServerTime2 < dwTime2) dwLastRecvGameServerTime2 = dwTime2;
			if (dwLastRecvGameServerTime3 && dwLastRecvGameServerTime3 < dwTime2) dwLastRecvGameServerTime3 = dwTime2;
			if (dwLastRecvGameServerTime4 && dwLastRecvGameServerTime4 < dwTime2) dwLastRecvGameServerTime4 = dwTime2;
			ReconnServer++;		//재연결 카운터

			return TRUE;
		}

		DisconnectServerCode = 1;
		DisconnectFlag = GetCurrentTime();
		return FALSE;
	}
	//}


	//예상 서버 시간
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
				//서버에 재접속 시도
				//Sleep(200);
				SendContinueServer(lpCurPlayer->dwObjectSerial, smWsockUserServer);

				char szBuff[256];
				wsprintf(szBuff, "Reconnect Server(User)");
				AddChatBuff(szBuff);
				if (dwLastRecvGameServerTime && dwLastRecvGameServerTime < dwTime2) dwLastRecvGameServerTime = dwTime2;
				if (dwLastRecvGameServerTime2 && dwLastRecvGameServerTime2 < dwTime2) dwLastRecvGameServerTime2 = dwTime2;
				if (dwLastRecvGameServerTime3 && dwLastRecvGameServerTime3 < dwTime2) dwLastRecvGameServerTime3 = dwTime2;
				if (dwLastRecvGameServerTime4 && dwLastRecvGameServerTime4 < dwTime2) dwLastRecvGameServerTime4 = dwTime2;

				ReconnDataServer++;		//서버 재연결 카운터

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

				//ReconnDataServer++;		//서버 재연결 카운터

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
		//지역서버 데드락 첵크
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


	//받은 경험치 돈데이타 수치를 서버에 보내서 확인
	SendTotalExpMoney();

	CheckMaxDamage();			//최대 데미지 검사


	return TRUE;
}

//서버에 관리자 모드 설정
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

//서버에 관리자 모드 설정
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


//서버에 네트워크 풀질 모드 설정
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


//IP접속 주소를 군 서버에 보낸다
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



//서버에 오브젝트 번호 설정
int SendSetObjectSerial(DWORD dwObjectSerial)
{

	//새버전 
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

//서버에 오브젝트 번호 설정 ( 처음 한번만 )
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


//서버에 재접속 시도
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

//해킹 시도한 유저 자동 신고
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


//크랙 첵크
int CheckCracker()
{
	char *szName;
	char TransBuff[1024];
	int len;

	szName = FindCracker();

	if (szName)
	{
		//크랙 발견
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

//크랙 첵크 ( 메세지 통보도 같이 사용 )
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
		//크랙 발견
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
				//내용 있음 , 공지 출력
				GetNoticeUrl(lpBuff);
			}
		}
	}

	return FALSE;
}

//크랙 윈도우 신고
int SendCrackWindow(HWND hWnd)
{
	char *szName;
	char TransBuff[1024];
	int len;

	//크랙 발견
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



//해킹 시도한 유저 자동 신고
int SendSetHackUser(int StopGame)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_SET_BLACKLIST;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = StopGame;
	TransCommand.LParam = 0;
	TransCommand.SParam = 0;

	WarningHack = StopGame;		//해킹 경고

	if (smWsockDataServer)
		smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);

	if (WarningHack)
	{
		DisconnectServerCode = 4;
		DisconnectFlag = GetCurrentTime();
	}

	return TRUE;
}

//해킹 시도한 유저 자동 신고
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


//서버에 관리자 모드 설정
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



//복사 아이템을 소지한 사람 신고 
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


//데이타 서버에 아이템 저장 요구 ( 복사 아이템 방지 )
int SendSaveThrowItem(sITEMINFO *lpItemInfo)
{
	smTRANS_COMMAND	TransCommand;

	//물약인 경우만 저장하도록 수정
	if ((lpItemInfo->CODE&sinITEM_MASK1) != (sinPM1&sinITEM_MASK1))	return FALSE;


	TransCommand.code = smTRANSCODE_SAVETHROWITEM;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = lpItemInfo->CODE;
	TransCommand.LParam = lpItemInfo->ItemHeader.Head;
	TransCommand.SParam = lpItemInfo->ItemHeader.dwChkSum;

	if ((lpItemInfo->CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		ComparePotion();	//물약갯수 비교

	return smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}
/*
//데이타 서버에 아이템 저장 요구 최대 32개 동시 ( 복사 아이템 방지 )
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

//아이템 거래내역 서버에 보내기
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


//데이타 서버에 아이템 저장 요구 ( 복사 아이템 방지 )
int SendSaveThrowItem2(sITEMINFO *lpItemInfo)
{
	smTRANS_COMMAND_EX	TransCommand;


	TransCommand.code = smTRANSCODE_SAVETHROWITEM2;
	TransCommand.size = sizeof(smTRANS_COMMAND_EX);
	TransCommand.WParam = lpItemInfo->CODE;
	TransCommand.LParam = lpItemInfo->ItemHeader.Head;
	TransCommand.SParam = lpItemInfo->ItemHeader.dwChkSum;
	TransCommand.EParam = lpItemInfo->PotionCount^TransCommand.SParam^TransCommand.code;	//물약 갯수

	TransCommand.WxParam = lpCurPlayer->smCharInfo.Money;
	TransCommand.LxParam = EncodeParam(TransCommand.WxParam);
	TransCommand.SxParam = GetTotalMoney() ^ TransCommand.LxParam;
	TransCommand.ExParam = lpItemInfo->Money;

	if ((lpItemInfo->CODE&sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
		ComparePotion();	//물약갯수 비교

	//이벤토리 돈 검사 설정
	AddInvenMoney(-lpItemInfo->Money);

	return smWsockDataServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}

//데이타 서버에 돈 저장 요구 ( 복사 아이템 방지 )
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

//다른 캐릭터 카피 전송 요구
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



//서버에 변경된 캐릭터 정보 보냄
int SendPlayUpdateInfo()
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_UPDATE_CINFO;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = 0;
	TransCommand.LParam = lpCurPlayer->smCharInfo.Level;			//레벨 정보
	TransCommand.SParam = 0;
	TransCommand.EParam = 0;

	return smWsockServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
}

//서버에 변경된 클랜 정보 보냄
int SendUpdateClanInfo()
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_UPDATE_CINFO;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = 1;
	TransCommand.LParam = lpCurPlayer->smCharInfo.Level;			//레벨 정보
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

	DWORD	dwDestObjectSerial;	//공격하는 캐릭터 고유번호

	int x,y,z;			// 좌표
	int AttackState;	// 속성
	int AttackSize;		// 공격 범위
	int Power;			// 공격 파워

	int	TargetCount;
	DWORD	dwTarObjectSerial[SKIL_ATTACK_CHAR_MAX];	//공격대상 캐릭터 고유번호
};
*/


TRANS_SKIL_ATTACKDATA	Back_TransSkilAttackData;

//범위형 공격
int Skil_RangeAttack(int x, int y, int z, int range, int power, int TargetState)
{

	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	int cnt, mCnt;
	//int	accuracy;
	int	dRange;
	int dx, dy, dz, dist;
	smCHAR	*lpChar;
	smWINSOCK *lpsmSendSock = 0;


	//클라이언트 로그 파일 기록
	Record_ClinetLogFile("구버전 공격패킷 - Skil_RangeAttack ");
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

				//처음 공격하는 캐릭한테만 체력바 출력
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
		(TransSkilAttackData.x * TransSkilAttackData.y * TransSkilAttackData.z);// 조작 방지 코드

	LastAttackDamage = power;

	memcpy(&Back_TransSkilAttackData, &TransSkilAttackData, sizeof(TRANS_SKIL_ATTACKDATA));		//백업

	lpsmSendSock = GetAreaServerSock();		//보낼 서버구하기

	if (lpsmSendSock && mCnt > 0)
	{
		//보낸 공격력 기록
		Record_SendDamage(lpsmSendSock, TransSkilAttackData.Power);

		lpsmSendSock->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}

	if (PkMode)
	{
		//범위형 공격( 유저만 공격 )
		Skil_RangeAttack_User(x, y, z, range, power, TargetState);
	}

	return TRUE;
}


//범위형 공격( 유저만 공격 )
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
		(TransSkilAttackData.x * TransSkilAttackData.y * TransSkilAttackData.z);// 조작 방지 코드


	if (mCnt > 0)
	{
		//보낸 공격력 기록
		Record_SendDamage(smWsockUserServer, TransSkilAttackData.Power);

		smWsockUserServer->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}


	//	lpsmSendSock = GetAreaServerSock();		//보낼 서버구하기

	//	if ( lpsmSendSock ) {
	//		lpsmSendSock->Send2( (char *)&TransSkilAttackData , TransSkilAttackData.size , TRUE );
	//	}
	return TRUE;
}

//범위형 공격
int Skil_RangeBoxAttack(smCHAR *lpCurChar, RECT *lpRect, int power, int TargetState, int UseAttackRating)
{

	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	int cnt, mCnt;
	int dx, dy, dz;
	int Attack, accuracy;
	smCHAR	*lpChar;
	smWINSOCK *lpsmSendSock = 0;


	//클라이언트 로그 파일 기록
	Record_ClinetLogFile("구버전 공격패킷 - Skil_RangeBoxAttack ");
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

			// 이동된 지역 좌표값을 구하는 것이구나! 놀랍다.
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

					//처음 공격하는 캐릭한테만 체력바 출력
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
		(TransSkilAttackData.x * TransSkilAttackData.y * TransSkilAttackData.z);// 조작 방지 코드


	memcpy(&Back_TransSkilAttackData, &TransSkilAttackData, sizeof(TRANS_SKIL_ATTACKDATA));		//백업

	lpsmSendSock = GetAreaServerSock();		//보낼 서버구하기

	if (lpsmSendSock && mCnt > 0)
	{
		//보낸 공격력 기록
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


//범위형 공격 이펙트 연출
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

			// 이동된 지역 좌표값을 구하는 것이구나! 놀랍다.
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

//범위형 공격 이펙트 연출
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




//코드 암호 첵크섬 만들기
DWORD EncodeParam(DWORD Param)
{
	return (Param + (Param << 4)) * 2002;
}



//상대 캐릭터에 공격 가함 
int QuickSendTransAttack(smCHAR *lpChar, int power, int AttackState, int Resistance)
{
	TRANS_ATTACKDATA	TransAttackData;
	smWINSOCK *lpsmSendSock = 0;
	int pow;

	//클라이언트 로그 파일 기록
	Record_ClinetLogFile("구버전 공격패킷 - QuickSendTransAttack ");
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
			TransAttackData.Power += pow;			//속성 공격력 추가 ( 서버에서 추가된 공격력 따로 계산 )
		}
	}

	TransAttackData.dwDestObjectSerial = lpCurPlayer->dwObjectSerial;
	TransAttackData.dwTarObjectSerial = lpChar->dwObjectSerial;
	TransAttackData.dwChkSum = TransAttackData.Power * 2002 +
		(TransAttackData.x * TransAttackData.y * TransAttackData.z);// 조작 방지 코드

	LastAttackDamage = TransAttackData.Power;

	//처음 공격하는 캐릭한테만 체력바 출력
	if (lpChar->smCharInfo.Life[1] && lpChar->smCharInfo.Life[0] == lpChar->smCharInfo.Life[1])
	{
		lpChar->EnableStateBar = TRUE;
	}

	if (lpChar->ServerCode)
	{
		lpsmSendSock = GetServerSock(lpChar->ServerCode);		//서버 코드 구하기
	}


	if (lpsmSendSock)
	{
		//보낸 공격력 기록
		Record_SendDamage(lpsmSendSock, TransAttackData.Power);

		return lpsmSendSock->Send2((char *)&TransAttackData, TransAttackData.size, TRUE);
	}

	return NULL;
}

//상대 캐릭터에 공격 가함 
int QuickSendTransAttack(smCHAR *lpChar, int power)
{
	return QuickSendTransAttack(lpChar, power, 1, 0);
}
//상대 캐릭터에 공격 가함 
int QuickSendTransAttack(smCHAR *lpChar, int power, int Resistance)
{
	return QuickSendTransAttack(lpChar, power, 1, Resistance);
}

///////////////////////// 트레이드 관련 //////////////////////////////
/*
sTRADE sTrade;	   //보낼 정보
sTRADE sTradeRecv; //받은 정보
*/

//아이템 교환 신청
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

//아이템 교환 정보 전송
int SendTradeItem(sTRADE *lpTrade, DWORD dwSender)
{
	int cnt;
	int len;
	int	PotionError = 0;
	TRANS_TRADEITEMS TransTradeItems;
	sTRADE	sTradeBuff;

	len = 0;

	//트레이드 정보 인증 확인
	if (CheckTrade((void *)lpTrade) == FALSE)
	{
		SendRequestTrade(dwSender, 3);			//거래 취소 요구
		cTrade.CancelTradeItem();					//거래창 닫기
		return FALSE;
	}

	memcpy(&sTradeBuff, lpTrade, sizeof(sTRADE));

	//물약갯수 비교
	PotionError = ComparePotion();

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (!sTradeBuff.TradeItem[cnt].Flag)
		{
			ZeroMemory(&sTradeBuff.TradeItem[cnt], sizeof(sITEM));
		}
		else
		{
			sTradeBuff.TradeItem[cnt].sItemInfo.ItemHeader.Head = 0;		//키값 지움
			if (PotionError)
			{
				sTradeBuff.TradeItem[cnt].sItemInfo.PotionCount = sTradeBuff.TradeItem[cnt].sItemInfo.ItemHeader.Head + 1;	//물약갯수 오류시 모든 포션카운트를 1로 바꾼다
			}
			len++;
		}
	}

	//데이타 압축 ( Z/NZ 방식 )
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

//아이템 교환 정보 수신 
int RecvTradeItems(TRANS_TRADEITEMS *lpTransTradeItems, sTRADE *lpTrade)
{
	//압축 데이타 해독 ( Z/NZ 방식 )
	DecodeCompress(lpTransTradeItems->TradeBuff, (BYTE *)lpTrade);
	memcpy(&TransLastRecvTrade, lpTransTradeItems, lpTransTradeItems->size);		//받은 트레이드 데이타 임시 복사
	return TRUE;
}

//교환창 아이템 송신
int SendTradeItem(DWORD dwSender)
{
	return SendTradeItem(&sTrade, dwSender);
}
//교환창 아이템 수신
int RecvTradeItems(TRANS_TRADEITEMS *lpTransTradeItems)
{
	int res;

	res = RecvTradeItems(lpTransTradeItems, &sTradeRecv);
	cTrade.LoadTradeItemIamge();
	cTrade.CheckTradeButtonOk();

	ItemPickUpFlag = 1;

	//트레이드 정보 인증 확인
	if (CheckTrade(&sTradeRecv) == FALSE)
	{
		SendRequestTrade(lpTransTradeItems->dwSender, 3);	//교환 취소
		cTrade.CancelTradeItem();
		return FALSE;
	}

	if (sTrade.CheckFlag && !sTradeRecv.CheckFlag)
	{
		sTrade.CheckFlag = 0;
		TradeItemCancelCount++;
		if (TradeItemCancelCount >= 3)
		{
			//3번 취소 당함
			SendRequestTrade(lpTransTradeItems->dwSender, 3);	//교환 취소
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

//아이템 교환 인증키 전송
int SendTradeSucessKey(DWORD dwSender)
{
	//아이템 교환 인증키 전송
	return SendTradeSucessKey(&sTrade, dwSender);
}



//-------> 이함수가 거의 통채로 많이 바꿨으니 구버전 적용시 확인 //////////////

//아이템 교환 인증키 전송
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
				//보낼 아이템이 인벤에 남아 있음을 발견
				CancelFlag = TRUE;

				//인벤토리 아이템 복사 오류
				SendInvenItemError(1, lpTrade->TradeItem[cnt].sItemInfo.CODE,
								   lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head,
								   lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum);
			}

			if ((TransTradeItemKey.dwItemCode[cnt] & sinITEM_MASK1) == (sinPM1&sinITEM_MASK1))
			{
				//물약이다! ( 아이템 체크섬에 물약갯수를 설정 )
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
				//데이타 서버에 아이템 저장 요구 최대 32개 동시 ( 복사 아이템 방지 )
				SendSaveThrowItems( &TransThrowItems );

				if ( smWsockServer )
					smWsockServer->Send2( (char *)&TransTradeItemKey , TransTradeItemKey.size , TRUE ) ;

				if ( smWsockUserServer && smWsockServer!=smWsockUserServer )
					smWsockUserServer->Send2( (char *)&TransTradeItemKey , TransTradeItemKey.size , TRUE ) ;
		*/
		if (smWsockDataServer)
			smWsockDataServer->Send2((char *)&TransTradeItemKey, TransTradeItemKey.size, TRUE);


		//아이템 교환키
		memcpy(&LastTransTradeItemKey, &TransTradeItemKey, sizeof(TRANS_TRADE_ITEMKEY));
		dwLastTransTradeKeyTime = dwPlayTime;
	}

	dwTradeMaskTime = dwPlayTime + 3000;

	return TRUE;
}


//아이템 교환 인증키 수신
int RecvTradeSucessKey(TRANS_TRADE_ITEMKEY *lpTransTradeItemKey, sTRADE *lpTrade)
{
	int cnt;
	sTRADE sChkTrade;
	DWORD	dwChkSum1, dwChkSum2;
	int	ItemFlag;

	dwTradeMaskTime = dwPlayTime + 3000;

	if (TransLastRecvTrade.code)
	{
		//마지막 받은 데이타로 압축 데이타 다시 해독 ( Z/NZ 방식 )
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
				//물약인경우
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

				//잘못된 아이템 수신
				if (!lpTrade->TradeItem[cnt].sItemInfo.ItemHeader.Head)
					lpTrade->TradeItem[cnt].Flag = 0;
				else
				{
					if (ItemFlag == 1)
					{
						//물약 재인증
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

	//SendRecordTradeItems( lpTrade );			//교환 내용 서버에 기록

	return TRUE;
}



//아이템 교환 내용 확인
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
		SendSaveMoney();			//돈 저장 ( 복사방지 )
	}

	for (cnt = 0; cnt < MAX_TRADE_ITEM; cnt++)
	{
		if (sTrade.TradeItem[cnt].Flag)
		{
			TransTradeCheckItem.dwSendItemCode[cnt] = sTrade.TradeItem[cnt].sItemInfo.CODE;
			TransTradeCheckItem.dwSendItemSum[cnt] = sTrade.TradeItem[cnt].sItemInfo.ItemHeader.dwChkSum;

			//SendSaveThrowItem( &sTrade.TradeItem[cnt].sItemInfo );		//아이템 복사 방지

			//아이템 인증 확인
			if (CheckItemForm(&sTrade.TradeItem[cnt].sItemInfo) == FALSE)
				return FALSE;		//잘못된 아이템 

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
		//데이타 서버에 아이템 저장 요구 최대 32개 동시 ( 복사 아이템 방지 )
		SendSaveThrowItems( &TransThrowItems );
	*/

	if (smWsockServer)
		smWsockServer->Send2((char *)&TransTradeCheckItem, TransTradeCheckItem.size, TRUE);

	if (smWsockUserServer && smWsockServer != smWsockUserServer)
		smWsockUserServer->Send2((char *)&TransTradeCheckItem, TransTradeCheckItem.size, TRUE);

	dwTradeMaskTime = dwPlayTime + 3000;

	return TRUE;
}

//아이템 교환 내용 확인 수신
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







//파티 허락 
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

//파티 인터페이스 버튼입력 
int	PartyButton(int Num, int Button)
{
	// Num 파티원 번호
	// Button  0-이임 1-해산 2-추방 3-탈퇴
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
		//파티장 권한
		return FALSE;
	}

	return smWsockUserServer->Send((char *)&smTransCommand, smTransCommand.size, TRUE);
}





////////////////////////// 돈과 경험치 이동 기록 ///////////////

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

//창고 저장
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


	//용량 첵크
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


	//창고가 열렸을때와 닫혔을때의 돈의 차를 검사하기 위한 변수
	WareHouseSubMoney += (CompWareHouseMoney - lpWareHouse->Money);
	CompWareHouseMoney = 0;
	/*
		if ( !flag ) {
			//데이타 서버에 아이템 저장 요구 ( 복사 아이템 방지 )
			SendSaveMoney();				//돈

			TransThrowItems.ThrowItemCount = 0;
			for( cnt=0;cnt<THROW_ITEMS_MAX;cnt++) {
				if ( lpWareHouse->WareHouseItem[cnt].Flag ) {
					TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwCode   = lpWareHouse->WareHouseItem[cnt].sItemInfo.CODE;
					TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwHead   = lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.Head;
					TransThrowItems.ThrowItems[TransThrowItems.ThrowItemCount].dwChkSum = lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwChkSum;
					TransThrowItems.ThrowItemCount++;
				}
			}
			//데이타 서버에 아이템 저장 요구 최대 32개 동시 ( 복사 아이템 방지 )
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
		//돈 저장 ( 서버에서 사용할 내용 / 불러올때는 서버에서 초기화 해줘야 함 )
		TransWareHouse.WareHouseMoney = lpWareHouse->Money ^ (dwChkSum^smTRANSCODE_WAREHOUSE);
		TransWareHouse.UserMoney = lpCurPlayer->smCharInfo.Money ^ (dwChkSum^smTRANSCODE_WAREHOUSE);
	}

	TransWareHouse.dwTemp[0] = 0;
	TransWareHouse.dwTemp[1] = 0;
	TransWareHouse.dwTemp[2] = 0;
	TransWareHouse.dwTemp[3] = 0;
	TransWareHouse.dwTemp[4] = 0;

	dwLastWareHouseChkSum = dwChkSum;				//최종 저장 창고 코드

	if (flag)
	{
		memcpy(lpTransWareHouse, &TransWareHouse, sizeof(TRANS_WAREHOUSE));
		return TRUE;
	}

	if (TransWareHouse.size > smSOCKBUFF_SIZE) TransWareHouse.size = smSOCKBUFF_SIZE;

	if (smWsockDataServer)
	{
		smWsockDataServer->Send2((char *)&TransWareHouse, TransWareHouse.size, TRUE);

		SaveGameData();						//게임을 저장
		return TRUE;
	}

	return FALSE;
}
//창고 저장
int	SaveWareHouse(sWAREHOUSE *lpWareHouse)
{
	return SaveWareHouse(lpWareHouse, 0);
}

//창고 가져오기
int	LoadWareHouse(TRANS_WAREHOUSE *lpTransWareHouse, sWAREHOUSE *lpWareHouse, int flag)
{
	sWAREHOUSE	WareHouseCheck;
	int cnt;
	DWORD	dwChkSum;
	char	*szComp;
	char	szMsgBuff[128];

	if (lpTransWareHouse->DataSize == 0)
	{
		//초기값 아무것도 없음
		ZeroMemory(lpWareHouse, sizeof(sWAREHOUSE));
		lpWareHouse->Money = 2023;
		lpWareHouse->Weight[0] = 197;
		CompWareHouseMoney = lpWareHouse->Money;	//창고가 열렸을때와 닫혔을때의 돈의 차를 검사하기 위한 변수
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
			//마지막 저장한 창고 데이타와 신규 로드 데이타의 코드가 틀림
			return FALSE;
		}

		memcpy(lpWareHouse, &WareHouseCheck, sizeof(sWAREHOUSE));

		CompWareHouseMoney = lpWareHouse->Money;	//창고가 열렸을때와 닫혔을때의 돈의 차를 검사하기 위한 변수

		for (cnt = 0; cnt < 100; cnt++)
		{
			if (lpWareHouse->WareHouseItem[cnt].Flag)
			{

				//2차 보안 값
				if (!lpWareHouse->WareHouseItem[cnt].sItemInfo.Temp0)
					lpWareHouse->WareHouseItem[cnt].sItemInfo.Temp0 = lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwChkSum + lpWareHouse->WareHouseItem[cnt].sItemInfo.CODE;

				//CheckCopyItem( &lpWareHouse->WareHouseItem[cnt].sItemInfo );		//복사 또는 불량아이템 검사

				if (!lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwTime)
				{
					//코드 위장
					lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwTime =
						(lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.Head >> 2) ^ (lpWareHouse->WareHouseItem[cnt].sItemInfo.ItemHeader.dwChkSum << 2);
				}

				//송편 확인
				if ((lpWareHouse->WareHouseItem[cnt].sItemInfo.CODE&sinITEM_MASK2) == sinSP1)
				{
					//송편 ( 포션카운터가 있으면 문제있는 송편 )
					if (lpWareHouse->WareHouseItem[cnt].sItemInfo.PotionCount > 1)	lpWareHouse->WareHouseItem[cnt].Flag = 0;

					// 장별 - SP1 아이템 창고에서 사라지는 문제
					/*
					//생성기간 확인
					if ( lpWareHouse->WareHouseItem[cnt].sItemInfo.dwCreateTime<(sinItemTime-(60*60*24*12)) ||
						lpWareHouse->WareHouseItem[cnt].sItemInfo.dwCreateTime>(sinItemTime+(60*60*24*7)) ) {
							lpWareHouse->WareHouseItem[cnt].Flag =0;
						}
						*/
				}

				if (DeleteEventItem_TimeOut(&lpWareHouse->WareHouseItem[cnt].sItemInfo) == TRUE)
				{
					//이벤트 아이템 날짜맞춰 제거
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

//창고 가져오기
int	LoadWareHouse(TRANS_WAREHOUSE *lpTransWareHouse, sWAREHOUSE *lpWareHouse)
{
	return LoadWareHouse(lpTransWareHouse, lpWareHouse, 0);
}

//이벤트 참가비 보냄
int SendCollectMoney(int Money)
{
	/*
		smTRANS_COMMAND	TransCommand;

		TransCommand.code =	smTRANSCODE_COLLECTMONEY;
		TransCommand.size = sizeof(smTRANS_COMMAND);
		TransCommand.WParam = Money;							//기부한 돈
		TransCommand.LParam = lpCurPlayer->smCharInfo.Money;		//보유한 돈
		TransCommand.SParam = 0;

		AddInvenMoney( -Money );				//인벤 돈 확인

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
		{ //코인이면 
			memcpy(&sItemMoney, &sItem[j], sizeof(sITEM));
			sItemMoney.sItemInfo.Money = Money;			//금액을 넘겨준다 
			sItemMoney.sItemInfo.ItemKindCode = smTRANSCODE_COLLECTMONEY;
			ReformItem(&sItemMoney.sItemInfo);		//인증 
			if (sinThrowItemToFeild(&sItemMoney))
			{
				AddInvenMoney(-Money);				//인벤 돈 확인
				return TRUE;
			}
		}
	}

	return FALSE;
}

//PK당해서 죽음
int PlayerKilling(DWORD dwObjectSerial)
{
	smTRANS_COMMAND	TransCommand;

	TransCommand.code = smTRANSCODE_PLAYERKILLING;
	TransCommand.size = sizeof(smTRANS_COMMAND);
	TransCommand.WParam = dwObjectSerial;				//상대방 코드
	TransCommand.LParam = lpCurPlayer->dwObjectSerial;		//내 코드
	TransCommand.SParam = 0;

	if (smWsockUserServer)
	{
		return smWsockUserServer->Send2((char *)&TransCommand, TransCommand.size, TRUE);
	}
	return FALSE;
}



//믹스쳐 데이타를 서버로 보냄
int SendCraftItemToServer(void *lpsCraftItem_Send)
{
	Log::Debug("SendCraftItemToServer : Requisitado por�m foi removido.");
	return NULL;
}
//위프용 윙 아이템 데이타를 서버로 보냄
int SendWingItemToServer(void *lpsCraftItem_Send)
{
	Log::Debug("SendWingItemToServer : Requisitado por�m foi removido.");
	return NULL;
}

//에이징 데이타를 서버로 보냄
int SendAgingItemToServer(void *lpsCraftItem_Send)
{
	Log::Debug("SendAgingItemToServer : Requisitado por�m foi removido.");
	return NULL;
}
//링크코어 등록 보냄
int SendLinkCoreToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendLinkCoreToServer : Requisitado por�m foi removido.");
	return NULL;
}
int SucessLinkCore(smTRANS_COMMAND_EX *lpTransCommandEx)
{
	int x, z, cnt;

	sITEMINFO	*lpItem;

	//인벤토리의 지정한 아이템을 찾아 검색하여 찾는다 ( 링크 코어 제거 )
	lpItem = FindInvenItem(lpTransCommandEx->WxParam, lpTransCommandEx->LxParam, lpTransCommandEx->SxParam);

	if (lpItem)
	{
		SendUseItemCodeToServer(lpItem);
		cInvenTory.DeleteInvenItemToServer(lpTransCommandEx->WxParam, lpTransCommandEx->LxParam, lpTransCommandEx->SxParam);
		ResetInvenItemCode();				//이벤 아이템 코드 초기화

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

	//바닥 확인
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
	SkillPlaySound(SKILL_SOUND_LEARN, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);			//스킬 효과음
	RestartPlayCount = 350;		//5초 동안 무적

	return TRUE;
}


//에이징 완료된 아이템을 서버로 보냄
int SendAgingUpgradeItemToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendAgingUpgradeItemToServer : Requisitado por�m foi removido.");
	return NULL;
}
/*
//아이템 확인
#define smTRANSCODE_CHECKITEM			0x50320300
//아이템 오류 확인
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


//인벤토리의 물약갯수를 전부 1로 세팅
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

//인벤토리의 아이템 찾기 (코드)
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

//아이템을 서버에 보내서 확인
int	SendCheckItemToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendCheckItemToServer : Requisitado por�m foi removido.");
	return NULL;
}
//아이템 확인 결과 도착
int RecvCheckItemFromServer(TRANS_ITEM_CODE *lpTransItemCode)
{
	Log::Debug("RecvCheckItemFromServer : Requisitado por�m foi removido.");
	return NULL;
}
//판매할 아이템을 서버로 보냄
int	SellItemToServer(sITEMINFO *lpItem, int Count)
{
	Log::Debug("SellItemToServer : Requisitado por�m foi removido.");
	return NULL;
}



int Chk_InventPosCount = 1;
DWORD	dwCheckInvenItemTime = 0;

//인벤토리의 장착된 아이템을 서버로 보내서 검사
int CheckInvenItemToServer()
{
	Log::Debug("CheckInvenItemToServer : Requisitado por�m foi removido.");
	return NULL;
}

////////////////////////////// 메모리 타임 첵크 쓰레드 ///////////////////////////////

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


//로딩중 화면 표시
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




//서버에서 메모리 값을 정해서 가져간다
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
//서버에서 메모리 값을 정해서 가져간다
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

	{ (DWORD)E_Shield_BlockRate << 1			,	(DWORD)((Meteo_UseMana - E_Shield_BlockRate) + 10) },		//스킬 테이블 보안

	{ (DWORD)Code_VRamBuffOpen << 1			,	(DWORD)3600	},			//마지막 꺼는 별도 첵크 한다
	{ (DWORD)0								,	(DWORD)0 }
};



//클라이언트 펑션 위치와 값을 서버에 보고
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

	//서버에 보고
	if (smWsockDataServer)
	{
		return smWsockDataServer->Send2((char *)&TransClientFuncPos, TransClientFuncPos.size, TRUE);
	}

	return FALSE;
}


//메모리 검사 모듈 받음
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


////////////////////////////// 아이템 임지 저장 큐처리 //////////////////////////

#define	RECV_TRANSITEM_QUE_MAX			256
#define	RECV_TRANSITEM_QUE_MASK			255

TRANS_ITEMINFO	Recv_TransItemInfo_Que[RECV_TRANSITEM_QUE_MAX];

int	RecvTransItemQue_Push = 0;
int	RecvTransItemQue_Pop = 0;

//아이템 큐에 임시 저장
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

//임시저장된 아이템 큐에서 찾기
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

//임시저장된 아이템 큐에 아이템 대기중인지 검사
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

//사용한 아이템 코드 서버로 보냄
int SendUseItemCodeToServer(sITEMINFO *lpItem)
{
	Log::Debug("SendUseItemCodeToServer : Requisitado por�m foi removido.");
	return NULL;
}
//주변 캐릭터용 이벤트 명령 코드
int RecvCommandUser(smTRANS_COMMAND_EX *lpTransCommandEx)
{
	smCHAR *lpChar;
	smCHAR *lpChar2;

	switch (lpTransCommandEx->WParam)
	{
		case smCOMMNAD_USER_WARP:
			//워프 이펙트 표시
			StartEffect(lpTransCommandEx->LParam, lpTransCommandEx->SParam, lpTransCommandEx->EParam, EFFECT_RETURN1);
			SkillPlaySound(SKILL_SOUND_LEARN, lpTransCommandEx->LParam, lpTransCommandEx->SParam, lpTransCommandEx->EParam);			//스킬 효과음
			break;


		case smCOMMNAD_USER_AGINGUP:
			//자동 플레이어를 찾는다
			lpChar = FindChrPlayer(lpTransCommandEx->WxParam);
			if (lpChar)
			{
				StartEffect(lpChar->pX, lpChar->pY, lpChar->pZ, EFFECT_AGING);
				esPlaySound(7, GetDistVolume(lpChar->pX, lpChar->pY, lpChar->pZ), 1600);	//사운드 연주
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

		case smCOMMNAD_USER_FORCEORB:		//포스 사용
			//자동 플레이어를 찾는다
			lpChar = FindChrPlayer(lpTransCommandEx->WxParam);
			if (lpChar)
			{
				StartSkill(lpChar->pX, lpChar->pY, lpChar->pZ, 0, 0, 0, SKILL_UP1);
				SkillPlaySound(SKILL_SOUND_LEARN, lpChar->pX, lpChar->pY, lpChar->pZ);			//스킬 효과음
			}
			break;

		case smCOMMNAD_USER_VALENTO:
			//발렌토 소울서커 이펙트
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

//주변 캐릭터에 이벤트 명령 코드 보내기
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

//지역서버에 명령 코드 전송
int	SendCommand_AreaServer(DWORD dwCode, int wParam, int lParam, int sParam, int eParam)
{
	Log::Debug("SendCommand_AreaServer : Requisitado por�m foi removido.");
	return NULL;
}

//지역서버에 명령 코드 전송
int	SendCommand_DataServer(DWORD dwCode, int wParam, int lParam, int sParam, int eParam)
{
	Log::Debug("SendCommand_DataServer : Requisitado por�m foi removido.");
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
//공격력 기록 초기화
int	Init_RecordDamage()
{
	ZeroMemory(sRecvDamage, sizeof(sDAMAGE_RECORD) * 3);
	ZeroMemory(sSendDamage, sizeof(sDAMAGE_RECORD) * 3);

	return TRUE;
}

//받은 공격력 기록
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

//보낸 공격력 기록
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

//파티원에게 스킬 사용 정보 보내기
int	SendPartySkillToServer(DWORD dwSkillCode, int SkillLevel, int Around, int wParam, int lParam, int sParam, int eParam)
{
	Log::Debug("SendPartySkillToServer : Requisitado por�m foi removido.");
	return NULL;
}
//스킬 취소
int SendCancelSkillToServer(DWORD dwSkillCode, int lParam, int sParam, int eParam)
{
	Log::Debug("SendCancelSkillToServer : Requisitado por�m foi removido.");
	return NULL;
}
//스킬 변경
int SendUpdateSkillToServer(DWORD dwSkillCode, int lParam, int sParam, int eParam)
{
	Log::Debug("SendUpdateSkillToServer : Requisitado por�m foi removido.");
	return NULL;
}
//서버에 스킬수행 보냄
int SendProcessSKillToServer(DWORD dwSkillCode, int point, int Param1, int Param2)
{
	Log::Debug("SendProcessSKillToServer : Requisitado por�m foi removido.");
	return NULL;
}
//파티 스킬 패킷 수신
int RecvPartySkillFromServer(TRANS_PARTY_SKILL *lpTransPartySkill)
{
	Log::Debug("RecvPartySkillFromServer : Requisitado por�m foi removido.");
	return NULL;
}
DWORD	dwLastClanUserCode = 0;

//클랜 메세지 보내기
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

//클랜 가입 승인 메세지 보내기
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

//클랜 메세지 받기
int RecvClanJoinService(smTRANS_CHAR_COMMAND2 *lpTransCharCommand)
{
	smCHAR *lpChar;

	switch (lpTransCharCommand->WParam)
	{
		case 1:		//가입 권유
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


/////////// DivineLightning 스킬 서브 함수 ///////////////////

DWORD	dwSkill_DivineLightning_Target[8];
int		DivineLightning_Target_Count = 0;
int		DivineLightning_FindCount = 0;

//디바인 라이트닝 스킬 연출 ( 준비함수 - 주인공만 )
int	SkillPlay_DivineLightning_Select(smCHAR *lpChar, int SkillPoint)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int	LightningCount;
	int cCount;
	TRANS_SKIL_ATTACKDATA	TransSkilAttackData;
	smWINSOCK *lpsmSendSock = 0;

	//클라이언트 로그 파일 기록
	Record_ClinetLogFile("구버전 공격패킷 - SkillPlay_DivineLightning_Select ");
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

	TransSkilAttackData.AttackState = 103;			//언데드 가중
	TransSkilAttackData.AttackSize = 160 * 160;
	TransSkilAttackData.Power = GetRandomPos(Divine_Lightning_Damage[SkillPoint - 1][0], Divine_Lightning_Damage[SkillPoint - 1][1]);

	lpsmSendSock = GetAreaServerSock();		//보낼 서버구하기

	if (lpsmSendSock && TargetCount > 0)
	{
		//보낸 공격력 기록
		Record_SendDamage(lpsmSendSock, TransSkilAttackData.Power);

		lpsmSendSock->Send2((char *)&TransSkilAttackData, TransSkilAttackData.size, TRUE);
	}

	return TRUE;
}

//디바인 라이트닝 스킬 연출 ( 이펙트 실현 )
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


//디바인 라이트닝 스킬 연출 ( 머미로드 )
int SkillPlay_MummyLord_Effect(smCHAR *lpChar, int Range)
{
	int cnt, TargetCount;
	int x, y, z, dist;
	int	dRange = Range*Range;

	TargetCount = 0;

	if (lpChar->smCharInfo.Brood != smCHAR_MONSTER_USER)
	{
		//몬스터가 유저 공격
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
		//유저랑 같은 편 ( 몬스터 공격 )
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


//베놈 스피어 스킬 연출 ( 이펙트 실현 )
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

//몬스터 스킬 이펙트 실현 ( 이펙트 실현 )
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
		ParkAssaParticle_ChaosKaraSkill_User(lpCurPlayer);	// 카오스카라 뱀파이어릭 (유저)
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
				ParkAssaParticle_ChaosKaraSkill_User(&chrOtherPlayer[cnt]);	// 카오스카라 뱀파이어릭 (유저)
				TargetCount++;
			}
		}
	}

	return TargetCount;
}




//스킬 실행 패킷 수신
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
			//홀리 리플렉션 타격 이펙트
			sinSkillEffect_Holy_Reflection_Defense(lpCurPlayer);
			break;

		case SKILL_PLAY_ENCHANT_WEAPON:
			//인첸트 웨폰
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
			//죽은자 살린기
			if (lpCurPlayer->MotionInfo && lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD)
			{
				SendProcessSKillToServer(SKILL_PLAY_RESURRECTION, point, 0, 0);

				lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_RESTART);		//시작모션
				StartEffect(lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ, EFFECT_GAME_START1);
				SetDynLight(lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ, 100, 100, 100, 0, 200);
				CharPlaySound(lpCurPlayer);

				sinSetLife(lpCurPlayer->smCharInfo.Life[1] / 2);
				ResetEnergyGraph(4);								//에너지 그래프 첵크 초기화
				lpCurPlayer->MoveFlag = FALSE;
				RestartPlayCount = 350;								//5초 동안 무적
				ReStartFlag = 0;
			}
			break;

		case SKILL_PLAY_VIRTUAL_LIFE:
			//버츄얼 라이프 적용
			cSkill.SetVirtualLife(Virtual_Life_Time[point - 1], point);
			AssaParticle_VirtualLife(lpCurPlayer, Virtual_Life_Time[point - 1]);
			break;

		case SKILL_PLAY_LOVELY_LIFE:
			//사랑의 초콜렛
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
			//배니쉬 스킬 해제
			lpCurPlayer->PlayVanish = 0;
			cSkill.CancelContinueSkill(SKILL_VANISH);

			if (dwM_BlurTime) dwM_BlurTime = dwPlayTime;
			break;

		case SKILL_PLAY_TRIUMPH_OF_VALHALLA:
			//트라이엄프
			cSkill.SetT_Of_Valhalla(point, lpTransCommand->EParam);
			AssaParticle_TriumphOfValhalla(lpCurPlayer, T_Of_Valhalla_Time[point - 1]);
			break;

		case SKILL_PLAY_SPARK_SHIELD:
			//스파크쉴드 이펙트

			// 유저번호로 찾는다
			lpChar = FindChrPlayer(lpTransCommand->LParam);
			lpChar2 = FindChrPlayer(lpTransCommand->SParam);
			if (lpChar && lpChar2)
			{
				//스파크 쉴드
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
			//홀오브발할라
			cSkill.HellOfValhalla(Hall_Of_Valhalla_Time[point - 1], point, lpTransCommand->SParam, lpTransCommand->EParam, 1);
			SkillValhallaHallOfValhallaHandEffect(lpCurPlayer, (float)lpTransCommand->EParam);
			lpCurPlayer->dwHallOfValhallaTime = dwPlayTime + Hall_Of_Valhalla_Time[point - 1] * 1000;	//홀오브발할라 유지시간
			break;

		case SKILL_PLAY_FORCE_OF_NATURE:
			//포스오브 네이쳐
			cSkill.ForeceOfNature(Force_Of_Nature_Time[point - 1], point, 1);
			SkillSagittarionForceOfNature1(lpCurPlayer, (float)Force_Of_Nature_Time[point - 1]);
			lpCurPlayer->dwForceOfNatureTime = dwPlayTime + Force_Of_Nature_Time[point - 1] * 1000;	//포스오브 네이쳐 유지시간
			break;


		case SKILL_PLAY_SOD_ITEM:
			//SOD 깜짝 아이템 이펙트

			switch (lpTransCommand->SParam)
			{
				case 1:	//폭탄
					SetBellatraFontEffect(E_BL_QUAKE_SEAL);
					SetBellatraFontEffect(E_BL_FONT_QUAKE_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 2:	//시간
					SetBellatraFontEffect(E_BL_STUN_SEAL);
					SetBellatraFontEffect(E_BL_FONT_STUN_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 3:	//얼음
					SetBellatraFontEffect(E_BL_FREEZE_SEAL);
					SetBellatraFontEffect(E_BL_FONT_FREEZE_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 4:	//토끼
					SetBellatraFontEffect(E_BL_RABBIE_SEAL);
					SetBellatraFontEffect(E_BL_FONT_RABBIE_SEAL);
					EffectWaveCamera(50, 3);
					SkillPlaySound(SKILL_SOUND_SKILL_DIASTROPHISM, lpCurPlayer->pX, lpCurPlayer->pY, lpCurPlayer->pZ);
					break;

				case 5:	//달수정
					SetBellatraFontEffect(E_BL_STYGIAN_SEAL);
					SetBellatraFontEffect(E_BL_FONT_STYGIAN_SEAL);
					break;

				case 6:	//태양수정
					SetBellatraFontEffect(E_BL_GUARDIAN_SEAL);
					SetBellatraFontEffect(E_BL_FONT_GUARDIAN_SEAL);
					break;

				case 7:	//천공수정
					SetBellatraFontEffect(E_BL_POINT_SEAL);
					SetBellatraFontEffect(E_BL_FONT_POINT_SEAL);
					break;
			}
			esPlaySound(19, 360);		//SOD 아이템 소리
			break;

		case SKILL_PLAY_HEALING:
			//매직 마스터 힐링사용
			lpChar = FindChrPlayer(lpTransCommand->LParam);
			if (lpChar)
			{
				sinEffect_Healing2(lpChar);		//힐링 이펙트
				SkillPlaySound(SKILL_SOUND_SKILL_HEALING, lpChar->pX, lpChar->pY, lpChar->pZ);		//힐링 사운드
			}
			break;

		case SKILL_PLAY_BLESS_SIEGE_ITEM:
			//블래스 캐슬 공성 아이템 사용

			switch (lpTransCommand->SParam)
			{

				////////////// 빌링용 /////////////////
				case (sinBI1 | sin05) :	//무적스크롤
					lpCurPlayer->PlayInvincible = 70 * 30;		//30초
					break;
				case (sinBI1 | sin06) :	//크리티컬 스크롤
					break;
				case (sinBI1 | sin07) :	//추가 회피 스크롤
					break;

					//////////////// 공성용 //////////////////

				case (sinBC1 | sin01) :	//무적스크롤
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_S_INVU);
					lpCurPlayer->PlayInvincible = 70 * 30;		//30초
					break;
				case (sinBC1 | sin02) :	//크리티컬 스크롤
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_S_CRITICAL);
					break;
				case (sinBC1 | sin03) :	//추가 회피 스크롤
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_S_EVASION);
					break;
					/*
						case (sinBC1|sin04):	//파티원 전원 회복
						case (sinBC1|sin05):	//부활 스크롤
							return FALSE;
					*/
				case (sinBC1 | sin06) :	//파이어 크리스탈 저항석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_R_FIRE_C);
					break;
				case (sinBC1 | sin07) :	//아이스 크리스탈 저항석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_R_ICE_C);
					break;
				case (sinBC1 | sin08) :	//라이트닝 크리스탈 저항석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_R_LIGHTING_C);
					break;
				case (sinBC1 | sin09) :	//대 파이터 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_FIGHTER);
					break;
				case (sinBC1 | sin10) :	//대 메카니션 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_MECHANICIAN);
					break;
				case (sinBC1 | sin11) :	//대파이크맨 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_PIKEMAN);
					break;
				case (sinBC1 | sin12) :	//대 이쳐 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_ARCHER);
					break;
				case (sinBC1 | sin13) :	//대 나이트 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_KNIGHT);
					break;
				case (sinBC1 | sin14) :	//대 아탈란타 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_ATALANTA);
					break;
				case (sinBC1 | sin15) :	//대 메지션 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_MAGICIAN);
					break;
				case (sinBC1 | sin16) :	//대 프리스티스 공격력 강화석
					//chaSiege.SetCastleItemSkill( SIN_CASTLEITEMSKILL_A_PRIESTESS);
					break;
			}
			break;

	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////

//int ReadClanInfo(DWORD dwClanNum)
//CLANINFO ClanInfo[CLAN_MAX]; //클랜 정보 저장
//
//cldata 없데이트용
//void Updatecldata();

//클랜 업데이트 요구 메세지
//#define	smTRANSCODE_CLAN_UPDATE			0x50320C10


//클랜원 정보 전송용 함수
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

//클랜원 정보 해독
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

//퀘스트 아이템 입수 필터링
int FiltQuestItem(TRANS_ITEMINFO	*lpTransItemInfo, DWORD dwPacketCode)
{
	int cnt;

	if (chaQuest.sHaQuestElementary[6].CODE != HAQUEST_CODE_ELEMENTARY_G)
	{

		cnt = cInvenTory.SearchItemCode(sinQT1 | sin16);
		if (cnt >= 1) { DeleteQuestItem(sinQT1 | sin16); }
		if (lpTransItemInfo->Item.CODE == (sinQT1 | sin16)) { return FALSE; }		//성근수정

	}

	switch (sinQuest_ChangeJob2.CODE)
	{
		case SIN_QUEST_CODE_CHANGEJOB2_NPC_M:
			//벌꿀 아이템 (최대 갯수 확인 )
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin01))
			{
				if (!cInvenTory.SearchItemCode(sinMA1 | sin01)) return FALSE;		//유리병 없는경우
				if (cInvenTory.SearchItemCode(sinQT1 | sin04)) return FALSE;		//로얄제리 있는 경우
				cnt = cInvenTory.SearchItemCode(sinMA2 | sin01);
				if (cnt >= 9) return FALSE;
			}
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin02)) return FALSE;
			return TRUE;

		case SIN_QUEST_CODE_CHANGEJOB2_NPC_D:
			//기름 아이템 (최대 갯수 확인 )
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin02))
			{
				if (!cInvenTory.SearchItemCode(sinMA1 | sin01)) return FALSE;		//유리병 없는경우
				if (cInvenTory.SearchItemCode(sinQT1 | sin05)) return FALSE;		//발모제 있는 경우
				cnt = cInvenTory.SearchItemCode(sinMA2 | sin02);
				if (cnt >= 7) return FALSE;
			}
			if (lpTransItemInfo->Item.CODE == (sinMA2 | sin01)) return FALSE;
			return TRUE;
	}

	//3차 전직 퀘스트
	//뱀프
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

	//레벨 퀘스트용 뱀프 1
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
	//레벨 퀘스트용 뱀프 2
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
	//레벨 퀘스트용 뱀프 3
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


	//3차 전업 퀘스트용 
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



	//엄프의 망치 퀘스트 
	if (lpTransItemInfo->Item.CODE == (sinQT1 | sin16))
	{

		if (chaQuest.sHaQuestElementary[6].CODE == HAQUEST_CODE_ELEMENTARY_G)
		{
			cnt = cInvenTory.SearchItemCode(sinQT1 | sin16);
			if (cnt >= 1)
				return FALSE;

			if (dwPacketCode == smTRANSCODE_GETITEM)
			{
				//아이템 획득 메세지
				char szBuff[256];
				//wsprintf( szBuff, mgGetQuestItem , lpTransItemInfo->Item.ItemName );//_ignore_//해외
				AddChatBuff(szBuff, 6);
			}
			return TRUE;
		}
		return FALSE;
	}


	if ((lpTransItemInfo->Item.CODE&sinITEM_MASK2) == sinMA1 ||
		(lpTransItemInfo->Item.CODE&sinITEM_MASK2) == sinMA2)
	{
		//퀘스트가 없는경우 퀘스트 아이템 들어온 경우

		return FALSE;
	}

	return TRUE;
}

POINT GetProcessModule();

//모듈검사 하여 서버로 보내기
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

//팔곤 데미지 보내기
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

//댄싱소드 데미지 보내기
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


//메그네틱 스피어 데미지 보내기
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

//머스펠 데미지 보내기
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



//저랩용 펫 데미지
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


//PC방용 펫 데미지
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



//아이템 택배 서비스 수신 요구
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

//아이템 택배 서비스 수신 요구
int	SendItemExpress(DWORD	dwItemCode)
{
	return	SendItemExpress(dwItemCode, 0);
}


//야호 모션 서버로 보내기
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

		lpsmSock = GetAreaServerSock();		//지역서버 소켓을 찾음
		if (lpsmSock)
			lpsmSock->Send2((char *)&smTransCommand, smTransCommand.size, TRUE);

		return TRUE;

	}
	return FALSE;
}

//별 포인트 정보를 서버에 보낸다
int	SendStarPointToServer(int Price, int Cash)
{
	Log::Debug("SendStarPointToServer : Requisitado por�m foi removido.");
	return NULL;
}
//기부한돈을 서버에 보낸다
int	SendGiveMoneyToServer(int Money)
{
	Log::Debug("SendGiveMoneyToServer : Requisitado por�m foi removido.");
	return NULL;
}
//클랜머니 정보를 서버에 보낸다
int	SendClanMoneyToServer(int Money, int Flag, int Kind)
{
	Log::Debug("SendClanMoneyToServer : Requisitado por�m foi removido.");
	return NULL;
}
int	SendClanMoneyToServer(int Money, int Flag)
{
	Log::Debug("SendClanMoneyToServer : Requisitado por�m foi removido.");
	return NULL;
}
//이용요금 정보를 서버에 보낸다
int	SendPaymentMoneyToServer(int Money, int Flag)
{
	Log::Debug("SendPaymentMoneyToServer : Requisitado por�m foi removido.");
	return NULL;
}


/*
//개인상점 오픈
#define smTRANSCODE_OPEN_PERTRADE	0x48478A90
//개인상점 부르기
#define smTRANSCODE_CALL_PERTRADE	0x48478AA0
//개인상점 아이템
#define	smTRANSCODE_MYSHOP_ITEM		0x48478AB0

int	Send_ShowMyShopItem( smCHAR *lpChar );							//개인상점 아이템 내용 요구
int	Send_MyShopItemList( smTRANS_COMMAND	*lpTransCommand );		//개인상점 아이템 내용 상대방에 보내기
int	Recv_MyShopItemList( TRANS_TRADEITEMS *lpTransTradeItems );		//개인상점 아이템 내용 받음
*/


//개인상점 아이템 내용 요구
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

//개인상점 아이템 내용 상대방에 보내기
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


//개인상점 아이템 내용 받음
int	Recv_MyShopItemList(TRANS_TRADEITEMS *lpTransTradeItems)
{
	sMYSHOP MyShopItem;
	smCHAR	*lpChar;

	//압축 데이타 해독 ( Z/NZ 방식 )
	DecodeCompress(lpTransTradeItems->TradeBuff, (BYTE *)&MyShopItem);

	//상렬씨 함수 호출 예정
	cCharShop.RecvShopItem(&MyShopItem);


	lpChar = FindAutoPlayer(lpTransTradeItems->dwSender);

	if (lpChar && lpChar->DisplayFlag && lpChar->smCharInfo.szName[0])
	{
		OpenEachPlayer(lpChar);
		DispEachMode = TRUE;
	}

	return TRUE;
}


//개인 상점 물건 구입
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

//개인 상점 아이템 정보 업데이트
int	UpdateMyShopList(void *lpMyShop)
{
	sMYSHOP *lpMyShopItem = (sMYSHOP *)lpMyShop;
	int	result = 0;
	int		len;

	ZeroMemory(&TransTradeItems_MyShop, sizeof(TRANS_TRADEITEMS));

	//데이타 압축 ( Z/NZ 방식 )
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

//개인 상점 오픈
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

	//데이타 압축 ( Z/NZ 방식 )
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
		//해외 중국(개인상점이름관련)
		//#ifdef _LANGUAGE_CHINESE
		//		ConvertStringTHAI((char *)&TransChatMessage, lstrlen((char *)&TransChatMessage)+16);
		//#endif
		result = lpsmSock->Send2((char *)&TransChatMessage, TransChatMessage.size, TRUE);

	if (result)
	{
		/*
		//해외 중국(개인상점이름관련)
		#ifdef _LANGUAGE_CHINESE
				ConvertStringTHAI(szTradeMsg, lstrlen(szTradeMsg)+16);
		#endif
		*/
		wsprintf(lpCurPlayer->szTradeMessage, "%s: %s", lpCurPlayer->smCharInfo.szName, szTradeMsg);
		lpCurPlayer->dwTradeMsgCode = dwCode;
	}

	return result;
}

//상점 닫기
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

// 장별 - 입력창
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


//날짜 확인 제거 아이템 
int DeleteEventItem_TimeOut(sITEMINFO	*lpsItem)
{
	// 박재원 - 에이징 마스터, 스킬 마스터 추가
	if (lpsItem->CODE == (sinBI1 | sin32) || lpsItem->CODE == (sinBI1 | sin33) || lpsItem->CODE == (sinBI1 | sin34) || lpsItem->CODE == (sinBI1 | sin51) ||// pluto 펫(해외)
	//	lpsItem->CODE == (sinBI1|sin35) || lpsItem->CODE == (sinBI1|sin36) || lpsItem->CODE == (sinBI1|sin37) || lpsItem->CODE == (sinBI1|sin38) )	//수표
		lpsItem->CODE == (sinBI1 | sin36) || lpsItem->CODE == (sinBI1 | sin37) ||
		lpsItem->CODE == (sinBI1 | sin38) || lpsItem->CODE == (sinBI1 | sin39) ||
		lpsItem->CODE == (sinBI1 | sin40) || lpsItem->CODE == (sinBI1 | sin41) ||
		lpsItem->CODE == (sinBI1 | sin42) || lpsItem->CODE == (sinBI1 | sin43) ||
		lpsItem->CODE == (sinBI1 | sin44) || lpsItem->CODE == (sinBI1 | sin45) ||
		lpsItem->CODE == (sinBI1 | sin46) || lpsItem->CODE == (sinBI1 | sin47) ||
		lpsItem->CODE == (sinBI1 | sin48) || lpsItem->CODE == (sinBI1 | sin49) ||
		lpsItem->CODE == (sinBI1 | sin50) || lpsItem->CODE == (sinBI1 | sin52) || // 박재원 - 빌링 도우미 펫 추가
		lpsItem->CODE == (sinBI1 | sin53) || lpsItem->CODE == (sinBI1 | sin54) ||

		lpsItem->CODE == (sinBI1 | sin65) || lpsItem->CODE == (sinBI1 | sin66) || // 장별 - 빌링 도우미 펫 (7일, 1일)
		lpsItem->CODE == (sinBI1 | sin67) || lpsItem->CODE == (sinBI1 | sin68) ||
		lpsItem->CODE == (sinBI1 | sin69) || lpsItem->CODE == (sinBI1 | sin70) ||
		lpsItem->CODE == (sinBI1 | sin71) || lpsItem->CODE == (sinBI1 | sin72) ||
		lpsItem->CODE == (sinBI1 | sin55) || lpsItem->CODE == (sinBI1 | sin77) ||	// 장별 - 피닉스펫(1시간)
		lpsItem->CODE == (sinBI1 | sin62) || lpsItem->CODE == (sinBI1 | sin63) ||	// 박재원 - 에이징 마스터(2차) 아이템 추가 D, E
		lpsItem->CODE == (sinBI1 | sin64) ||										// 박재원 - 에이징 마스터(2차) 아이템 추가 F
		lpsItem->CODE == (sinBI1 | sin73) || lpsItem->CODE == (sinBI1 | sin74) || // 장별 - 빌링 도우미 펫 추가(테리 1시간 외 4종)
		lpsItem->CODE == (sinBI1 | sin75) || lpsItem->CODE == (sinBI1 | sin76)) // 장별 - 빌링 도우미 펫 (1시간)
	{
		return FALSE;
	}

	struct	tm	tm;
	time_t		ttm;//,ttm2;
	DWORD		dwCreateTime;

	DWORD		dwItemCode;

	dwItemCode = lpsItem->CODE&sinITEM_MASK2;

	DWORD NowTime = GetPlayTime_T(); // 현재 시간(second단위)

	//한복 14일
	if (lpsItem->CODE == (sinDA1 | sin48) || lpsItem->CODE == (sinDA2 | sin48))
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 14) < NowTime)
			return true;
	}
	//코스튬 7일
	if (lpsItem->CODE == (sinDA1 | sin31) || lpsItem->CODE == (sinDA1 | sin33) ||
		lpsItem->CODE == (sinDA1 | sin35) || lpsItem->CODE == (sinDA1 | sin37) ||
		lpsItem->CODE == (sinDA1 | sin39) || lpsItem->CODE == (sinDA1 | sin41) ||
		lpsItem->CODE == (sinDA1 | sin43) || lpsItem->CODE == (sinDA1 | sin45) ||
		lpsItem->CODE == (sinDA2 | sin31) || lpsItem->CODE == (sinDA2 | sin33) ||
		lpsItem->CODE == (sinDA2 | sin35) || lpsItem->CODE == (sinDA2 | sin37) ||
		lpsItem->CODE == (sinDA2 | sin39) || lpsItem->CODE == (sinDA2 | sin41) ||
		lpsItem->CODE == (sinDA2 | sin43) || lpsItem->CODE == (sinDA2 | sin45) ||
		lpsItem->CODE == (sinDB1 | sin31) || // 박재원 - 스피드 부츠(7일) 추가
		lpsItem->CODE == (sinOA2 | sin31) || // 박재원 - 슈퍼 암릿(7일) 추가
		lpsItem->CODE == (sinOR2 | sin31) || // 박재원 - 보스 몬스터 링 추가(바벨)
		lpsItem->CODE == (sinOR2 | sin32))  // 박재원 - 보스 몬스터 링 추가(퓨리)
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return true;
	}
	//코스튬 30일
	if (lpsItem->CODE == (sinDA1 | sin32) || lpsItem->CODE == (sinDA1 | sin34) ||
		lpsItem->CODE == (sinDA1 | sin36) || lpsItem->CODE == (sinDA1 | sin38) ||
		lpsItem->CODE == (sinDA1 | sin40) || lpsItem->CODE == (sinDA1 | sin42) ||
		lpsItem->CODE == (sinDA1 | sin44) || lpsItem->CODE == (sinDA1 | sin46) ||
		lpsItem->CODE == (sinDA1 | sin54) || lpsItem->CODE == (sinDA1 | sin55) || // 박재원 - 수영복 복장 추가
		lpsItem->CODE == (sinDA2 | sin32) || lpsItem->CODE == (sinDA2 | sin34) ||
		lpsItem->CODE == (sinDA2 | sin36) || lpsItem->CODE == (sinDA2 | sin38) ||
		lpsItem->CODE == (sinDA2 | sin40) || lpsItem->CODE == (sinDA2 | sin42) ||
		lpsItem->CODE == (sinDA2 | sin44) || lpsItem->CODE == (sinDA2 | sin46) ||
		lpsItem->CODE == (sinDA2 | sin54) || lpsItem->CODE == (sinDA2 | sin55) || // 박재원 - 수영복 복장 추가
		lpsItem->CODE == (sinDB1 | sin32) || // 박재원 - 스피드 부츠(30일) 추가
		lpsItem->CODE == (sinOA2 | sin32) || // 박재원 - 슈퍼 암릿(30일) 추가
		lpsItem->CODE == (sinSP1 | sin34))  // 박재원 - 호랑이 캡슐(30일) 추가

	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 30) < NowTime)
			return true;
	}

	// 1일
	if (lpsItem->CODE == (sinDB1 | sin33) || lpsItem->CODE == (sinOA2 | sin33)) // 장별 - 스피드 부츠(1일) 추가 // 장별 - 슈퍼 암릿(1일)
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 1) < NowTime)
			return true;
	}

	// 1시간
	if (lpsItem->CODE == (sinDB1 | sin34) || lpsItem->CODE == (sinOA2 | sin34)) // 장별 - 스피드 부츠(1시간) 추가 // 장별 - 슈퍼 암릿(1시간)
	{
		if (lpsItem->dwCreateTime + (60 * 60) < NowTime)
			return true;
	}

	//작성자 : 박재원, 작성일 : 08.04.07
	//내  용 : 클랜치프 링 생성후 5일후에 삭제
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
		// 생성시간 + 5일 < 현재시간
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 5) < NowTime)
			return TRUE;
	}

	//작성자 : 박재원, 작성일 : 09.12.08
	//내  용 : 산타 링, 산타 아뮬렛 생성후 3일 후에 삭제
	if (lpsItem->CODE == (sinOR2 | sin27) || lpsItem->CODE == (sinOA1 | sin32))  // 산타 링, 산타 아뮬렛 추가
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // 박재원 : 산타링, 산타아뮬렛이 믹스쳐되면서 시간이 초기화 되어 강제로 삭제시키기 위함.
			// 생성시간 + 3일 < 현재시간
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 3) < NowTime)
			return TRUE;
	}

	//작성자 : 박재원, 작성일 : 09.07.31
	//내  용 : 이벤트 링, 이벤트 아뮬렛 생성후 7일 후에 삭제
	if (lpsItem->CODE == (sinOR2 | sin28) || lpsItem->CODE == (sinOA1 | sin33))  // 이벤트 링, 이벤트 아뮬렛 추가
	{
		// 생성시간 + 7일 < 현재시간
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return TRUE;
	}

	//작성자 : 박재원, 작성일 : 09.12.17
	//내  용 : 이벤트 링, 이벤트 아뮬렛 생성후 1시간 후에 삭제
	if (lpsItem->CODE == (sinOR2 | sin29) || lpsItem->CODE == (sinOA1 | sin34))  // 이벤트 링, 이벤트 아뮬렛 추가
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // 박재원 - 이벤트링, 이벤트아뮬렛 강제로 삭제시키기 위함.
		//		return TRUE;
		// 생성시간 + 1시간 < 현재시간
		if (lpsItem->dwCreateTime + (60 * 60) < NowTime)
			return TRUE;
	}

	//작성자 : 박재원, 작성일 : 09.12.17
	//내  용 : 이벤트 링, 이벤트 아뮬렛 생성후 1일 후에 삭제
	if (lpsItem->CODE == (sinOR2 | sin30) || lpsItem->CODE == (sinOA1 | sin35))  // 이벤트 링, 이벤트 아뮬렛 추가
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // 박재원 - 이벤트링, 이벤트아뮬렛 강제로 삭제시키기 위함.
		//		return TRUE;
		// 생성시간 + 1일 < 현재시간
		if (lpsItem->dwCreateTime + (60 * 60 * 24) < NowTime)
			return TRUE;
	}

	//작성자 : 장별, 작성일 : 10.02.02
	//내  용 : 하트링 생성후 7일 후에 삭제
	if (lpsItem->CODE == (sinOR2 | sin33))  // 하트링 추가
	{
		//	if( lpsItem->dwCreateTime+0 < NowTime ) // 박재원 - 이벤트링, 이벤트아뮬렛 강제로 삭제시키기 위함.
		//		return TRUE;
		// 생성시간 + 7일 < 현재시간
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return TRUE;
	}


	// 7일
	if (lpsItem->CODE == (sinOA1 | sin36) || lpsItem->CODE == (sinOA1 | sin37) || // 장별 - 눈꽃 아뮬렛, 하트 아뮬렛
		lpsItem->CODE == (sinOR2 | sin36) || lpsItem->CODE == (sinOR2 | sin37) || // 장별 - 소울스톤
		lpsItem->CODE == (sinOR2 | sin38) || lpsItem->CODE == (sinOR2 | sin39) ||
		lpsItem->CODE == (sinOR2 | sin40))
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 7) < NowTime)
			return TRUE;
	}

	// 장별 - 소울스톤 14일 // 장별 - 복날 이벤트 아뮬렛, 링
	if (lpsItem->CODE == (sinOA1 | sin39) ||
		lpsItem->CODE == (sinOA1 | sin40) || lpsItem->CODE == (sinOA1 | sin41) ||
		lpsItem->CODE == (sinOA1 | sin42) ||
		lpsItem->CODE == (sinOA1 | sin38) || lpsItem->CODE == (sinOR2 | sin34) || lpsItem->CODE == (sinOR2 | sin35))
	{
		if (lpsItem->dwCreateTime + (60 * 60 * 24 * 14) < NowTime)
			return TRUE;
	}



	//(지존반지 , 퍼즐등은 날짜 확인하여 제거)
	if (lpsItem->CODE != (sinOR2 | sin01) &&
		dwItemCode != sinPZ1 && dwItemCode != sinPZ2)
		return FALSE;

#ifdef _LANGUAGE_THAI_DELETE_RING
	if (lpsItem->CODE == (sinOR2 | sin01)) return TRUE;
#endif

	tm.tm_year = 2004 - 1900;	// 2004년
	tm.tm_mon = 4 - 1;			// 4월
	tm.tm_mday = 1;			// 1일
	tm.tm_hour = 15;			// 15시
	tm.tm_min = 30;				// 30분
	tm.tm_sec = 0;				// 0초
	ttm = mktime(&tm);		//제한 시간

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
				CheckBellatraFontEffect(&SoD_SetFontEffect, TRUE);	//로딩 종료
			}

			if (lpTransCommand->smTransCommand.SParam < 0)
			{
				//SoD 라운드 이펙트
				//Sod_SodEffect( lpTransCommand->smTransCommand.LParam+1 );
				sSodScore.EffectStartCount = 70 * 3;
				sSodScore.EffectCode = lpTransCommand->smTransCommand.LParam + 1;
			}
			else
			{
				if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) break;		//죽은경우 무시

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
			//실패 돌아가기
			if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) break;		//죽은경우 무시

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
			//모두 클리어
			if (lpCurPlayer->MotionInfo->State == CHRMOTION_STATE_DEAD) break;		//죽은경우 무시

			sSodScore.SodNextStageNum = lpTransCommand->smTransCommand.SParam;
			sSodScore.dwSoD_NextStageTime = dwPlayTime + 7000;
			sSodScore.NextRound = lpTransCommand->smTransCommand.LParam + 1;

			StopBGM();
			esPlayContSound(16);
			lpCurPlayer->SetMotionFromCode(CHRMOTION_STATE_YAHOO);

			//SetBellatraFontEffect( E_BL_FONT_STAGE );
			SetBellatraFontEffect(E_BL_FONT_CON);
			SoDGateFlag = FALSE;

			//획득한 금액 출력
			char szBuff[128];
			wsprintf(szBuff, mgSOD_Clear, lpTransCommand->smTransCommand.EParam);
			cMessageBox.ShowMessageEvent(szBuff);

			break;

		case 2:
			//점수 목록 받음
			if (lpTransCommand->smTransCommand.LParam)
			{
				//내점수 획득에 의한 점수 받음
				sSodScore.ScoreEffectCount = 255;

			}
			else
			{
				//주기적으로 보내지는 전체 점수


			}

			if (!sSodScore.dwSoD_NextStageTime)
			{
				if (sSodScore.Round != lpTransCommand->smTransCommand.SParam)
				{
					switch (lpTransCommand->smTransCommand.SParam)
					{
						case 1:		//1라운드 시작
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

		case smCODE_SOD_EFFECT:					//주문 시스템 추가 - SOD 이펙트 재사용
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

//포스오브 아이템 입수
int RecvForceOrbItem(TRANS_ITEMINFO_GROUP2 *lpTransItemGroup2)
{
	TRANS_ITEMINFO_GROUP	TransItemGroup;

	ZeroMemory(&TransItemGroup, sizeof(TRANS_ITEMINFO_GROUP));
	memcpy(&TransItemGroup, lpTransItemGroup2, TRANS_GROUPITEM_HEADER_SIZE);

	DecodeCompress((BYTE *)lpTransItemGroup2->szBuff, (BYTE *)TransItemGroup.sItemInfo, sizeof(sITEMINFO)*TRANS_GROUPITEM_MAX);

	//박상렬 함수 호출
	//포스 아이템을 받는다
	sinRecvForceOrb(TransItemGroup.sItemInfo, TransItemGroup.ItemCount);


	return TRUE;
}

//퀘스트 명령 보내기
int SendQuestCommandToServer(DWORD dwQuestCode, int Param1, int Param2, int Param3)
{
	Log::Debug("SendQuestCommandToServer : Requisitado por�m foi removido.");
	return NULL;
}
//대전 퀘스트 실행
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

//블래스캐슬 설정정보 서버에 보냄
int	SendBlessCastleToServer(smTRANS_BLESSCASTLE *lpBlessCastleSetup, int Mode)
{
	Log::Debug("SendBlessCastleToServer : Requisitado por�m foi removido.");
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
//블래스캐슬 설정정보 수신
int RecvBlessCastInfo(void *lpPacket)
{
	smTRANS_BLESSCASTLE *lpBlessCastleSetup = (smTRANS_BLESSCASTLE *)lpPacket;
	rsUSER_LIST_TOP10	*lpClanListTop10;

	switch (lpBlessCastleSetup->smTransCommand.WParam)
	{
		case 1:		//설정정보 수신
			chaSiege.ShowSiegeMenu(lpBlessCastleSetup);
			break;

		case 2:		//순위 정보 수신
			lpClanListTop10 = (rsUSER_LIST_TOP10 *)lpPacket;
			//순위 출력 함수 호출
			chaSiege.ShowSiegeScore(lpClanListTop10);
			break;
	}

	return TRUE;
}

//블레스 캐슬 마스터 클랜 설정
int SetBlessCastleMaster(DWORD dwClanCode, int Mode)
{

	if (!Mode && lpCurPlayer->OnStageField >= 0 && StageField[lpCurPlayer->OnStageField]->FieldCode == rsCASTLE_FIELD)
	{

	}
	else
	{
		if (rsBlessCastle.dwMasterClan != dwClanCode)
		{
			//캐슬 마스터 업데이트
			UpdateCastleMasterClan(rsBlessCastle.dwMasterClan);
		}

		rsBlessCastle.dwMasterClan = dwClanCode;
	}

	return TRUE;
}

//속성정보 서버로 전송
int	SendResistanceToServer()
{
	Log::Debug("SendResistanceToServer : Requisitado por�m foi removido.");
	return NULL;
}
//설문조사 결과 서버에 보내기
int	SendPublicPollingToServer(int PollCode, int PollCnt, BYTE *bPollingData)
{
	Log::Debug("SendPublicPollingToServer : Requisitado por�m foi removido.");
	return NULL;
}
int XTrap_Recv(smTRANS_COMMAND *lpPacket, smWINSOCK *lpsmSock)
{
	return TRUE;
}

int	HackTrap_OpenFlagMask = 0;

int HackTrap_CheckOpenFlag()
{
	Log::Debug("HackTrap_CheckOpenFlag : Requisitado por�m foi removido.");
	return NULL;
}

int	SendHackTrapToServer(DWORD dwTrapCode, int Param)
{
	Log::Debug("SendHackTrapToServer : Requisitado por�m foi removido.");
	return NULL;
}

int SendSmeltingItemToServer(void *lpsSmeltingItem_Send)
{
	Log::Debug("SendSmeltingItemToServer : Requisitado por�m foi removido.");
	return NULL;
}

int SendManufactureItemToServer(void *lpsManufactureItem_Send)
{
	Log::Debug("SendManufactureItemToServer : Requisitado por�m foi removido.");
	return NULL;
}

int SendMixtureResetItemToServer(void *lpsMixtureResetItem_Send)
{
	Log::Debug("SendMixtureResetItemToServer : Requisitado por�m foi removido.");
	return NULL;
}