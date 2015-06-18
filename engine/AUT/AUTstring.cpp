#include "AUTstring.h"

void deleteCRLF(string &str) { // delete 13 & 10
	if (str.at(0) == '\n' || str.at(0) == '\r')
		str.erase(0, 1);

	int ll = str.length();
	if (str.at(ll) == '\n' || str.at(ll) == '\r')
		str.erase(ll, 1);
	ll--;
	if (str.at(ll) == '\n' || str.at(ll) == '\r')
		str.erase(ll, 1);
}

void deleteCRLF(wchar_t *str) { // delete 13 & 10
	int shift = 0;
	if (str[0] == '\n' || str[0] == '\r') 
		shift = 1;
	if (str[1] == '\n' || str[1] == '\r') 
		shift = 2;

	int ll = wcslen(str) - 1;

	if (shift) {
		ll -= shift;

		for (int i = 0; i < ll + 1; i++)
			str[i] = str[i + shift];
	}

	if (str[ll] == '\n' || str[ll] == '\r')
		str[ll] = '\0';
	if (str[ll - 1] == '\n' || str[ll - 1] == '\r')
		str[ll - 1] = '\0';
}

void SplitFileNameC(IN const string &str, IN const char *szTok, OUT string &path, OUT string &filename)
{
	size_t found;
	found = str.find_last_of(szTok);
	path = str.substr(0, found);
	filename = str.substr(found + 1);
}

void SplitFileNameW(IN const wstring &str, IN const wchar_t *wszTok, OUT wstring &path, OUT wstring &filename)
{
	size_t found;
	found = str.find_last_of(wszTok);
	path = str.substr(0, found);
	filename = str.substr(found + 1);
}