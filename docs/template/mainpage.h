/**
 * \mainpage
 * 
 * \todo      实现“用法”中的各种功能。
 * 
 * MgxParser 是一个用 C++ 开发的帝国时代录像解释器，是帝国时代录像库的核心组件。\n
 * 帝国时代录像库： http://aocrec.com
 * 
 * 用法：
 * - 解析录像文件，输出包含默认录像信息的 JSON 字符串（不生成地图）\n
 * 
 *         ./mgxparser ./test.mgx
 * 
 * - 生成普通小地图（width 可省略，默认为300，生成尺寸为300*150）
 * 
 *         ./mgxparser -m[ width] ./test.mgx
 * 
 * - 生成高清小地图（width 可省略，默认为900，生成尺寸为900*450）
 * 
 *         ./mgxparser -M[ width] ./test.mgx
 * 
 * - 最大化地输出信息
 * 
 *         ./mgxparser --verbose ./test.mgx
 * 
 * - 输出版本信息
 * 
 *         ./mgxparser --version
 * 
 * - 输出帮助信息
 * 
 *         ./mgxparser --help[-h]
 * 
 */