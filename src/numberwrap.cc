#include <nan.h>
#include "kfolders.h"
#include "kfargsres.h"
#include "wstrutils.h"
#include "kferrs.h"

using namespace v8;

unsigned getFolderType_win(unsigned); // kfprop.cc
unsigned getRedirCaps_win(unsigned);  // ""

enum kfNumericType: unsigned { redirectFlags, folderType };

class NumberByIdWorker : public Nan::AsyncWorker {
  public:
    NumberByIdWorker(KFByIdArgsResolver& inArgs, kfNumericType valType)
      : Nan::AsyncWorker(inArgs.getCallback()),
      _id(inArgs.getId()), _type(valType), _uVal(0), _errCode(0)
    {}

    ~NumberByIdWorker() {}

    void Execute() {
      try {
         
        switch (_type)
        {
          case kfNumericType::folderType:
            _uVal = getFolderType_win(_id); break;
          case kfNumericType::redirectFlags:
            _uVal = getRedirCaps_win(_id); break;
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
        Nan::New<Uint32>(_uVal)
      };
      callback->Call(argc, argv);
    }

  private:
    unsigned _id;
    unsigned _type;
    unsigned _uVal;
    unsigned long _errCode;
};

NAN_METHOD(getFolderType) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");
  if (!args.hasCallback())
    return Nan::ThrowSyntaxError("Callback function required");

  NumberByIdWorker* pWorker = new NumberByIdWorker(args, kfNumericType::folderType);
  Nan::AsyncQueueWorker(pWorker);
}

NAN_METHOD(getRedirectionCapabilities) {

  KFByIdArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");
  if (!args.hasCallback())
    return Nan::ThrowSyntaxError("Callback function required");

  NumberByIdWorker* pWorker = new NumberByIdWorker(args, kfNumericType::redirectFlags);
  Nan::AsyncQueueWorker(pWorker);
}
