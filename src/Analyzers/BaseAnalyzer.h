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

#include <string>

using namespace std;

/**
 * \brief      Analyzer 是 MgxParser 的核心组件之一，用于对录像文件进行实际的解
 * 析。它会读取帝国时代录像文件中的原始数据并进行必要的处理。Record 类的对象会将
 * 录像文件交给 Analyzer 进行解析，然后将 Analyzer 中得到的数据加工成最后的输出
 * 结果。例如：把表示民族的数字转换成文字。 
 *
 */
class BaseAnalyzer {
    public:
        /**
         * \brief      构造函数，需要接收一个文件路径进行读取和解析。
         * 
         * \param      path                录像文件的路径
         * \param      outEncoding         转换后的编码
         */
        BaseAnalyzer(const string& path, const string& outEncoding);

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
        virtual string generateMap(string path, bool hd);

        bool valid = false; ///< 表示解释结果的可用性

        string      filename; ///< 录像文件名（去除路径后）
        string      playDate; ///< 游戏发生时间，对老录像只能推断 \todo 有时需要从上传时间来推断，是否放在更外层的类里面？
        string      status; ///< 解析完成类型：success, fail, partly, etc.
        string      message; ///< 对 \p status 的具体说明
        string      parseMode; ///< 解析模式：normal, verbose, etc. 可以在命令行中指定
        double      parseTime; ///< 解析耗时（毫秒）
        string      encoding; ///< 录像文件内字符串的原始编码
        string      language; ///< 录像文件的语言
        string      gameHash; ///< 代表本局游戏的唯一编码，**同一局游戏的不同视角应该是相同的**
        string      versionString; ///< 原始版本字符串，\todo 还有 \p logVersion 等版本信息也要完善
        string      instructions; ///< 游戏内自带的说明，需要转码
        int32_t     difficulty; ///< 難易度 (0:非常に難しい、1:難しい、2:普通、3:簡単、4:非常に簡単)
        int32_t     gameType; ///< \todo 完善这个属性的解释
        float       gameSpeed; ///< \todo 也有的值是 int，要确定要到底用哪个来确定速度
        bool        mapVisibility; ///< 地图全开（好像和迷雾不是一个概念？）
        bool        fullTechTree; ///< 完整科技
        int32_t     popLimit; ///< 人口上限
        bool        lockDiplomacy; ///< 锁定组队
        string      victoryCondition; ///< \todo 关于胜利的设定很复杂，需要进一步研究，最好进游戏看一看
        int32_t     startingAge; ///< 开始年代
        long        duration; ///< 游戏时长
        bool        isMultiplayer; ///< 是否联机游戏
        bool        includeAI; ///< 是否有 AI 参战

    protected:

};