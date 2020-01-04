#include <ShObjIdl.h> // for KF_DEFINITION_FLAGS; also has KNOWNFOLDER_DEFINITION
#include <winsdkver.h> // WINVER_MAXVER, 
#include <sdkddkver.h> // NTDDI_*
#include <memory.h>  // memset
#include <assert.h>
#include "wstrutils.h"
#include "kfolders.h"
#include <iostream>

std::vector<mmrkfolders::winFolderIdsItem>
mmrkfolders::winFolderIds = {
  { "UNKNOWN", GUID_NULL },
  { "AddNewPrograms", FOLDERID_AddNewPrograms },
  { "AdminTools", FOLDERID_AdminTools },
  { "CDBurning", FOLDERID_CDBurning },
  { "ChangeRemovePrograms", FOLDERID_ChangeRemovePrograms },
  { "CommonAdminTools", FOLDERID_CommonAdminTools },
  { "CommonOEMLinks", FOLDERID_CommonOEMLinks },
  { "CommonPrograms", FOLDERID_CommonPrograms },
  { "CommonStartMenu", FOLDERID_CommonStartMenu },
  { "CommonStartup", FOLDERID_CommonStartup },
  { "CommonTemplates", FOLDERID_CommonTemplates },
  { "ComputerFolder", FOLDERID_ComputerFolder },
  { "ConnectionsFolder", FOLDERID_ConnectionsFolder },
  { "ControlPanelFolder", FOLDERID_ControlPanelFolder },
  { "Cookies", FOLDERID_Cookies },
  { "Desktop", FOLDERID_Desktop },
  { "Documents", FOLDERID_Documents },
  { "Downloads", FOLDERID_Downloads },
  { "Favorites", FOLDERID_Favorites },
  { "Fonts", FOLDERID_Fonts },
  { "History", FOLDERID_History },
  { "InternetCache", FOLDERID_InternetCache },
  { "InternetFolder", FOLDERID_InternetFolder },
  { "Links", FOLDERID_Links },
  { "LocalAppData", FOLDERID_LocalAppData },
  { "LocalAppDataLow", FOLDERID_LocalAppDataLow },
  { "LocalizedResourcesDir", FOLDERID_LocalizedResourcesDir },
  { "Music", FOLDERID_Music },
  { "NetHood", FOLDERID_NetHood },
  { "NetworkFolder", FOLDERID_NetworkFolder },
  { "Pictures", FOLDERID_Pictures },
  { "Playlists", FOLDERID_Playlists },
  { "PrintersFolder", FOLDERID_PrintersFolder },
  { "PrintHood", FOLDERID_PrintHood },
  { "Profile", FOLDERID_Profile },
  { "ProgramData", FOLDERID_ProgramData },
  { "ProgramFiles", FOLDERID_ProgramFiles },
  { "ProgramFilesCommon", FOLDERID_ProgramFilesCommon },
  { "Programs", FOLDERID_Programs },
  { "PublicDesktop", FOLDERID_PublicDesktop },
  { "PublicDocuments", FOLDERID_PublicDocuments },
  { "PublicMusic", FOLDERID_PublicMusic },
  { "PublicPictures", FOLDERID_PublicPictures },
  { "PublicVideos", FOLDERID_PublicVideos },
  { "QuickLaunch", FOLDERID_QuickLaunch },
  { "Recent", FOLDERID_Recent },
  { "RecycleBinFolder", FOLDERID_RecycleBinFolder },
  { "ResourceDir", FOLDERID_ResourceDir },
  { "RoamingAppData", FOLDERID_RoamingAppData },
  { "SampleMusic", FOLDERID_SampleMusic },
  { "SamplePictures", FOLDERID_SamplePictures },
  { "SampleVideos", FOLDERID_SampleVideos },
  { "SavedSearches", FOLDERID_SavedSearches },
  { "SEARCH_CSC", FOLDERID_SEARCH_CSC },
  { "SearchHome", FOLDERID_SearchHome },
  { "SEARCH_MAPI", FOLDERID_SEARCH_MAPI },
  { "SendTo", FOLDERID_SendTo },
  { "StartMenu", FOLDERID_StartMenu },
  { "Startup", FOLDERID_Startup },
  { "System", FOLDERID_System },
  { "SystemX86", FOLDERID_SystemX86 },
  { "Templates", FOLDERID_Templates },
  { "UsersFiles", FOLDERID_UsersFiles },
  { "Videos", FOLDERID_Videos },
  { "Windows", FOLDERID_Windows },
  { "ProgramFilesX86", FOLDERID_ProgramFilesX86 },
  { "ProgramFilesCommonX86", FOLDERID_ProgramFilesCommonX86 }

#ifdef _WIN64
  ,
  { "ProgramFilesX64", FOLDERID_ProgramFilesX64 },
  { "ProgramFilesCommonX64", FOLDERID_ProgramFilesCommonX64 }
#endif

#ifdef WINVER_MAXVER
#if (WINVER_MAXVER >= 0x0600)
// Introduced in Windows Vista:
  ,
  { "AppUpdates", FOLDERID_AppUpdates },
  { "ConflictFolder", FOLDERID_ConflictFolder },
  { "Contacts", FOLDERID_Contacts },
  { "GameTasks", FOLDERID_GameTasks },
  { "OriginalImages", FOLDERID_OriginalImages },
  { "PhotoAlbums", FOLDERID_PhotoAlbums },
  { "Public", FOLDERID_Public },
  { "PublicDownloads", FOLDERID_PublicDownloads },
  { "PublicGameTasks", FOLDERID_PublicGameTasks },
  { "SamplePlaylists", FOLDERID_SamplePlaylists },
  { "SavedGames", FOLDERID_SavedGames },
  { "SyncManagerFolder", FOLDERID_SyncManagerFolder },
  { "SyncResultsFolder", FOLDERID_SyncResultsFolder },
  { "SyncSetupFolder", FOLDERID_SyncSetupFolder },
  { "UserProfiles", FOLDERID_UserProfiles }
#endif

#if (WINVER_MAXVER >= 0x0601)
// Introduced in Windows 7:
  ,
  { "DeviceMetadataStore", FOLDERID_DeviceMetadataStore },
  { "DocumentsLibrary", FOLDERID_DocumentsLibrary },
  { "HomeGroup", FOLDERID_HomeGroup },
  { "ImplicitAppShortcuts", FOLDERID_ImplicitAppShortcuts },
  { "Libraries", FOLDERID_Libraries },
  { "MusicLibrary", FOLDERID_MusicLibrary },
  { "PicturesLibrary", FOLDERID_PicturesLibrary },
  { "PublicLibraries", FOLDERID_PublicLibraries },
  { "PublicRingtones", FOLDERID_PublicRingtones },
  { "RecordedTVLibrary", FOLDERID_RecordedTVLibrary },
  { "Ringtones", FOLDERID_Ringtones },
  { "SidebarDefaultParts", FOLDERID_SidebarDefaultParts },
  { "SidebarParts", FOLDERID_SidebarParts },
  { "UserPinned", FOLDERID_UserPinned },
  { "UserProgramFiles", FOLDERID_UserProgramFiles },
  { "UserProgramFilesCommon", FOLDERID_UserProgramFilesCommon },
  { "UsersLibraries", FOLDERID_UsersLibraries },
  { "VideosLibrary", FOLDERID_VideosLibrary }
#else
// Not used & undefined/unsupported as of Windows 7
  ,
  { "RecordedTV", FOLDERID_RecordedTV },
  { "TreeProperties", FOLDERID_TreeProperties }
#endif

#if (WINVER_MAXVER >= 0x0602)
// Introduced in Windows 8:
  ,
  { "AccountPictures", FOLDERID_AccountPictures },
  { "ApplicationShortcuts", FOLDERID_ApplicationShortcuts },
  { "AppsFolder", FOLDERID_AppsFolder },
  { "HomeGroupCurrentUser", FOLDERID_HomeGroupCurrentUser },
  { "PublicUserTiles", FOLDERID_PublicUserTiles },
  { "RoamedTileImages", FOLDERID_RoamedTileImages },
  { "RoamingTiles", FOLDERID_RoamingTiles },
  { "Screenshots", FOLDERID_Screenshots }
#endif

#if (WINVER_MAXVER >= 0x0603)
// Introduced in Windows 8.1:
  ,
  { "CameraRoll", FOLDERID_CameraRoll },
  { "SearchHistory", FOLDERID_SearchHistory },
  { "SearchTemplates", FOLDERID_SearchTemplates },
  { "SkyDrive", FOLDERID_SkyDrive },
  { "SkyDriveCameraRoll", FOLDERID_SkyDriveCameraRoll },
  { "SkyDriveDocuments", FOLDERID_SkyDriveDocuments },
  { "SkyDrivePictures", FOLDERID_SkyDrivePictures }
#endif

#if (WINVER_MAXVER >= 0x0A00)
// Introduced in Windows 10 (build 10069), but UNDOCUMENTED!!!
// sources:
// https://naughter.wordpress.com/2015/05/15/changes-in-the-windows-10-sdk-compared-to-windows-8-1-part-one/
// https://naughter.wordpress.com/2015/05/24/changes-in-the-windows-10-sdk-compared-to-windows-8-1-part-two/
  ,
  { "StartMenuAllPrograms", FOLDERID_StartMenuAllPrograms },
  { "OneDrive", FOLDERID_OneDrive },
  { "CameraRollLibrary", FOLDERID_CameraRollLibrary },
  { "SavedPictures", FOLDERID_SavedPictures },
  { "SavedPicturesLibrary", FOLDERID_SavedPicturesLibrary },
  { "RetailDemo", FOLDERID_RetailDemo },
  { "DevelopmentFiles", FOLDERID_DevelopmentFiles },
  { "AppCaptures", FOLDERID_AppCaptures },
  { "LocalDocuments", FOLDERID_LocalDocuments },
  { "LocalPictures", FOLDERID_LocalPictures },
  { "LocalVideos", FOLDERID_LocalVideos },
  { "LocalMusic", FOLDERID_LocalMusic },
  { "LocalDownloads", FOLDERID_LocalDownloads },
// MS documentation says the next one was "Introduced in Windows 10, version 1703"; but that's false!
// See part two of the above links
  { "Objects3D", FOLDERID_Objects3D }

/* MORE UNDOCUMENTED!
  FOLDERID_RecordedCalls
   * there's some evidence this existed as far back as build 10240 (ver 1507, NTDDI_WIN10); see
      https://github.com/tpn/winsdk-10/blob/master/Include/10.0.10240.0/um/KnownFolders.h
*/
#endif

#ifdef NTDDI_VERSION
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
// These are undocumented! But see
// https://naughter.wordpress.com/2017/02/28/changes-in-the-windows-v10-0-15021-sdk-compared-to-windows-v10-0-14393-sdk-part-two/
  , NTDDI_VERSION,
  { "AllAppMods", FOLDERID_AllAppMods },
  { "CurrentAppMods", FOLDERID_CurrentAppMods }
#endif

#if defined(NTDDI_WIN10_RS3) && (NTDDI_VERSION >= NTDDI_WIN10_RS3)
// Introduced in Windows 10, version 1709:
  ,
  { "AppDataDesktop", FOLDERID_AppDataDesktop },
  { "AppDataDocuments", FOLDERID_AppDataDocuments },
  { "AppDataFavorites", FOLDERID_AppDataFavorites },
  { "AppDataProgramData", FOLDERID_AppDataProgramData }
#endif

#if (WINVER_MAXVER < 0x0A00) || (defined(NTDDI_WIN10_RS4) && NTDDI_VERSION <= NTDDI_WIN10_RS4)
// Deprecated in Windows 10, version 1803 and later versions
// (version 1803 => OS build 17134.556)
  ,{ "Games", FOLDERID_Games }
#endif
#endif // ifdef NTDDI_VERSION

#endif // ifdef WINVER_MAXVER
};

