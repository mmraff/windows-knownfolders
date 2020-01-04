// Functions to query the WinAPI to look up Known Folders by string

#if !defined(UNICODE)
#define UNICODE
#endif

#include <Shlobj.h>
#include "kfolders.h"
#include "kferrs.h"
#include "wstrutils.h"
#include <iostream> // for debugging

IKnownFolderManager* getKnownFolderManager(); // kflist.cc
IKnownFolder* getKnownFolder(IKnownFolderManager*, const KNOWNFOLDERID&); // ""
unsigned trackNewFolderId(const KNOWNFOLDERID&); // ""

unsigned fetchIdByString(wchar_t* pStr, mmrkfolders::kfStrType strType)
{
  unsigned kfIdx = 0;
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();
  IKnownFolder* pKFInterface = NULL;

  HRESULT hr;
  char* pExtraMsg = NULL;
  if (strType == mmrkfolders::kfStrType::canonicalName)
  {
    pExtraMsg = "IKnownFolderManager::GetFolderByName";
    hr = pKFManager->GetFolderByName(pStr, &pKFInterface);
  }
  else if (strType == mmrkfolders::kfStrType::path)
  {
    pExtraMsg = "IKnownFolderManager::FindFolderFromPath";
    hr = pKFManager->FindFolderFromPath(
      pStr, FFFP_EXACTMATCH, &pKFInterface
    );
  }
  if (SUCCEEDED(hr))
  {
    KNOWNFOLDERID kfid;
    hr = pKFInterface->GetId(&kfid);
    pKFInterface->Release();

    if (FAILED(hr))
    {
      pKFManager->Release();
      CoUninitialize();
      throw APIError(hr, pExtraMsg);
    }

    using namespace mmrkfolders;
    MapPGUIDtoIndex::iterator iter = fidToIndex.find(kfid);
    kfIdx = (iter != fidToIndex.end()) ? iter->second : trackNewFolderId(kfid);
  }
  pKFManager->Release();
  CoUninitialize();

  return kfIdx;
}
