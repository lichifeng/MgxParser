'use strict';
const path = require('path');
const express = require('express');
const multer = require('multer');
const bodyParser = require('body-parser');
const http = require('http');

const app = express();

// 加载C++扩展
let parserPath = './mgxnode.node';

// 如果通过命令行参数指定了.node文件的路径，则使用
if (process.argv.length >= 3) {
    parserPath = path.resolve(process.argv[2]);
}

const parser = require(parserPath);

// 使用body-parser中间件来解析json请求体
app.use(bodyParser.json());

// 配置multer，设置上传的文件存储方式为内存存储
const upload = multer({ storage: multer.memoryStorage() });

// 定义/ping路由，用于检查服务器是否存活。如果存活则返回200 OK
app.get('/', (req, res) => {
    res.status(200).send(`This is a MgxParser, a parser of Age of Empires II game record file.\r\nSee https://github.com/lichifeng/MgxParser for more details.\r\n ${parser.info()}`);
});

// 定义/parse路由，同时处理JSON和文件上传
app.post('/', upload.single('file'), (req, res) => {
    const data = req.body;
    
    // 如果上传了文件，则使用parser处理数据
    if (req?.file?.buffer) {
        // 如果data.command是一个JSON格式的字符串
        if (typeof data.command === 'string') {
            try {
                // 尝试将其转换为一个JavaScript对象
                data.command = JSON.parse(data.command);

                // 如果data.command中包含有map
                if (data.command.map) {
                    // 删除mapName键（防止这个命令在容器内生成大量地图文件）
                    delete data.command.mapName;

                    // 判断map的值小写化后是不是为"hd"或"normal"
                    data.command.map = data.command.map.toLowerCase();
                    if (data.command.map === 'hd') {
                        data.command.mapWidth = 600;
                        data.command.mapHeight = 300;
                    } else if (data.command.map === 'normal') {
                        data.command.mapWidth = 300;
                        data.command.mapHeight = 150;
                    }
                }
            } catch (error) {
                console.error(`[BAD_COMMAND] ${req.ip} - ${new Date().toISOString()}`);
                data.command = {};
            }
        } else {
            console.error(`[NO_COMMAND] ${req.ip} - ${new Date().toISOString()}`);
            data.command = {};
        }

        console.log(`[COMMAND] ${req.ip} - ${new Date().toISOString()} - ${JSON.stringify(data.command)}`);

        // 将文件内容作为Buffer添加到JSON数据的input键中
        data.command.input = req?.file?.buffer;

        const retval = parser.parse(data.command);
        // 检查retval.result中是否存在mapbuffer键，如果存在则将其转换为base64编码的字符串
        if (retval.mapbuffer) {
            retval.mapbuffer = retval.mapbuffer?.toString('base64');
        }
        if (!retval.result) {
            console.error(`[BAD_RESULT] ${req.ip} - ${new Date().toISOString()}`);
        }
        res.json(retval);
    } else {
        console.error(`[NO_FILE] ${req.ip} - ${new Date().toISOString()}`);
        res.status(400).json({ error: 'No valid record file in request.' });
    }
});

// 启动服务器
const port = process.env.PORT || 4400;
const server = http.createServer(app);
server.maxConnections = process.env.MAX_CONNECTIONS || 100; // 设置最大并发连接数
server.listen(port, () => console.log(`[${new Date().toISOString()}] MgxParser is listening on port ${port}, PID: ${process.pid}, MAX_CONNECTIONS: ${server.maxConnections}`));