mmrkfolders::MapPGUIDtoIndex mmrkfolders::fidToIndex;


const mmrkfolders::winFolderIdsItem
mmrkfolders::winFolderTypes[] = {
  { "NONE", GUID_NULL },

  { "CompressedFolder", FOLDERTYPEID_CompressedFolder },
  { "Contacts", FOLDERTYPEID_Contacts },
  { "ControlPanelCategory", FOLDERTYPEID_ControlPanelCategory },
  { "ControlPanelClassic", FOLDERTYPEID_ControlPanelClassic },
  { "Documents", FOLDERTYPEID_Documents },
  { "Games", FOLDERTYPEID_Games },
  { "Invalid", FOLDERTYPEID_Invalid },
  { "NetworkExplorer", FOLDERTYPEID_NetworkExplorer },
  { "Pictures", FOLDERTYPEID_Pictures },
  { "Printers", FOLDERTYPEID_Printers },
  { "RecycleBin", FOLDERTYPEID_RecycleBin },
  { "SoftwareExplorer", FOLDERTYPEID_SoftwareExplorer },
  { "UserFiles", FOLDERTYPEID_UserFiles }

#ifdef WINVER_MAXVER
#if (WINVER_MAXVER >= 0x0601)
  // Introduced in Windows 7
  ,
  { "Communications", FOLDERTYPEID_Communications },
  { "Generic", FOLDERTYPEID_Generic },
  { "GenericLibrary", FOLDERTYPEID_GenericLibrary },
  { "GenericSearchResults", FOLDERTYPEID_GenericSearchResults },
  { "Music", FOLDERTYPEID_Music },
  { "OpenSearch", FOLDERTYPEID_OpenSearch },
  { "OtherUsers", FOLDERTYPEID_OtherUsers },
  { "PublishedItems", FOLDERTYPEID_PublishedItems },
  { "RecordedTV", FOLDERTYPEID_RecordedTV },
  { "SavedGames", FOLDERTYPEID_SavedGames },
  { "SearchConnector", FOLDERTYPEID_SearchConnector },
  { "Searches", FOLDERTYPEID_Searches },
  { "SearchHome", FOLDERTYPEID_SearchHome },
  { "StartMenu", FOLDERTYPEID_StartMenu },
  { "UsersLibraries", FOLDERTYPEID_UsersLibraries },
  { "Videos", FOLDERTYPEID_Videos }
#else
  // The following Not Supported in Windows 7 and later systems
  ,
  { "Library", FOLDERTYPEID_Library },
  { "MusicDetails", FOLDERTYPEID_MusicDetails },
  { "MusicIcons", FOLDERTYPEID_MusicIcons },
  { "NotSpecified", FOLDERTYPEID_NotSpecified }
#endif

#if (WINVER_MAXVER >= 0x0602)
  // Introduced in Windows 8
  ,{ "AccountPictures", FOLDERTYPEID_AccountPictures }
#endif

#if (WINVER_MAXVER >= 0x0603)
  // Introduced in Windows 8.1
  ,
  { "StorageProviderGeneric", FOLDERTYPEID_StorageProviderGeneric },
  { "StorageProviderDocuments", FOLDERTYPEID_StorageProviderDocuments },
  { "StorageProviderPictures", FOLDERTYPEID_StorageProviderPictures },
  { "StorageProviderMusic", FOLDERTYPEID_StorageProviderMusic },
  { "StorageProviderVideos", FOLDERTYPEID_StorageProviderVideos }
#endif
#endif // ifdef WINVER_MAXVER
};

