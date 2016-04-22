#pragma once

#ifndef __CE_USER_HPP__
#define __CE_USER_HPP__

#include "clan_Enti.h"

#define CLANUSER_HELP "image\\clanImage\\Help\\clanuser_help.msg"

class CLANEntity_user: public CLANEntity
{
public:
	CLANEntity_user();
	~CLANEntity_user();

	int Init(int Life);
	int Clear();

	void Main();
	void RenderMain();
	void Draw();


	void menuInit(int num);
public:
	
	
	
	//int menuSu;
	//int nClanWon_Num;
	int nScl_Bar;           //스크롤바
	RECT scrIconPos;		//스크롤바의 움직이는아이콘위치설정
	char szClanUserMsg[256];
	char string[500];
	char szTemp[1024];
	BOOL bSubChip;
	
};


#endif 





#ifdef CE_USER_CPP

	class CLANEntity_user cClanUser;

#else

	extern class CLANEntity_user cClanUser;

#endif
