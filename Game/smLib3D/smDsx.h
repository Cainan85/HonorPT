#include <ddraw.h>
#include <d3d.h>
#include <d3dtypes.h>

//######################################################################################
//작 성 자 : 오 영 석
#include <Shlwapi.h>
//######################################################################################


/***************************************
* Direct3D 초기화용 구조체
***************************************/

// Direct3D Device 정보
// 이정보를 이용하여 IDirect3DDevice2를 생성합니다.
typedef struct _DEVICEDESC
{
    // Device Info
    GUID  guid;                 // GUID
    char  szName[256];          // 이름
    char  szDesc[256];          // 설명
    BOOL  bIsHardware;          // 하드웨어 드라이브 인가?

    // Device Caps
    D3DDEVICEDESC   Desc;
	_DEVICEDESC		*lpNext;
} DEVICEDESC, *LPDEVICEDESC;



extern int WindowMode;
extern int smTextureBPP;
extern int smFlipCount;

extern int smScreenWidth, smScreenHeight;

extern D3DRECT                 D3DRect;
extern LPDEVICEDESC            lpD3DDeviceDesc;

extern LPDIRECTDRAW4           lpDD;
extern LPDIRECTDRAWSURFACE4    lpDDSPrimary;
extern LPDIRECTDRAWSURFACE4    lpDDSBack;

extern LPDIRECT3D3	           lpD3D;
extern LPDIRECT3DDEVICE3       lpD3DDevice;
extern LPDIRECT3DVIEWPORT3     lpD3DViewport;
extern LPDIRECTDRAWSURFACE4    lpDDSZBuffer;





/***************************************
* Direct3D 직접모드 초기화 하기
***************************************/


// Direct3D 생성
BOOL CreateD3D();

// Direct3D 제거
void ReleaseD3D();


// 비디오 모드 전환
BOOL SetDisplayMode( HWND hWnd, DWORD Width, DWORD Height, DWORD BPP );

// Back Surface 의 DC얻기
HDC GetDC();

// DC 제거
void ReleaseDC( HDC hDC );



// Primary Surface Lost 검사
void IsLost();

// 페이지 플리핑
int Flip();

// Viewport 생성
BOOL CreateViewport();

// Back Surface 지우기
void Clear( DWORD dwColor );


// 랜드링 초기화
void InitRender();

// 랜드링 시작
void BeginRender();

// 랜드링 끝
void EndRender();


//######################################################################################
//작 성 자 : 오 영 석
extern int MESSAGE( char *t);
//######################################################################################


typedef struct {
	float r, g, b, a;
} COLORVALUE, *LPCOLORVALUE;

typedef struct {
	float	x, y, z;
} VECTOR, *LPVECTOR;

typedef struct {
    float   tu, tv;
} TVERTEX, *LPTVERTEX;



#pragma comment( lib, "ddraw.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "winmm.lib" )


//######################################################################################
//작 성 자 : 오 영 석
#pragma comment(lib, "shlwapi.lib")
//######################################################################################


