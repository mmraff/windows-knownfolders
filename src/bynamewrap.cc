#include <nan.h>
#include "kfolders.h"
#include "kfargsres.h"
#include "wstrutils.h"
#include "kferrs.h"

using namespace v8;

unsigned fetchIdByString(wchar_t*, mmrkfolders::kfStrType); // getid.cc

class IdByNameWorker : public Nan::AsyncWorker {
  public:
    IdByNameWorker(KFByStringArgsResolver& inArgs)
      : Nan::AsyncWorker(inArgs.getCallback()),
      _pName(NULL),_id(0), _errCode(0)
    {
      try {
        _pName = getWideStrCopy(*(Nan::Utf8String(inArgs.getString())), MAX_PATH);
      }
      catch (WinKFError& er) {
        _errCode = er.code();
        SetErrorMessage(er.what());
        return;
      }
    }

    ~IdByNameWorker() {}

    void Execute() {
      if (_errCode != 0) return;

      try {
        _id = fetchIdByString(_pName, mmrkfolders::kfStrType::canonicalName);
      }
      catch (WinKFError& er)
      {
        _errCode = er.code();
        SetErrorMessage(er.what());
      }
      freeWideStr(_pName);
    }

    void HandleErrorCallback() {
      const unsigned argc = 1;
      Local<Value> exc = (this->ErrorMessage() == NULL) ?
        Nan::ErrnoException(_errCode, NULL, "Unknown error") :
        Nan::Error(this->ErrorMessage());
      Local<Value> argv[argc] = { exc };
      callback->Call(argc, argv);
    }

    void HandleOKCallback()
    {
      const unsigned argc = 2;
      Local<Value> argv[argc] = {
        Nan::Null(),
        Nan::New<Uint32>(_id)
      };
      callback->Call(argc, argv);
    }

  private:
    wchar_t* _pName;
    unsigned _id;
    unsigned long _errCode;
};

NAN_METHOD(getByName) {

  KFByStringArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (!args.hasCallback()) return Nan::ThrowSyntaxError("Must provide callback");

  IdByNameWorker* pWorker = new IdByNameWorker(args);
  Nan::AsyncQueueWorker(pWorker);
}
