#!/bin/bash
if [ -f app.pid ]; then
    PID=$(cat app.pid)
    if kill -0 $PID 2>/dev/null; then
        kill $PID
        rm app.pid
        echo "✅ 服务已停止"
    else
        echo "⚠️  服务未运行"
        rm -f app.pid
    fi
else
    echo "⚠️  未找到进程文件"
fi