const unsigned mmrkfolders::countWinFolderTypes =
  sizeof(mmrkfolders::winFolderTypes) / sizeof(mmrkfolders::winFolderIdsItem);

mmrkfolders::MapPGUIDtoIndex mmrkfolders::fTypeIdToIndex;

const mmrkfolders::kfNumberedItem
mmrkfolders::kfDefFlag[] = {
    { "LOCAL_REDIRECT_ONLY", KFDF_LOCAL_REDIRECT_ONLY }
  , { "ROAMABLE", KFDF_ROAMABLE }
  , { "PRECREATE", KFDF_PRECREATE }

#ifdef WINVER_MAXVER
#if (WINVER_MAXVER >= 0x0601)
  // Introduced in Windows 7
  , { "STREAM", KFDF_STREAM }
  , { "PUBLISH_EXPANDED_PATH", KFDF_PUBLISHEXPANDEDPATH }
#endif
#if (WINVER_MAXVER >= 0x0603)
  // Introduced in Windows 8.1
  , { "NO_REDIRECT_UI", KFDF_NO_REDIRECT_UI }
#endif
#endif // ifdef WINVER_MAXVER
};

const unsigned mmrkfolders::countKFDefFlagItems =
  sizeof(mmrkfolders::kfDefFlag) / sizeof(mmrkfolders::kfNumberedItem);

