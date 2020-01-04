#include <assert.h>
#include "argsresolv.h"

using namespace v8;

// --- ArgsResolver (base class) ------------------------------------

ArgsResolver::ArgsResolver(const Nan::FunctionCallbackInfo<v8::Value>& callerInfo)
  : _info(callerInfo), _cbIdx(-1), _errCode(ArgsError::ok), _error(Nan::Null())
{
  // Only take note if there is a callback function, and don't require it.
  unsigned argCount = _info.Length();
  for (unsigned idx = 0; idx < argCount; ++idx)
  {
    if (_info[idx]->IsFunction()) {
      _cbIdx = idx;
      break;
    }
  }
}

Nan::Callback* ArgsResolver::getCallback()
{
  assert(_cbIdx != -1 && "Attempt to GetCallback() when there is none!");

  return new Nan::Callback(_info[_cbIdx].As<Function>());
}

bool ArgsResolver::nextIsString(int currIdx, bool requireNonempty)
{
  assert(currIdx < _info.Length() && "nextIsString called with index beyond end of args");

  if (_info[currIdx]->IsUndefined() || _info[currIdx]->IsNull())
  {
    _errCode = ArgsError::noValue;
  }
  else if (!_info[currIdx]->IsString() && !_info[currIdx]->IsStringObject())
  {
    _errCode = ArgsError::invalidType;
  }
  else if (requireNonempty && _info[currIdx]->ToString()->Length() == 0)
  {
    _errCode = ArgsError::emptyString;
  }
  return _errCode ? false : true;
}

bool ArgsResolver::nextIsBool(int currIdx)
{
  assert(currIdx < _info.Length() && "nextIsBool called with index beyond end of args");

  if (_info[currIdx]->IsUndefined() || _info[currIdx]->IsNull())
  {
    _errCode = ArgsError::noValue;
  }
  else if (!_info[currIdx]->IsBoolean() && !_info[currIdx]->IsBooleanObject())
  {
    _errCode = ArgsError::invalidType;
  }
  return _errCode ? false : true;
}

bool ArgsResolver::nextIsNumber(int currIdx)
{
  assert(currIdx < _info.Length() && "nextIsNumber called with index beyond end of args");

  if (_info[currIdx]->IsUndefined() || _info[currIdx]->IsNull())
  {
    _errCode = ArgsError::noValue;
  }
  //else if (_info[currIdx]->IsNumber())
  //{
  //  if (_info[currIdx]->isNaN()) _errCode = ArgsError::notANumber;
  //}
  //else if (!_info[currIdx]->IsNumberObject())
  else if (!_info[currIdx]->IsNumber() && !_info[currIdx]->IsNumberObject())
  {
    _errCode = ArgsError::invalidType;
  }
  return _errCode ? false : true;
}
