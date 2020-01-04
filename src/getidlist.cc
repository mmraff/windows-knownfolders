#if !defined(UNICODE)
#define UNICODE
#endif

#include <Shlobj.h>
//#include <memory.h>
#include "kfolders.h"
#include "wstrutils.h"
#include "kferrs.h"

// username parameter is for future development, for case: current-user-is-admin.

mmrkfolders::kfNumberedItem getIDListByKFId(
  unsigned kfIdx, unsigned opts, char* username = NULL
)
{
	HRESULT hr;
	PIDLIST_ABSOLUTE pIdL = NULL;
  mmrkfolders::kfNumberedItem result = { NULL, 0 };

  // Ensure that we control the path options passed to the API:
  unsigned filteredOpts = opts & (
    KF_FLAG_DEFAULT_PATH | KF_FLAG_DONT_VERIFY | KF_FLAG_NOT_PARENT_RELATIVE
#ifdef NTDDI_VERSION
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    | KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET
#endif
#endif
  );

  using namespace mmrkfolders;

  if (kfIdx < 0) throw UsageError(ERANGE);
    // but the caller should also catch this before the call!
  if (kfIdx >= winFolderIds.size()) throw UsageError(ERANGE);

  const winFolderIdsItem& item = winFolderIds[kfIdx];

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	hr = SHGetKnownFolderIDList(item._guid, filteredOpts, NULL, &pIdL);
	CoUninitialize();

	if (SUCCEEDED(hr)) {
    BYTE* pCurrShItem = (BYTE*)pIdL;
    size_t ushortSize = sizeof(USHORT);
    size_t length = 0;
    errno_t resCode = 0;
    const size_t LENGTH_LIMIT = 65536;
    // TODO: maybe that number is too arbitrary? Find out whether there is
    // an actual limit (roughly corresponding to MAX_PATH?)

    // We must traverse the ITEMIDs in the buffer to get the length
    // of the list, because the only place where size information is
    // available is in the length field (cb) of each item.
    // There is no delimiter except the two NULL bytes at the end of the list.
    while (true)
    {
      USHORT cb = *((USHORT*)pCurrShItem);
      if (cb == 0)
      {
        length += ushortSize;
        break;
      }
      else if (length + cb + ushortSize <= LENGTH_LIMIT)
      {
        length += cb;
        pCurrShItem += cb;
      }
      else {
        resCode = E_ABORT;
      }
    }

    if (length && resCode == 0)
    {
      result._name = (char*)malloc(length);
      if (result._name == NULL) resCode = ENOMEM;
      else
      {
        resCode = memcpy_s(result._name, length, pIdL, length);
        if (resCode != 0) free(result._name);
        else result._value = length;
      }
    }

    CoTaskMemFree(pIdL); // required cleanup
    if (resCode) throw SysError(resCode);
	}

  return result;
}
