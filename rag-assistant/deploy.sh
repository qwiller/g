#!/bin/bash
# 银河麒麟智能问答助手 - 部署脚本

set -e

echo "🚀 开始部署银河麒麟智能问答助手..."

# 检查Python版本
echo "📋 检查Python环境..."
python3 --version

# 检查是否在麒麟系统上
if [ -f /etc/kylin-release ]; then
    echo "✅ 检测到银河麒麟操作系统"
    cat /etc/kylin-release
else
    echo "⚠️  未检测到银河麒麟系统，但可以继续安装"
fi

# 安装系统依赖
echo "📦 安装系统依赖..."
sudo apt update
sudo apt install -y python3-pip python3-venv python3-tk

# 创建虚拟环境
echo "🔧 创建Python虚拟环境..."
python3 -m venv venv
source venv/bin/activate

# 安装Python依赖
echo "📚 安装Python依赖..."
pip install --upgrade pip
pip install -r requirements.txt

# 创建服务文件
echo "⚙️  创建系统服务..."
sudo tee /etc/systemd/system/kylin-qa-assistant.service > /dev/null <<EOF
[Unit]
Description=银河麒麟智能问答助手
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$(pwd)
Environment=PATH=$(pwd)/venv/bin
ExecStart=$(pwd)/venv/bin/python backend/main.py
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

# 重新加载systemd
sudo systemctl daemon-reload

# 启用服务
sudo systemctl enable kylin-qa-assistant.service

echo "✅ 部署完成！"
echo ""
echo "🎯 使用说明："
echo "1. 启动服务: sudo systemctl start kylin-qa-assistant"
echo "2. 停止服务: sudo systemctl stop kylin-qa-assistant"
echo "3. 查看状态: sudo systemctl status kylin-qa-assistant"
echo "4. 查看日志: sudo journalctl -u kylin-qa-assistant -f"
echo ""
echo "🌐 访问地址:"
echo "- Web界面: http://localhost:8000"
echo "- API文档: http://localhost:8000/docs"
echo ""
echo "🖥️  桌面应用:"
echo "- 运行: python3 desktop_app.py"
echo ""
echo "📝 注意事项:"
echo "- 确保8000端口未被占用"
echo "- 首次使用需要上传文档"
echo "- 支持PDF、Markdown、文本文件"
