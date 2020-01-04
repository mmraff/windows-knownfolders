// Functions to query the WinAPI for individual properties of Known Folders

#if !defined(UNICODE)
#define UNICODE
#endif

#include <Shlobj.h>
#include "kfolders.h"
#include "kferrs.h"
#include "wstrutils.h" // used by getPathByKFId
#include <iostream> // for debugging

IKnownFolderManager* getKnownFolderManager(); // kflist.cc
IKnownFolder* getKnownFolder(IKnownFolderManager*, const KNOWNFOLDERID&); // ""

char* getCategory_win(unsigned kfIdx)
{
  if (kfIdx < 0) throw UsageError(ERANGE);
  if (kfIdx >= mmrkfolders::winFolderIds.size()) throw UsageError(ERANGE);

  const mmrkfolders::winFolderIdsItem& item = mmrkfolders::winFolderIds[kfIdx];
  char* pCat = NULL;

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();
  IKnownFolder* pKFInterface = getKnownFolder(pKFManager, item._guid);
  KF_CATEGORY kfCat;

  HRESULT hr = pKFInterface->GetCategory(&kfCat);
  if (SUCCEEDED(hr))
  {
    switch (kfCat) {
      case KF_CATEGORY_VIRTUAL: pCat = "virtual"; break;
      case KF_CATEGORY_FIXED:   pCat = "fixed"; break;
      case KF_CATEGORY_COMMON:  pCat = "common"; break;
      case KF_CATEGORY_PERUSER: pCat = "peruser"; break;
      default: ;
    }
  }
  pKFInterface->Release();
  pKFManager->Release();
  CoUninitialize();
  if (FAILED(hr))
  {
    throw APIError(hr, "IKnownFolder::GetCategory");
  }

  return pCat;
}

unsigned getFolderType_win(unsigned kfIdx)
{
  if (kfIdx < 0) throw UsageError(ERANGE);
  if (kfIdx >= mmrkfolders::winFolderIds.size()) throw UsageError(ERANGE);

  const mmrkfolders::winFolderIdsItem& item = mmrkfolders::winFolderIds[kfIdx];
  unsigned fTypeIdx = 0;

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();
  IKnownFolder* pKFInterface = getKnownFolder(pKFManager, item._guid);
  FOLDERTYPEID kfTypeId;

  HRESULT hr = pKFInterface->GetFolderType(&kfTypeId);
  if (SUCCEEDED(hr))
  {
    mmrkfolders::MapPGUIDtoIndex::iterator iter =
      mmrkfolders::fTypeIdToIndex.find(kfTypeId);
    if (iter != mmrkfolders::fTypeIdToIndex.end()) fTypeIdx = iter->second;
  }

  pKFInterface->Release();
  pKFManager->Release();
  CoUninitialize();
  // The funny thing is that if a folder has no folder type, we get a
  // failure HRESULT! That's no good. Just return 0 as default.
  //if (FAILED(hr))
  //{
  //  throw APIError(hr, "IKnownFolder::GetFolderType");
  //}

  return fTypeIdx;
}

unsigned getRedirCaps_win(unsigned kfIdx)
{
  if (kfIdx < 0) throw UsageError(ERANGE);
  if (kfIdx >= mmrkfolders::winFolderIds.size()) throw UsageError(ERANGE);

  const mmrkfolders::winFolderIdsItem& item = mmrkfolders::winFolderIds[kfIdx];

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();
  IKnownFolder* pKFInterface = getKnownFolder(pKFManager, item._guid);
  KF_REDIRECTION_CAPABILITIES kfRedirCaps;

  HRESULT hr = pKFInterface->GetRedirectionCapabilities(&kfRedirCaps);

  pKFInterface->Release();
  pKFManager->Release();
  CoUninitialize();
  if (FAILED(hr))
  {
    throw APIError(hr, "IKnownFolder::GetRedirectionCapabilities");
  }

  return (unsigned) kfRedirCaps;
}

// username parameter is for future development, for case: current-user-is-admin.
char* getPathByKFId(unsigned kfIdx, unsigned opts, char* username = NULL)
{
	HRESULT hr;
	PWSTR pwsPath = NULL;
  char* pmbPath = NULL;
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

	hr = SHGetKnownFolderPath(item._guid, filteredOpts, NULL, &pwsPath);
	CoUninitialize();

	if (SUCCEEDED(hr)) {
    try {
      pmbPath = getMultibyteStrCopy(pwsPath);
      CoTaskMemFree(pwsPath); // required cleanup
    }
    catch (...) {
      CoTaskMemFree(pwsPath);
      throw;
    }
	}

  return FAILED(hr) ? NULL : pmbPath;
}
