//######################################################################################
//작 성 일 : 2003-12-18 오후 1:53:14
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
#include "smd3d.h"

//
int									g_IsCreateFilterEffect;
DDPIXELFORMAT						g_ddpfPixelFormatZ;

static LPDIRECTDRAWSURFACE4			s_lpDDS_NewTarget_Primary;
static LPDIRECT3DTEXTURE2			s_lpD3D_NewTargetTexture_Primary;
static LPDIRECTDRAWSURFACE4			s_lpDDS_NewTargetZ_Primary;

static LPDIRECTDRAWSURFACE4			s_lpDDS_NewTarget_Back;
static LPDIRECT3DTEXTURE2			s_lpD3D_NewTargetTexture_Back;
static LPDIRECTDRAWSURFACE4			s_lpDDS_NewTargetZ_Back;

static LPDIRECTDRAWSURFACE4			s_lpDDS_NewTarget_Rate_Back;
static LPDIRECT3DTEXTURE2			s_lpD3D_NewTargetTexture_Rate_Back;
static LPDIRECTDRAWSURFACE4			s_lpDDS_NewTargetZ_Rate_Back;

//
static D3DTLVERTEX					s_TLVertex_NewTarget[4];
static D3DTLVERTEX					s_TLVertex_RateZoomIn[4];
static D3DTLVERTEX					s_TLVertex_RateZoomOut[4];

//
static eNEW_RENDER_TARGET_TYPE		s_eWorkTarget = ORG_TARGET;
static DWORD						s_dwIsBoolFilterEffect;
static DWORD						s_dwFilterEffectWork;
static DWORD						s_dwFilterEffectRGBAlpha[3];
static int							s_isFirstMotionBlur;

static DWORD						s_dwBlurRGBAlpha = RGBA_MAKE(255,255,255,255/4);
static float						s_texNum		 = 1.f / 1024.f;


////////////////////////////////////////////////////////////////////////////////
//
static int  CreateFilterEffectTLVertex(void);
static void RenderOriginalDraw( LPDIRECT3DTEXTURE2 lpD3D_Texture, D3DTLVERTEX *lpTLVertexBuff );
static void RenderAlphaDraw( LPDIRECT3DTEXTURE2 lpD3D_Texture, D3DTLVERTEX *lpTLVertexBuff, DWORD dwRGBAlpha );
static void RenderBlurDraw( LPDIRECT3DTEXTURE2 lpD3D_Texture, D3DTLVERTEX *lpTLVertexBuff );


