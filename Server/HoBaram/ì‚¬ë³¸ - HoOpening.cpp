#include "..\\sinbaram\\SinLinkHeader.h"
#include "..\\HoBaram\\HoLinkHeader.h"
#include "..\\smreg.h"
#include "..\\HoBaram\\HoTextFile.h"
#include "..\\HoBaram\\HoMinMax.h"
#include "..\\HoBaram\\HoNewParticle.h"
#include "..\\HoBaram\\HoNewParticleMgr.h"

#include "..\\language.h"

//######################################################################################
//작 성 자 : 오 영 석
#include "..\\CAutoCamera.h"

static CAutoCamera		s_AutoCamera;
//######################################################################################

#ifdef _LANGUAGE_KOREAN
char *szJoinWebUrl="http://www.pristontale.co.kr";
#endif

#ifdef _LANGUAGE_CHINESE
char *szJoinWebUrl="http://pt.163.com";
#endif

#ifdef _LANGUAGE_JAPANESE
char *szJoinWebUrl="http://www.pristontale.jp";
#endif

#ifdef	_LANGUAGE_TAIWAN
char *szJoinWebUrl="http://www.pristontale.com.tw";
#endif

#ifdef	_LANGUAGE_ENGLISH
char *szJoinWebUrl="http://www.pristontale.com";
#endif


char	UserAccount[256];
char	UserPassword[256];
HoOpening::HoOpening()
{
	//Bird = NULL;
	//BirdBone = NULL;
	Stage = NULL;
	PatCamera = NULL;

	LogoBlendValue = 0;
	LoginBlendValue = 0;
	
	Load();
	StartConnectServerFlag = FALSE;
	WorkState = STATE_OPENING;
	
	MidY = (600/2);
	SetWindowText(hTextWnd, NULL);
	//ID, 비밀번호 Buffer초기화..
	memset(UserAccount, 0, sizeof(UserAccount));
	memset(UserPassword, 0, sizeof(UserPassword));
	
	TarDist = 1600*fONE;
	CaAngle.x = 220;
	CaAngle.y = 0;
	CaAngle.z = 0;

	//TarPosi.x = 0;
	//TarPosi.y = 0;
	//TarPosi.z = 0;
	MouseDownServerIndex = -1;
	CameraFrame = 0;
}


HoOpening::~HoOpening()
{
	Close();
}

int HoOpening::Init()
{
	GameErrorValue = -1;
	ConnectErrorCount = 0;
	FocusWndFlag = FALSE;
	SetWindowText(hTextWnd, "");
	SendMessage( hTextWnd , EM_SETLIMITTEXT , 31, 0 );
	
	LoginInputState = 0;
	ConnectError = ERROR_NONE;
	ServerWaitTimeCount = 0;
	StartQuitFlag = FALSE;
	
	WorkState = STATE_OPENING;
	StartConnectServerFlag = FALSE;
	ConnectServerSucessFlag = FALSE;

	FadeCurrentState = FADE_OUT;
	FadeNextState = FADE_OUT;
	
	
	LogoBlendValue = 0;
	LoginBlendValue = 0;
	ServerSelectBlendValue = 0;
	
	MidX = WinSizeX / 2;
	MidY = WinSizeY	/ 2;
	//smRender.Init();
	smRender.SMMULT_PERSPECTIVE_HEIGHT = RENDCLIP_WIDE_MULT_PERSPECTIVE_HEIGHT;

	SetWindowText(hTextWnd, NULL);
	//ID, 비밀번호 Buffer초기화..
	memset(UserAccount, 0, sizeof(UserAccount));
	memset(UserPassword, 0, sizeof(UserPassword));
	
	TarDist = 400*fONE;
	
    CaAngle.x = 100;
    CaAngle.y = 0;
	CaAngle.z = 0;
	
	SetFade();
	
	smInitColorZClip(0, 0, 0);
		
	smRender.Color_R = 40;
	smRender.Color_G = 0;
	smRender.Color_B = -30;
	smRender.Color_A = 0;
	smRender.ClearLight();
	
	StopBGM();
	OpenBGM("StartImage\\Opening\\Intro.wav");
	PlayBGM();
	SetVolumeBGM(OpeningBgmVolume);
	
#ifndef _LANGUAGE_KOREAN
	char *buffer;
	buffer = GetRegString( HKEY_LOCAL_MACHINE , "SOFTWARE\\Triglow Pictures\\PristonTale", "Account" );
    
	if(buffer != NULL)
	{
		memset(UserAccount, 0, sizeof(UserAccount));
		memcpy(UserAccount, buffer, strlen(buffer));
		LoginInputState = 1;
	}
#endif
	
	CurrentWorkState = STATE_OPENING;
	NextWorkState    = STATE_OPENING;
	MouseDownServerIndex = -1;
	MouseDownFlag = FALSE;
	
	CameraFrame = 160;

	/*
	ChangeSky(0x02);
	ChangeSky(0x03);

	POINT3D pos;
	pos.x = 0;
	//pos.y = fONE*300;
	//pos.z = fONE*1000;
	pos.y = 0;
	pos.z = 0;
	int partNum = g_NewParticleMgr.Start("Opening", pos);
	*/

	//######################################################################################
	//작 성 자 : 오 영 석	
	ChangeSky(0x21);
	ChangeSky(0x23);

	g_NewParticleMgr.Init();
	InitAssaEffect();

	smIdentityMatrix( m_CameraRotInvert );

	m_IsDrawOpening		     = TRUE;
	m_AutoCameraFrameTime	 = 50;
	m_ParticleSpawnTimeCount = 0;

	s_AutoCamera.m_Frame = 0;
	//######################################################################################
	
	return TRUE;
}


//######################################################################################
//작 성 자 : 오 영 석
void HoOpening::SetCameraCharPos(void)
{
	int x = 0;
	int y = 0;
	int z = (440 << FLOATNS);

	smEMATRIX eCameraRotInvert;	
	smEMatrixInvert( eCameraRotInvert, s_AutoCamera.m_eCameraRotMat );

	memcpy( (smEMATRIX *)&m_CameraRotInvert, &eCameraRotInvert, sizeof(smEMATRIX) );
	smMatrixShift( m_CameraRotInvert, 4 );

	int rx = x * m_CameraRotInvert._11 +
			 y * m_CameraRotInvert._21 +
			 z * m_CameraRotInvert._31 ;

	int ry = x * m_CameraRotInvert._12 +
			 y * m_CameraRotInvert._22 +
			 z * m_CameraRotInvert._32 ;

	int rz = x * m_CameraRotInvert._13 +
		     y * m_CameraRotInvert._23 +
			 z * m_CameraRotInvert._33 ;

	rx >>= FLOATS_EM;
	ry >>= FLOATS_EM;
	rz >>= FLOATS_EM;

	lpCurPlayer->pX = int(s_AutoCamera.m_fCameraPos_x) + rx;
	lpCurPlayer->pY = int(s_AutoCamera.m_fCameraPos_y) + ry;
	lpCurPlayer->pZ = int(s_AutoCamera.m_fCameraPos_z) + rz;
}
//######################################################################################


