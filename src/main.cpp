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

#include <exception>

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
    cout << "\033[31m~~~~~~~~~~~~~~~~~~~~ DEBUG HINTS ~~~~~~~~~~~~~~~~~~~~~~\033[0m" << endl;
    if (argc <= 1)
    {
        cout << "No Record Specified!" << endl;
        return 1;
    }

    // Start timing
    double start, end, elapsed;
    start = clock();

    auto a = DefaultAnalyzer(argv[1]);
    cout << "\033[1;31m";
    try
    {
        a.run();
    }
    catch(const exception& e)
    {
        std::cerr << setw(25) << endl << "!!!Exception: " << e.what() << endl;;
    }

    cout << "\033[0m";

    end = clock();
    elapsed = (end - start) / CLOCKS_PER_SEC * 1000;

    cout << setfill(' ');
    cout << "\033[4;33;44m" << setw(30) << "Parsing time: \033[0m  " << elapsed << " ms" << endl;
    cout << "\033[4;33;44m" << setw(30) << "Filename: \033[0m  " << a.filename << endl;
    cout << "\033[4;33;44m" << setw(30) << "VersionString: \033[0m  " << a.versionStr << endl;
    cout << "\033[4;33;44m" << setw(30) << "SaveVersion: \033[0m  " << a.saveVersion << endl;
    cout << "\033[4;33;44m" << setw(30) << "HD/DE version: \033[0m  " << a.DD_version << endl;
    cout << "\033[4;33;44m" << setw(30) << "HD/DE internalVersion: \033[0m  " << a.DD_internalVersion << endl;
    cout << "\033[4;33;44m" << setw(30) << "VersionCode: \033[0m  " << a.versionCode << endl;
    cout << "\033[4;33;44m" << setw(30) << "Status: \033[0m  " << a.status << endl;
    cout << "\033[4;33;44m" << setw(30) << "Message: \033[0m  " << endl << a.message << endl;
    cout << a.fixEncoding(a.instructions) << endl;

    //cout << "\033[31m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m  " << endl;

    a.extract("../header.dat", "../body.dat");

    return 0;
}