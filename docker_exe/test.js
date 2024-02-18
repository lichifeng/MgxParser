'use strict';
const axios = require('axios');
const FormData = require('form-data');
const fs = require('fs');

// 从命令行获取文件路径
const filePath = process.argv[2];

// 定义一个异步函数来处理请求
async function handleRequests(n, b = 100) {
    // 开始计时
    console.time("Running Time");

    // 重复n次
    for (let i = 0; i < n; i++) {
        // 创建一个包含多个请求的数组
        const requestPack = Array(b).fill().map(() => {
            // 在每个请求中都创建一个新的FormData实例
            const form = new FormData();

            // 添加JSON数据
            form.append('command', JSON.stringify({ map: "normal" }));

            // 添加文件
            form.append('file', fs.createReadStream(filePath));

            // 返回新的请求
            return axios.post('http://localhost:4400/', form, {
                headers: form.getHeaders()
            });
        });

        try {
            // 使用Promise.all来同时发送一批请求
            const responses = await Promise.all(requestPack);

            // 遍历所有响应
            for (const response of responses) {
                // 如果状态码是200，打印guid键的值
                if (response.status === 200) {
                    console.log(response.data);
                } else {
                    console.error(response.data.status);
                }
            }
            console.log(`Batch ${i + 1} done.`);
        } catch (error) {
            console.error(error);
        }
    }

    // 结束计时
    console.timeEnd("Running Time");
}

handleRequests(1, 1);