const mmrkfolders::kfNumberedItem
mmrkfolders::kfRedirCaps[] = {
  { "ALLOW_ALL", KF_REDIRECTION_CAPABILITIES_ALLOW_ALL },
  { "REDIRECTABLE", KF_REDIRECTION_CAPABILITIES_REDIRECTABLE },
  { "DENY_ALL", KF_REDIRECTION_CAPABILITIES_DENY_ALL },
  { "DENY_POLICY_REDIRECTED", KF_REDIRECTION_CAPABILITIES_DENY_POLICY_REDIRECTED },
  { "DENY_POLICY", KF_REDIRECTION_CAPABILITIES_DENY_POLICY },
  { "DENY_PERMISSIONS", KF_REDIRECTION_CAPABILITIES_DENY_PERMISSIONS }
};

const unsigned mmrkfolders::countRedirCapsItems =
  sizeof(mmrkfolders::kfRedirCaps) / sizeof(mmrkfolders::kfNumberedItem);

mmrkfolders::KFDefinition::KFDefinition()
{
  memset(this, NULL, sizeof(struct mmrkfolders::KFDefinition));
}

mmrkfolders::KFDefinition::~KFDefinition()
{
  if (this->name)          freeMultibyteStr(this->name);
  if (this->description)   freeMultibyteStr(this->description);
  if (this->relativePath)  freeMultibyteStr(this->relativePath);
  if (this->parsingName)   freeMultibyteStr(this->parsingName);
  if (this->tooltip)       freeMultibyteStr(this->tooltip);
  if (this->localizedName) freeMultibyteStr(this->localizedName);
  if (this->icon)          freeMultibyteStr(this->icon);
  if (this->security)      freeMultibyteStr(this->security);
}