//######################################################################################
//작 성 자 : 오 영 석
void HoOpening::SetParticleEffectSpawn(void)
{
	m_ParticleSpawnTimeCount++;
	if( (m_ParticleSpawnTimeCount % 70) != 0 )
		return;

	m_ParticleSpawnTimeCount = 0;

	//
	POINT3D curPos;
	POINT3D desPos;
	int		Type   = (rand() % 2);
	int		rand_1 = (rand() % 1000) << 8;
	int		rand_2 = (rand() % 1000) << 8;
	int		rand_3 = (rand() % 1000) << 8;
	int		rand_4 = (rand() % 1000) << 8;
	
	//
	if( Type )
	{
		curPos.x = -rand_1;
		desPos.x =  rand_2;
	}
	else
	{
		curPos.x =  rand_1;
		desPos.x = -rand_2;
	}
	
	//
	curPos.y =  200000;
	desPos.y = -200000;

	//
	if( Type )
	{
		curPos.z =  rand_3;
		desPos.z = -rand_4;
	}
	else
	{
		curPos.z = -rand_3;
		desPos.z =  rand_4;
	}

	//
	curPos.x = lpCurPlayer->pX + curPos.x;
	curPos.y = lpCurPlayer->pY + curPos.y;
	curPos.z = lpCurPlayer->pZ + curPos.z;

	//
	desPos.x = lpCurPlayer->pX + desPos.x;
	desPos.y = lpCurPlayer->pY + desPos.y;
	desPos.z = lpCurPlayer->pZ + desPos.z;

	//
	AssaParticle_TerrainFire(&curPos, &desPos);
}
//######################################################################################


//######################################################################################
//작 성 자 : 오 영 석
int HoOpening::DebugModeLoadFieldInfo(void)
{
	FILE *fp = fopen( "StartImage\\Opening\\OpeningField.txt", "rb" );
	if( fp == NULL )
		return FALSE;

	Stage = NULL;

	//
	char *p, line[256], decode[256], strBuff[256];
	while( !feof( fp ) )//  feof: file end까지 읽어라 
	{
		if( fgets( line, 255, fp ) == NULL)
		{
			fclose(fp);
			break;
		}

		p = GetWord( decode, line);

		//
		if( lstrcmp( decode, "*필드명") == 0 )
		{
			p = GetWord( strBuff, p );
			Stage = smSTAGE3D_ReadASE( strBuff );
			if( Stage )
                Stage->Head = TRUE;
		}
		else if( lstrcmp( decode, "*카메라명") == 0 )
		{
			p = GetWord( strBuff, p );
			s_AutoCamera.ReadASE_AutoCamera( strBuff );
		}
		else if( lstrcmp( decode, "*카메라속도") == 0 )
		{
			p = GetWord( strBuff, p );
			m_AutoCameraFrameTime = atoi( strBuff );
		}
		else if( lstrcmp( decode, "*워프프레임") == 0 )
		{
			p = GetWord( strBuff, p );
			int StFrame = atoi( strBuff );

			p = GetWord( strBuff, p );
			int EndFrame = atoi( strBuff );

			s_AutoCamera.SetWarpFrame( StFrame, EndFrame );
		}
		else if( lstrcmp( decode, "*지형물체") == 0 )
		{
			p = GetWord( strBuff, p );				
			Stage->StageObject->AddObjectFile( strBuff );
		}
		else if( lstrcmp( decode, "*지형애니") == 0 )
		{
			p = GetWord( strBuff, p );
			Stage->StageObject->AddObjectFile( strBuff, strBuff );
		}
	}

	fclose(fp);

	if( Stage == NULL )
		return FALSE;

	return TRUE;
}
//######################################################################################


int HoOpening::Load()
{
	//######################################################################################
	//작 성 자 : 오 영 석
	int IsLoad = FALSE;
	PatCamera = NULL;
	ObjCamera = NULL;

	if( smConfig.DebugMode ) 
		IsLoad = DebugModeLoadFieldInfo();

	if( IsLoad == FALSE )
	{
		Stage = smSTAGE3D_ReadASE( "StartImage\\Opening\\map\\stage.ASE" );
		s_AutoCamera.ReadASE_AutoCamera( "StartImage\\Opening\\opening_camera.ASE" );

		if(Stage)
		{
			Stage->Head = TRUE;			
		}
	}

	if( Stage == NULL )
		return FALSE;

	smGameStage[0] = Stage;
	smGameStage[1] = NULL;
	//######################################################################################
	
	GameLogo[0]     = CreateTextureMaterial( "StartImage\\Opening\\GameLogo1.tga"  , 0, 0, 0,0, SMMAT_BLEND_ALPHA );//Test
	GameLogo[1]     = CreateTextureMaterial( "StartImage\\Opening\\GameLogo2.tga"  , 0, 0, 0,0, SMMAT_BLEND_ALPHA );//Test
	GameLogo[2]     = CreateTextureMaterial( "StartImage\\Opening\\GameLogo3.tga"  , 0, 0, 0,0, SMMAT_BLEND_ALPHA );//Test
	GameLogo[3]     = CreateTextureMaterial( "StartImage\\Opening\\GameLogo4.tga"  , 0, 0, 0,0, SMMAT_BLEND_ALPHA );//Test
	
	TriGlowLogo     = CreateTextureMaterial( "StartImage\\Opening\\TriGlowLogo.tga"  , 0, 0, 0,0, SMMAT_BLEND_ALPHA );//Test
	ButtonImage     = CreateTextureMaterial( "StartImage\\Opening\\GameLogo.tga"  , 0, 0, 0,0, SMMAT_BLEND_ALPHA );//Test

	ButtonLoginSur[0].left   =  0;
	ButtonLoginSur[0].top    =  0;
	ButtonLoginSur[0].right  = 60;
	ButtonLoginSur[0].bottom = 60;

	ButtonLoginSur[1].left   =   0;
	ButtonLoginSur[1].top    =  60;
	ButtonLoginSur[1].right  =  60;
	ButtonLoginSur[1].bottom =  60;
	
	
    
	ButtonNewJoinSur[0].left   = 120;
	ButtonNewJoinSur[0].top    =  0;
	ButtonNewJoinSur[0].right  = 60;
	ButtonNewJoinSur[0].bottom = 60;

	ButtonNewJoinSur[1].left   =  120;
	ButtonNewJoinSur[1].top    =  60;
	ButtonNewJoinSur[1].right  =  60;
	ButtonNewJoinSur[1].bottom =  60;

	ButtonQuitSur[0].left   = 60;
	ButtonQuitSur[0].top    =  0;
	ButtonQuitSur[0].right  = 60;
	ButtonQuitSur[0].bottom = 60;

	ButtonQuitSur[1].left   =  60;
	ButtonQuitSur[1].top    =  60;
	ButtonQuitSur[1].right  =  60;
	ButtonQuitSur[1].bottom =  60;
	
	BlackBoxImage = CreateTextureMaterial( "StartImage\\Login\\BlackBox.tga" ,0 , TRUE ,FALSE , FALSE , SMMAT_BLEND_ALPHA );
	Message.Init();
	MessageBox.Init();
	
	ReadTextures();
		
	if(Stage)
	{
		Stage->smMaterialGroup->CheckMatreialTextureSwap();
	}	

	return TRUE; 
}

int HoOpening::DrawWide()
{
	dsDrawTexImage(BlackBoxImage,0, 0, 800, 62, 255);
	dsDrawTexImage(BlackBoxImage,0, 600-62, 800, 600, 255);
	return TRUE;
}

int		CaretCount = 0;
DWORD	End = 0;
extern int Client_Version;

