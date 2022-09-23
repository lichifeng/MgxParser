/**
 * \file       Analyzer.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-22
 * 
 * \copyright  Copyright (c) 2020-2022
 *  
 */
#pragma once

#define HEADER_INIT 4*1024*1024

#include <fstream>
#include <cstddef>
#include <vector>
#include "../BaseAnalyzer.h"

using namespace std;

/**
 * \brief      默认解析器
 *
 */
class DefaultAnalyzer: public BaseAnalyzer
{
public:
    DefaultAnalyzer() {}

    DefaultAnalyzer(const string& path) {
        this->path = path;
    }

    bool run();
    string generateMap(const string&, bool);

    /**
     * \brief      Extract header&body streams into separate files
     * 
     * \param      headerPath       filename of generated header file
     * \param      body             filename of generated body file
     */
    void extract(const string&, const string&) const;

    string            path; ///< 录像文件输入路径
    string            filename; ///< 录像文件名（去除路径后）
    string            ext; ///< 录像文件扩展名
    uintmax_t         filesize; ///< 录像文件大小，单位是 bytes
   
   
    string            playDate; ///< 游戏发生时间，对老录像只能推断 \todo 有时需要从上传时间来推断，是否放在更外层的类里面？
    string            status; ///< 解析完成类型：success, fail, partly, etc.
    string            message; ///< 对 \p status 的具体说明
    string            parseMode; ///< 解析模式：normal, verbose, etc. 可以在命令行中指定
    double            parseTime; ///< 解析耗时（毫秒）

protected:
    bool              _locateStreams(); ///< 对文件流进行处理，定位 header & body 的起始位置
    int               _inflateRawHeader(); ///< 解压 header 数据

    ifstream          _f; ///< 录像文件的原始流
    vector<uint8_t>   _body; ///< body stream
    uint8_t*          _bodyCursor; ///< 读取 body 数据的游标（指针）
    uintmax_t         _bodySize; ///< body 数据的长度
   
    vector<uint8_t>   _header; ///< 解压后的 header 数据
    uint8_t*          _headerCursor; ///< 读取 header 数据的游标（指针）
   
};
