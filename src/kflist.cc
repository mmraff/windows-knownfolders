#if !defined(UNICODE)
#define UNICODE
#endif

#include <Shlobj.h>
#include "kfolders.h"
#include "kferrs.h"
#include "wstrutils.h"
#include <iostream> // for debugging
#include <wchar.h> // ditto
#define GUID_SIZE 128

// GUID_SIZE and this function are for debugging!
// Always call CoTaskMemFree on the non-NULL result when done using it!
PWSTR getGUIDString(REFGUID rguid)
{
  PWSTR pRetVal = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * GUID_SIZE);
  int resultSize;
  if (pRetVal)
	{
		resultSize = StringFromGUID2(rguid, pRetVal, GUID_SIZE);
	}
  return resultSize > 0 ? pRetVal : NULL;
}

// ---------- Building blocks ----------

// getKnownFolderManager: Get an instance of KnownFolderManager interface,
// which is needed for several really useful data-fetching methods.
// pre: a successful call of CoInitializeEx()
IKnownFolderManager* getKnownFolderManager()
{
  IKnownFolderManager* pMgr = NULL;
  HRESULT hr = CoCreateInstance(
    CLSID_KnownFolderManager,
    NULL, 
    CLSCTX_INPROC_SERVER,
    IID_PPV_ARGS(&pMgr)
  );
  if (FAILED(hr))
  {
    CoUninitialize();
    throw APIError(hr, "CoCreateInstance of KnownFolderManager");
  }
  return pMgr;
}

// getKnownFolder: Get an instance of KnownFolder interface, which is needed
// for really useful data-fetching methods specific to a folder.
// pre:
// * a successful call of CoInitializeEx()
// * successful acquisition of a KnownFolderManager interface instance
IKnownFolder* getKnownFolder(
  IKnownFolderManager* pKFManager, const KNOWNFOLDERID& kfid
)
{
  IKnownFolder *pKFInterface = NULL;
  HRESULT hr = pKFManager->GetFolder(kfid, &pKFInterface);
  if (FAILED(hr))
  {
    pKFManager->Release();
    CoUninitialize();
    throw APIError(hr, "IKnownFolderManager.GetFolder");
  }
  return pKFInterface;
}

// A convenient return type for a function that calls IKnownFolderManager::GetFolderIds(),
// which inconveniently has two in/out arguments, both important.
struct ResultGetFolderIds {
  KNOWNFOLDERID* kfidArray;
  unsigned kfidCount;
};

// Helper function for functions that retrieve data for *all* registered Known folders
struct ResultGetFolderIds
fetchAllKFIDs(IKnownFolderManager* pKFManager)
{
  struct ResultGetFolderIds result = { NULL, 0 };
  HRESULT hr = pKFManager->GetFolderIds(&(result.kfidArray), &(result.kfidCount));
  if (FAILED(hr))
  {
    pKFManager->Release();
    CoUninitialize();
    throw APIError(hr, "IKnownFolderManager.GetFolderIds");
  }
  return result;
}

// The given Known Folder Id is being seen for the first time, so we need to
// create an entry for it and return the associated index.
// This is factored out to a separate function because the details of extra
// known folder Id management have changed, and may yet change again, so it's
// best to keep them encapsulated.
unsigned trackNewFolderId(const KNOWNFOLDERID& kfid)
{
  unsigned folderIndex;
  using namespace mmrkfolders;

  folderIndex = winFolderIds.size();
  fidToIndex[kfid] = folderIndex;
  // We don't know what to name it yet; but the name is (so far) only used for the
  // export of constants that happens on module initialization anyway.
  // So an empty string is OK.
  winFolderIds.push_back({ "", fidToIndex.find(kfid)->first });

  return folderIndex;
}

