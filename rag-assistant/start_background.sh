#!/bin/bash
cd "$(dirname "$0")"
source venv/bin/activate

# 检查是否已经在运行
if [ -f app.pid ]; then
    PID=$(cat app.pid)
    if kill -0 $PID 2>/dev/null; then
        echo "❌ 服务已在运行 (PID: $PID)"
        echo "🔄 如需重启，请先运行: ./stop_service.sh"
        exit 1
    fi
fi

echo "🚀 后台启动银河麒麟智能问答助手..."
nohup python3 backend/main.py > app.log 2>&1 &
echo $! > app.pid
sleep 2

if kill -0 $(cat app.pid) 2>/dev/null; then
    echo "✅ 服务启动成功"
    echo "📖 访问地址: http://localhost:8000"
    echo "📋 进程ID: $(cat app.pid)"
    echo "📄 日志文件: app.log"
    echo "⏹️  停止服务: ./stop_service.sh"
else
    echo "❌ 服务启动失败，请查看 app.log"
fi