//######################################################################################
//작 성 일 : 2003-12-18 오후 1:55:10
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
int CreateNewRenderTarget(void)
{
	HRESULT hresult;
	if( CreateFilterEffectTLVertex() == FALSE || s_lpDDS_NewTarget_Primary || s_lpDDS_NewTarget_Back )
		return 0;

	s_dwIsBoolFilterEffect = 0;

	if( lpDD == NULL || lpD3DDevice == NULL || D3DRect.x2 > 1024 || D3DRect.y2 > 1024 )
		return -1;
	
	if( lpD3DDeviceDesc->Desc.dwMaxTextureHeight < 1024 || lpD3DDeviceDesc->Desc.dwMaxTextureWidth < 1024 )
		return -2;

	////////////////////////////////////////////////////////////////////////////////
	//
	LPDIRECTDRAWSURFACE4 lpDDS4;
    lpD3DDevice->GetRenderTarget( &lpDDS4 );
    lpDDS4->Release();

	DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	lpDDS4->GetSurfaceDesc( &ddsd );

	ddsd.dwWidth  = 1024;
	ddsd.dwHeight = 1024;

	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_3DDEVICE;
	if( lpD3DDeviceDesc->bIsHardware )
		ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	////////////////////////////////////////////////////////////////////////////////
	//
    hresult = lpDD->CreateSurface( &ddsd, &s_lpDDS_NewTarget_Primary, NULL );
	if( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. CreateSurface() #1. NEW Target\n" );
        return -3;
	}

	hresult = s_lpDDS_NewTarget_Primary->QueryInterface( IID_IDirect3DTexture2, (LPVOID*)&s_lpD3D_NewTargetTexture_Primary );
	if( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. QueryInterface() #1. NEW Target TEXTURE\n" );
        return -3;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	hresult = lpDD->CreateSurface( &ddsd, &s_lpDDS_NewTarget_Back, NULL );
	if( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. CreateSurface() #2. NEW Target\n" );
        return -3;
	}

	hresult = s_lpDDS_NewTarget_Back->QueryInterface( IID_IDirect3DTexture2, (LPVOID*)&s_lpD3D_NewTargetTexture_Back );
	if( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. QueryInterface() #2. NEW Target TEXTURE\n" );
        return -3;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	DDSURFACEDESC2 ddsdRate;
	CopyMemory( &ddsdRate, &ddsd, sizeof(ddsdRate) );
	ddsdRate.dwWidth  = 256;
	ddsdRate.dwHeight = 256;

	hresult = lpDD->CreateSurface( &ddsdRate, &s_lpDDS_NewTarget_Rate_Back, NULL );
	if( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. CreateSurface() #3. NEW Target\n" );
        return -3;
	}

	hresult = s_lpDDS_NewTarget_Rate_Back->QueryInterface( IID_IDirect3DTexture2, (LPVOID*)&s_lpD3D_NewTargetTexture_Rate_Back );
	if( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. QueryInterface() #3. NEW Target TEXTURE\n" );
        return -3;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	DDSURFACEDESC2 ddsdZ;
    ZeroMemory( &ddsdZ, sizeof(ddsdZ) );
    ddsdZ.dwSize   = sizeof(ddsdZ);
    ddsdZ.dwFlags  = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
	ddsdZ.dwWidth  = ddsd.dwWidth;
	ddsdZ.dwHeight = ddsd.dwHeight;

	::CopyMemory( &ddsdZ.ddpfPixelFormat, &g_ddpfPixelFormatZ, sizeof(g_ddpfPixelFormatZ) );
	if( lpD3DDeviceDesc->bIsHardware )
		ddsdZ.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
	else
        ddsdZ.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;

	////////////////////////////////////////////////////////////////////////////////
	//
    hresult = lpDD->CreateSurface( &ddsdZ, &s_lpDDS_NewTargetZ_Primary, NULL );
    if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. CreateSurface() #1. Z Buffer\n" );
        return -3;
	}

    hresult = s_lpDDS_NewTarget_Primary->AddAttachedSurface( s_lpDDS_NewTargetZ_Primary );
    if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. AddAttachedSurface() #1.\n" );
        return -3;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
    hresult = lpDD->CreateSurface( &ddsdZ, &s_lpDDS_NewTargetZ_Back, NULL );
    if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. CreateSurface() #2. Z Buffer\n" );
        return -3;
	}

    hresult = s_lpDDS_NewTarget_Back->AddAttachedSurface( s_lpDDS_NewTargetZ_Back );
    if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. AddAttachedSurface() #2.\n" );
        return -3;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	DDSURFACEDESC2 ddsdZRate;
	CopyMemory( &ddsdZRate, &ddsdZ, sizeof(ddsdZRate) );
	ddsdZRate.dwWidth  = 256;
	ddsdZRate.dwHeight = 256;

    hresult = lpDD->CreateSurface( &ddsdZRate, &s_lpDDS_NewTargetZ_Rate_Back, NULL );
    if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. CreateSurface() #3. Z Buffer\n" );
        return -3;
	}

    hresult = s_lpDDS_NewTarget_Rate_Back->AddAttachedSurface( s_lpDDS_NewTargetZ_Rate_Back );
    if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. AddAttachedSurface() #3.\n" );
        return -3;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	hresult = lpD3DDevice->SetRenderTarget( lpDDSBack, 0L );
	if ( hresult != DD_OK )
	{
		ReleaseNewRenderTarget();
		MESSAGE( "CreateNewRenderTarget() Error. ChangeRenderTarget().\n" );
		return -4;
	}

	s_dwIsBoolFilterEffect = 1;	
	return 1;
}


