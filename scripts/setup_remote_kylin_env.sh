#!/bin/bash

# 远程麒麟系统开发环境配置脚本
# 在远程麒麟服务器上运行此脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 项目配置
PROJECT_NAME="kylin-qa-assistant"
PROJECT_DIR="$HOME/$PROJECT_NAME"
USER_NAME=$(whoami)

echo "========================================"
echo "  麒麟系统远程开发环境配置"
echo "========================================"
echo
echo "用户: $USER_NAME"
echo "项目目录: $PROJECT_DIR"
echo

# 检查系统信息
check_system_info() {
    log_info "检查系统信息..."
    
    echo "操作系统信息:"
    cat /etc/os-release | head -5
    echo
    
    echo "内核版本:"
    uname -r
    echo
    
    echo "CPU架构:"
    uname -m
    echo
    
    echo "可用内存:"
    free -h
    echo
    
    echo "磁盘空间:"
    df -h /
    echo
}

# 更新系统包
update_system() {
    log_info "更新系统包..."
    
    sudo apt update
    sudo apt upgrade -y
    
    log_success "系统包更新完成"
}

# 安装基础开发工具
install_basic_tools() {
    log_info "安装基础开发工具..."
    
    sudo apt install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        wget \
        curl \
        vim \
        nano \
        htop \
        tree \
        unzip \
        zip \
        rsync \
        openssh-server
    
    log_success "基础开发工具安装完成"
}

# 安装Qt5开发环境
install_qt5_dev() {
    log_info "安装Qt5开发环境..."
    
    sudo apt install -y \
        qtbase5-dev \
        qtbase5-dev-tools \
        qtdeclarative5-dev \
        qttools5-dev \
        qttools5-dev-tools \
        qtmultimedia5-dev \
        qml-module-qtquick2 \
        qml-module-qtquick-controls2 \
        qml-module-qtquick-layouts \
        libqt5texttospeech5-dev
    
    # 验证Qt5安装
    if pkg-config --exists Qt5Core; then
        QT5_VERSION=$(pkg-config --modversion Qt5Core)
        log_success "Qt5安装成功，版本: $QT5_VERSION"
    else
        log_error "Qt5安装失败"
        return 1
    fi
}

# 安装项目依赖
install_project_deps() {
    log_info "安装项目依赖..."
    
    sudo apt install -y \
        libpoppler-cpp-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        libjsoncpp-dev \
        libfmt-dev \
        python3 \
        python3-pip
    
    log_success "项目依赖安装完成"
}

# 安装调试工具
install_debug_tools() {
    log_info "安装调试工具..."
    
    sudo apt install -y \
        gdb \
        gdbserver \
        valgrind \
        strace \
        ltrace
    
    # 验证GDB安装
    if command -v gdb >/dev/null 2>&1; then
        GDB_VERSION=$(gdb --version | head -1)
        log_success "GDB安装成功: $GDB_VERSION"
    else
        log_error "GDB安装失败"
        return 1
    fi
    
    # 验证GDBServer安装
    if command -v gdbserver >/dev/null 2>&1; then
        log_success "GDBServer安装成功"
    else
        log_error "GDBServer安装失败"
        return 1
    fi
}

# 配置SSH服务
configure_ssh() {
    log_info "配置SSH服务..."
    
    # 启动SSH服务
    sudo systemctl enable ssh
    sudo systemctl start ssh
    
    # 检查SSH状态
    if systemctl is-active --quiet ssh; then
        log_success "SSH服务运行正常"
    else
        log_error "SSH服务启动失败"
        return 1
    fi
    
    # 配置SSH密钥目录
    mkdir -p ~/.ssh
    chmod 700 ~/.ssh
    
    # 如果authorized_keys不存在，创建它
    if [ ! -f ~/.ssh/authorized_keys ]; then
        touch ~/.ssh/authorized_keys
        chmod 600 ~/.ssh/authorized_keys
    fi
    
    log_success "SSH配置完成"
}

# 创建项目目录
create_project_dir() {
    log_info "创建项目目录..."
    
    mkdir -p "$PROJECT_DIR"
    cd "$PROJECT_DIR"
    
    # 创建基本目录结构
    mkdir -p src include build docs scripts tests
    
    log_success "项目目录创建完成: $PROJECT_DIR"
}

# 配置Git
configure_git() {
    log_info "配置Git..."
    
    # 检查Git配置
    if ! git config --global user.name >/dev/null 2>&1; then
        read -p "请输入Git用户名: " git_username
        git config --global user.name "$git_username"
    fi
    
    if ! git config --global user.email >/dev/null 2>&1; then
        read -p "请输入Git邮箱: " git_email
        git config --global user.email "$git_email"
    fi
    
    # 配置Git安全目录
    git config --global --add safe.directory "$PROJECT_DIR"
    
    log_success "Git配置完成"
}

