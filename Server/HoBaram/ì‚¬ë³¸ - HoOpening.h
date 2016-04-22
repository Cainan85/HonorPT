#ifndef _HO_OPENING_H_
#define _HO_OPENING_H_

#define FADE_IN		0
#define FADE_OUT	1

#define ERROR_NONE				     0	   //에러 없음..
#define ERROR_CONNECT			     1	   //접속 에러...
#define ERROR_NO_ACCOUNT		     2     //계정이 없음...
#define ERROR_NO_PASSWORD		     3     //패스워드 틀림.
#define ERROR_NO_BETA_TESTER	     4     //배타 테스터가 아님..
#define ERROR_NO_ACCOUNT_IN		     5     //이미 아이디로 접속중..
#define ERROR_NO_FILTER_PASSWORD     6     //Password는 영문과 숫자 조합..
#define ERROR_NO_VERSION		     7     //Version이 맞지 않음...
#define ERROR_NO_LIMIT			     8     //서버에 인원이 많음...
#define ERROR_NO_SERVER_CONNECT      9     //서버와의 연결이 끊어 졌습니다 

#define ERROR_NO_USE_DATE_OVER	    10     //사용기간 종료 
#define ERROR_NO_USE_TIME_OVER	    11     //사용시간 종료 
#define ERROR_DELETE_ACCOUNT_NOTIFY 12     //서버 계정 삭제 요청중.

#define ERROR_SERVER_TRAFFIC_BUSY	13     //서버가 혼잡하오니 30초후에 이용해 주시기 바랍니다.
#define ERROR_SERVER_WAIT_MOMNENT	14     //잠시후 이용해 주십시요


enum {STATE_START, STATE_OPENING, STATE_LOGIN_INPUT, STATE_SERVER_SELECT, STATE_END}; //Opening의 현재 상태 플레그..
class HoOpening
{
	
//private:
public:

	smPAT3D     *PatCamera;
	POINT3D     PatCameraWorldPos;
	smOBJ3D     *ObjCamera;
	int			CameraFrame;
	
	smSTAGE3D	*Stage;				//뒷 지형

	//smPAT3D     *BirdBone;
	//smPAT3D     *Bird;
	

	POINT3D	CaAngle;
	//POINT3D	TarPosi;
	int		TarDist;
	
	//Login Dragon 창 관련
	int			GameLogo[4];
	int			TriGlowLogo;
	
	//버튼 이미지 관련..
	int			ButtonImage;
	RECT		ButtonLoginSur[2];
	RECT		ButtonQuitSur[2];
	RECT		ButtonNewJoinSur[2];
	
	//Login Server 창 관련.
	//int			MatServerSelect;
	int			LoginInputState;
	BOOL		StartQuitFlag;
	
	//Opening 작업 전이 과정...
	int			WorkState;

	int			CurrentWorkState;
	int			NextWorkState;

	//서버 접속 관련...
	BOOL		StartConnectServerFlag; //서버에 접속중...
	BOOL		ConnectServerSucessFlag; //서버에 접속 성공...
	int			ServerWaitTimeCount;
	
	
	int			ConnectError;	  //접속 중의 에러...
	int			ConnectErrorCount;
	HoMessage	Message;		  //Error Message들...
	BOOL		FocusWndFlag;

	BOOL		MouseDownFlag;

private:
	int			DrawWide();

	//FadeIn, FadeOut관련.
	int			BlackBoxImage; 
	
	BOOL		FadeCurrentState;	//0:아무것도 안보임 fade In을 해야함. 1:화면에 다보임 fade out을 해야함..
	BOOL		FadeNextState;
	
	int			ScreenBlendValue;
	int			LogoBlendValue;
	int			LoginBlendValue;
	int			ServerSelectBlendValue;

	int			MouseDownServerIndex;
	int			OpeningBgmVolume;
	
	int			MainFadeIn(); 
	int			MainFadeOut();
	int			MainFade();
	int			DrawFade();
	int			FastFade();

	HoMessageBox	MessageBox;

	//######################################################################################
	//작 성 자 : 오 영 석
	smMATRIX	m_CameraRotInvert;
	int			m_IsDrawOpening;
	int			m_AutoCameraFrameTime;
	int			m_ParticleSpawnTimeCount;
	//######################################################################################

public:
	//int			AddObject(char	*name);
	int			LoginDataServer();
	int			LoginGameServer(int serverIndex);

	int		Init();
	int		Load();
	void	Close();
	
	int		Main();
	int		Draw();
	

	//화면 연출...
	void	SetFade();
	int		KeyFilter(char *message);
	void    SetConnectError(int errorCode) { ConnectError = errorCode; }
	HoOpening();
	~HoOpening();


	//######################################################################################
	//작 성 자 : 오 영 석
	int  DebugModeLoadFieldInfo(void);
	void SetCameraCharPos(void);
	void SetParticleEffectSpawn(void);
	//######################################################################################
};

extern HWND hTextWnd;
#endif