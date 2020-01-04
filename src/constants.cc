#include <nan.h>
#include <assert.h>
#include "kfolders.h"

using v8::String;
using v8::Uint32;

// TODO:
// Find out what token to use to tell what side of the Nan threshold we're on
// (Nan::ForceSet OK < VERSION_WHERE_FORCESET_DEPRECATED <= LATEST_VERSION)
#define KFOLDERS_DEFINE_CONSTANT1(target, name, val)                   \
do {                                                                   \
  v8::Local<String> cName = Nan::New<String>(name).ToLocalChecked();   \
  v8::Local<Uint32> cVal = Nan::New<Uint32>(val);                      \
  v8::PropertyAttribute cAttribs =                                     \
    static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete); \
  Nan::ForceSet(target, cName, cVal, cAttribs).FromJust();             \
}                                                                      \
while (0)
/*
#define KFOLDERS_DEFINE_CONSTANT2(target, name, val)                   \
do {                                                                   \
  v8::Local<String> cName = Nan::New<String>(name).ToLocalChecked();   \
  v8::Local<Uint32> cVal = Nan::New<Uint32>(val);                      \
  v8::PropertyAttribute cAttribs =                                     \
    static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete); \
  Nan::DefineOwnProperty(target, cName, cVal, cAttribs).FromJust();    \
}                                                                      \
while (0)
*/

void addWinFolderIds(v8::Handle<v8::Object>& target)
{
  // Constants that correspond to FOLDERID values available on the
  // current system according to the installed library header
  unsigned IdCount = mmrkfolders::winFolderIds.size();
  unsigned idx;
  v8::Local<v8::Object> folderIds = Nan::New<v8::Object>();
  for (idx = 0; idx < IdCount; ++idx)
  {
    const mmrkfolders::winFolderIdsItem item = mmrkfolders::winFolderIds[idx];
    mmrkfolders::fidToIndex[item._guid] = idx; // build the map
    KFOLDERS_DEFINE_CONSTANT1(folderIds, item._name, idx);
  }
  target->Set(Nan::New<String>("folderIds").ToLocalChecked(), folderIds);

  // Constants that correspond to FOLDERTYPEID values available on the
  // current system according to the installed library header
  v8::Local<v8::Object> folderTypeIds = Nan::New<v8::Object>();
  for (idx = 0; idx < mmrkfolders::countWinFolderTypes; ++idx)
  {
    const mmrkfolders::winFolderIdsItem item = mmrkfolders::winFolderTypes[idx];
    mmrkfolders::fTypeIdToIndex[item._guid] = idx; // build the map
    KFOLDERS_DEFINE_CONSTANT1(folderTypeIds, item._name, idx);
  }
  target->Set(Nan::New<String>("folderTypeIds").ToLocalChecked(), folderTypeIds);
}

void addListTypesConstants(v8::Handle<v8::Object>& target)
{
  v8::Local<v8::Object> listTypes = Nan::New<v8::Object>();

  using namespace mmrkfolders;
  KFOLDERS_DEFINE_CONSTANT1(listTypes, "FOLDER_ID",      kfListType::folderIds);
  KFOLDERS_DEFINE_CONSTANT1(listTypes, "CANONICAL_NAME", kfListType::canonicalNames);
  KFOLDERS_DEFINE_CONSTANT1(listTypes, "DISPLAY_NAME",   kfListType::displayNames);
  KFOLDERS_DEFINE_CONSTANT1(listTypes, "DETAILS",        kfListType::details);

  target->Set(Nan::New<String>("listTypes").ToLocalChecked(), listTypes);
}

// The KNOWN_FOLDER_FLAG values that apply to fetching information
void addPathOptsConstants(v8::Handle<v8::Object>& target)
{
  v8::Local<v8::Object> pathOpts = Nan::New<v8::Object>();

  using namespace mmrkfolders;
  KFOLDERS_DEFINE_CONSTANT1(pathOpts, "API_DEFAULT",         kfPathOption::apiDefault);
  KFOLDERS_DEFINE_CONSTANT1(pathOpts, "DEFAULT_PATH",        kfPathOption::defaultPath);
  KFOLDERS_DEFINE_CONSTANT1(pathOpts, "DONT_VERIFY",         kfPathOption::doNotVerify);
  KFOLDERS_DEFINE_CONSTANT1(pathOpts, "NOT_PARENT_RELATIVE", kfPathOption::notParentRel);
#ifdef NTDDI_VERSION
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
  // "Introduced in Windows 10, version 1703."
  // Although no symbol is defined for v1703, NTDDI_WIN10_RS2 represents
  // v1704, close enough for items only available on >= Windows 10 v1703
  KFOLDERS_DEFINE_CONSTANT1(pathOpts, "REDIRECTION_TARGET",  kfPathOption::redirectTarget);
#endif
#endif

  target->Set(Nan::New<String>("pathOptions").ToLocalChecked(), pathOpts);
}

void addDefFlagConstants(v8::Handle<v8::Object>& target)
{
  v8::Local<v8::Object> defFlags = Nan::New<v8::Object>();

  using namespace mmrkfolders;
  for (unsigned idx = 0; idx < countKFDefFlagItems; ++idx)
  {
    kfNumberedItem item = kfDefFlag[idx];
    KFOLDERS_DEFINE_CONSTANT1(defFlags, item._name, item._value);
  }

  target->Set(Nan::New<String>("definitionFlags").ToLocalChecked(), defFlags);
}

void addRedirCapConstants(v8::Handle<v8::Object>& target)
{
  v8::Local<v8::Object> redirFlags = Nan::New<v8::Object>();

  using namespace mmrkfolders;
  for (unsigned idx = 0; idx < countRedirCapsItems; ++idx)
  {
    kfNumberedItem item = kfRedirCaps[idx];
    KFOLDERS_DEFINE_CONSTANT1(redirFlags, item._name, item._value);
  }

  target->Set(Nan::New<String>("redirectFlags").ToLocalChecked(), redirFlags);
}
