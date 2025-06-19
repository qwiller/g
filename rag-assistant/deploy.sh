#!/bin/bash
# 银河麒麟智能问答助手 - 小白专用部署脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_step() {
    echo -e "\n${BLUE}🔄 $1${NC}"
}

# 欢迎信息
echo -e "${GREEN}"
echo "=================================================="
echo "🤖 银河麒麟智能问答助手 - 小白部署脚本"
echo "=================================================="
echo -e "${NC}"
print_info "本脚本将自动为您安装和配置智能问答助手"
print_info "整个过程大约需要5-10分钟，请耐心等待"
echo ""

# 检查系统
print_step "检查系统环境"
if [ -f /etc/kylin-release ]; then
    print_success "检测到银河麒麟操作系统"
    cat /etc/kylin-release
else
    print_warning "未检测到银河麒麟系统，但可以继续安装"
fi

# 检查Python
print_info "检查Python版本..."
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    print_success "Python环境正常: $PYTHON_VERSION"
else
    print_error "未找到Python3，请先安装Python3"
    exit 1
fi

# 检查网络连接
print_step "检查网络连接"
if ping -c 1 8.8.8.8 &> /dev/null; then
    print_success "网络连接正常"
else
    print_warning "网络连接可能有问题，但继续安装"
fi

# 安装系统依赖
print_step "安装系统依赖包"
print_info "正在更新软件包列表..."
sudo apt update -qq

print_info "正在安装必要的系统依赖..."
sudo apt install -y python3-pip python3-venv python3-tk curl git

print_success "系统依赖安装完成"

# 创建虚拟环境
print_step "创建Python虚拟环境"
if [ -d "venv" ]; then
    print_warning "虚拟环境已存在，将重新创建"
    rm -rf venv
fi

python3 -m venv venv
source venv/bin/activate
print_success "虚拟环境创建完成"

# 安装Python依赖
print_step "安装Python依赖包"
print_info "正在升级pip..."
pip install --upgrade pip -q

print_info "正在安装应用依赖..."
if pip install -r requirements.txt -q; then
    print_success "Python依赖安装完成"
else
    print_warning "使用国内镜像源重试..."
    pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple/ -q
    print_success "Python依赖安装完成（使用镜像源）"
fi

# 检查端口占用
print_step "检查端口占用情况"
if netstat -tlnp 2>/dev/null | grep -q ":8000 "; then
    print_warning "端口8000已被占用，请手动停止占用进程或稍后重试"
    netstat -tlnp | grep ":8000 "
else
    print_success "端口8000可用"
fi

# 创建系统服务
print_step "创建系统服务"
print_info "正在创建systemd服务文件..."

# 检查sudo权限并提示用户
if ! sudo -n true 2>/dev/null; then
    print_info "需要管理员权限来创建系统服务，请输入您的密码："
fi

# 创建服务文件，如果失败则提供备选方案
if sudo tee /etc/systemd/system/kylin-qa-assistant.service > /dev/null <<EOF
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
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF
then
    # 重新加载systemd
    sudo systemctl daemon-reload
    sudo systemctl enable kylin-qa-assistant.service
    print_success "系统服务创建完成"
else
    print_warning "系统服务创建失败，将使用手动启动方式"
    print_info "您可以稍后手动运行: python3 backend/main.py"
    MANUAL_START=true
fi

# 启动服务
print_step "启动智能问答助手服务"
if [ "$MANUAL_START" = true ]; then
    print_info "使用手动启动方式..."
    # 后台启动服务
    nohup python3 backend/main.py > app.log 2>&1 &
    APP_PID=$!
    echo $APP_PID > app.pid
    sleep 3

    if kill -0 $APP_PID 2>/dev/null; then
        print_success "服务启动成功（手动模式）"
        print_info "进程ID: $APP_PID"
    else
        print_error "服务启动失败，请查看 app.log"
    fi
else
    if sudo systemctl start kylin-qa-assistant.service; then
        print_success "服务启动成功"
        sleep 3

        # 检查服务状态
        if sudo systemctl is-active --quiet kylin-qa-assistant.service; then
            print_success "服务运行正常"
        else
            print_error "服务启动失败，请查看日志"
            sudo journalctl -u kylin-qa-assistant.service -n 10 --no-pager
        fi
    else
        print_error "服务启动失败"
    fi
fi

# 测试服务
print_step "测试服务连接"
sleep 2
if curl -s http://localhost:8000/status > /dev/null; then
    print_success "Web服务响应正常"
else
    print_warning "Web服务可能还在启动中，请稍等片刻"
fi

# 部署完成
echo ""
echo -e "${GREEN}"
echo "=================================================="
echo "🎉 部署完成！"
echo "=================================================="
echo -e "${NC}"

print_success "银河麒麟智能问答助手已成功部署"
echo ""

echo -e "${BLUE}🌐 访问方式:${NC}"
echo "  Web界面: http://localhost:8000"
echo "  API文档: http://localhost:8000/docs"
echo ""

echo -e "${BLUE}🖥️  桌面应用:${NC}"
echo "  cd $(pwd)"
echo "  source venv/bin/activate"
echo "  python3 desktop_app.py"
echo ""

echo -e "${BLUE}⚙️  服务管理:${NC}"
echo "  启动服务: sudo systemctl start kylin-qa-assistant"
echo "  停止服务: sudo systemctl stop kylin-qa-assistant"
echo "  重启服务: sudo systemctl restart kylin-qa-assistant"
echo "  查看状态: sudo systemctl status kylin-qa-assistant"
echo "  查看日志: sudo journalctl -u kylin-qa-assistant -f"
echo ""

echo -e "${BLUE}📚 使用指南:${NC}"
echo "  1. 打开浏览器访问 http://localhost:8000"
echo "  2. 上传PDF、Markdown或文本文档"
echo "  3. 在问答区域输入问题"
echo "  4. 体验AI智能回答和语音播放功能"
echo ""

echo -e "${BLUE}🔧 故障排除:${NC}"
echo "  如果遇到问题，请查看部署指南: DEPLOYMENT_GUIDE.md"
echo "  或查看服务日志: sudo journalctl -u kylin-qa-assistant -f"
echo ""

print_success "部署脚本执行完成，祝您使用愉快！"
