#pragma once

#include <exception>
#include <string>

class RadException : public std::exception
{
public:
    RadException(std::string msg);
    const char* what() const noexcept override;
private:
    std::string _msg;
};