//######################################################################################
//작 성 일 : 2003-12-18 오후 3:17:49
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
void ReleaseNewRenderTarget(void)
{
	//
	if( s_lpD3D_NewTargetTexture_Back )
	{
		s_lpD3D_NewTargetTexture_Back->Release();
		s_lpD3D_NewTargetTexture_Back = NULL;
	}

	if( s_lpDDS_NewTargetZ_Back )
	{
		if( s_lpDDS_NewTarget_Back )
            s_lpDDS_NewTarget_Back->DeleteAttachedSurface( 0L, s_lpDDS_NewTargetZ_Back );

		s_lpDDS_NewTargetZ_Back->Release();
		s_lpDDS_NewTargetZ_Back = NULL;
	}

	if( s_lpDDS_NewTarget_Back )
	{
		s_lpDDS_NewTarget_Back->Release();
		s_lpDDS_NewTarget_Back = NULL;
	}

	//
	if( s_lpD3D_NewTargetTexture_Rate_Back )
	{
		s_lpD3D_NewTargetTexture_Rate_Back->Release();
		s_lpD3D_NewTargetTexture_Rate_Back = NULL;
	}

	if( s_lpDDS_NewTargetZ_Rate_Back )
	{
		if( s_lpDDS_NewTarget_Rate_Back )
            s_lpDDS_NewTarget_Rate_Back->DeleteAttachedSurface( 0L, s_lpDDS_NewTargetZ_Rate_Back );

		s_lpDDS_NewTargetZ_Rate_Back->Release();
		s_lpDDS_NewTargetZ_Rate_Back = NULL;
	}

	if( s_lpDDS_NewTarget_Rate_Back )
	{
		s_lpDDS_NewTarget_Rate_Back->Release();
		s_lpDDS_NewTarget_Rate_Back = NULL;
	}

	//
	if( s_lpD3D_NewTargetTexture_Primary )
	{
		s_lpD3D_NewTargetTexture_Primary->Release();
		s_lpD3D_NewTargetTexture_Primary = NULL;
	}

	if( s_lpDDS_NewTargetZ_Primary )
	{
		if( s_lpDDS_NewTarget_Primary )
            s_lpDDS_NewTarget_Primary->DeleteAttachedSurface( 0L, s_lpDDS_NewTargetZ_Primary );

		s_lpDDS_NewTargetZ_Primary->Release();
		s_lpDDS_NewTargetZ_Primary = NULL;
	}

	if( s_lpDDS_NewTarget_Primary )
	{
		s_lpDDS_NewTarget_Primary->Release();
		s_lpDDS_NewTarget_Primary = NULL;
	}

	//
	s_dwIsBoolFilterEffect = 0;
}


//######################################################################################
//작 성 일 : 2003-12-30 오후 1:01:41
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
inline int IsCreateNewRenderTarget(void)
{
	return s_dwIsBoolFilterEffect;
}


//######################################################################################
//작 성 일 : 2003-12-29 오후 5:56:37
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
inline int GetRenderTarget(void)
{
	return s_eWorkTarget;
}


//######################################################################################
//작 성 일 : 2003-12-18 오후 3:24:52
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
int ChangeRenderTarget( eNEW_RENDER_TARGET_TYPE eRTtype )
{
	if( s_dwIsBoolFilterEffect == 0 || s_eWorkTarget == eRTtype )
		return 0;

	switch( eRTtype )
	{
	case ORG_TARGET:
		lpD3DDevice->SetRenderTarget( lpDDSBack, 0L );
		s_eWorkTarget = eRTtype;
		break;

	case NEW_TARGET_PRIMARY:
		lpD3DDevice->SetRenderTarget( s_lpDDS_NewTarget_Primary, 0L );
		s_eWorkTarget = eRTtype;
		break;

	case NEW_TARGET_BACK:
		lpD3DDevice->SetRenderTarget( s_lpDDS_NewTarget_Back, 0L );
		s_eWorkTarget = eRTtype;
		break;

	case NEW_TARGET_RATE_BACK:
		lpD3DDevice->SetRenderTarget( s_lpDDS_NewTarget_Rate_Back, 0L );
		s_eWorkTarget = eRTtype;
		break;
	}

	return 1;
}


