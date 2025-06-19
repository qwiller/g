#!/bin/bash
# 银河麒麟AI SDK集成部署脚本
# 适用于银河麒麟操作系统

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
}

# 检查是否为银河麒麟系统
check_kylin_os() {
    log_step "检查操作系统..."
    
    if [ -f /etc/kylin-release ]; then
        KYLIN_VERSION=$(cat /etc/kylin-release)
        log_info "检测到银河麒麟系统: $KYLIN_VERSION"
    elif grep -q "Kylin" /etc/os-release 2>/dev/null; then
        log_info "检测到银河麒麟系统"
    else
        log_warn "未检测到银河麒麟系统，继续安装但可能遇到兼容性问题"
    fi
}

# 检查系统依赖
check_dependencies() {
    log_step "检查系统依赖..."
    
    # 检查Python
    if ! command -v python3 &> /dev/null; then
        log_error "Python3 未安装，请先安装 Python 3.8+"
        exit 1
    fi
    
    PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
    log_info "Python版本: $PYTHON_VERSION"
    
    # 检查pip
    if ! command -v pip3 &> /dev/null; then
        log_warn "pip3 未安装，尝试安装..."
        sudo apt update
        sudo apt install -y python3-pip
    fi
    
    # 检查git
    if ! command -v git &> /dev/null; then
        log_warn "Git 未安装，尝试安装..."
        sudo apt update
        sudo apt install -y git
    fi
}

# 安装银河麒麟AI SDK
install_kylin_sdk() {
    log_step "安装银河麒麟AI SDK..."
    
    # 更新包管理器
    sudo apt update
    
    # 安装OCR功能包
    log_info "安装OCR功能包..."
    if sudo apt install -y libkysdk-ocr libkysdk-ocr-dev; then
        log_info "OCR功能包安装成功"
    else
        log_warn "OCR功能包安装失败，将使用基础功能"
    fi
    
    # 安装搜索功能包
    log_info "安装搜索功能包..."
    if sudo apt install -y libkysdk-search libkysdk-search-dev; then
        log_info "搜索功能包安装成功"
    else
        log_warn "搜索功能包安装失败，将使用基础功能"
    fi
    
    # 安装音效功能包
    log_info "安装音效功能包..."
    if sudo apt install -y libkysdk-soundeffects libkysdk-soundeffects-dev; then
        log_info "音效功能包安装成功"
    else
        log_warn "音效功能包安装失败，将使用基础功能"
    fi
    
    # 更新库缓存
    sudo ldconfig
    
    # 验证安装
    log_info "验证SDK安装..."
    if ls /usr/lib/libkysdk-* >/dev/null 2>&1; then
        log_info "SDK库文件安装成功"
        ls -la /usr/lib/libkysdk-*
    else
        log_warn "未找到SDK库文件，某些功能可能不可用"
    fi
}

# 设置环境变量
setup_environment() {
    log_step "设置环境变量..."
    
    # 添加到当前会话
    export LD_LIBRARY_PATH=/usr/lib:/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
    export KYSDK_PATH=/usr/include/kysdk
    
    # 添加到用户配置文件
    BASHRC_FILE="$HOME/.bashrc"
    
    if ! grep -q "KYSDK_PATH" "$BASHRC_FILE"; then
        echo "" >> "$BASHRC_FILE"
        echo "# 银河麒麟AI SDK环境变量" >> "$BASHRC_FILE"
        echo "export LD_LIBRARY_PATH=/usr/lib:/usr/lib/x86_64-linux-gnu:\$LD_LIBRARY_PATH" >> "$BASHRC_FILE"
        echo "export KYSDK_PATH=/usr/include/kysdk" >> "$BASHRC_FILE"
        log_info "环境变量已添加到 $BASHRC_FILE"
    else
        log_info "环境变量已存在"
    fi
}

# 安装Python依赖
install_python_dependencies() {
    log_step "安装Python依赖..."
    
    # 创建虚拟环境
    if [ ! -d "venv" ]; then
        log_info "创建Python虚拟环境..."
        python3 -m venv venv
    fi
    
    # 激活虚拟环境
    source venv/bin/activate
    
    # 升级pip
    pip install --upgrade pip
    
    # 安装基础依赖
    log_info "安装基础依赖..."
    pip install -r requirements.txt
    
    # 安装SDK相关依赖
    log_info "安装SDK相关依赖..."
    pip install Pillow ctypes-sh
    
    log_info "Python依赖安装完成"
}