#define	DISP_LOGIN_LEN		15




int HoOpening::Draw()
{
	FastFade();
	//CaAngle.y += 1;
	//Event 처리...
	int x,y,z;
	int anx,any;

	//######################################################################################
	//작 성 자 : 오 영 석
	if( GetFilterEffect() == FILTER_EFFECT_SET_BRIGHT_CONTRAST )
	{
		BeginRender();
        ChangeRenderTarget( NEW_TARGET_BACK );
	}
	//######################################################################################

	BeginRender();
	

	//######################################################################################
	//작 성 자 : 오 영 석

	//Terrain
	//anx = CaAngle.x;
	//any = CaAngle.y;
	//any &=ANGCLIP;
	//anx &=ANGCLIP;
 	//GetMoveLocation( 0 , 0, -TarDist, anx, any, 0 );
	//x = PatCameraWorldPos.x + GeoResult_X;
	//y = PatCameraWorldPos.y + GeoResult_Y;
	//z = PatCameraWorldPos.z + GeoResult_Z;
	
	//DrawOpeningSky(x, y, z, anx, any, 0);
   	//Stage->DrawStage( x,y,z, anx, any, 0);
	
	//lpD3DDevice->BeginScene();
	//	g_NewParticleMgr.Draw(0, 0, 0, anx, any, 0);
	//lpD3DDevice->EndScene();

	int anz;

	x = int(s_AutoCamera.m_fCameraPos_x);
	y = int(s_AutoCamera.m_fCameraPos_y);
	z = int(s_AutoCamera.m_fCameraPos_z);

	anx = s_AutoCamera.m_iCameraRot_x;
	any = s_AutoCamera.m_iCameraRot_y;
	anz = s_AutoCamera.m_iCameraRot_z;

	smRender.OpenCameraPosi( x, y, z, &s_AutoCamera.m_eCameraRotMat );
	DrawOpeningSky(x, y, z, anx, any, anz);

	if( m_IsDrawOpening )
	{	
		Stage->DrawOpeningStage( x,y,z, anx, any, anz, s_AutoCamera.m_Frame );

		lpD3DDevice->BeginScene();
		g_NewParticleMgr.Draw(x,y,z, anx, any, anz);
		lpD3DDevice->EndScene();
	}

	smRender.CloseCameraPosi();
	//######################################################################################

	
	//######################################################################################
	//작 성 자 : 오 영 석
	if( GetFilterEffect() == FILTER_EFFECT_SET_BRIGHT_CONTRAST && GetRenderTarget() == NEW_TARGET_BACK )
	{
		SetNewTargetTextureState();
		lpD3DDevice->BeginScene();
		RenderFilterEffect();
		lpD3DDevice->EndScene();
		DefaultsNewTargetTextureState();
	}
	//######################################################################################

	//텍스쳐 프레임 증가
	IncTextureFrame();
	//EndRender();
	
	int startLogo = 150;
	if(NextWorkState == STATE_LOGIN_INPUT || 
	   NextWorkState == STATE_SERVER_SELECT ||
	   NextWorkState == STATE_END)
	{
		//Logo 찍히는 위치등등...
		//int startLogo = 150;
		int sizeLogo  = 256;
		int sizeLogin = 256;
		
		lpD3DDevice->BeginScene();
		//GameLogo찍기.
		dsDrawTexImage( GameLogo[0], startLogo,		       70, sizeLogo, sizeLogo, LogoBlendValue);
		dsDrawTexImage( GameLogo[1], startLogo+sizeLogo*1, 70, sizeLogo, sizeLogo, LogoBlendValue); 
		lpD3DDevice->EndScene();
	}
	
	if( NextWorkState == STATE_LOGIN_INPUT || CurrentWorkState == STATE_LOGIN_INPUT || 
	   (NextWorkState == STATE_SERVER_SELECT && CurrentWorkState != STATE_SERVER_SELECT))
	{
		//int startLogo = 150;
		int sizeLogo  = 128;
		int sizeLogin = 256;
		
 	 	lpD3DDevice->BeginScene();
    	
		dsDrawTexImage( GameLogo[2], startLogo+128,		    70+256, 256, 256, LogoBlendValue);
     	HoDrawTexImage( GameLogo[3], float(startLogo+208), float(70+283), 128.f, 128.f-50.f, 0, 0, 128.f, 128.f-50.f, LogoBlendValue);
 		dsDrawTexImage( TriGlowLogo, 800-113, 600-62-48, 128, 64, LoginBlendValue);
		
		
	 	if(pCursorPos.x > 321 && pCursorPos.y > 438 && pCursorPos.x < 322+ButtonLoginSur[0].right && pCursorPos.y < 438+ButtonLoginSur[0].bottom)
		{
			HoDrawTexImage( ButtonImage, float(321), float(439), float(ButtonLoginSur[1].right), float(ButtonLoginSur[1].bottom), 
        			float(ButtonLoginSur[1].left), float(ButtonLoginSur[1].top), float(ButtonLoginSur[1].right), float(ButtonLoginSur[1].bottom), LogoBlendValue);
		}
		else
		{
			HoDrawTexImage( ButtonImage, float(321), float(439), float(ButtonLoginSur[0].right), float(ButtonLoginSur[0].bottom), 
        			float(ButtonLoginSur[0].left), float(ButtonLoginSur[0].top), float(ButtonLoginSur[0].right), float(ButtonLoginSur[0].bottom), LogoBlendValue);
		}
        
		if(pCursorPos.x > (321+60) && pCursorPos.y > 438 && pCursorPos.x < 322+60+ButtonQuitSur[0].right && pCursorPos.y < 438+ButtonQuitSur[0].bottom)
		{
			HoDrawTexImage( ButtonImage, float(321+60), float(439), float(ButtonQuitSur[1].right), float(ButtonQuitSur[1].bottom), 
        			float(ButtonQuitSur[1].left), float(ButtonQuitSur[1].top), float(ButtonQuitSur[1].right), float(ButtonQuitSur[1].bottom), LogoBlendValue);
		}
		else
		{
			HoDrawTexImage( ButtonImage, float(321+60), float(439), float(ButtonQuitSur[0].right), float(ButtonQuitSur[0].bottom), 
        			float(ButtonQuitSur[0].left), float(ButtonQuitSur[0].top), float(ButtonQuitSur[0].right), float(ButtonQuitSur[0].bottom), LogoBlendValue);
		}
		

		if(pCursorPos.x > (321+120) && pCursorPos.y > 438 && pCursorPos.x < 322+120+ButtonNewJoinSur[0].right && pCursorPos.y < 438+ButtonNewJoinSur[0].bottom)
		{
			HoDrawTexImage( ButtonImage, float(321+120), float(439), (float)ButtonNewJoinSur[1].right, (float)ButtonNewJoinSur[1].bottom, 
        			(float)ButtonNewJoinSur[1].left, (float)ButtonNewJoinSur[1].top, (float)ButtonNewJoinSur[1].right, (float)ButtonNewJoinSur[1].bottom, LogoBlendValue);
		}
		else
		{
			HoDrawTexImage( ButtonImage, float(321+120), float(439), float(ButtonNewJoinSur[0].right), float(ButtonNewJoinSur[0].bottom), 
        			float(ButtonNewJoinSur[0].left), float(ButtonNewJoinSur[0].top), float(ButtonNewJoinSur[0].right), float(ButtonNewJoinSur[0].bottom), LogoBlendValue);
		}
		lpD3DDevice->EndScene();
	}

	//서버 선택시...
	if(NextWorkState == STATE_SERVER_SELECT || NextWorkState == STATE_END
	   || (CurrentWorkState == STATE_SERVER_SELECT && NextWorkState == STATE_LOGIN_INPUT) )
	{
   		lpD3DDevice->BeginScene();
		dsDrawTexImage( GameLogo[2], startLogo+128,		    70+256, 256, 256, LogoBlendValue);
     	HoDrawTexImage( GameLogo[3], float(startLogo+224), float(70+251), 128.f, 50.f, 0.f, 128.f-50.f, 128.f, 50.f, LogoBlendValue);
		
		if(pCursorPos.x > (321+60) && pCursorPos.y > 438 && pCursorPos.x < 322+60+ButtonQuitSur[0].right && pCursorPos.y < 438+ButtonQuitSur[0].bottom)
		{
			HoDrawTexImage( ButtonImage, float(321+60), float(439), float(ButtonQuitSur[1].right), float(ButtonQuitSur[1].bottom), 
        			float(ButtonQuitSur[1].left), float(ButtonQuitSur[1].top), float(ButtonQuitSur[1].right), float(ButtonQuitSur[1].bottom), LogoBlendValue);
		}
		else
		{
			HoDrawTexImage( ButtonImage, float(321+60), float(439), float(ButtonQuitSur[0].right), float(ButtonQuitSur[0].bottom), 
        			float(ButtonQuitSur[0].left), float(ButtonQuitSur[0].top), float(ButtonQuitSur[0].right), float(ButtonQuitSur[0].bottom), LogoBlendValue);
		}
		lpD3DDevice->EndScene();

		
		//서버 정보 표시..
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		HDC opHdc;
		lpDDSBack->GetDC( &opHdc );
			
		SetBkMode( opHdc, TRANSPARENT );
		
		HFONT oldFont;
		if(hFont != NULL)
			oldFont = (HFONT)SelectObject( opHdc , hFont );
		
		int selectHeight;
		int selectStartPosX;
		int selectStartPosY;
		
		if(UserServerListInfoCount != 0)
			selectHeight = 90/UserServerListInfoCount;
			//selectHeight = 20;
  		selectStartPosX = 343;
 		selectStartPosY = 342+(selectHeight-12)/2; //원래 시작 위치는 330
		
		for(int index = 0; index < UserServerListInfoCount; index++)
		{
			sprintf(buffer, "%s", UserServerListInfo[index].szServerName);
			POINT startServerText;

			if(MouseDownServerIndex != index)
			{
				startServerText.x = selectStartPosX+65-(strlen(buffer)*6/2);
				startServerText.y = selectStartPosY+index*selectHeight;//344+index*(14+4);
			}
			else
			{
				startServerText.x = selectStartPosX+65-(strlen(buffer)*6/2);
				startServerText.y = selectStartPosY+index*selectHeight;//344+index*(14+4);
			}

			//그림자 찍기..
			SetTextColor( opHdc, RGB(0, 0, 0));
			TextOut(opHdc, startServerText.x+1, startServerText.y+1, buffer, strlen(buffer));
			
			if(MouseDownServerIndex == -1)
			{
				if( pCursorPos.x > 330        && (pCursorPos.y > selectStartPosY+index*selectHeight) &&
					pCursorPos.x < (330+140)  && (pCursorPos.y < (selectStartPosY+index*selectHeight+12)) &&
					ConnectError == ERROR_NONE && !StartConnectServerFlag)
					SetTextColor( opHdc, RGB(250, 200, 0) );
				else
					SetTextColor( opHdc, RGB(190, 190, 190) );
			}
			else
			{
				if(MouseDownServerIndex == index)
					SetTextColor( opHdc, RGB(250, 200, 0));
				else
					SetTextColor( opHdc, RGB(190, 190, 190));
			}
			TextOut(opHdc, startServerText.x, startServerText.y, buffer, strlen(buffer));
		}
		if(oldFont)
			SelectObject(opHdc, oldFont);
		lpDDSBack->ReleaseDC( opHdc );
	}
	
	
	

	lpD3DDevice->BeginScene();
	DrawFade();
	//와이드로 보일때 뿌옇게 보이는 부분 없애기...
	//DrawWide();
	lpD3DDevice->EndScene();
	
	//버전 표시...
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	HDC opHdc;
	lpDDSBack->GetDC( &opHdc );
		
	SetBkMode( opHdc, TRANSPARENT );
	SetTextColor( opHdc, RGB(255, 255, 255) );
	if(hFont != NULL)
	{
		HFONT oldFont;
		oldFont = (HFONT)SelectObject( opHdc , hFont );
		
		if ( Client_Version<1000 ) {
			float Version = (float)Client_Version/100;
			sprintf(buffer, "%s%2.2f%s", "Ver: ",Version,"b");
		}
		else {
			int v1 = Client_Version/1000;
			int v2 = (Client_Version/10)%100;
			int v3 = Client_Version%10;
			wsprintf(buffer, "%s%d.%d.%d", "Ver: ",v1,v2,v3 );
		}

		TextOut(opHdc, 800-74, 600-62-48+50, buffer, strlen(buffer));
		
		if(oldFont)
			SelectObject(opHdc, oldFont);
	}
	lpDDSBack->ReleaseDC( opHdc );

	if(CurrentWorkState == STATE_LOGIN_INPUT && NextWorkState == STATE_LOGIN_INPUT
		&&ScreenBlendValue <= 100)
	{
		//Login 계정과 암호 입력...
		if(LoginBlendValue >= 255)
		{
			if(FocusWndFlag == FALSE)
			{
				FocusWndFlag = TRUE;
				hFocusWnd = hTextWnd;
			}
			
			if(StartConnectServerFlag == FALSE && ConnectError == ERROR_NONE)
			{
				if(VRKeyBuff[VK_TAB])
				{
					if(LoginInputState == 0)
					{
						LoginInputState = 1;
						SetWindowText(hFocusWnd, UserPassword);
						SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserPassword), lstrlen(UserPassword)+1);
					}
					else if(LoginInputState == 1)
					{
						LoginInputState = 0;
						SetWindowText(hFocusWnd, UserAccount);
						SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserAccount), lstrlen(UserAccount)+1);
					}
				}
				
 				if(pCursorPos.x > 368 && pCursorPos.x < 463 && pCursorPos.y > 369 && pCursorPos.y < 383 && MouseButton[0])
				{
					if(LoginInputState == 1)
					{
						LoginInputState = 0;
						SetWindowText(hFocusWnd, UserAccount);
						SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserAccount), lstrlen(UserAccount)+1);
					}
				}
			
				if(pCursorPos.x > 366 && pCursorPos.x < 463 && pCursorPos.y > 410 && pCursorPos.y < 425 && MouseButton[0])
				{
					if(LoginInputState == 0)
					{
						LoginInputState = 1;
						SetWindowText(hFocusWnd, UserPassword);
						SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserPassword), lstrlen(UserPassword)+1);
					}
				}
			}
			
			if(hFocusWnd && ConnectError == ERROR_NONE && StartConnectServerFlag==FALSE)
			{
				if(LoginInputState == 0) //Account 입력 상태..
				{
					GetWindowText(hFocusWnd, UserAccount, sizeof(UserAccount));
					KeyFilter(UserAccount);
					
				}	
				else if(LoginInputState == 1) //Password 입력 상태..
				{
					GetWindowText(hFocusWnd, UserPassword, sizeof(UserPassword));
					KeyFilter(UserPassword);
				}
			}
			
			if(hFocusWnd)
				SendMessage(hFocusWnd, EM_GETSEL, NULL, (LPARAM)&End);
			
			HDC opHdc;
			HFONT oldFont;
			lpDDSBack->GetDC( &opHdc );
			
			SetBkMode( opHdc, TRANSPARENT );
			SetTextColor( opHdc, RGB(255, 255, 255) );
			if(hFont != NULL)
				oldFont = (HFONT)SelectObject( opHdc , hFont );
			
			if(hFocusWnd)
			{
 				if( CaretCount < 16)
				{
					if ( End>DISP_LOGIN_LEN ) End=DISP_LOGIN_LEN;

 					if(LoginInputState == 0)//Account 입력시..
						TextOut(opHdc, 600/2+60+End*6,180+191, "|", 1);
					else if(LoginInputState == 1)//Password 입력시..
						TextOut(opHdc, 600/2+60+End*6,180+230, "|", 1);
				}

				CaretCount++;
				if(CaretCount > 32)
					CaretCount = 0;
			}			
			
			int sp,len;

			sp = 0;
  			len = lstrlen(UserAccount);
			if ( len>DISP_LOGIN_LEN ) {
 				sp = len-DISP_LOGIN_LEN;
				len -= sp;
			}
  			TextOut(opHdc , 600/2+61+2, 180+191, UserAccount+sp, len);
			len = lstrlen(UserPassword);
			if ( len>DISP_LOGIN_LEN ) len=DISP_LOGIN_LEN;

			for(int count = 0; count < len; count++)
				TextOut(opHdc, 600/2+62+count*6, 180+230, "*", 1);
			
			if(oldFont)
			SelectObject(opHdc, oldFont);
			lpDDSBack->ReleaseDC( opHdc );
		}
	}
	
	if(StartConnectServerFlag && ConnectError == ERROR_NONE)
		DrawMessage(310, 300, HoTextLoginMessage[0]);//"      서버에 접속중....    ");
	
	if(ConnectError == ERROR_CONNECT)
	{
		StartConnectServerFlag = FALSE;
		DrawMessage(310, 300, HoTextLoginMessage[1]);//"      서버 접속 불가       ");
	}
	else if(ConnectError == ERROR_NO_ACCOUNT)
		DrawMessage(310, 300, HoTextLoginMessage[2]);//"   계정을 다시 입력하세요  ");
	else if(ConnectError == ERROR_NO_PASSWORD)
		DrawMessage(310, 300, HoTextLoginMessage[3]);//"비밀 번호를 다시 입력하세요");
   	else if(ConnectError == ERROR_NO_BETA_TESTER)
		DrawMessage(220, 300, HoTextLoginMessage[4]);//"  해당 아이디는 현재 사용권한이 없으니 고객센터로 문의 바랍니다.  ");
	else if(ConnectError == ERROR_NO_ACCOUNT_IN)
		DrawMessage(310, 300, HoTextLoginMessage[5]);//"   아이디가 접속중입니다   ");
	else if(ConnectError == ERROR_NO_VERSION)
		DrawMessage(310, 300, HoTextLoginMessage[6]);//"  Version이 맞지 않습니다  ");
	else if(ConnectError == ERROR_NO_LIMIT)
		DrawMessage(310, 300, HoTextLoginMessage[7]);//"   서버에 인원이 많습니다  ");
	else if(ConnectError == ERROR_NO_SERVER_CONNECT)
		DrawMessage(290, 300, HoTextLoginMessage[8]);//"서버와의 연결이 끊어 졌습니다");
	else if(ConnectError == ERROR_NO_USE_DATE_OVER)
		DrawMessage(310, 300, HoTextLoginMessage[9]);//"  사용기간이 종료되었습니다 "
	else if(ConnectError == ERROR_NO_USE_TIME_OVER)
		DrawMessage(310, 300, HoTextLoginMessage[10]);   //"  사용시간이 종료되었습니다 "
	else if(ConnectError == ERROR_DELETE_ACCOUNT_NOTIFY) //" 서버계정 삭제 요청중입니다 "
		DrawMessage(310, 300, HoTextLoginMessage[11]);
	else if(ConnectError == ERROR_SERVER_TRAFFIC_BUSY)   //서버폭주!!! 잠시후 이용 바랍니다.
     	DrawMessage(290, 300, HoTextLoginMessage[12]);
	else if(ConnectError == ERROR_SERVER_WAIT_MOMNENT)   //잠시후 이용해 주십시요
		DrawMessage(310, 300, HoTextLoginMessage[13]);
	return TRUE;
}


