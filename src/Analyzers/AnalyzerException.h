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
class AnalyzerException: public exception
{
private:
    string _msg;
public:
    AnalyzerException():_msg("Undocumented exception during analyzing.") {};
    AnalyzerException(const char* msg):_msg(msg) {};
    AnalyzerException(const string& msg):_msg(msg) {};

    inline const char* what() const throw() {
        return _msg.data();
    }
};
