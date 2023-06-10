#include "Error.h"

RadException::RadException(std::string msg) :
    _msg(msg)
{
}

const char *RadException::what() const noexcept
{
    return _msg.c_str();
}