HANDLE	hHoLoginThread =0;

DWORD WINAPI HoLoginThreadProc( void *pInfo )
{
	HoOpening	*lpHoOpening = (HoOpening *)pInfo;
	
	//서버와의 접속을 시도...
	if(!smWsockDataServer) {
		ConnectServer_InfoMain();
	
		if(smWsockDataServer)
		{
			ServerUserCharacterInfo = NULL;
			DownServerListInfo = NULL;
			ServerLoginError = NULL;
			lpHoOpening->ServerWaitTimeCount = 0;
			
			if(HoCommandLineFlag)
			{
				memset(UserAccount, 0, sizeof(UserAccount));
				memset(UserPassword, 0, sizeof(UserPassword));

				memcpy(UserAccount,  HoCommandInfo.Id,   strlen(HoCommandInfo.Id));
				memcpy(UserPassword, HoCommandInfo.Pass, strlen(HoCommandInfo.Pass));
			}
			TransUserCommand(smTRANSCODE_ID_GETUSERINFO, UserAccount,UserPassword); //User의 정보를 알아 오기...
		}
		else {
			lpHoOpening->ConnectError = ERROR_CONNECT;
			lpHoOpening->StartConnectServerFlag = FALSE;
		}
	}

	hHoLoginThread = 0;

	ExitThread( TRUE );
	return TRUE;
}

