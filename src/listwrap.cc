#include <nan.h>
#include <ShlObj.h>
#include "kfolders.h"
#include "kfargsres.h"
#include "wstrutils.h"
#include "kferrs.h"
#include <iostream> // for debugging

using namespace v8;

void getAllKnownFolders(std::vector<unsigned>&); // kflist.cc
void getAllCanonicalNames(std::vector<mmrkfolders::kfNumberedItem>&); // ""
void getAllDisplayNames(std::vector<mmrkfolders::kfNumberedItem>&, unsigned); // ""
void getAllKnownFolderDetails(mmrkfolders::KFDefinitionList&); // ""
Local<Object> transformDef(mmrkfolders::KFDefinition&); // retvals.cc
Local<Object> transformIndexedItem(mmrkfolders::kfNumberedItem&); // ""

class GetAllKFWorker : public Nan::AsyncWorker {
  public:
    GetAllKFWorker(KFOptsArgsResolver& inArgs)
      : Nan::AsyncWorker(inArgs.getCallback()),
        _opts(inArgs.getOptions()), _errCode(0)
    {}

    ~GetAllKFWorker() {}

    void Execute()
    {
      try {
        using namespace mmrkfolders;
        switch (_opts)
        {
          case kfListType::details:
            getAllKnownFolderDetails(_defs); break;
          case kfListType::canonicalNames:
            getAllCanonicalNames(_names); break;
          case kfListType::displayNames:
            getAllDisplayNames(_names, 0); break;
          case kfListType::folderIds:
          default:
            getAllKnownFolders(_FIDs);
        }
      }
      catch (WinKFError& er) {
        _errCode = er.code();
        SetErrorMessage(er.what()); // TODO: add the extra if any!!!
      }
    }

    void HandleErrorCallback()
    {
      const unsigned argc = 1;
      Local<Value> exc = (this->ErrorMessage() == NULL) ?
        Nan::ErrnoException(_errCode, NULL, "Unknown error") :
        Nan::Error(this->ErrorMessage());
      Local<Value> argv[argc] = { exc };
      callback->Call(argc, argv);
    }

    void HandleOKCallback()
    {
      Local<Array> resultList;
      const unsigned argc = 2;
      using namespace mmrkfolders;
      switch (_opts)
      {
        case kfListType::details:
          resultList = transformDefList(); break;
        case kfListType::canonicalNames:
          resultList = transformStringList(); break;
        case kfListType::displayNames:
          resultList = transformStringList(); break;
        case kfListType::folderIds:
        default:
          resultList = transformIDList();
      }

      Local<Value> argv[argc] = {
        Nan::Null(),
        resultList
      };
      callback->Call(argc, argv);
    }

  private:
    unsigned _opts;
    mmrkfolders::KFDefinitionList _defs;
    std::vector<mmrkfolders::kfNumberedItem> _names;
    std::vector<unsigned> _FIDs;
    unsigned long _errCode;

    Local<Array> transformIDList()
    {
      size_t count = _FIDs.size();
      Local<Array> results = Nan::New<Array>(count);

      for (size_t i = 0; i < count; i++)
      {
        Nan::Set(results, i, Nan::New<Uint32>(_FIDs[i]));
      }

      return results;
    }

    Local<Array> transformStringList()
    {
      size_t count = _names.size();
      Local<Array> results = Nan::New<Array>(count);

      for (size_t i = 0; i < count; i++)
      {
        Nan::Set(results, i, transformIndexedItem(_names[i]));
        freeMultibyteStr(_names[i]._name);
      }

      return results;
    }

    Local<Array> transformDefList()
    {
      size_t count = _defs.size();
      Local<Array> results = Nan::New<Array>(count);

      for (size_t i = 0; i < count; i++)
      {
        Nan::Set(results, i, transformDef(_defs[i]));
      }

      return results;
    }
};

NAN_METHOD(list) {

  KFOptsArgsResolver args(info);
  if (args.hasError()) return Nan::ThrowError(args.getError());
  if (!args.hasCallback())
    return Nan::ThrowSyntaxError("Callback function required");

  GetAllKFWorker* pWorker = new GetAllKFWorker(args);
  Nan::AsyncQueueWorker(pWorker);
}
