#include <nan.h>
#include "kfolders.h"

#define SET_NAN_STR_OR_NULL(obj, src, field) \
  obj->Set(Nan::New<v8::String>(#field).ToLocalChecked(), src.field == NULL ? \
           Nan::Null() : Nan::New<v8::String>(src.field).ToLocalChecked())

#define SET_NAN_UINT32(obj, src, field) \
  obj->Set(Nan::New<v8::String>(#field).ToLocalChecked(), \
           Nan::New<v8::Uint32>(src.field));

v8::Local<v8::Object> transformDef(mmrkfolders::KFDefinition& def)
{
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  SET_NAN_UINT32(result, def, fid);
  SET_NAN_STR_OR_NULL(result, def, category);
  SET_NAN_STR_OR_NULL(result, def, name);
  SET_NAN_STR_OR_NULL(result, def, description);
  SET_NAN_UINT32(result, def, fidParent);
  SET_NAN_STR_OR_NULL(result, def, relativePath);
  SET_NAN_STR_OR_NULL(result, def, parsingName);
  SET_NAN_STR_OR_NULL(result, def, tooltip);
  SET_NAN_STR_OR_NULL(result, def, localizedName);
  SET_NAN_STR_OR_NULL(result, def, icon);
  SET_NAN_STR_OR_NULL(result, def, security);
  SET_NAN_UINT32(result, def, attributes);
  SET_NAN_UINT32(result, def, kfDefFlags);
  SET_NAN_UINT32(result, def, folderTypeId);

  return result;
}

v8::Local<v8::Object> transformIndexedItem(mmrkfolders::kfNumberedItem& item)
{
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  result->Set(
    Nan::New<v8::String>("name").ToLocalChecked(),
    item._name == NULL ? Nan::Null() :
                         Nan::New<v8::String>(item._name).ToLocalChecked()
  );
  result->Set(Nan::New<v8::String>("fid").ToLocalChecked(),
              Nan::New<v8::Uint32>(item._value));

  return result;
}
