#ifdef HANDES_EXPORTS
#define HANDES_API __declspec(dllexport)
#else
#define HANDES_API __declspec(dllimport)
#endif

HANDES_API BOOL FAR PASCAL DecryptFunc( LPTSTR cipher_text, LPTSTR plain_text );
HANDES_API BOOL FAR PASCAL EncryptFunc( LPTSTR plain_text, LPTSTR cipher_text, long varKey, unsigned char keygubun );