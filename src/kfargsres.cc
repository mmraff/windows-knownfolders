#include <assert.h>
#include "kfargsres.h"

using namespace v8;

KFFlagArgResolver::KFFlagArgResolver(
  const Nan::FunctionCallbackInfo<v8::Value>& callerInfo
) : ArgsResolver(callerInfo), _flag(false)
{
  int argIdx = 0;
  if (_info.Length() == 0) return;

  if (this->nextIsBool(argIdx))
  {
    _flag = _info[argIdx]->IsTrue();
    ++argIdx;
  }
  if (_info[argIdx]->IsFunction())
  {
    if (_error != Nan::Null()) _error = Nan::Null();
    _cbIdx = argIdx;
  }
  else _error = Nan::TypeError("Invalid argument type");
}

KFByStringArgsResolver::KFByStringArgsResolver(
  const Nan::FunctionCallbackInfo<v8::Value>& callerInfo
) : ArgsResolver(callerInfo)
{
  if (_info.Length() == 0)
  {
    _error = Nan::SyntaxError("No argument given");
  }
  else if (!this->nextIsString(0))
  {
    switch (_errCode) {
      case ArgsError::noValue:
        _error = Nan::SyntaxError("No value given where string expected");
        break;
      case ArgsError::emptyString:
        _error = Nan::SyntaxError("Empty string");
        break;
      case ArgsError::invalidType:
        _error = Nan::TypeError("String expected");
        break;
      default:
        assert(NULL && "Unhandled ArgsError case - should never happen!");
    }
  }
  else _hasString = true;
}

v8::Local<v8::String> KFByStringArgsResolver::getString()
{
  assert(_hasString && "Attempt to getString() when there is none!");

  return _info[0].As<String>();
}


KFByIdArgsResolver::KFByIdArgsResolver(
  const Nan::FunctionCallbackInfo<v8::Value>& callerInfo
) : ArgsResolver(callerInfo), _id(0)
{
  if (_info.Length() == 0)
  {
    _error = Nan::SyntaxError("No argument given");
  }
  else if (!this->nextIsNumber(0))
  {
    switch (_errCode) {
      case ArgsError::noValue:
        _error = Nan::SyntaxError("No Id value given");
        break;
      case ArgsError::notANumber:
        _error = Nan::Error("NaN given for Id argument");
        break;
      case ArgsError::invalidType:
        _error = Nan::TypeError("Invalid type for Id argument");
        break;
      default:
        assert(NULL && "Unhandled ArgsError case - should never happen!");
    }
  }
  else if (_info[0]->IsNumber())
  {
    if (_info[0]->IsUint32()) _id = _info[0]->Uint32Value();
    else if (_info[0]->IsInt32())
      _error = Nan::RangeError("Id cannot be a negative value");
    else
      _error = Nan::TypeError("Invalid Id value");
  }
  else if (_info[0]->IsNumberObject())
  {
    Nan::Maybe<uint32_t> uintMaybe = Nan::To<uint32_t>(_info[0]);
    if (uintMaybe.IsJust()) _id = uintMaybe.FromJust();
    else
    {
      Nan::Maybe<int32_t> intMaybe = Nan::To<int32_t>(_info[0]);
      if (intMaybe.IsJust())
        _error = Nan::RangeError("Id cannot be a negative value");
      else
        _error = Nan::TypeError("Invalid Id value");
    }
  }
  // NOTE that we don't try to validate the given folderId here; this is
  // because someone (maybe an app) may have installed a custom Known Folder,
  // and before we use the API, this code only knows about the Known Folders
  // that the API documentation has listed, minus the ones that are found to be
  // not defined by the available library header.
}

unsigned KFByIdArgsResolver::getId()
{
  assert(_error == Nan::Null() && "Attempt to getId() when there is none!");

  return _id;
}

// An args resolver specifically for the list() function.
// Takes an optional numeric value (options), then expects a callback function.
KFOptsArgsResolver::KFOptsArgsResolver(
  const Nan::FunctionCallbackInfo<v8::Value>& callerInfo
) : ArgsResolver(callerInfo), _opts(0)
{
  int currIdx = 0; // Type int chosen to silence warnings. See NOTE below.

  if (_info[currIdx]->IsNumber())
  {
    if (_info[currIdx]->IsUint32()) {
      _opts = _info[currIdx]->Uint32Value();
      ++currIdx;
    }
    else if (_info[currIdx]->IsInt32())
      _error = Nan::RangeError("Option cannot be a negative value");
    else
      _error = Nan::TypeError("Invalid type for option");
  }
  else if (_info[currIdx]->IsNumberObject())
  {
    Nan::Maybe<uint32_t> uintMaybe = Nan::To<uint32_t>(_info[currIdx]);
    if (uintMaybe.IsJust()) {
      _opts = uintMaybe.FromJust();
      ++currIdx;
    }
    else
    {
      Nan::Maybe<int32_t> intMaybe = Nan::To<int32_t>(_info[currIdx]);
      if (intMaybe.IsJust())
        _error = Nan::RangeError("Invalid option value");
      else
        _error = Nan::TypeError("Invalid type for option");
    }
  }
  if (this->hasError()) return;

  // We won't enforce a callback requirement here (unless the options argument
  // was omitted - see below); user must check with hasCallback().
  // NOTE: the Nan authors have made Length() return an int instead of
  // unsigned/size_t
  if (currIdx >= _info.Length()) return;

  if (!_info[currIdx]->IsFunction())
  {
    _error = Nan::TypeError("Invalid argument, expected function reference");
  }
}

// An args resolver that takes an Id, then an optional numeric value (options),
// and then expects a callback function
KFIdAndOptsArgsResolver::KFIdAndOptsArgsResolver(
  const Nan::FunctionCallbackInfo<v8::Value>& callerInfo
) : KFByIdArgsResolver(callerInfo), _opts(0)
{
  int currIdx = 1;
  // The argument list must start with a folder Id.
  // The base class constructor should have taken care of that.
  if (this->hasError()) return;
  if (_info.Length() < 2) {
    // An Id was passed, but nothing else
    _error = Nan::SyntaxError("Callback function is required");
    return;
  }

  // Check for an (optional) option value
  if (_info[1]->IsNumber())
  {
    if (_info[1]->IsUint32()) {
      _opts = _info[1]->Uint32Value();
      ++currIdx;
    }
    else if (_info[currIdx]->IsInt32())
      _error = Nan::RangeError("Option cannot be a negative value");
    else
      _error = Nan::TypeError("Invalid type for option");
  }
  else if (_info[1]->IsNumberObject())
  {
    Nan::Maybe<uint32_t> uintMaybe = Nan::To<uint32_t>(_info[1]);
    if (uintMaybe.IsJust()) {
      _opts = uintMaybe.FromJust();
      ++currIdx;
    }
    else
    {
      Nan::Maybe<int32_t> intMaybe = Nan::To<int32_t>(_info[1]);
      if (intMaybe.IsJust())
        _error = Nan::RangeError("Invalid option value");
      else
        _error = Nan::TypeError("Invalid type for option");
    }
  }
  // NOTE: option value is validated in the function that calls the API,
  // to keep header inclusion minimized.
  if (this->hasError()) return;

  // We won't enforce a callback requirement here; user must check hasCallback()
  if (currIdx >=_info.Length()) return;

  if (!_info[currIdx]->IsFunction())
  {
    _error = Nan::TypeError("Invalid argument, expected function reference");
  }
}