// Extract the canonical name from the definition of the given known folder.
char* getKFDefName(
  IKnownFolderManager* pKFManager, const KNOWNFOLDERID& kfid
)
{
  char* pName;
  IKnownFolder *pKFInterface = getKnownFolder(pKFManager, kfid);

  // Get the definition data from the folder interface
  KNOWNFOLDER_DEFINITION apiDef;
  HRESULT hr = pKFInterface->GetFolderDefinition(&apiDef);
  if (FAILED(hr))
  {
    pKFInterface->Release();
    pKFManager->Release();
    CoUninitialize();
    throw APIError(hr, "IKnownFolder.GetFolderDefinition");
  }

  if (apiDef.pszName)  // Not stated to be optional, but just in case...
  {
    try {
      pName = getMultibyteStrCopy(apiDef.pszName);
    }
    catch (...) {
      FreeKnownFolderDefinitionFields(&apiDef);
      pKFInterface->Release();
      pKFManager->Release();
      CoUninitialize();
      throw;
    }
  }

  FreeKnownFolderDefinitionFields(&apiDef);
  pKFInterface->Release();
  return pName;
}

// Request the display name from the shellItem of the given known folder.
// Warning: the given known folder may not have a display name, or even an
// associated ShellItem. In those cases, we return NULL.
char* getShDisplayName(const KNOWNFOLDERID& kfid, unsigned opts)
{
  // Ensure that we control the path options passed to the API:
  KNOWN_FOLDER_FLAG filteredOpts = (KNOWN_FOLDER_FLAG)opts & (
    KF_FLAG_DEFAULT_PATH | KF_FLAG_DONT_VERIFY | KF_FLAG_NOT_PARENT_RELATIVE
#ifdef NTDDI_VERSION
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    | KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET
#endif
#endif
  );

  char* pName = NULL;
  IShellItem* pShItem = NULL;
  HRESULT hr = SHGetKnownFolderItem(
    kfid, filteredOpts, NULL, IID_PPV_ARGS(&pShItem)
  );
  if (FAILED(hr))
  {
    if (hr != 0x80070002L && hr != 0x80070003L)
    // Only if neither "file does not exist" nor "path not found"
    {
      CoUninitialize();
      throw APIError(hr, "SHGetKnownFolderItem");
    }
  }
  else
  {
    PWSTR pWStrName = NULL;
    hr = pShItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pWStrName);
    if (SUCCEEDED(hr) && pWStrName != NULL)
    {
      try {
        pName = getMultibyteStrCopy(pWStrName);
        CoTaskMemFree(pWStrName);
      }
      catch (...) {
        CoTaskMemFree(pWStrName);
        pShItem->Release();
        CoUninitialize();
        throw;
      }
    }
    pShItem->Release();
  }

  return pName;
}

