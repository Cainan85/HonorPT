/*////////////////////////////////////////////////////////////////////////////////////////////////////////
 작성자 : 유광열
 날짜   : 2002.7.9
 수정일 : 2002.7.11
////////////////////////////////////////////////////////////////////////////////////////////////////////*/
#ifndef __JTS_H__
#define __JTS_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#define MAXTXTFILE 516

int GetTXT(char* FileName,char* SaveFile);
int CheckJTS(int num);
char* GetCharacterJTS(char* SaveFile, char* SaveCh, int num);
char* GetCharacter_ptr(char* SaveFile,int pos);
int CheckJTS_ptr(char* SaveFile,int pos);
int GetPointerPos(char* Savefile,int pos);
int IsJTS(char* SaveFile);

#endif