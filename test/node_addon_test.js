'use strict';
const path = require('path');

// 检查是否提供了命令行参数
if (process.argv.length < 3) {
  console.error('请提供.node文件的路径');
  process.exit(1);
}

// 加载C++扩展
const addonPath = path.resolve(process.argv[2]);
const addon = require(addonPath);

// 使用C++扩展
const addonDir = path.dirname(addonPath);
const imagePath = path.join(addonDir, 'testmap.png');

const parsed = addon.parse({
  input: path.resolve('test/test_records/AOC10_4v4_3_192a8268.zip'),
  map: 'hd', // lowercase, or 'normal'
  mapWidth: 600,
  mapHeight: 300,
  mapName: imagePath,
  jsonIndent: 2
});

console.log(parsed.result);