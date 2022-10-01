/**
 * \file       AnalyzerException.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-23
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#pragma once

#include <exception>
#include <string>

using namespace std;

/**
 * \brief      Exception thrown from Analyzers.
 * 
 */
class ParserException: public exception
{
private:
    string _msg;
public:
    ParserException():_msg("Undocumented exception during analyzing.") {};
    ParserException(const char* msg):_msg(msg) {};
    ParserException(const string& msg):_msg(msg) {};

    inline const char* what() const throw() {
        return _msg.data();
    }
};