# 创建开发脚本
create_dev_scripts() {
    log_info "创建开发脚本..."
    
    # 创建构建脚本
    cat > "$PROJECT_DIR/build.sh" << 'EOF'
#!/bin/bash

# 项目构建脚本

set -e

BUILD_TYPE=${1:-Debug}
BUILD_DIR="build"

echo "开始构建项目..."
echo "构建类型: $BUILD_TYPE"

# 创建构建目录
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置CMake
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      ..

# 编译
make -j$(nproc)

echo "构建完成！"
echo "可执行文件: $BUILD_DIR/src/KylinQAAssistant"
EOF

    chmod +x "$PROJECT_DIR/build.sh"
    
    # 创建GDB服务器启动脚本
    cat > "$PROJECT_DIR/start_gdbserver.sh" << 'EOF'
#!/bin/bash

# GDB服务器启动脚本

GDB_PORT=${1:-5555}
EXECUTABLE=${2:-"./build/src/KylinQAAssistant"}

if [ ! -f "$EXECUTABLE" ]; then
    echo "错误: 可执行文件不存在: $EXECUTABLE"
    echo "请先运行 ./build.sh 编译项目"
    exit 1
fi

echo "启动GDB服务器..."
echo "端口: $GDB_PORT"
echo "可执行文件: $EXECUTABLE"
echo "等待调试器连接..."

# 停止现有的gdbserver进程
pkill gdbserver 2>/dev/null || true
sleep 1

# 启动GDB服务器
gdbserver :$GDB_PORT "$EXECUTABLE" "$@"
EOF

    chmod +x "$PROJECT_DIR/start_gdbserver.sh"
    
    # 创建清理脚本
    cat > "$PROJECT_DIR/clean.sh" << 'EOF'
#!/bin/bash

# 项目清理脚本

echo "清理构建文件..."
rm -rf build
echo "清理完成！"
EOF

    chmod +x "$PROJECT_DIR/clean.sh"
    
    log_success "开发脚本创建完成"
}

# 配置防火墙
configure_firewall() {
    log_info "配置防火墙..."
    
    # 检查是否有ufw
    if command -v ufw >/dev/null 2>&1; then
        # 允许SSH端口
        sudo ufw allow 22/tcp
        # 允许GDB调试端口
        sudo ufw allow 5555/tcp
        # 允许应用程序端口
        sudo ufw allow 8080/tcp
        
        log_success "防火墙规则已配置"
    else
        log_warning "未找到ufw防火墙，请手动配置防火墙规则"
    fi
}

# 创建VS Code服务器配置
create_vscode_server_config() {
    log_info "创建VS Code服务器配置..."
    
    # 创建VS Code服务器目录
    mkdir -p ~/.vscode-server/data/Machine
    
    # 创建设置文件
    cat > ~/.vscode-server/data/Machine/settings.json << 'EOF'
{
    "terminal.integrated.defaultProfile.linux": "bash",
    "files.watcherExclude": {
        "**/build/**": true,
        "**/.git/**": true
    },
    "search.exclude": {
        "**/build": true,
        "**/.git": true
    }
}
EOF

    log_success "VS Code服务器配置完成"
}

# 显示配置信息
show_config_info() {
    echo
    echo "========================================"
    echo "           配置完成"
    echo "========================================"
    echo
    echo "📋 系统信息:"
    echo "  用户: $USER_NAME"
    echo "  项目目录: $PROJECT_DIR"
    echo "  SSH状态: $(systemctl is-active ssh)"
    echo
    echo "🛠️ 开发工具:"
    echo "  GCC: $(gcc --version | head -1)"
    echo "  CMake: $(cmake --version | head -1)"
    echo "  Qt5: $(pkg-config --modversion Qt5Core 2>/dev/null || echo '未安装')"
    echo "  GDB: $(gdb --version | head -1)"
    echo
    echo "🚀 使用方法:"
    echo "  1. 编译项目: cd $PROJECT_DIR && ./build.sh"
    echo "  2. 启动调试: ./start_gdbserver.sh"
    echo "  3. 清理构建: ./clean.sh"
    echo
    echo "🔗 连接信息:"
    local_ip=$(hostname -I | awk '{print $1}' 2>/dev/null || echo "未知")
    echo "  SSH连接: ssh $USER_NAME@$local_ip"
    echo "  GDB端口: 5555"
    echo "  项目路径: $PROJECT_DIR"
    echo
    echo "📁 项目结构:"
    echo "  源代码: $PROJECT_DIR/src/"
    echo "  头文件: $PROJECT_DIR/include/"
    echo "  构建目录: $PROJECT_DIR/build/"
    echo "  脚本: $PROJECT_DIR/*.sh"
    echo
}

# 主函数
main() {
    check_system_info
    update_system
    install_basic_tools
    install_qt5_dev
    install_project_deps
    install_debug_tools
    configure_ssh
    create_project_dir
    configure_git
    create_dev_scripts
    configure_firewall
    create_vscode_server_config
    show_config_info
}

# 执行主函数
main "$@"
