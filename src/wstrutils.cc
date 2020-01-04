#if !defined(UNICODE)
#define UNICODE
#endif

#include <stdlib.h> // for malloc & free
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "kferrs.h"

wchar_t* getWideStrCopy(const char* pSrc, size_t limit = 0)
{
  size_t sz = 0, retNum = 0, count = 0;
  errno_t resCode = 0;

  assert(pSrc != NULL && "Null passed as src pointer!");

  resCode = mbstowcs_s(&sz, NULL, 0, pSrc, 0); // to get required buffer size
  if (resCode != 0)
  {
    if (resCode == EILSEQ) throw UsageError(EILSEQ);
    else throw SysError(resCode);
  }

  if (limit && limit < sz)
  {
    sz = limit + 1;
    count = _TRUNCATE;
  }
  else count = sz - 1;

  wchar_t* pDest = (wchar_t*)malloc(sizeof(wchar_t) * sz);
  if (pDest == NULL) throw SysError(ENOMEM);

  // We do all we can to avoid an error in the following, so if something goes
  // wrong, we throw mystery value resCode:
  resCode = mbstowcs_s(
    &retNum, // [out]
    pDest,   // [out]
    sz,      // [in]
    pSrc,    // [in]
    count    // [in]
  );
  if (resCode != 0) {
    free(pDest);
    throw SysError(resCode);
  }

  return pDest;
}

char* getMultibyteStrCopy(const wchar_t* pSrc)
{
  size_t sz = 0, retNum = 0;
  errno_t resCode = 0;

  assert(pSrc != NULL && "Null passed as src pointer!");

  resCode = wcstombs_s(&sz, NULL, 0, pSrc, 0); // to get required buffer size
  if (resCode != 0) throw SysError(resCode);
  
  char* pDest = (char*)malloc(sz);
  if (pDest == NULL) throw SysError(ENOMEM);

  resCode = wcstombs_s(
    &retNum, // [out]
    pDest,   // [out]
    sz,      // [in]
    pSrc,    // [in]
    sz       // [in]
  );
  if (resCode != 0) {
    free(pDest);
    throw SysError(resCode);
  }

  return pDest;
}

void freeWideStr(wchar_t* pStr) { if (pStr) free(pStr); }

void freeMultibyteStr(char* pStr) { if (pStr) free(pStr); }

// POSTSCRIPT about STL wstring conversion features:
// Despite the apparent promise of simplicity of some examples, e.g., at
// https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
// codecvt_utf8 and wstring_convert are deprecated in C++17, even though they
// were only just introduced in C++11... and for good reason.
// Search the web for "wstring_convert sucks" or "codecvt trash" to see serious
// complaints that persuaded me not to use those.

// For more study, see
// https://github.com/OpenMPT/openmpt/blob/master/common/mptString.cpp#L842

