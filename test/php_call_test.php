<?php
// 检查是否收到了POST请求
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // 检查是否收到了文件
    if (isset($_FILES['file'])) {
        // 获取上传的文件
        $file = $_FILES['file']['tmp_name'];

        // 获取command参数
        $command = $_POST['command'];

        // 调用MgxParser_S_EXE处理文件
        $output = shell_exec("./build/MgxParser_D_EXE $file");

        // 创建一个响应对象
        $response = array(
            'command' => $command,
            'output' => json_decode($output)
        );

        // 设置响应头部为JSON
        header('Content-Type: application/json');

        // 输出响应
        echo json_encode($response);
    } else {
        // 如果没有收到文件，返回一个错误消息
        echo "No file uploaded.";
    }
} else {
    // 如果不是POST请求，返回一个错误消息
    echo "Invalid request.";
}
?>