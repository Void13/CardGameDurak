#pragma once
#ifndef H__AUTString
#define H__AUTString

#include <windows.h>
#include <string.h>

class AUTStringC{
public:
	AUTStringC(){
		szStr = NULL;
		dwLen = 0;
	}
	AUTStringC(const AUTStringC &Inst){
		szStr = Inst.szStr;
		dwLen = Inst.dwLen;
	}
	AUTStringC(char *p){
		assign(p);
	}
	AUTStringC(char *szStr, DWORD dwLen): szStr(szStr), dwLen(dwLen){}
	~AUTStringC(){
		delete[] szStr;
		szStr = NULL;
		dwLen = 0;
	}
	void assign(char *p){
		szStr = p;
		dwLen = strlen(p);
	}
	int cmp(const AUTStringC &Str1, const AUTStringC &Str2) const {
		DWORD dwMinLen;
		if (Str1.dwLen > Str2.dwLen)
			dwMinLen = Str2.dwLen;
		else
			dwMinLen = Str1.dwLen;

		return strncmp(Str1.szStr, Str2.szStr, dwMinLen);
	}
	bool operator ()(const AUTStringC &Str1, const AUTStringC &Str2) const {
		if (cmp(Str1, Str2)) return false;
		return true;
	}
	bool operator >(const AUTStringC &Str) const {
		return cmp(*this, Str) > 0;
	}
	bool operator <(const AUTStringC &Str) const {
		return cmp(*this, Str) < 0;
	}
	bool operator >=(const AUTStringC &Str) const {
		return cmp(*this, Str) >= 0;
	}
	bool operator <=(const AUTStringC &Str) const {
		return cmp(*this, Str) <= 0;
	}
	void operator =(char *p){
		assign(p);
	}
	char				*szStr;
	DWORD				dwLen;
};

#include <string>
#include <xstring>

using namespace std;

void deleteCRLF(string &str);

void deleteCRLF(wchar_t *str);

void SplitFileNameC(IN const string &str, IN const char *szTok, OUT string &path, OUT string &filename);

void SplitFileNameW(IN const wstring &str, IN const wchar_t *wszTok, OUT wstring &path, OUT wstring &filename);

#ifdef SplitFileName
#	undef SplitFileName
#endif

#ifdef UNICODE
#	define SplitFileName SplitFileNameW
#else
#	define SplitFileName SplitFileNameC
#endif

#endif