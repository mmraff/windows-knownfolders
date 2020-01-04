#include <nan.h>
#include "kfolders.h"
#include "kfargsres.h"
#include "wstrutils.h"
#include "kferrs.h"

using namespace v8;

char* getCanonicalName_win(unsigned); // kflist.cc
char* getDisplayName_win(unsigned, unsigned); // ""
char* getCategory_win(unsigned); // kfprop.cc

enum kfNameType: unsigned { canonical, display, category };

class NameByIdWorker : public Nan::AsyncWorker {
  public:
    NameByIdWorker(KFByIdArgsResolver& inArgs, kfNameType nameType)
      : Nan::AsyncWorker(inArgs.getCallback()),
      _id(inArgs.getId()), _type(nameType), _pName(NULL), _errCode(0)
    {}

    ~NameByIdWorker() {}

    void Execute() {
      try {
        switch (_type)
        {
          case kfNameType::canonical:
            _pName = getCanonicalName_win(_id); break;
          case kfNameType::display:
            _pName = getDisplayName_win(_id, 0); break;
          case kfNameType::category:
            _pName = getCategory_win(_id); break;
          default: ;
        }
      }
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
        _pName == NULL ? Nan::Null() : Nan::New<String>(_pName).ToLocalChecked()
      };
      if (_pName && _type != kfNameType::category)
        freeMultibyteStr(_pName);
      callback->Call(argc, argv);
    }

  private:
    unsigned _id;
    kfNameType _type;
    char* _pName;
    unsigned long _errCode;
};

NAN_METHOD(getCanonicalName) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");
  if (!args.hasCallback())
    return Nan::ThrowSyntaxError("Callback function required");

  NameByIdWorker* pWorker = new NameByIdWorker(args, kfNameType::canonical);
  Nan::AsyncQueueWorker(pWorker);
}

NAN_METHOD(getDisplayName) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");
  if (!args.hasCallback())
    return Nan::ThrowSyntaxError("Callback function required");

  NameByIdWorker* pWorker = new NameByIdWorker(args, kfNameType::display);
  Nan::AsyncQueueWorker(pWorker);
}

NAN_METHOD(getCategory) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");
  if (!args.hasCallback())
    return Nan::ThrowSyntaxError("Callback function required");

  NameByIdWorker* pWorker = new NameByIdWorker(args, kfNameType::category);
  Nan::AsyncQueueWorker(pWorker);
}