//######################################################################################
//작 성 일 : 2003-12-30 오후 12:53:57
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
inline int GetFilterEffect(void)
{
	return s_dwFilterEffectWork;
}


//######################################################################################
//작 성 일 : 2003-12-30 오후 1:07:04
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
void SetFilterEffect( eFILTER_EFFECT_TYPE eFEtype, DWORD dwAlpha )
{
	switch( eFEtype )
	{
	case FILTER_EFFECT_NULL:
		s_dwFilterEffectWork = 0;
		break;

	case FILTER_EFFECT_SET_MOTION_BLUR:
		s_isFirstMotionBlur = 1;
		s_dwFilterEffectWork |= FILTER_EFFECT_SET_MOTION_BLUR;
		s_dwFilterEffectRGBAlpha[ FILTER_EFFECT_SET_MOTION_BLUR ] = RGBA_MAKE(255,255,255,dwAlpha);
		break;

	case FILTER_EFFECT_SET_BRIGHT_CONTRAST:
		s_dwFilterEffectWork |= FILTER_EFFECT_SET_BRIGHT_CONTRAST;
		s_dwFilterEffectRGBAlpha[ FILTER_EFFECT_SET_BRIGHT_CONTRAST ] = RGBA_MAKE(255,255,255,dwAlpha);
		break;

	case FILTER_EFFECT_DEL_MOTION_BLUR:
		s_dwFilterEffectWork &= (~FILTER_EFFECT_SET_MOTION_BLUR);
		break;

	case FILTER_EFFECT_DEL_BRIGHT_CONTRAST:
		s_dwFilterEffectWork &= (~FILTER_EFFECT_SET_BRIGHT_CONTRAST);
		break;
	}
}


//######################################################################################
//작 성 일 : 2003-12-23 오후 6:40:07
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
void SetNewTargetTextureState(void)
{
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFN_LINEAR );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

	lpD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    lpD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

	lpD3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    lpD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	lpD3DDevice->SetTexture( 1, NULL );

	lpD3DDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, FALSE );
}


//######################################################################################
//작 성 일 : 2003-12-23 오후 6:46:30
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
void DefaultsNewTargetTextureState(void)
{
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFG_POINT );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_POINT );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
	lpD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
}



