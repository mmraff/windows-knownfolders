#include <nan.h>
#include "kfolders.h"
#include "kfargsres.h"
//#include "wstrutils.h" // This will be needed when passing a username is implemented
#include "kferrs.h"

using namespace v8;

mmrkfolders::kfNumberedItem getIDListByKFId(
  unsigned, unsigned, char* username = NULL
); // getidlist.cc

class IdlistByIdWorker : public Nan::AsyncWorker {
  public:
    IdlistByIdWorker(KFIdAndOptsArgsResolver& inArgs)
      : Nan::AsyncWorker(inArgs.getCallback()),
      _id(inArgs.getId()), _opts(inArgs.getOptions()),
      _pBytes(NULL), _count(0), _errCode(0)
    {}

    ~IdlistByIdWorker() {}

    void Execute() {
      try {
        mmrkfolders::kfNumberedItem result = getIDListByKFId(_id, _opts);
        _pBytes = result._name;
        _count = result._value;
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
        Nan::Null()
      };
      if (_pBytes)
      {
        argv[1] = Nan::CopyBuffer(_pBytes, _count).ToLocalChecked();
        free(_pBytes);
      }
      callback->Call(argc, argv);
    }

  private:
    unsigned _id;
    unsigned _opts;
    char* _pBytes;
    unsigned _count;
    unsigned long _errCode;
};

NAN_METHOD(getIdList) {

  // This constructor enforces requirement for a callback function.
  KFIdAndOptsArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (args.getId() >= mmrkfolders::winFolderIds.size())
    return Nan::ThrowRangeError("Unrecognized folder Id");
  else if (args.getId() == 0)
    return Nan::ThrowRangeError("Null Id is invalid here");

  using namespace mmrkfolders;
  unsigned opts = args.getOptions();
  unsigned filteredOpts = opts & (
      kfPathOption::defaultPath
    | kfPathOption::doNotVerify
    | kfPathOption::notParentRel
#ifdef NTDDI_VERSION
#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    // Although no symbol is defined for v1703, NTDDI_WIN10_RS2 represents
    // v1704, close enough for items only available on >= Windows 10 v1703
    | kfPathOption::redirectTarget
#endif
#endif
  );
  if (opts && (opts != filteredOpts))
    return Nan::ThrowRangeError("Invalid pathOption value");

  IdlistByIdWorker* pWorker = new IdlistByIdWorker(args);
  Nan::AsyncQueueWorker(pWorker);
}
