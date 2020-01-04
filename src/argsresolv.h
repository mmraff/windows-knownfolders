#ifndef __ARGSRESOLV_H__
#define __ARGSRESOLV_H__

#include <nan.h>

enum ArgsError : unsigned {
  ok = 0,
  noValue,
  emptyString,
  notANumber,
  outOfRange,
  invalidType
};

class ArgsResolver {
  public:
    ArgsResolver(const Nan::FunctionCallbackInfo<v8::Value>& callerInfo);
    inline ~ArgsResolver() {}
    inline bool hasError() { return _error->IsNull() == false; }
    inline bool hasCallback() { return _cbIdx != -1; }
    inline const v8::Local<v8::Value>& getError() { return _error; }
    Nan::Callback* getCallback();

  protected:
    const Nan::FunctionCallbackInfo<v8::Value>& _info;
    ArgsError _errCode;
    v8::Local<v8::Value> _error;
    int _cbIdx;

    bool nextIsString(int currIdx, bool requireNonempty = true);
    bool nextIsBool(int);
    bool nextIsNumber(int);

  private:
    // Prevent copy constructing - not in our best interest
    ArgsResolver(const ArgsResolver&);
    void operator=(const ArgsResolver&);
};

#endif

