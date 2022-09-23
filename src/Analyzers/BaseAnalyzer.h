/**
 * \file       BaseAnalyzer.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      分析器的基类（接口），所有 analyzer 都需要继承这个接口。
 * \version    0.1
 * \date       2022-09-21
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#pragma once

#include <string>

using namespace std;

/**
 * \brief      Analyzer 是 MgxParser 的核心组件之一，用于对录像文件进行实际的解
 * 析。
 *
 * \details    类的对象会将录像文件交给 Analyzer 进行解析，然后将 Analyzer 中得
 * 到的数据加工成最后的输出结果。例如：把表示民族的数字转换成文字。 （以后可以有
 * 不同的解析器，用不同的方法进行解析）。按照设想，解析器可以有多个，每个解析器
 * 的主要文件都应该放在`Analyzers/<analyzer-name>/`文件夹中。文件夹中必须包含
 * `Analyzer.h`文件并实现一个 BaseAnalyzer 的子类。 \n 这个设计的使用场景设想是
 * 这样的：正常情况下用默认解析器进行常规解析，这个过程中一般不会对 body 中的命
 * 令进行详细分析。后续如果需要对 body 中的命令进行深入分析，那就可以再做一个专
 * 门的解析器，跳过 header 部分的解析，只分析 body 部分即可。
 * 
 */
class BaseAnalyzer {
    public:
        BaseAnalyzer() {} // 之前这里没有放函数体，链接时一直提示 undefined reference to BaseAnalyzer::BaseAnalyzer()

        /**
         * \brief      构造函数，需要接收一个文件路径进行读取和解析。
         * 
         * \param      path                录像文件的路径
         */
        BaseAnalyzer(const string& path) {}

        /**
         * \brief      运行解析流程，并返回解析状态。这个函数会修改 \p valid 的值，让使用者能够通过其了解解释器的状态。比如：
         *             
         *     SomeAnalyzer a = SomeAnalyzer('/path/to/test.mgx');
         *     if(a.valid) {
         *         dosomething();
         *     }
         * 
         * \return     true                完成了一次有效的解析（即使只读取了部分信息）
         * \return     false               解析失败（没有获得任何有价值的信息）
         */
        virtual bool run() = 0;

        /**
         * \brief      生成小地图。
         * 
         * \param      path                生成地图的存储路径
         * \param      hd                  是否生成高清地图
         * \return     string              地图文件的路径，如果生成失败则为空字符串
         */
        virtual string generateMap(const string& path, bool hd) = 0;

        bool valid = false; ///< 表示解释结果的可用性

    protected:

};