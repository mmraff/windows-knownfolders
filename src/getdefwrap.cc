#include <nan.h>
#include "kfolders.h"
#include "kfargsres.h"
#include "kferrs.h"

using namespace v8;

void getFolderDefinition(unsigned, mmrkfolders::KFDefinition&); // kflist.cc
Local<Object> transformDef(mmrkfolders::KFDefinition&); // retvals.cc

class DefByIdWorker : public Nan::AsyncWorker {
  public:
    DefByIdWorker(KFByIdArgsResolver& inArgs)
      : Nan::AsyncWorker(inArgs.getCallback()),
      _id(inArgs.getId()), _errCode(0)
    {}

    ~DefByIdWorker() {}

    void Execute() {
      try { getFolderDefinition(_id, _def); }
      catch (WinKFError& er) {
        _errCode = er.code();
        SetErrorMessage(er.what());
      }
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
        transformDef(_def)
      };
      callback->Call(argc, argv);
    }

  private:
    unsigned _id;
    mmrkfolders::KFDefinition _def;
    unsigned long _errCode;
};

NAN_METHOD(getDefinition) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (!args.hasCallback()) return Nan::ThrowSyntaxError("Must provide callback");
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");

  DefByIdWorker* pWorker = new DefByIdWorker(args);
  Nan::AsyncQueueWorker(pWorker);
}
