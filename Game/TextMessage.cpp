#include "TextMessage.h"

#include "language.h"

#ifdef	_LANGUAGE_KOREAN
//////////////////// 한국어 - 기본 ////////////////////////////
#include "Korean\\k_TextMessage.h"
#include "Korean\\k_QuestMsg.h"
#endif

#ifdef	_LANGUAGE_CHINESE
//중국어
#include "Chinese\\C_TextMessage.h"
#include "Chinese\\C_QuestMsg.h"
#endif

#ifdef	_LANGUAGE_JAPANESE
//일본어
#include "Japanese\\J_TextMessage.h"
#include "Japanese\\J_QuestMsg.h"
#endif

#ifdef	_LANGUAGE_TAIWAN
//자유중국
#include "Taiwan\\T_TextMessage.h"
#include "Taiwan\\T_QuestMsg.h"
#endif

#ifdef	_LANGUAGE_ENGLISH
//영국어
#include "English\\E_TextMessage.h"
#include "English\\E_QuestMsg.h"
#endif

#ifdef	_LANGUAGE_THAI
//태국어
#include "THAI\\Th_TextMessage.h"
#include "THAI\\Th_QuestMsg.h"
#endif

//베트남어
#ifdef _LANGUAGE_VEITNAM
#include "VEITNAM\\V_TextMessage.h"
#include "VEITNAM\\V_QuestMsg.h"
#endif

//브라질어
#ifdef _LANGUAGE_BRAZIL
#include "BRAZIL\\B_TextMessage.h"
#include "BRAZIL\\B_QuestMsg.h"
#endif

//아르헨티나어
#ifdef _LANGUAGE_ARGENTINA
#include "ARGENTINA\\A_TextMessage.h"
#include "ARGENTINA\\A_QuestMsg.h"
#endif