//######################################################################################
//작 성 일 : 2003-12-23 오후 5:26:42
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
static int CreateFilterEffectTLVertex(void)
{
	DWORD color  = RGBA_MAKE(255,255,255,255);
	int   width  = D3DRect.x2 / 4;
	int   height = D3DRect.y2 / 4;
	int   cnt;

	if( width > 256 || height > 256 )
		return FALSE;

	////////////////////////////////////////////////////////////////////////////////
	//
	for( cnt=0; cnt < 4; cnt++ )
	{
		s_TLVertex_NewTarget[cnt].sz	   = 1.f;
		s_TLVertex_NewTarget[cnt].rhw      = 1.f;
		s_TLVertex_NewTarget[cnt].color	   = color;
		s_TLVertex_NewTarget[cnt].specular = 0;
	}

	s_TLVertex_NewTarget[0].sx = -0.5f;
	s_TLVertex_NewTarget[0].sy = -0.5f;
	s_TLVertex_NewTarget[0].tu = 0;
	s_TLVertex_NewTarget[0].tv = 0;
	
	s_TLVertex_NewTarget[1].sx = -0.5f;
	s_TLVertex_NewTarget[1].sy = float(D3DRect.y2) - 0.5f;
	s_TLVertex_NewTarget[1].tu = 0;
	s_TLVertex_NewTarget[1].tv = float(D3DRect.y2)/1024.f;
	
	s_TLVertex_NewTarget[2].sx = float(D3DRect.x2) - 0.5f;
	s_TLVertex_NewTarget[2].sy = -0.5f;
	s_TLVertex_NewTarget[2].tu = float(D3DRect.x2)/1024.f;
	s_TLVertex_NewTarget[2].tv = 0;
	
	s_TLVertex_NewTarget[3].sx = float(D3DRect.x2) - 0.5f;
	s_TLVertex_NewTarget[3].sy = float(D3DRect.y2) - 0.5f;
	s_TLVertex_NewTarget[3].tu = float(D3DRect.x2)/1024.f;
	s_TLVertex_NewTarget[3].tv = float(D3DRect.y2)/1024.f;

	////////////////////////////////////////////////////////////////////////////////
	//
	for( cnt=0; cnt < 4; cnt++ )
	{
		s_TLVertex_RateZoomOut[cnt].sz	     = 1.f;
		s_TLVertex_RateZoomOut[cnt].rhw      = 1.f;
		s_TLVertex_RateZoomOut[cnt].color	 = color;
		s_TLVertex_RateZoomOut[cnt].specular = 0;
	}
	

	s_TLVertex_RateZoomOut[0].sx = -0.5f;
	s_TLVertex_RateZoomOut[0].sy = -0.5f;
	s_TLVertex_RateZoomOut[0].tu = 0;
	s_TLVertex_RateZoomOut[0].tv = 0;
	
	s_TLVertex_RateZoomOut[1].sx = -0.5f;
	s_TLVertex_RateZoomOut[1].sy = float(height) - 0.5f;
	s_TLVertex_RateZoomOut[1].tu = 0;
	s_TLVertex_RateZoomOut[1].tv = float(D3DRect.y2)/1024.f;
	
	s_TLVertex_RateZoomOut[2].sx = float(width)  - 0.5f;
	s_TLVertex_RateZoomOut[2].sy = -0.5f;
	s_TLVertex_RateZoomOut[2].tu = float(D3DRect.x2)/1024.f;
	s_TLVertex_RateZoomOut[2].tv = 0;
	
	s_TLVertex_RateZoomOut[3].sx = float(width)  - 0.5f;
	s_TLVertex_RateZoomOut[3].sy = float(height) - 0.5f;
	s_TLVertex_RateZoomOut[3].tu = float(D3DRect.x2)/1024.f;
	s_TLVertex_RateZoomOut[3].tv = float(D3DRect.y2)/1024.f;

	////////////////////////////////////////////////////////////////////////////////
	//
	for( cnt=0; cnt < 4; cnt++ )
	{
		s_TLVertex_RateZoomIn[cnt].sz	    = 1.f;
		s_TLVertex_RateZoomIn[cnt].rhw      = 1.f;
		s_TLVertex_RateZoomIn[cnt].color	= color;
		s_TLVertex_RateZoomIn[cnt].specular = 0;
	}

	s_TLVertex_RateZoomIn[0].sx = -0.5f;
	s_TLVertex_RateZoomIn[0].sy = -0.5f;
	s_TLVertex_RateZoomIn[0].tu = 0;
	s_TLVertex_RateZoomIn[0].tv = 0;
	
	s_TLVertex_RateZoomIn[1].sx = -0.5f;
	s_TLVertex_RateZoomIn[1].sy = float(D3DRect.y2) - 0.5f;
	s_TLVertex_RateZoomIn[1].tu = 0;
	s_TLVertex_RateZoomIn[1].tv = float(height)/256.f;
	
	s_TLVertex_RateZoomIn[2].sx = float(D3DRect.x2) - 0.5f;
	s_TLVertex_RateZoomIn[2].sy = -0.5f;
	s_TLVertex_RateZoomIn[2].tu = float(width)/256.f;
	s_TLVertex_RateZoomIn[2].tv = 0;
	
	s_TLVertex_RateZoomIn[3].sx = float(D3DRect.x2) - 0.5f;
	s_TLVertex_RateZoomIn[3].sy = float(D3DRect.y2) - 0.5f;
	s_TLVertex_RateZoomIn[3].tu = float(width) /256.f;
	s_TLVertex_RateZoomIn[3].tv = float(height)/256.f;

	////////////////////////////////////////////////////////////////////////////////
	//
	return TRUE;
}


//######################################################################################
//작 성 일 : 2003-12-18 오후 4:10:07
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
static void RenderOriginalDraw( LPDIRECT3DTEXTURE2 lpD3D_Texture, D3DTLVERTEX *lpTLVertexBuff )
{
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, FALSE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	
	lpD3DDevice->SetTexture( 0, lpD3D_Texture );
	lpD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, lpTLVertexBuff, 4, NULL );
}


