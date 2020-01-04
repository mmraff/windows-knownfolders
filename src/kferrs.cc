#include <string.h>
#include <errno.h>
#include <lmerr.h> // for network error definitions! (also brings WinError.h)
#include "kferrs.h"

const char* APIError::what() const
{
  switch (_code)
  {
    case ERROR_ACCESS_DENIED:
    case E_ACCESSDENIED:
      return "Access is denied to current user";
    case E_FAIL: // From SHGetKnownFolderPath doc:
      return "Failed";
    case E_INVALIDARG: // From SHGetKnownFolderPath doc:
      return "One or more arguments are invalid";

    // Because I can't find a header that has names for these:
    case 0x80070002L:
      return "File not found";
    case 0x80070003L:
      return "Path not found";
  }
  return NULL;
}

const char* SysError::what() const
{
  switch (_code)
  {
    case EILSEQ:
      return "System gave a wide character that could not be converted to multibyte";
    case ENOMEM:
      return "Failed to allocate memory";
    case E_ABORT:
      return "Operation aborted on suspicion of corrupted data";
  }
  return NULL;
}

const char* UsageError::what() const
{
  switch (_code)
  {
    case EILSEQ:
      return "Invalid multibyte character";
    case EMSGSIZE:
      return "Given name is too long";
    case ERANGE:
      return "Value is out of range";
  }
  return NULL;
}

