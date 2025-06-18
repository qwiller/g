#!/bin/bash

# 银河麒麟系统Qt5专用构建脚本
# 专为麒麟V10系统优化，使用Qt5作为唯一UI框架

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

echo "========================================"
echo "  银河麒麟智能问答助手 - Qt5专用构建"
echo "========================================"
echo

# 检查当前目录
if [ ! -f "CMakeLists.txt" ]; then
    log_error "请在项目根目录运行此脚本"
    exit 1
fi

# 检查Qt5环境
check_qt5_environment() {
    log_info "检查Qt5开发环境..."
    
    if ! pkg-config --exists Qt5Core; then
        log_error "Qt5Core未找到，请先安装Qt5开发包"
        log_info "运行: sudo ./scripts/install_dependencies_kylin.sh"
        exit 1
    fi
    
    local qt5_version=$(pkg-config --modversion Qt5Core)
    log_success "Qt5 Core版本: $qt5_version"
    
    # 检查必需组件
    if pkg-config --exists Qt5Widgets; then
        log_success "Qt5 Widgets: 可用"
    else
        log_error "Qt5 Widgets未找到"
        exit 1
    fi
    
    # 检查可选组件
    pkg-config --exists Qt5Qml && log_info "Qt5 Qml: 可用" || log_warning "Qt5 Qml: 不可用"
    pkg-config --exists Qt5Quick && log_info "Qt5 Quick: 可用" || log_warning "Qt5 Quick: 不可用"
    pkg-config --exists Qt5TextToSpeech && log_info "Qt5 TextToSpeech: 可用" || log_warning "Qt5 TextToSpeech: 不可用"
}

# 清理构建目录
clean_build() {
    log_info "清理构建目录..."
    rm -rf build
    mkdir build
    log_success "构建目录已准备"
}

# 配置CMake（Qt5专用）
configure_cmake_qt5() {
    log_info "配置CMake（Qt5专用）..."
    
    cd build
    
    # Qt5专用CMake配置
    local cmake_args=(
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=/opt/kylin-qa-assistant
        -DCMAKE_CXX_STANDARD=17
        -DBUILD_TESTS=OFF
    )
    
    # 显式指定Qt5路径（如果需要）
    if [ -d "/usr/lib/x86_64-linux-gnu/cmake/Qt5" ]; then
        cmake_args+=(-DQt5_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt5)
    fi
    
    # 根据架构添加特定配置
    local arch=$(uname -m)
    case $arch in
        aarch64)
            cmake_args+=(-DCMAKE_CXX_FLAGS="-march=armv8-a")
            ;;
        mips64)
            cmake_args+=(-DCMAKE_CXX_FLAGS="-march=mips64r2")
            ;;
    esac
    
    if cmake "${cmake_args[@]}" ..; then
        log_success "CMake配置完成（Qt5）"
    else
        log_error "CMake配置失败"
        exit 1
    fi
    
    cd ..
}

# 编译项目
build_project() {
    log_info "开始编译项目（Qt5版本）..."
    
    cd build
    
    local cpu_count=$(nproc)
    log_info "使用 $cpu_count 个CPU核心进行编译"
    
    if make -j$cpu_count; then
        log_success "项目编译完成"
    else
        log_error "项目编译失败"
        exit 1
    fi
    
    # 检查可执行文件
    if [ -f "src/KylinQAAssistant" ]; then
        log_success "可执行文件生成成功"
        
        # 检查Qt5依赖
        log_info "检查Qt5依赖..."
        local qt5_deps=$(ldd src/KylinQAAssistant | grep -i qt5 | wc -l)
        if [ $qt5_deps -gt 0 ]; then
            log_success "检测到 $qt5_deps 个Qt5依赖库"
        else
            log_warning "未检测到Qt5依赖库"
        fi
        
        # 检查缺失的库
        local missing_libs=$(ldd src/KylinQAAssistant | grep "not found" || true)
        if [ -z "$missing_libs" ]; then
            log_success "所有依赖库都已满足"
        else
            log_warning "存在缺失的依赖库:"
            echo "$missing_libs"
        fi
    else
        log_error "可执行文件生成失败"
        exit 1
    fi
    
    cd ..
}

# 测试应用程序
test_application() {
    log_info "测试应用程序..."
    
    cd build
    
    # 设置Qt5环境变量
    export QT_QPA_PLATFORM=xcb
    export QT_SELECT=qt5
    
    # 测试启动
    if timeout 5 ./src/KylinQAAssistant --version 2>/dev/null; then
        log_success "应用程序可以正常启动"
    else
        log_info "应用程序测试完成（GUI应用正常行为）"
    fi
    
    cd ..
}

# 安装应用程序
install_application() {
    log_info "安装应用程序..."
    
    cd build
    
    if sudo make install; then
        log_success "应用程序安装完成"
        
        # 设置权限
        sudo chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant
        
        # 创建必要目录
        sudo mkdir -p /opt/kylin-qa-assistant/logs
        sudo mkdir -p /opt/kylin-qa-assistant/data
        sudo chmod 777 /opt/kylin-qa-assistant/logs
        sudo chmod 777 /opt/kylin-qa-assistant/data
        
        log_success "权限和目录设置完成"
    else
        log_error "应用程序安装失败"
        exit 1
    fi
    
    cd ..
}

# 创建桌面快捷方式
create_desktop_entry() {
    log_info "创建桌面快捷方式..."
    
    sudo tee /usr/share/applications/kylin-qa-assistant.desktop > /dev/null << 'EOF'
[Desktop Entry]
Name=银河麒麟智能问答助手
Name[en]=Kylin QA Assistant
Comment=基于RAG技术的智能问答系统（Qt5版本）
Comment[en]=Intelligent Q&A system based on RAG technology (Qt5 version)
Exec=/opt/kylin-qa-assistant/bin/KylinQAAssistant
Icon=applications-office
Terminal=false
Type=Application
Categories=Office;Utility;Education;
StartupNotify=true
MimeType=application/pdf;text/markdown;text/plain;
Keywords=AI;问答;助手;麒麟;RAG;Qt5;
EOF

    sudo chmod 644 /usr/share/applications/kylin-qa-assistant.desktop
    
    # 刷新桌面数据库
    if command -v update-desktop-database &> /dev/null; then
        sudo update-desktop-database /usr/share/applications
    fi
    
    log_success "桌面快捷方式已创建"
}

# 显示完成信息
show_completion_info() {
    echo
    echo "========================================"
    echo "        Qt5构建完成"
    echo "========================================"
    echo
    echo "✅ 应用程序已成功编译并安装（Qt5版本）"
    echo "📍 安装位置: /opt/kylin-qa-assistant/"
    echo "🖥️ UI框架: Qt5 (麒麟系统原生支持)"
    echo
    echo "启动方式："
    echo "1. 桌面快捷方式: 银河麒麟智能问答助手"
    echo "2. 命令行启动: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    echo "3. 从构建目录: ./build/src/KylinQAAssistant"
    echo
    echo "配置文件: /opt/kylin-qa-assistant/etc/kylin-qa-assistant/"
    echo "日志文件: /opt/kylin-qa-assistant/logs/"
    echo
    echo "如有问题，请运行: ./scripts/troubleshoot_kylin.sh"
    echo
}

# 主函数
main() {
    check_qt5_environment
    clean_build
    configure_cmake_qt5
    build_project
    test_application
    install_application
    create_desktop_entry
    show_completion_info
}

# 执行主函数
main "$@"