# 创建配置文件
create_config() {
    log_step "创建配置文件..."
    
    # 创建配置目录
    mkdir -p config
    mkdir -p uploads
    mkdir -p logs
    
    # 创建AI配置文件
    if [ ! -f "config/ai_settings.json" ]; then
        log_info "创建AI配置文件..."
        cat > config/ai_settings.json << EOF
{
  "ai_provider": "mixed",
  "kylin_sdk": {
    "enabled": true,
    "ocr_enabled": true,
    "search_enabled": true,
    "sound_enabled": true,
    "search_directories": ["/home", "/opt", "/usr/share/doc"]
  },
  "siliconflow": {
    "enabled": true,
    "api_key": "sk-owsayozifrzvaxuxvyvywmyzcceokwatdbolevdnfnbwlurp",
    "api_base": "https://api.siliconflow.cn/v1",
    "model": "deepseek-ai/DeepSeek-V3"
  },
  "rag_settings": {
    "chunk_size": 500,
    "chunk_overlap": 50,
    "max_search_results": 3,
    "enable_local_search": true
  }
}
EOF
        log_info "AI配置文件创建完成"
    else
        log_info "AI配置文件已存在"
    fi
    
    # 创建环境配置文件
    if [ ! -f ".env" ]; then
        log_info "创建环境配置文件..."
        cat > .env << EOF
# 服务配置
HOST=0.0.0.0
PORT=8000
DEBUG=false

# 日志配置
LOG_LEVEL=INFO
LOG_FILE=logs/app.log

# SDK配置
KYLIN_SDK_ENABLED=true
OCR_ENABLED=true
SEARCH_ENABLED=true
SOUND_ENABLED=true
EOF
        log_info "环境配置文件创建完成"
    else
        log_info "环境配置文件已存在"
    fi
}

# 设置权限
setup_permissions() {
    log_step "设置文件权限..."
    
    # 设置脚本执行权限
    chmod +x *.sh
    find backend -name "*.py" -exec chmod +x {} \;
    
    # 设置目录权限
    chmod 755 config uploads logs
    
    # 设置文件所有者
    chown -R $(whoami):$(whoami) .
    
    log_info "权限设置完成"
}

# 测试安装
test_installation() {
    log_step "测试安装..."
    
    # 激活虚拟环境
    source venv/bin/activate
    
    # 测试Python导入
    log_info "测试Python模块导入..."
    python3 -c "
import sys
sys.path.append('backend')
try:
    from kylin_sdk_wrapper import get_kylin_sdk
    sdk = get_kylin_sdk()
    availability = sdk.is_available()
    print(f'SDK模块可用性: {availability}')
    if any(availability.values()):
        print('✅ 银河麒麟SDK集成成功')
    else:
        print('⚠️  SDK功能不可用，将使用基础功能')
except Exception as e:
    print(f'⚠️  SDK测试失败: {e}')
    print('将使用基础功能模式')
"
    
    log_info "安装测试完成"
}

# 启动服务
start_service() {
    log_step "启动服务..."
    
    # 检查端口是否被占用
    if netstat -tlnp 2>/dev/null | grep -q ":8000 "; then
        log_warn "端口8000已被占用，尝试停止现有服务..."
        pkill -f "python.*main.py" || true
        sleep 2
    fi
    
    # 启动服务
    log_info "启动银河麒麟智能问答助手..."
    ./start_background.sh
    
    # 等待服务启动
    sleep 5
    
    # 检查服务状态
    if curl -s http://localhost:8000/ >/dev/null; then
        log_info "✅ 服务启动成功！"
        log_info "🌐 访问地址: http://localhost:8000"
        log_info "📚 API文档: http://localhost:8000/docs"
    else
        log_error "❌ 服务启动失败，请检查日志"
        tail -n 20 logs/app.log 2>/dev/null || echo "日志文件不存在"
    fi
}

# 显示使用说明
show_usage() {
    log_step "部署完成！"
    
    echo ""
    echo "🎉 银河麒麟AI SDK集成部署完成！"
    echo ""
    echo "📋 使用说明:"
    echo "  • Web界面: http://localhost:8000"
    echo "  • API文档: http://localhost:8000/docs"
    echo "  • 启动服务: ./start_background.sh"
    echo "  • 停止服务: ./stop_service.sh"
    echo "  • 查看日志: tail -f logs/app.log"
    echo "  • 运行测试: python3 test_kylin_sdk_integration.py"
    echo ""
    echo "🔧 功能特性:"
    echo "  • ✅ 文档问答 (PDF、Markdown、文本)"
    echo "  • ✅ OCR文字识别 (图片文档)"
    echo "  • ✅ 本地文件搜索"
    echo "  • ✅ 系统音效播放"
    echo "  • ✅ 混合AI服务 (本地+远程)"
    echo ""
    echo "📖 详细文档: KYLIN_SDK_DEPLOYMENT_GUIDE.md"
    echo ""
}

# 主函数
main() {
    echo "🚀 银河麒麟AI SDK集成部署脚本"
    echo "=================================="
    echo ""
    
    # 检查是否在正确的目录
    if [ ! -f "requirements.txt" ]; then
        log_error "请在项目根目录运行此脚本"
        exit 1
    fi
    
    # 执行部署步骤
    check_kylin_os
    check_dependencies
    install_kylin_sdk
    setup_environment
    install_python_dependencies
    create_config
    setup_permissions
    test_installation
    start_service
    show_usage
    
    log_info "🎯 部署完成！"
}

# 错误处理
trap 'log_error "部署过程中发生错误，请检查上述输出"; exit 1' ERR

# 运行主函数
main "$@"
