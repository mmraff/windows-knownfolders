#include <nan.h>

using namespace v8;

void addWinFolderIds(Handle<Object>&);
void addListTypesConstants(Handle<Object>&);
void addPathOptsConstants(Handle<Object>&);
void addDefFlagConstants(Handle<Object>&);
void addRedirCapConstants(v8::Handle<v8::Object>&);

NAN_METHOD(findFromPath);
NAN_METHOD(getByName);
NAN_METHOD(getIdName);
NAN_METHOD(getIdTypeName);
NAN_METHOD(getPath);
NAN_METHOD(getCanonicalName);
NAN_METHOD(getDisplayName);
NAN_METHOD(getCategory);
NAN_METHOD(getFolderType);
NAN_METHOD(getDefinition);
NAN_METHOD(getIdList);
NAN_METHOD(getRedirectionCapabilities);
NAN_METHOD(list);

NAN_METHOD(getDevInfo); // TEMP

NAN_MODULE_INIT(init) {

  NAN_EXPORT(target, findFromPath);
  NAN_EXPORT(target, getByName);
  NAN_EXPORT(target, getIdName);
  NAN_EXPORT(target, getIdTypeName);
  NAN_EXPORT(target, getPath);
  NAN_EXPORT(target, getCanonicalName);
  NAN_EXPORT(target, getDisplayName);
  NAN_EXPORT(target, getCategory);
  NAN_EXPORT(target, getFolderType);
  NAN_EXPORT(target, getDefinition);
  NAN_EXPORT(target, getIdList);
  NAN_EXPORT(target, getRedirectionCapabilities);
  NAN_EXPORT(target, list);

  NAN_EXPORT(target, getDevInfo); // TEMP

  addWinFolderIds(target);
  addListTypesConstants(target);
  addPathOptsConstants(target);
  addDefFlagConstants(target);
  addRedirCapConstants(target);
}

NODE_MODULE(winknownfolders, init);