int HoOpening::LoginDataServer()
{
	DWORD dwHoLoginThreadId;

	if ( !hHoLoginThread ) {
		hHoLoginThread = CreateThread( NULL , 0, HoLoginThreadProc , this , 0, &dwHoLoginThreadId );

		PlayWav2(13-1);
		StartConnectServerFlag = TRUE;
		return TRUE;
	}
	return FALSE;
}

int HoOpening::LoginGameServer(int serverIndex)
{
	
	PlayWav2(13-1);
	DisconnectServer_GameMain();
	
	//이부분이 받은 서버로 접속 하는 부분이다...
	if(ConnectServer_GameMain( UserServerListInfo[serverIndex].szServerIp1, 
							   UserServerListInfo[serverIndex].dwServerPort1, 
							   UserServerListInfo[serverIndex].szServerIp2, 
							   UserServerListInfo[serverIndex].dwServerPort2,
							   UserServerListInfo[serverIndex].szServerIp3,
							   UserServerListInfo[serverIndex].dwServerPort3))
	{
		StartConnectServerFlag = TRUE;
	}
	else
		ConnectError = ERROR_CONNECT;
	
	return TRUE;
}

//KeyFilter를 한다...
int HoOpening::KeyFilter(char *message)
{
	if(message == NULL || hFocusWnd == NULL)
		return FALSE;

	//if(lstrlen(message) >= 17 && message[16] != '\t') {
	//	message[16] = 0;
	if(lstrlen(message) >= 31 && message[30] != '\t')
	{
		message[30] = 0;
		SetWindowText(hFocusWnd, 0);
		SetWindowText(hFocusWnd, message);
		SendMessage(hFocusWnd, EM_SETSEL, lstrlen(message)+1, lstrlen(message)+1);
	}
	
	/*
	//Edit에서 Left를 안먹게 한다.
	int leftCaret;
	SendMessage(hFocusWnd, EM_GETSEL, (WPARAM)&leftCaret, NULL);
	if(LoginInputState == 0)
	{
		if(leftCaret < (int)lstrlen(UserAccount))
			SendMessage(hFocusWnd, EM_SETSEL, lstrlen(UserAccount), lstrlen(UserAccount)+1);
	}
	else if(LoginInputState == 1)
	{
		if(leftCaret < (int)lstrlen(UserPassword))
			SendMessage(hFocusWnd, EM_SETSEL, lstrlen(UserPassword), lstrlen(UserPassword)+1);
	}
	*/

	//안쓰는 문자는 체크한다.(Filter)
	for(int index = 0; index < (int)strlen(message); index++)
	{
		if(message[index] =='\t') //Tab키를 눌렀을 경우.
		{
			message[index] = 0;
			SetWindowText(hFocusWnd, 0);
			
			if(LoginInputState == 0)
			{
				LoginInputState = 1;
				SetWindowText(hFocusWnd, UserPassword);
				SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserPassword)+1, lstrlen(UserPassword)+1);
			}
			else if(LoginInputState == 1)
			{
				LoginInputState = 0;
				SetWindowText(hFocusWnd, UserAccount);
				SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserAccount)+1, lstrlen(UserAccount)+1);
			}
		}
		
		
		if(message[index] == ' ')
		{
			message[index] = 0;
			SetWindowText(hFocusWnd, message);
			SendMessage(hTextWnd, EM_SETSEL, lstrlen(message)+1, lstrlen(message)+1);
			break;
		}
		
