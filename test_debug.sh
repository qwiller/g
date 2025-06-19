#!/bin/bash

# 调试环境测试脚本

echo "=== 麒麟系统调试环境测试 ==="

# 检查编译工具
echo "检查编译工具..."
which g++ || { echo "错误: g++未安装"; exit 1; }
which gdb || { echo "错误: gdb未安装"; exit 1; }
which gdbserver || { echo "错误: gdbserver未安装"; exit 1; }

echo "✓ 编译工具检查完成"

# 编译测试程序
echo "编译调试测试程序..."
g++ -g -O0 -std=c++17 -o debug_test debug_test.cpp

if [ $? -eq 0 ]; then
    echo "✓ 编译成功"
else
    echo "✗ 编译失败"
    exit 1
fi

# 测试程序运行
echo "测试程序运行..."
./debug_test

if [ $? -eq 0 ]; then
    echo "✓ 程序运行成功"
else
    echo "✗ 程序运行失败"
    exit 1
fi

# 测试GDB
echo "测试GDB调试..."
echo "run" | gdb -batch -ex "file debug_test" -ex "run" -ex "quit" > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "✓ GDB调试测试成功"
else
    echo "✗ GDB调试测试失败"
fi

# 测试GDB服务器
echo "测试GDB服务器..."
gdbserver --version > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "✓ GDB服务器可用"
else
    echo "✗ GDB服务器不可用"
fi

echo "=== 调试环境测试完成 ==="

# 清理
rm -f debug_test

echo ""
echo "调试环境配置建议："
echo "1. 在VS Code中打开debug_test.cpp"
echo "2. 在第15行、第27行、第34行设置断点"
echo "3. 按F5开始调试"
echo "4. 使用F10单步执行，观察变量变化"
