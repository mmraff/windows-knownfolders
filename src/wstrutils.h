#ifndef __WSTRUTILS_H__
#define __WSTRUTILS_H__

wchar_t* getWideStrCopy(const char*, size_t limit = 0);
char* getMultibyteStrCopy(const wchar_t*);
void freeWideStr(wchar_t*);
void freeMultibyteStr(char*);

#endif