/*
		if(LoginInputState == 1) //암호 입력중..
		{
			//영문자하고 숫자만 받는다.
			if( !((message[index] >= 0x61 && message[index] <= 0x7a) || 
				(message[index]>= 0x41 && message[index] <= 0x5a)  || 
				(message[index] >= 0x30 && message[index] < 0x31+10)))
			{
				
				message[index] = 0;
				SetWindowText(hFocusWnd, 0);
				SetWindowText(hFocusWnd, UserPassword);
				SendMessage(hTextWnd, EM_SETSEL, lstrlen(UserPassword)+1, lstrlen(UserPassword)+1);
			}
		}
*/
	}
	return TRUE;
};

//서서히 밝아짐...
int HoOpening::MainFadeIn()
{
	if(FadeNextState == FADE_IN)
	{
		ScreenBlendValue -= 2;
		if(ScreenBlendValue < 0)
			ScreenBlendValue = 0;
		
		if(ScreenBlendValue <= 0 )
		{
			ScreenBlendValue = 0;
			FadeCurrentState = FADE_IN;
			FadeNextState = FADE_IN;
		}
	}
	return TRUE;
}

//서서히 어두워짐...
int HoOpening::MainFadeOut()
{
	if(FadeNextState == FADE_OUT)
	{
		if(ScreenBlendValue < 255)
		{
			OpeningBgmVolume -= 2;
			if(OpeningBgmVolume < 0)
				OpeningBgmVolume = 0;
			ScreenBlendValue += 3;
		}

		if(ScreenBlendValue >= 255)
		{
			ScreenBlendValue = 255;
			LogoBlendValue = 255;
			LoginBlendValue = 255;
			FadeCurrentState = FADE_OUT;
			FadeNextState = FADE_OUT;
		}
	}
	return TRUE;
}

int HoOpening::FastFade()
{
	if( (FadeNextState == FADE_IN && LoginBlendValue < 255) ||
		(FadeNextState == FADE_OUT && LoginBlendValue > 0) )
	{

		static bool fastBlendFlag = false;
		for(int index = 0; index < 256; index++)
		{
			if(VRKeyBuff[index] == TRUE)
			{
				fastBlendFlag = true;
				break;
			}
		}
		
		if(MouseDownFlag == FALSE)
		{
 			if(MouseButton[0] && fastBlendFlag == false)
			{
 				fastBlendFlag = true;
				MouseDownFlag = TRUE;
			}
		}

		if(fastBlendFlag)
		{
			if(FadeNextState == FADE_IN) //환해진다.
			{
				if( (CurrentWorkState == STATE_OPENING &&
				    NextWorkState == STATE_OPENING) ||
				   (CurrentWorkState == STATE_OPENING && NextWorkState == STATE_LOGIN_INPUT) )
				{
					LogoBlendValue = 255;
					ScreenBlendValue = 0;
					FadeCurrentState = FADE_IN;
					
					if(HoCommandLineFlag == FALSE)
					{
						LoginBlendValue = 255;
						CurrentWorkState = STATE_LOGIN_INPUT;
						NextWorkState = STATE_LOGIN_INPUT;
					}

					if(HoCommandLineFlag)
						NextWorkState = STATE_LOGIN_INPUT;
				}				
			}
			else if(FadeNextState == FADE_OUT)//어두워 진다.
			{
//				StartLogoLoginFlag = FALSE;	
				LogoBlendValue = 0;
				LoginBlendValue = 0;
				ScreenBlendValue = 255;
				OpeningBgmVolume = 0;
				FadeCurrentState = FADE_OUT;
			}
			fastBlendFlag = false;
		}
	}
	return TRUE;
}

int HoOpening::DrawFade()
{
	if(ScreenBlendValue >= 0 && ScreenBlendValue <= 255)
		dsDrawTexImage(BlackBoxImage,0, 0, 800, 600, ScreenBlendValue);

	return TRUE;
}

int HoOpening::MainFade()
{
	if( (FadeNextState == FADE_IN) ||
		(FadeNextState == FADE_OUT) )
	{
		if(FadeCurrentState != FadeNextState)
		{
			if(FadeNextState == FADE_IN)
				MainFadeIn();
			else
			{
				SetVolumeBGM(OpeningBgmVolume);
				MainFadeOut();
			}
		}
	}
	return TRUE;
}

void HoOpening::SetFade()
{
	//Fade In을 해야 할경우....(서서희 밝아짐)

	if(FadeCurrentState == FADE_OUT)
	{
		FadeNextState = FADE_IN;
		ScreenBlendValue = 254;
		OpeningBgmVolume = 350;
	}
	else//Fade Out을 해야 할 경우...(어두워짐)
	{
		FadeNextState = FADE_OUT;
		ScreenBlendValue = 1;
	}
}