// Helper function for functions that need the full definition of a Known Folder;
// populates the given KFDefinition struct.
// pre:
// * a successful call of CoInitializeEx()
// * obtained interface to KnownFolderManager
// * obtained registered KNOWNFOLDERID from KnownFolderManager
void requestKFDefinition(
  IKnownFolderManager* pKFManager,
  const KNOWNFOLDERID& kfid,
  mmrkfolders::KFDefinition& def
)
{
  IKnownFolder *pKFInterface = getKnownFolder(pKFManager, kfid);

  // Get the definition data from the folder interface
  KNOWNFOLDER_DEFINITION apiDef;
  HRESULT hr = pKFInterface->GetFolderDefinition(&apiDef);
  if (FAILED(hr))
  {
    pKFInterface->Release();
    pKFManager->Release();
    CoUninitialize();
    throw APIError(hr, "IKnownFolder.GetFolderDefinition");
  }

  switch (apiDef.category) {
    case KF_CATEGORY_VIRTUAL: def.category = "virtual"; break;
    case KF_CATEGORY_FIXED: def.category = "fixed"; break;
    case KF_CATEGORY_COMMON: def.category = "common"; break;
    case KF_CATEGORY_PERUSER: def.category = "peruser"; break;
    default: def.category = "";
  }

  using namespace mmrkfolders;

  if (apiDef.pszName) { // Not stated to be optional, but just in case...
    def.SetString(KFDefField::name, apiDef.pszName);
  }
  if (apiDef.pszDescription) { // Not stated to be optional, but just in case...
    def.SetString(KFDefField::description, apiDef.pszDescription);
  }
  if (apiDef.pszRelativePath) { // "Applies to common and per-user folders only."
    def.SetString(KFDefField::relativePath, apiDef.pszRelativePath);
  }
  if (apiDef.pszParsingName) { // "Applies to virtual folders only."
    def.SetString(KFDefField::parsingName, apiDef.pszParsingName);
  }
  if (apiDef.pszTooltip) {
    def.SetString(KFDefField::tooltip, apiDef.pszTooltip);
  }
  if (apiDef.pszLocalizedName) {
    def.SetString(KFDefField::localizedName, apiDef.pszLocalizedName);
  }
  if (apiDef.pszIcon) {
    def.SetString(KFDefField::icon, apiDef.pszIcon);
  }
  if (apiDef.pszSecurity) {
    def.SetString(KFDefField::security, apiDef.pszSecurity);
  }

  def.attributes = apiDef.dwAttributes;
  def.kfDefFlags = (unsigned) apiDef.kfdFlags;

  MapPGUIDtoIndex::iterator iter;

  iter = fTypeIdToIndex.find(apiDef.ftidType);
  def.folderTypeId = (iter != fTypeIdToIndex.end()) ? iter->second : 0;

  // "This value is optional if no value is provided for pszRelativePath";
  // Ambiguous. Is it required if there is a value for pszRelativePath?
  // Can it be set even if no pszRelativePath?
  iter = fidToIndex.find(apiDef.fidParent);
  def.fidParent = (iter != fidToIndex.end()) ?
                  iter->second : trackNewFolderId(apiDef.fidParent);

  iter = fidToIndex.find(kfid);
  def.fid = (iter != fidToIndex.end()) ?
            iter->second : trackNewFolderId(kfid);

  FreeKnownFolderDefinitionFields(&apiDef);
  pKFInterface->Release();
}
// ---------- END Building blocks ----------

// ---------- Interface functions ----------

char* getCanonicalName_win(unsigned kfIdx)
{
  if (kfIdx < 0) throw UsageError(ERANGE);
  if (kfIdx >= mmrkfolders::winFolderIds.size()) throw UsageError(ERANGE);

  const mmrkfolders::winFolderIdsItem& item = mmrkfolders::winFolderIds[kfIdx];

  //PWSTR sGUID = getGUIDString(item._guid);
  //wprintf(L"getFolderDefinition: GUID is %s\n", sGUID);
  //CoTaskMemFree(sGUID);

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();

  char* pName = getKFDefName(pKFManager, item._guid);
  pKFManager->Release();
  CoUninitialize();

  return pName;
}

char* getDisplayName_win(unsigned kfIdx, unsigned opts)
{
  if (kfIdx < 0) throw UsageError(ERANGE);
  if (kfIdx >= mmrkfolders::winFolderIds.size()) throw UsageError(ERANGE);

  const mmrkfolders::winFolderIdsItem& item = mmrkfolders::winFolderIds[kfIdx];

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  char* pName = getShDisplayName(item._guid, opts);

  CoUninitialize();

  return pName;
}

void getFolderDefinition(unsigned kfIdx, mmrkfolders::KFDefinition& def)
{
  if (kfIdx < 0) throw UsageError(ERANGE);
  if (kfIdx >= mmrkfolders::winFolderIds.size()) throw UsageError(ERANGE);

  const mmrkfolders::winFolderIdsItem& item = mmrkfolders::winFolderIds[kfIdx];

  //PWSTR sGUID = getGUIDString(item._guid);
  //wprintf(L"getFolderDefinition: GUID is %s\n", sGUID);
  //CoTaskMemFree(sGUID);

  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();

  requestKFDefinition(pKFManager, item._guid, def);

  pKFManager->Release();
  CoUninitialize();
}

