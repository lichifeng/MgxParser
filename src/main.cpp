/**
 * \file       main.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      主程序入口，目前还没有开发完成。
 * \version    0.1
 * \date       2022-09-21
 * 
 * \copyright Copyright (c) 2022
 * 
 */

#include "CompileConfig.h"
#include <iostream>
#include "Analyzers/default/Analyzer.h"

#include <algorithm>

using namespace std;

/**
 * \brief      主程序入口。
 * 
 * \param      argc                
 * \param      argv                
 * \return     int                 
 */
int main(int argc, char* argv[])
{

    if (argc <= 1)
    {
        cout << "No Record Specified!" << endl;
        return 1;
    }

    auto a = DefaultAnalyzer(argv[1]);
    a.run();

    cout << "Status: " << a.status << endl;
    cout << "Message: " << a.message <<endl;
    cout << "Filename: " << a.filename <<endl;
    cout << "Ext: " << a.ext <<endl;
    
    a.extract("../header.dat", "../body.dat");

    return 0;
}