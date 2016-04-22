#define	_LANGUAGE_BRAZIL
#define	_LANGUAGE_BRAZIL_FIELDLEVEL

#if defined (_LOG_)
#include "stdio.h"
#define MAKELOG(description)					\
{												\
	FILE *fpc_log = NULL;						\
	fpc_log = fopen(szLOG_FILENAME,"a+t");		\
	if( fpc_log != NULL )						\
	{											\
		fprintf(fpc_log,"\n %s",(description)); \
		fclose(fpc_log);						\
		fpc_log = NULL;							\
	}											\
}
#else
#define MAKELOG(desciption)
#endif

#define	ATTACK_RATE		70
#define	DEFENSE_RATE	80

#define	_NEW_PARTRADE

#define	_WINMODE_DEBUG

#define	_PLAY_MORYON

#ifdef	_LANGUAGE_BRAZIL		
#define	_PACKET_PASS_XOR	0x6E
#endif

#define	_MODE_EXP64

#define	_SECURITY_UPDATE

#ifdef	_LANGUAGE_BRAZIL
#define _LANGUAGE_ENGLISH_NEXTUP

#define	_ON_MAP
#define _ON_PRIVATE_SHOP
#define	_ON_PET
#endif

// Utilidades globais.
#include "Log.h"