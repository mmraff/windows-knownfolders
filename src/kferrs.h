#ifndef __MMRKFERRS_H__
#define __MMRKFERRS_H__

#include <stdexcept>

class WinKFError : public std::exception {
  public:
    WinKFError(unsigned long hcode) : _code(hcode), _more(NULL) {}
    WinKFError(unsigned long hcode, const char* more) : _code(hcode), _more(more) {}
    WinKFError(const WinKFError& other) : _code(other._code), _more(other._more) {}
    virtual const char* what() const { return ""; }
    unsigned long code() const { return _code; }
    const char* more() const { return _more; }
  protected:
    unsigned long _code;
    const char* _more;
};

class APIError : public WinKFError {
  public:
    APIError(unsigned long hcode) : WinKFError(hcode) {}
    APIError(unsigned long hcode, const char* more) : WinKFError(hcode, more) {}
    APIError(const APIError& other) : WinKFError(other) {}
    virtual const char* what() const;
};

class SysError : public WinKFError {
  public:
    SysError(unsigned long hcode) : WinKFError(hcode) {}
    SysError(unsigned long hcode, const char* more) : WinKFError(hcode, more) {}
    SysError(const SysError& other) : WinKFError(other) {}
    virtual const char* what() const;
};

class UsageError : public WinKFError {
  public:
    UsageError(unsigned long hcode) : WinKFError(hcode) {}
    UsageError(unsigned long hcode, const char* more) : WinKFError(hcode, more) {}
    UsageError(const UsageError& other) : WinKFError(other) {}
    virtual const char* what() const;
};

#endif // __MMRKFERRS_H__

