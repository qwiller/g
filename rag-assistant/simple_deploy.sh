#!/bin/bash
# 银河麒麟智能问答助手 - 简化部署脚本（无需sudo权限）

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() { echo -e "${BLUE}ℹ️  $1${NC}"; }
print_success() { echo -e "${GREEN}✅ $1${NC}"; }
print_warning() { echo -e "${YELLOW}⚠️  $1${NC}"; }
print_error() { echo -e "${RED}❌ $1${NC}"; }
print_step() { echo -e "\n${BLUE}🔄 $1${NC}"; }

echo -e "${GREEN}"
echo "=================================================="
echo "🤖 银河麒麟智能问答助手 - 简化部署"
echo "=================================================="
echo -e "${NC}"
print_info "本脚本无需管理员权限，适合普通用户使用"
print_info "部署完成后需要手动启动服务"
echo ""

# 检查Python
print_step "检查Python环境"
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    print_success "Python环境正常: $PYTHON_VERSION"
else
    print_error "未找到Python3，请先安装Python3"
    exit 1
fi

# 检查pip
if command -v pip3 &> /dev/null; then
    print_success "pip3 已安装"
else
    print_warning "pip3 未安装，尝试安装..."
    if command -v apt &> /dev/null; then
        print_info "请运行: sudo apt install python3-pip"
        exit 1
    fi
fi

# 创建虚拟环境
print_step "创建Python虚拟环境"
if [ -d "venv" ]; then
    print_warning "虚拟环境已存在，将重新创建"
    rm -rf venv
fi

python3 -m venv venv
if [ $? -eq 0 ]; then
    print_success "虚拟环境创建完成"
else
    print_error "虚拟环境创建失败"
    exit 1
fi

# 激活虚拟环境
source venv/bin/activate

# 升级pip
print_step "升级pip"
pip install --upgrade pip -q
print_success "pip升级完成"

# 安装依赖
print_step "安装Python依赖"
print_info "正在安装应用依赖..."

if pip install -r requirements.txt -q; then
    print_success "依赖安装完成"
elif pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple/ -q; then
    print_success "依赖安装完成（使用镜像源）"
else
    print_error "依赖安装失败"
    exit 1
fi

# 检查端口
print_step "检查端口占用"
if command -v netstat &> /dev/null; then
    if netstat -tlnp 2>/dev/null | grep -q ":8000 "; then
        print_warning "端口8000已被占用"
        netstat -tlnp | grep ":8000 "
    else
        print_success "端口8000可用"
    fi
fi

# 创建启动脚本
print_step "创建启动脚本"
cat > start_service.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
source venv/bin/activate
echo "🚀 启动银河麒麟智能问答助手..."
echo "📖 访问地址: http://localhost:8000"
echo "⏹️  按 Ctrl+C 停止服务"
python3 backend/main.py
EOF

chmod +x start_service.sh
print_success "启动脚本创建完成: start_service.sh"

# 创建后台启动脚本
cat > start_background.sh << 'EOF'
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
EOF

chmod +x start_background.sh
print_success "后台启动脚本创建完成: start_background.sh"

# 创建停止脚本
cat > stop_service.sh << 'EOF'
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
EOF

chmod +x stop_service.sh
print_success "停止脚本创建完成: stop_service.sh"

# 测试启动
print_step "测试服务启动"
print_info "正在测试服务启动..."

# 后台启动测试
nohup python3 backend/main.py > test.log 2>&1 &
TEST_PID=$!
sleep 3

if kill -0 $TEST_PID 2>/dev/null; then
    print_success "服务测试启动成功"
    
    # 测试连接
    if curl -s http://localhost:8000/status > /dev/null; then
        print_success "Web服务响应正常"
    else
        print_warning "Web服务可能还在启动中"
    fi
    
    # 停止测试服务
    kill $TEST_PID 2>/dev/null
    rm -f test.log
else
    print_error "服务测试启动失败"
    cat test.log
    rm -f test.log
fi

# 部署完成
echo ""
echo -e "${GREEN}"
echo "=================================================="
echo "🎉 简化部署完成！"
echo "=================================================="
echo -e "${NC}"

print_success "银河麒麟智能问答助手部署完成"
echo ""

echo -e "${BLUE}🚀 启动方式:${NC}"
echo "  前台启动: ./start_service.sh"
echo "  后台启动: ./start_background.sh"
echo "  停止服务: ./stop_service.sh"
echo ""

echo -e "${BLUE}🌐 访问地址:${NC}"
echo "  Web界面: http://localhost:8000"
echo "  API文档: http://localhost:8000/docs"
echo ""

echo -e "${BLUE}🖥️  桌面应用:${NC}"
echo "  source venv/bin/activate"
echo "  python3 desktop_app.py"
echo ""

echo -e "${BLUE}📋 使用说明:${NC}"
echo "  1. 运行 ./start_background.sh 启动服务"
echo "  2. 打开浏览器访问 http://localhost:8000"
echo "  3. 上传文档开始问答"
echo "  4. 运行 ./stop_service.sh 停止服务"
echo ""

print_success "现在可以启动服务开始使用了！"
