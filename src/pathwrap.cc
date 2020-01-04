#include <nan.h>
#include "kfolders.h"
#include "kfargsres.h"
#include "wstrutils.h"
#include "kferrs.h"

using namespace v8;

char* getPathByKFId(unsigned, unsigned, char* username = NULL); // getpath.cc

class PathByIdWorker : public Nan::AsyncWorker {
  public:
    PathByIdWorker(KFIdAndOptsArgsResolver& inArgs)
      : Nan::AsyncWorker(inArgs.getCallback()),
      _id(inArgs.getId()), _opts(inArgs.getOptions()), _pPath(NULL), _errCode(0)
    {}

    ~PathByIdWorker() {}

    void Execute() {
      try { _pPath = getPathByKFId(_id, _opts); }
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
        _pPath == NULL ? Nan::Null() : Nan::New<String>(_pPath).ToLocalChecked()
      };
      if (_pPath) freeMultibyteStr(_pPath);
      callback->Call(argc, argv);
    }

  private:
    unsigned _id;
    unsigned _opts;
    char* _pPath;
    unsigned long _errCode;
};

NAN_METHOD(getPath) {

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

  PathByIdWorker* pWorker = new PathByIdWorker(args);
  Nan::AsyncQueueWorker(pWorker);
}
