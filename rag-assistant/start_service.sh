#!/bin/bash
cd "$(dirname "$0")"
source venv/bin/activate
echo "🚀 启动银河麒麟智能问答助手..."
echo "📖 访问地址: http://localhost:8000"
echo "⏹️  按 Ctrl+C 停止服务"
python3 backend/main.py
