/*----------------------------------------------------------------------------*
*	파일명 :  sinPetMessage.cpp
*	하는일 :  먼가 도움을 준다 
*	작성일 :  최종업데이트 3월 
*	적성자 :  박상열 
*-----------------------------------------------------------------------------*/	

#include "sinLinkHeader.h"

#include "..\\Language.h"

#ifdef	_LANGUAGE_KOREAN


//채팅 창
char *pet_ChatBox[] = {
	"대화내용을 볼수있어염",
	"무엇이 굼금하셈?",
	"저랑 이야기 할까요?",
	"말하고싶은 내용을 입력해주셈",
	"",
};

//채팅창 '전체'탭
char *pet_ChatTab_All[] = {
	"",
	"",
	"",
	"",
};

//채팅창 '클랜'탭
char *pet_ChatTab_Clan[] = {
	"",
	"",
	"",
	"",
};

//채팅창 '동료'탭
char *pet_ChatTab_Party[] = {
	"",
	"",
	"",
	"",
};

//채팅창 '거래'탭
char *pet_ChatTab_Trade[] = {
	"",
	"",
	"",
	"",
};

//채팅창 '귓말'탭
char *pet_ChatTab_Whisper[] = {
	"",
	"",
	"",
	"",
};

//근력 바
char *pet_Bar_Stamina[] = {
	"",
	"",
	"",
	"",
};

//생명력 바
char *pet_Bar_Life[] = {
	"",
	"",
	"",
	"",
};

//왼쪽 스킬 단축키 창
char *pet_LeftSkill[] = {
	"",
	"",
	"",
	"",
};

//오른쪽 스킬 단축키창
char *pet_RightSkill[] = {
	"",
	"",
	"",
	"",
};

//기력 바
char *pet_Bar_Mana[] = {
	"",
	"",
	"",
	"",
};

//밤낮 표시
char *pet_DayandNight[] = {
	"",
	"",
	"",
	"",
};

//경험치 바
char *pet_Bar_Exp[] = {
	"",
	"",
	"",
	"",
};

//경험치 숫자
char *pet_Exp_Num[] = {
	"",
	"",
	"",
	"",
};

//물약 단축아이콘
char *pet_Potion_ShortKey[] = {
	"",
	"",
	"",
	"",
};

//뛰기 걷기
char *pet_RunAndWalk[] = {
	"",
	"",
	"",
	"",
};

//카메라 수동
char *pet_Camera_Hand[] = {
	"",
	"",
	"",
	"",
};

//카메라 자동
char *pet_Camera_Auto[] = {
	"",
	"",
	"",
	"",
};

//카메라 고정
char *pet_Camera_Fix[] = {
	"",
	"",
	"",
	"",
};

//미니 맵 창
char *pet_Mini_Map[] = {
	"",
	"",
	"",
	"",
};

//캐릭터 정보 아이콘
char *pet_CarStatus[] = {
	"",
	"",
	"",
	"",
};

//인벤토리 정보 아이콘
char *pet_InvenTory[] = {
	"",
	"",
	"",
	"",
};

//마법 / 기술 아이콘
char *pet_Skill[] = {
	"",
	"",
	"",
	"",
};

//동료 아이콘
char *pet_Party[] = {
	"",
	"",
	"",
	"",
};

//퀘스트 아이콘
char *pet_Quest[] = {
	"",
	"",
	"",
	"",
};


//시스템 아이콘
char *pet_System[] = {
	"",
	"",
	"",
	"",
};

//인벤토리내 아이템
char *pet_InvenTory_Item[] = {
	"",
	"",
	"",
	"",
};


//바닥에 떨어진 아이템
char *pet_Drop_Item[] = {
	"",
	"",
	"",
	"",
};

//몬스터 
char *pet_Monster[] = {
	"",
	"",
	"",
	"",
};

//Pet 드레곤 
char *pet_Message[] = {
	"",
	"",
	"",
	"",
};

/////////////////// 전투 메세지
char *pet_Battle_Message[] = {
	"열심히 해보셈",
	"힘내셈!!",
	"노라줘 -0- ",
	""
};

/////////////////// 일반 메세지
char *pet_Normal_Message[] = {
	"랄랄라~",
	"아싸 날조타",
	"워3 한판해여",
	""
};

/////////////////// 레벨업 메세지
char *pet_Level_Message[][10] = {
	//Level 2
	{	"축하합니다 ~ 드디어 레법업이네여 이제겨우 프리스톤 대륙 용사의 면모가 보이기 시작하는군요",
		"힘 : 공격력 , 근력 , 회복력 , 생명력 , 흡수력 , 무게 ",
		"정신 :  명중력 , 공격력 , 기력 , 기력 회복속도 ",
		"",
	},

	//Level 3
	{	"축하합니다 ~ 레벨 3이군여 잇힝~",
		"조케똬~ ",
		"부럽똬~",
		"",
	},

};

#endif