//######################################################################################
//작 성 일 : 2003-12-18 오후 4:32:22
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
static void RenderAlphaDraw( LPDIRECT3DTEXTURE2 lpD3D_Texture, D3DTLVERTEX *lpTLVertexBuff, DWORD dwRGBAlpha )
{
	//
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, FALSE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );

	int   cnt = 0;
	DWORD color[4];

	for( cnt=0; cnt < 4; cnt++ )
	{
		color[cnt] = lpTLVertexBuff[cnt].color;
		lpTLVertexBuff[cnt].color = dwRGBAlpha;
	}

	//
	lpD3DDevice->SetTexture( 0, lpD3D_Texture );
	lpD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, lpTLVertexBuff, 4, NULL );

	//	
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

	for( cnt=0; cnt < 4; cnt++ )
		lpTLVertexBuff[cnt].color = color[cnt];
}


//######################################################################################
//작 성 일 : 2003-12-23 오후 7:41:17
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
static void RenderBlurDraw( LPDIRECT3DTEXTURE2 lpD3D_Texture, D3DTLVERTEX *lpTLVertexBuff )
{
	//
	int   cnt = 0;
	DWORD color[4];
	float uv[4][2];

	for( cnt=0; cnt < 4; cnt++ )
	{
		color[cnt] = lpTLVertexBuff[cnt].color;
		lpTLVertexBuff[cnt].color = s_dwBlurRGBAlpha;

		uv[cnt][0] = lpTLVertexBuff[cnt].tu;
		uv[cnt][1] = lpTLVertexBuff[cnt].tv;
	}

	//
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, FALSE );
	lpD3DDevice->SetTexture( 0, lpD3D_Texture );

	//
	lpTLVertexBuff[0].tu = uv[0][0] + s_texNum;
	lpTLVertexBuff[0].tv = uv[0][1];
	lpTLVertexBuff[1].tu = uv[1][0] + s_texNum;
	lpTLVertexBuff[1].tv = uv[1][1];
	lpTLVertexBuff[2].tu = uv[2][0] + s_texNum;
	lpTLVertexBuff[2].tv = uv[2][1];
	lpTLVertexBuff[3].tu = uv[3][0] + s_texNum;
	lpTLVertexBuff[3].tv = uv[3][1];
	lpD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, lpTLVertexBuff, 4, NULL );

	//
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );

	//
	lpTLVertexBuff[0].tu = uv[0][0];
	lpTLVertexBuff[0].tv = uv[0][1] + s_texNum;
	lpTLVertexBuff[1].tu = uv[1][0];
	lpTLVertexBuff[1].tv = uv[1][1] + s_texNum;
	lpTLVertexBuff[2].tu = uv[2][0];
	lpTLVertexBuff[2].tv = uv[2][1] + s_texNum;
	lpTLVertexBuff[3].tu = uv[3][0];
	lpTLVertexBuff[3].tv = uv[3][1] + s_texNum;
	lpD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, lpTLVertexBuff, 4, NULL );

	//
	lpTLVertexBuff[0].tu = uv[0][0] - s_texNum;
	lpTLVertexBuff[0].tv = uv[0][1];
	lpTLVertexBuff[1].tu = uv[1][0] - s_texNum;
	lpTLVertexBuff[1].tv = uv[1][1];
	lpTLVertexBuff[2].tu = uv[2][0] - s_texNum;
	lpTLVertexBuff[2].tv = uv[2][1];
	lpTLVertexBuff[3].tu = uv[3][0] - s_texNum;
	lpTLVertexBuff[3].tv = uv[3][1];
	lpD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, lpTLVertexBuff, 4, NULL );

	//
	lpTLVertexBuff[0].tu = uv[0][0];
	lpTLVertexBuff[0].tv = uv[0][1] - s_texNum;
	lpTLVertexBuff[1].tu = uv[1][0];
	lpTLVertexBuff[1].tv = uv[1][1] - s_texNum;
	lpTLVertexBuff[2].tu = uv[2][0];
	lpTLVertexBuff[2].tv = uv[2][1] - s_texNum;
	lpTLVertexBuff[3].tu = uv[3][0];
	lpTLVertexBuff[3].tv = uv[3][1] - s_texNum;
	lpD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, lpTLVertexBuff, 4, NULL );

	//
	lpD3DDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

	for( cnt=0; cnt < 4; cnt++ )
	{
		lpTLVertexBuff[cnt].color = color[cnt];

		lpTLVertexBuff[cnt].tu = uv[cnt][0];
		lpTLVertexBuff[cnt].tv = uv[cnt][1];
	}
}


