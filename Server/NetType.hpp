
#ifndef	_NET_TYPE_HEADER
#define _NET_TYPE_HEADER

#define	_W_SERVER
//#define	_W_DATA_SERVER

//새로운 아이템 인증코드 적용
#define _NEW_ITEM_FORMCODE_

//새로운 복합 암호화 아이템 인증코드 적용 ( 중국 , 영문 서버 적용 )
#define _NEW_ITEM_FORMCODE2
//더욱 새로운 복합 암호화 아이템 인증코드 적용(태국/일본)
#define _NEW_ITEM_FORMCODE3

//#define _SERVER_MODE_OLD
	
#ifdef _W_SERVER
//#define	_npGAME_GUARD_AUTH //XTrap 컴파일시 주석처리
#endif

#endif
