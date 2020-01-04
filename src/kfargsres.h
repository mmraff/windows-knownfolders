#ifndef __KFARGSRSLV_H__
#define __KFARGSRSLV_H__

#include "argsresolv.h"

class KFFlagArgResolver : public ArgsResolver {
  public:
    KFFlagArgResolver(const Nan::FunctionCallbackInfo<v8::Value>&);
    bool getFlag() { return _flag; }

  protected:
    unsigned _flag;

  private:
    KFFlagArgResolver(const KFFlagArgResolver&);
    void operator=(const KFFlagArgResolver&);
};

class KFByStringArgsResolver : public ArgsResolver {
  public:
    KFByStringArgsResolver(const Nan::FunctionCallbackInfo<v8::Value>&);
    v8::Local<v8::String> getString();

  protected:
    bool _hasString;

  private:
    KFByStringArgsResolver(const KFByStringArgsResolver&);
    void operator=(const KFByStringArgsResolver&);
};

class KFByIdArgsResolver : public ArgsResolver {
  public:
    KFByIdArgsResolver(const Nan::FunctionCallbackInfo<v8::Value>&);
    unsigned getId();

  protected:
    unsigned _id;

  private:
    KFByIdArgsResolver(const KFByIdArgsResolver&);
    void operator=(const KFByIdArgsResolver&);
};

class KFOptsArgsResolver : public ArgsResolver {
  public:
    KFOptsArgsResolver(const Nan::FunctionCallbackInfo<v8::Value>&);
    unsigned getOptions() { return _opts; }

  protected:
    unsigned _opts;

  private:
    KFOptsArgsResolver(const KFOptsArgsResolver&);
    void operator=(const KFOptsArgsResolver&);
};

// This is used by getPathById?
class KFIdAndOptsArgsResolver : public KFByIdArgsResolver {
  static const Nan::Utf8String objProtoName;

  public:
    KFIdAndOptsArgsResolver(const Nan::FunctionCallbackInfo<v8::Value>&);
    unsigned getOptions() { return _opts; }

  protected:
    unsigned _opts;

  private:
    KFIdAndOptsArgsResolver(const KFIdAndOptsArgsResolver&);
    void operator=(const KFIdAndOptsArgsResolver&);
};

#endif