void mmrkfolders::KFDefinition::SetString(
  mmrkfolders::KFDefField field, const wchar_t* valueW
)
{
  switch (field)
  {
    // The required cases. Assumed required where "optional" is not specified in the API doc!
    // Note: 'category' won't be a wide string, so it's not allowed here.
    case mmrkfolders::KFDefField::name:
      this->name = getMultibyteStrCopy(valueW); break;
    case mmrkfolders::KFDefField::description:
      this->description = getMultibyteStrCopy(valueW); break;

    // The optional cases:
    case mmrkfolders::KFDefField::relativePath:
      this->relativePath = valueW ? getMultibyteStrCopy(valueW) : NULL; break;
    case mmrkfolders::KFDefField::parsingName: // API doc says "virtual folders only"
      this->parsingName = valueW ? getMultibyteStrCopy(valueW) : NULL; break;
    case mmrkfolders::KFDefField::tooltip:
      this->tooltip = valueW ? getMultibyteStrCopy(valueW) : NULL; break;
    case mmrkfolders::KFDefField::localizedName:
      this->localizedName = valueW ? getMultibyteStrCopy(valueW) : NULL; break;
    case mmrkfolders::KFDefField::icon:
      this->icon = valueW ? getMultibyteStrCopy(valueW) : NULL; break;
    case mmrkfolders::KFDefField::security:
      this->security = valueW ? getMultibyteStrCopy(valueW) : NULL; break;
    default:
      assert(NULL && "Invalid fieldname passed to KFDefinition::SetString()");
  }
}


/*
int main(int argc, char* argv[])
{
  // Load the folderId-to-Index lookup table
  unsigned idx = 0;
  for (auto item : mmrkfolders::winFolderIds)
  {
    mmrkfolders::fidToIndex[item._guid] = idx++;
  }
  std::cout << "Inserted " << idx << " items.\n";
  std::cout << "Contains " << mmrkfolders::fidToIndex.size() << " items.\n";
  std::cout << "UsersFiles is at index "
            << mmrkfolders::fidToIndex.[FOLDERID_UsersFiles] << "\n";
  return 0;
}
*/