void HoOpening::Close()
{
	if(Stage)
	{
		delete Stage;
		Stage = NULL;

		//######################################################################################
		//작 성 자 : 오 영 석		
		lpCurPlayer->pX = lpCurPlayer->pY = lpCurPlayer->pZ = 0;
		smGameStage[0] = NULL;
		smGameStage[1] = NULL;
		//######################################################################################
	}

	if(PatCamera)
	{
		delete PatCamera;
		PatCamera = NULL;
	}
	
	BackColor_R = 0;
	BackColor_G = 0;
	BackColor_B = 0;

	BackColor_mR = 0;
	BackColor_mG = 0;
	BackColor_mB = 0;
	
	InitSky();

	/*
	if(Bird)
	{
		delete Bird;
		Bird = NULL;
	}

	if(BirdBone)
	{
		delete BirdBone;
		BirdBone = NULL;
	}
	*/

	//InitSky();
	//smRender.Color_R = 40;
	//smRender.Color_G = 0;
	//smRender.Color_B = -30;
	//smRender.Color_A = 0;
}



//static BOOL HanGameErrorFlag = FALSE;
//return 1: 현재 Login진행시.
//return 2: Loing 성공시..
int HoOpening::Main()
{
	if(MouseButton[0] == FALSE)
		MouseDownFlag = FALSE;

	//######################################################################################
	//작 성 자 : 오 영 석
	/*
	if(PatCamera)
	{
		MainSky();
		
		float timeDelta = 1.f/70.f;
		g_NewParticleMgr.Main(0.0f, timeDelta);
		
 		CameraFrame += 10;
 		if(CameraFrame >= PatCamera->MaxFrame)
 			CameraFrame = 0;

		smMATRIX mWorld;
		AnimObjectTree( ObjCamera, CameraFrame, 0, 0, 0);
		memcpy(&mWorld, &ObjCamera->mWorld, sizeof(mWorld));
		
  		PatCameraWorldPos.x = mWorld._41; 
 		PatCameraWorldPos.z = mWorld._42;
        PatCameraWorldPos.y = mWorld._43;
	}*/

	float timeDelta = 1.f/70.f;
	g_NewParticleMgr.Main(0.0f, timeDelta);

	MainSky();
	m_IsDrawOpening = s_AutoCamera.FrameMain( m_AutoCameraFrameTime );

	SetCameraCharPos();
	SetParticleEffectSpawn();
	
	MainAssaEffect();
	//######################################################################################


	//에러 메시지 출력시.. 없애는 경우...
	if(ConnectError != ERROR_NONE)
	{
		if(ConnectErrorCount >= 160)
		{
			MouseDownServerIndex = -1;
			bool errorEndFlag = false;
			//마우스를 클릭 했을 경우.
			if(MouseButton[0] || MouseDblClick)
				errorEndFlag = true;
			
			//키보드를 눌렀을 경우.
			for(int index = 0; index < 256; index++)
			{
				if(VRKeyBuff[index] == TRUE)
				{
					errorEndFlag = true;
					break;
				}
			}
			//시간을 초과 했을 경우.
			if(ConnectErrorCount >= 250)
				errorEndFlag = true;
			
			if(errorEndFlag)
			{
				ConnectError = ERROR_NONE;
				FocusWndFlag = FALSE;
				ConnectErrorCount = 0;
			}
			
			//한게임 에러 처리...
			if( errorEndFlag && HoCommandLineFlag
				&& CurrentWorkState == STATE_LOGIN_INPUT
				&& NextWorkState    == STATE_LOGIN_INPUT)
			{
//				HanGameErrorFlag		= TRUE;
				HoCommandLineFlag		= FALSE;
				NextWorkState			= STATE_LOGIN_INPUT;
				CurrentWorkState		= STATE_OPENING;
				ConnectServerSucessFlag = FALSE;
				DisconnectServerFull();
			}
		}
		ConnectErrorCount++;
	}
	
	

	MainFade();
	
	//지형
 	if(CurrentWorkState == STATE_OPENING && NextWorkState == STATE_OPENING )
	{
		if(CameraFrame > 250*160) //Opening이 어느 정도 진행이 되면 작업 상태 변경.
			NextWorkState = STATE_LOGIN_INPUT;
	}
	
	//Opening 상태에서 Login 입력 상태로 변화시...
	if(CurrentWorkState == STATE_OPENING && NextWorkState == STATE_LOGIN_INPUT)
	{
		if(FadeNextState == FADE_IN) //FADE_IN이 다됐을 경우만...
		{
			LogoBlendValue += 1;
			if(LogoBlendValue >= 255)
				LogoBlendValue = 255;

			if(LogoBlendValue >= 255)
			{
				//한게임 로긴 아이디를 성공적으로 받았을 경우..
				if(HoCommandLineFlag)
				{
					CurrentWorkState = STATE_LOGIN_INPUT;
					LoginDataServer();
				}
				else//아닌 경우 로긴 창을 보여준다.
				{
					LoginBlendValue += 1;
					if(LoginBlendValue >= 255)
					{
						LoginBlendValue = 255;
						CurrentWorkState = STATE_LOGIN_INPUT;
					}
				}
			}
		}
	}
	
	//로긴 입력을 할 경우.
	if( CurrentWorkState == STATE_LOGIN_INPUT
		&& NextWorkState == STATE_LOGIN_INPUT
		&& (LoginBlendValue >= 255)
		&& (ScreenBlendValue <= 0)
		&& StartConnectServerFlag == FALSE
		&& ConnectError == ERROR_NONE)
	{
		//Enter를 눌렀을 경우.
		if( FocusWndFlag && hFocusWnd == NULL 
			&& ConnectServerSucessFlag == FALSE
			&& ConnectError == ERROR_NONE)
			LoginDataServer();
		
		int downFlag = -1;
		
		if(MouseDownFlag == FALSE)
		{
			if(MouseButton[0])
			{
				if(pCursorPos.x > 321 && pCursorPos.y > 438 && pCursorPos.x < 322+ButtonLoginSur[0].right && pCursorPos.y < 438+ButtonLoginSur[0].bottom)
				{
					MouseDownFlag = TRUE;
					hFocusWnd = NULL;
				}
				else if(pCursorPos.x > (321+60) && pCursorPos.y > 438 && pCursorPos.x < 322+60+ButtonQuitSur[0].right && pCursorPos.y < 438+ButtonQuitSur[0].bottom)
				{
					MouseDownFlag = TRUE;
					StartQuitFlag = TRUE;
					SetFade();
					PlayWav2(4-1);
				}
				else if(pCursorPos.x > (321+120) && pCursorPos.y > 438 && pCursorPos.x < 322+120+ButtonNewJoinSur[0].right && pCursorPos.y < 438+ButtonNewJoinSur[0].bottom)
				{
					MouseDownFlag = TRUE;
					PlayWav2(4-1);
					if(HoCommunity == COMMUNITY_TRIGLOW)
						ShellExecute(NULL, NULL, szJoinWebUrl, NULL, NULL, NULL);
					else if(HoCommunity == COMMUNITY_HANGAME)
						ShellExecute(NULL, NULL, "http://www.hangame.naver.com", NULL, NULL, NULL);
					
					quit = 1;
				}
			}
		}
	}
	
	//Game Server를 선택하기
	if( CurrentWorkState == STATE_SERVER_SELECT 
		&& NextWorkState == STATE_SERVER_SELECT)
	{
		int selectHeight;
		int selectStartPosX;
		int selectStartPosY;
		
		if(UserServerListInfoCount != 0)
			selectHeight = 90/UserServerListInfoCount;
			//selectHeight = 20;
  		selectStartPosX = 343;
 		selectStartPosY = 342+(selectHeight-12)/2; //원래 시작 위치는 330
		
		for(int index = 0; index < UserServerListInfoCount; index++)
		{
			if( pCursorPos.x > 330        && (pCursorPos.y > selectStartPosY+index*selectHeight) &&
				pCursorPos.x < (330+140)  && (pCursorPos.y < (selectStartPosY+index*selectHeight+12)) &&
				ConnectError == ERROR_NONE && !StartConnectServerFlag)
			{
				if(MouseButton[0] && MouseDownFlag == FALSE)
				{
					MouseDownFlag = TRUE;
					MouseDownServerIndex = index;
					LoginGameServer(index);
				}
			}
		}
	}

	if( CurrentWorkState == STATE_SERVER_SELECT
		&& NextWorkState == STATE_SERVER_SELECT)
	{
		BOOL flag=FALSE;
		if(ConnectError == ERROR_NONE && !StartConnectServerFlag && StartQuitFlag == FALSE)
		{
			if(MouseDownFlag == FALSE)
			{
				if(MouseButton[0])
				{
					if(pCursorPos.x > (321+60) && pCursorPos.y > 438 && pCursorPos.x < 322+60+ButtonQuitSur[0].right && pCursorPos.y < 438+ButtonQuitSur[0].bottom)
					{
						flag = TRUE;
						MouseDownFlag = TRUE;
					}
				}
			}
		}
		
		//DataServer와의 연결이 끊어 질경우..
		if(!smWsockDataServer && GameErrorValue == 0)
		{
			NextWorkState = STATE_LOGIN_INPUT;
			ConnectServerSucessFlag = FALSE;
			DisconnectServerFull();
			ConnectError = ERROR_NO_SERVER_CONNECT;
		}

		//나가기 버튼을 누른 경우..
		if(flag)
		{
			PlayWav2(4-1);
			//한게임이 아니면.
			if(!HoCommandLineFlag)
			{
				FocusWndFlag = FALSE;
				NextWorkState = STATE_LOGIN_INPUT;
				CurrentWorkState = STATE_LOGIN_INPUT;
				ConnectServerSucessFlag = FALSE;
				DisconnectServerFull();
			}
			else
			{
				DisconnectServerFull();
				StartQuitFlag = TRUE;
				SetFade();
				ConnectServerSucessFlag = FALSE;
				MouseDownFlag = FALSE;
			}
		}
	}
	
	//서버에 연결중이면....
	if(StartConnectServerFlag)
	{
		if(ConnectError != ERROR_NONE)
		{
			StartConnectServerFlag = FALSE;
			ServerWaitTimeCount = 0;
		}
		
		if(ServerLoginError)
		{
			if( CurrentWorkState == STATE_LOGIN_INPUT
				&& NextWorkState == STATE_LOGIN_INPUT)
				DisconnectServerFull();

			if( CurrentWorkState == STATE_SERVER_SELECT
				&& NextWorkState == STATE_SERVER_SELECT)
				DisconnectServer_GameMain();
			
			if(ServerLoginError->WParam == 0) //연결 불가.
				ConnectError = ERROR_CONNECT;
			else if(ServerLoginError->WParam == -1)
				ConnectError = ERROR_NO_ACCOUNT;
			else if(ServerLoginError->WParam == -2)
				ConnectError = ERROR_NO_PASSWORD;
			else if(ServerLoginError->WParam == -3)
				ConnectError = ERROR_NO_BETA_TESTER;
			else if(ServerLoginError->WParam == -4)
				ConnectError = ERROR_NO_ACCOUNT_IN;
			else if(ServerLoginError->WParam == -5)
				ConnectError = ERROR_NO_USE_DATE_OVER;
			else if(ServerLoginError->WParam == -6)
				ConnectError = ERROR_NO_USE_TIME_OVER;
			else if(ServerLoginError->WParam == -8)
				ConnectError = ERROR_DELETE_ACCOUNT_NOTIFY;
			else if(ServerLoginError->WParam == -16)
				ConnectError = ERROR_SERVER_TRAFFIC_BUSY;
			else if(ServerLoginError->WParam == -17)
				ConnectError = ERROR_SERVER_WAIT_MOMNENT;
			
			StartConnectServerFlag = FALSE;
			ServerWaitTimeCount = 0;
			ServerLoginError = NULL;
		}

 		ServerWaitTimeCount++;
		if(ServerWaitTimeCount >= 3000)	//서버를 기다리는 시간...
		{
			ServerWaitTimeCount = 0;
			ConnectError = ERROR_CONNECT;
			StartConnectServerFlag = FALSE;
			ConnectServerSucessFlag = FALSE;
		}
		
		//Data서버 로그인시..
		if( CurrentWorkState == STATE_LOGIN_INPUT &&
			NextWorkState == STATE_LOGIN_INPUT)
		{
			if( ServerUserCharacterInfo
				&& DownServerListInfo
				&& ConnectError == ERROR_NONE
				&& GameErrorValue == 0)
			{
				StartConnectServerFlag = FALSE;
				ConnectServerSucessFlag = TRUE;
				CurrentWorkState = STATE_SERVER_SELECT;
				NextWorkState = STATE_SERVER_SELECT;
				GameErrorValue = -1;
			}
		}
		
		//Game서버 로그인시..
		if( CurrentWorkState == STATE_SERVER_SELECT 
			&& NextWorkState == STATE_SERVER_SELECT) 
		{
			//count = 0;
			if( ConnectError == ERROR_NONE && GameErrorValue == 0)
			{
				StartConnectServerFlag = FALSE;
				SetFade();
				NextWorkState = STATE_END;
				GameErrorValue = -1;
			}
		}
		
		if(GameErrorValue != 0)
		{
			//Version이 맞지 않을 경우...
			if(GameErrorValue == 1)
			{
				DisconnectServerFull();
				ConnectError = ERROR_NO_VERSION;
				StartConnectServerFlag = FALSE;
				GameErrorValue = -1;
			}
			
			//서버 인원수 제한이 있을 경우..
			if(GameErrorValue == 2)
			{
				if(CurrentWorkState == STATE_LOGIN_INPUT
				   &&NextWorkState == STATE_LOGIN_INPUT)
					DisconnectServerFull();

				if(CurrentWorkState == STATE_SERVER_SELECT
					&&NextWorkState == STATE_SERVER_SELECT)
					DisconnectServer_GameMain();
				
				ConnectError = ERROR_NO_LIMIT;
				StartConnectServerFlag = FALSE;
				GameErrorValue = -1;
			}
		}
	}
	
	if(ConnectServerSucessFlag)
	{
		if(FadeCurrentState == FADE_OUT && FadeNextState == FADE_OUT)
		{
#ifdef  _LANGUAGE_KOREAN			
			SetRegString( HKEY_LOCAL_MACHINE , "SOFTWARE\\Triglow Pictures\\PristonTale" , "Account","  ");
#endif

#ifndef _LANGUAGE_KOREAN			
			//Login 계정을 레지스트리에 저장한다..
			SetRegString( HKEY_LOCAL_MACHINE , "SOFTWARE\\Triglow Pictures\\PristonTale" , "Account", UserAccount);
#endif

			SetWindowText(hTextWnd, NULL);
			SendMessage( hTextWnd , EM_SETLIMITTEXT , 80, 0 );

			StopBGM();
			return 2;
		}
	}
	
	if(StartQuitFlag)
	{
		if(FadeCurrentState == FADE_OUT && FadeNextState == FADE_OUT)
			quit = 1;
	}

	return 1;	
}


