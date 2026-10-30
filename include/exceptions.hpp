#pragma once

#include <string>
#include <iostream>
#include <cstdlib>

class Exception {
private:
    std::string errorMessage;
protected:
    Exception(const std::string& message) 
        : errorMessage(message) {}
    
    Exception(const char* message) {
        if (!message) {
            errorMessage = "";
        } else {
            errorMessage = message;
        }
    }

    const char* what() {
        return errorMessage.c_str();
    }
};

class NullPointerException : public Exception {
public:
    NullPointerException(const std::string& message) : Exception(message) {}
    NullPointerException(const char* message) : Exception(message) {}
};

class InvalidArgumentException : public Exception {
public:
    InvalidArgumentException(const std::string& message) : Exception(message) {}
    InvalidArgumentException(const char* message) : Exception(message) {}
};

class LogicErrorException : public Exception {
public:
    LogicErrorException(const std::string& message) : Exception(message) {}
    LogicErrorException(const char* message) : Exception(message) {}
};

class OutOfRangeException : public Exception {
public:
    OutOfRangeException(const std::string& message) : Exception(message) {}
    OutOfRangeException(const char* message) : Exception(message) {}
};