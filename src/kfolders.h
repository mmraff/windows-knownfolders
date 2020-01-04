#ifndef __MMR_KFOLDERS_H__
#define __MMR_KFOLDERS_H__

#include <guiddef.h>
#include <ShlGuid.h>
#include <ShlObj.h>
#include <sdkddkver.h> // NTDDI_*
#include <bitset>
#include <unordered_map>
#include <vector>

// Must leave the following here in the header file, even though it looks like
// the CC file is the only place that GUID_NULL is used, because we're including
// GUID_NULL in the list of ids for those potential cases where the query gives
// an undefined result. In other words, it *will* get used elsewhere.
#ifndef GUID_NULL
DEFINE_GUID(GUID_NULL, 0x00L, 0, 0, 0,0,0,0,0,0,0,0);
#endif

namespace mmrkfolders {

  enum kfListType : unsigned {
    folderIds,
    canonicalNames,
    displayNames,
    details
  };

  enum kfPathOption : unsigned {
    apiDefault = KF_FLAG_DEFAULT,
    defaultPath = KF_FLAG_DEFAULT_PATH,
    doNotVerify = KF_FLAG_DONT_VERIFY,
    notParentRel = KF_FLAG_NOT_PARENT_RELATIVE //| KF_FLAG_DEFAULT_PATH
#ifdef NTDDI_VERSION
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    // Although no symbol is defined for v1703, NTDDI_WIN10_RS2 represents
    // v1704, close enough for items only available on >= Windows 10 v1703
    , redirectTarget = KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET
#endif
#endif
  };

  enum kfStrType : unsigned {
    canonicalName, path
  };

  typedef struct {
    const char* _name;
    const GUID& _guid;
  } winFolderIdsItem;

  extern std::vector<winFolderIdsItem> winFolderIds;
  extern const winFolderIdsItem winFolderTypes[];
  extern const unsigned countWinFolderTypes;

  struct guidHashFtor {
    std::hash<std::bitset<128>> hash_fn;
    size_t operator()(const GUID& rGuid) const {
      unsigned long long *pBits = (unsigned long long*) &rGuid;
      std::bitset<128> guidBitset(*pBits);
      guidBitset <<= 64;
      ++pBits;
      guidBitset |= std::bitset<128>(*pBits);
      return hash_fn(guidBitset);
    }
  };
  struct guidEqualFtor {
    bool operator()(const GUID& rLHS, const GUID& rRHS) const {
      // GUID operator== is defined in Guiddef.h
      return rLHS == rRHS;
    }
  };

  typedef std::unordered_map<
    const GUID, unsigned, guidHashFtor, guidEqualFtor
  > MapPGUIDtoIndex;

  extern MapPGUIDtoIndex fidToIndex;

  extern MapPGUIDtoIndex fTypeIdToIndex;

  typedef struct {
    char* _name;
    unsigned _value;
  } kfNumberedItem;
  extern const kfNumberedItem kfDefFlag[];
  extern const unsigned countKFDefFlagItems;

  extern const kfNumberedItem kfRedirCaps[];
  extern const unsigned countRedirCapsItems;

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) || (__cplusplus >= 201103L)
  enum class KFDefField {
#else
  enum KFDefField {
#endif
    category, name, description, fidParent, relativePath, parsingName,
    tooltip, localizedName, icon, security, attributes, kfDefFlags,
    folderTypeId
  };

  struct KFDefinition
  {
    KFDefinition();
    ~KFDefinition();
    void SetString(KFDefField, const wchar_t*);

    unsigned fid;
    char *category;
    char *name;
    char *description;
    unsigned fidParent;
    char *relativePath;
    char *parsingName;
    char *tooltip;
    char *localizedName;
    char *icon;
    char *security;
    unsigned attributes;
    unsigned kfDefFlags;
    unsigned folderTypeId;
  };

  typedef std::vector<KFDefinition> KFDefinitionList;

} // namespace mmrkfolders

#endif // ndef __MMR_KFOLDERS_H__