//######################################################################################
//작 성 일 : 2004-01-12 오후 1:16:59
//리 턴 값 :
//설    명 :
//작 성 자 : 오 영 석
//######################################################################################
void RenderFilterEffect(void)
{
	if( s_dwFilterEffectWork == FILTER_EFFECT_SET_MOTION_BLUR )
	{
		if( s_isFirstMotionBlur )
		{
			ChangeRenderTarget( NEW_TARGET_PRIMARY );
			BeginRender();
			RenderOriginalDraw( s_lpD3D_NewTargetTexture_Back, s_TLVertex_NewTarget );
			s_isFirstMotionBlur = 0;
		}
		else
		{
            ChangeRenderTarget( NEW_TARGET_PRIMARY );
            RenderAlphaDraw( s_lpD3D_NewTargetTexture_Back, s_TLVertex_NewTarget, s_dwFilterEffectRGBAlpha[ FILTER_EFFECT_SET_MOTION_BLUR ] );
		}

		ChangeRenderTarget( ORG_TARGET );
		RenderOriginalDraw( s_lpD3D_NewTargetTexture_Primary, s_TLVertex_NewTarget );
	}
	else if( s_dwFilterEffectWork == FILTER_EFFECT_SET_BRIGHT_CONTRAST )
	{
		ChangeRenderTarget( NEW_TARGET_RATE_BACK );
		RenderOriginalDraw( s_lpD3D_NewTargetTexture_Back, s_TLVertex_RateZoomOut );

		ChangeRenderTarget( ORG_TARGET );
		RenderBlurDraw( s_lpD3D_NewTargetTexture_Rate_Back, s_TLVertex_RateZoomIn );
		RenderAlphaDraw( s_lpD3D_NewTargetTexture_Back, s_TLVertex_NewTarget, s_dwFilterEffectRGBAlpha[ FILTER_EFFECT_SET_BRIGHT_CONTRAST ] );
	}
	else if( s_dwFilterEffectWork == (FILTER_EFFECT_SET_MOTION_BLUR | FILTER_EFFECT_SET_BRIGHT_CONTRAST) )
	{
		if( s_isFirstMotionBlur )
		{
			ChangeRenderTarget( NEW_TARGET_PRIMARY );
			BeginRender();
			RenderOriginalDraw( s_lpD3D_NewTargetTexture_Back, s_TLVertex_NewTarget );
			s_isFirstMotionBlur = 0;
		}
		else
		{
            ChangeRenderTarget( NEW_TARGET_PRIMARY );
            RenderAlphaDraw( s_lpD3D_NewTargetTexture_Back, s_TLVertex_NewTarget, s_dwFilterEffectRGBAlpha[ FILTER_EFFECT_SET_MOTION_BLUR ] );
		}

		ChangeRenderTarget( NEW_TARGET_RATE_BACK );
		RenderOriginalDraw( s_lpD3D_NewTargetTexture_Primary, s_TLVertex_RateZoomOut );

		ChangeRenderTarget( ORG_TARGET );
		RenderBlurDraw( s_lpD3D_NewTargetTexture_Rate_Back, s_TLVertex_RateZoomIn );
		RenderAlphaDraw( s_lpD3D_NewTargetTexture_Primary, s_TLVertex_NewTarget, s_dwFilterEffectRGBAlpha[ FILTER_EFFECT_SET_BRIGHT_CONTRAST ] );
	}
}


