#include <nan.h>
#include <winsdkver.h>
#include "kfolders.h"
#include "kfargsres.h"

NAN_METHOD(getIdName) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());

  using namespace mmrkfolders;
  if (args.getId() < winFolderIds.size()) {
    const char* pFIdName = winFolderIds[args.getId()]._name;
    info.GetReturnValue().Set(Nan::New<v8::String>(pFIdName).ToLocalChecked());
  }
  else return Nan::ThrowRangeError("Unrecognized folder Id");
}

NAN_METHOD(getIdTypeName) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());

  using namespace mmrkfolders;
  if (args.getId() < countWinFolderTypes) {
    const char* pFIdTypeName = winFolderTypes[args.getId()]._name;
    info.GetReturnValue().Set(Nan::New<v8::String>(pFIdTypeName).ToLocalChecked());
  }
  else return Nan::ThrowRangeError("Invalid folder type Id");
}

// TEMP TEST
NAN_METHOD(getDevInfo) {

  v8::Local<v8::Object> retData = Nan::New<v8::Object>();
  v8::Local<v8::String> cName;
  v8::Local<v8::Uint32> cUIntVal;
  v8::Local<v8::Boolean> cBoolVal;
  v8::PropertyAttribute cAttribs =
    static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
#ifdef WINVER
  cName = Nan::New<v8::String>("WINVER").ToLocalChecked();
  cUIntVal = Nan::New<v8::Uint32>(WINVER);
  Nan::ForceSet(retData, cName, cUIntVal, cAttribs).FromJust();
#endif
#ifdef WINVER_MAXVER
  cName = Nan::New<v8::String>("WINVER_MAXVER").ToLocalChecked();
  cUIntVal = Nan::New<v8::Uint32>(WINVER_MAXVER);
  Nan::ForceSet(retData, cName, cUIntVal, cAttribs).FromJust();
#endif
#ifdef NTDDI_VERSION
  cName = Nan::New<v8::String>("NTDDI_VERSION").ToLocalChecked();
  cUIntVal = Nan::New<v8::Uint32>(NTDDI_VERSION);
  Nan::ForceSet(retData, cName, cUIntVal, cAttribs).FromJust();
#endif
#ifdef _MSC_VER
  // "This macro is always defined" - but I'm skeptical
  cName = Nan::New<v8::String>("_MSC_VER").ToLocalChecked();
  cUIntVal = Nan::New<v8::Uint32>(_MSC_VER);
  Nan::ForceSet(retData, cName, cUIntVal, cAttribs).FromJust();
#endif
  cName = Nan::New<v8::String>("X64").ToLocalChecked();
#if defined(_WIN64)
  cBoolVal = Nan::True();
#else
  cBoolVal = Nan::False();
#endif
  Nan::ForceSet(retData, cName, cBoolVal, cAttribs).FromJust();

  info.GetReturnValue().Set(retData);  
}
