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
    cout << "========================================" << endl;
    if (argc <= 1)
    {
        cout << "No Record Specified!" << endl;
        return 1;
    }

    // Start timing
    double start, end, elapsed;
    start = clock();

    auto a = DefaultAnalyzer(argv[1]);
    try
    {
        a.run();
    }
    catch(const exception& e)
    {
        std::cerr << setw(22) << "!!!Exception: " << e.what() << '\n';
    }

    end = clock();
    elapsed = (end - start) / CLOCKS_PER_SEC * 1000;

    ios_base::fmtflags f( cout.flags() );

    cout << setw(22) << "Parsing time: " << elapsed << " ms" << endl;
    cout << setw(22) << "Filename: " << a.filename << endl;
    cout << setw(22) << "VersionString: " << a.versionStr << endl;
    cout << setw(22) << "SaveVersion: " << a.saveVersion << endl;
    // cout << setw(22) << "HD version: " << a.HD_version << endl;
    // cout << setw(22) << "HD internal version: " << a.HD_internalVersion << endl;
    cout << setw(22) << "VersionCode: " << a.versionCode << endl;
    cout << setw(22) << "Status: " << a.status << endl;
    cout << setw(22) << "Message: " << a.message << endl;
    
    cout.flags( f );

    cout << "========================================" << endl;

    //a.extract("../header.dat", "../body.dat");

    return 0;
}