// getAllKnownFolders: fetches all registered Known Folder Ids, and
// populates the given vector with their corresponding indices.
void getAllKnownFolders(std::vector<unsigned>& foundFIDs)
{
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();

  struct ResultGetFolderIds idData = fetchAllKFIDs(pKFManager);
  KNOWNFOLDERID* pRegisteredKFIDs = idData.kfidArray;
  unsigned countKFIDs = idData.kfidCount;

  foundFIDs.resize(countKFIDs);

  using namespace mmrkfolders;
  MapPGUIDtoIndex::iterator iter;
  unsigned folderIndex;
  for (unsigned i = 0; i < countKFIDs; ++i)
  {
    iter = fidToIndex.find(pRegisteredKFIDs[i]);
    if (iter != fidToIndex.end())
    {
      foundFIDs[i] = iter->second;
    }
    else
    {
      folderIndex = trackNewFolderId(pRegisteredKFIDs[i]);
      foundFIDs[i] = folderIndex;
    }
  }

  CoTaskMemFree(pRegisteredKFIDs);
  pKFManager->Release();
  CoUninitialize();
}

// getAllCanonicalNames: fetches the canonical names of all registered Known
// Folders, and populates the given vector with them.
void getAllCanonicalNames(std::vector<mmrkfolders::kfNumberedItem>& names)
{
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();

  struct ResultGetFolderIds idData = fetchAllKFIDs(pKFManager);
  KNOWNFOLDERID* pRegisteredKFIDs = idData.kfidArray;
  unsigned countKFIDs = idData.kfidCount;

  using namespace mmrkfolders;
  MapPGUIDtoIndex::iterator iter;
  unsigned folderIndex;

  for (unsigned i = 0; i < countKFIDs; ++i)
  {
    iter = fidToIndex.find(pRegisteredKFIDs[i]);
    folderIndex = (iter != fidToIndex.end()) ?
                  iter->second : trackNewFolderId(pRegisteredKFIDs[i]);
    names.push_back(
      { getKFDefName(pKFManager, pRegisteredKFIDs[i]), folderIndex }
    );
  }

  CoTaskMemFree(pRegisteredKFIDs);
  pKFManager->Release();
  CoUninitialize();
}

// getAllDisplayNames: fetches the ShellItem Display Names of all registered
// Known Folders, and populates the given vector with them.
void getAllDisplayNames(
  std::vector<mmrkfolders::kfNumberedItem>& names,
  unsigned opts
)
{
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();

  struct ResultGetFolderIds idData = fetchAllKFIDs(pKFManager);
  KNOWNFOLDERID* pRegisteredKFIDs = idData.kfidArray;
  unsigned countKFIDs = idData.kfidCount;

  using namespace mmrkfolders;
  MapPGUIDtoIndex::iterator iter;
  unsigned folderIndex;

  for (unsigned i = 0; i < countKFIDs; ++i)
  {
    char* pName = getShDisplayName(pRegisteredKFIDs[i], opts);
    // At first, I thought there was no reason to put an item on the list
    // if it has no display name. I reconsidered.
    //if (pName) {
      iter = fidToIndex.find(pRegisteredKFIDs[i]);
      folderIndex = (iter != fidToIndex.end()) ?
                    iter->second : trackNewFolderId(pRegisteredKFIDs[i]);
      names.push_back({ pName, folderIndex });
    //}
  }

  CoTaskMemFree(pRegisteredKFIDs);
  pKFManager->Release();
  CoUninitialize();
}

// getAllKnownFolderDetails: fetches the definitions of all registered Known
// Folders, and populates the given list (vector) with the bundles of data.
void getAllKnownFolderDetails(mmrkfolders::KFDefinitionList& defList)
{
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IKnownFolderManager* pKFManager = getKnownFolderManager();

  struct ResultGetFolderIds idData = fetchAllKFIDs(pKFManager);
  KNOWNFOLDERID* pRegisteredKFIDs = idData.kfidArray;
  unsigned countKFIDs = idData.kfidCount;

  defList.resize(countKFIDs);

  for (unsigned i = 0; i < countKFIDs; ++i)
  {
    requestKFDefinition(pKFManager, pRegisteredKFIDs[i], defList[i]);

    // DEBUG
    //PWSTR sGUID = getGUIDString(pRegisteredKFIDs[i]);
    //wprintf(L"getAllKnownFolderDetails: GUID is %s\n", sGUID);
    //CoTaskMemFree(sGUID);
  }

  CoTaskMemFree(pRegisteredKFIDs);
  pKFManager->Release();
  CoUninitialize();
}
// ---------- END Interface functions ----------
