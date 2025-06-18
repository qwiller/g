#!/bin/bash

# 银河麒麟系统快速安装脚本
# 针对常见的麒麟系统版本优化

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

# 检查root权限
check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "此脚本需要root权限运行"
        log_info "请使用: sudo $0"
        exit 1
    fi
}

# 检测麒麟系统版本
detect_kylin_version() {
    log_info "检测麒麟系统版本..."
    
    local version_info=""
    
    if [ -f /etc/kylin-release ]; then
        version_info=$(cat /etc/kylin-release)
    elif [ -f /etc/os-release ]; then
        version_info=$(grep "PRETTY_NAME" /etc/os-release | cut -d'"' -f2)
    fi
    
    log_info "系统信息: $version_info"
    
    # 根据版本信息判断包管理器和包名
    if echo "$version_info" | grep -qi "V10"; then
        echo "v10"
    elif echo "$version_info" | grep -qi "V4"; then
        echo "v4"
    else
        echo "unknown"
    fi
}

# 安装基础依赖（麒麟V10）
install_deps_v10() {
    log_info "为麒麟V10安装依赖..."
    
    # 更新包索引
    apt update
    
    # 安装基础开发工具
    apt install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        wget \
        curl
    
    # 安装Qt5（麒麟V10专用配置）
    log_info "安装Qt5开发包（麒麟V10优化版本）..."

    # 优先安装核心Qt5包
    if apt install -y qtbase5-dev qtbase5-dev-tools; then
        log_success "Qt5核心包安装成功"

        # 尝试安装扩展包
        apt install -y \
            qtdeclarative5-dev \
            qttools5-dev \
            qtmultimedia5-dev \
            qml-module-qtquick2 \
            qml-module-qtquick-controls2 2>/dev/null || log_warning "部分Qt5扩展包安装失败"

    else
        # 回退方案
        log_warning "尝试安装基础Qt5包..."
        if apt install -y qt5-default qtbase5-dev 2>/dev/null; then
            log_success "Qt5基础包安装成功"
        else
            log_error "Qt5安装失败"
            return 1
        fi
    fi
    
    # 安装其他依赖
    apt install -y \
        libpoppler-cpp-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        python3 \
        python3-pip
    
    # 尝试安装可选依赖
    apt install -y \
        libjsoncpp-dev \
        libfmt-dev 2>/dev/null || log_warning "部分可选依赖安装失败"
    
    log_success "麒麟V10依赖安装完成"
}

# 安装基础依赖（麒麟V4）
install_deps_v4() {
    log_info "为麒麟V4安装依赖..."
    
    # 麒麟V4通常基于CentOS，使用yum
    yum makecache
    
    yum groupinstall -y "Development Tools"
    yum install -y \
        cmake \
        git \
        pkgconfig \
        wget \
        curl \
        qt5-qtbase-devel \
        qt5-qtdeclarative-devel \
        qt5-qttools-devel \
        poppler-cpp-devel \
        libcurl-devel \
        openssl-devel \
        python3 \
        python3-pip
    
    log_success "麒麟V4依赖安装完成"
}

# 通用依赖安装（未知版本）
install_deps_generic() {
    log_warning "未识别的麒麟版本，尝试通用安装方法..."
    
    # 检测包管理器
    if command -v apt &> /dev/null; then
        log_info "使用APT包管理器"
        install_deps_v10
    elif command -v yum &> /dev/null; then
        log_info "使用YUM包管理器"
        install_deps_v4
    elif command -v dnf &> /dev/null; then
        log_info "使用DNF包管理器"
        dnf groupinstall -y "Development Tools"
        dnf install -y cmake git qt5-qtbase-devel qt5-qtdeclarative-devel
    else
        log_error "未找到支持的包管理器"
        return 1
    fi
}

# 验证安装
verify_deps() {
    log_info "验证依赖安装..."
    
    local missing=()
    
    command -v gcc >/dev/null || missing+=("gcc")
    command -v cmake >/dev/null || missing+=("cmake")
    
    if pkg-config --exists Qt5Core 2>/dev/null; then
        log_success "Qt5: ✓"
    elif pkg-config --exists Qt6Core 2>/dev/null; then
        log_success "Qt6: ✓"
    else
        missing+=("Qt开发包")
    fi
    
    if pkg-config --exists poppler-cpp 2>/dev/null; then
        log_success "Poppler: ✓"
    else
        missing+=("poppler-cpp")
    fi
    
    if [ ${#missing[@]} -eq 0 ]; then
        log_success "所有核心依赖验证通过"
        return 0
    else
        log_error "以下依赖缺失:"
        for dep in "${missing[@]}"; do
            echo "  - $dep"
        done
        return 1
    fi
}

# 编译项目
build_project() {
    log_info "开始编译项目..."
    
    # 清理并创建构建目录
    rm -rf build
    mkdir build
    cd build
    
    # 配置CMake
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/opt/kylin-qa-assistant \
          ..
    
    # 编译
    make -j$(nproc)
    
    log_success "项目编译完成"
}

# 安装应用程序
install_app() {
    log_info "安装应用程序..."
    
    cd build
    make install
    
    # 创建桌面快捷方式
    cat > /usr/share/applications/kylin-qa-assistant.desktop << EOF
[Desktop Entry]
Name=银河麒麟智能问答助手
Name[en]=Kylin QA Assistant
Comment=基于RAG技术的智能问答系统
Comment[en]=Intelligent Q&A system based on RAG technology
Exec=/opt/kylin-qa-assistant/bin/KylinQAAssistant
Icon=/opt/kylin-qa-assistant/share/kylin-qa-assistant/resources/app_icon.png
Terminal=false
Type=Application
Categories=Office;Utility;
StartupNotify=true
EOF
    
    log_success "应用程序安装完成"
    log_info "可执行文件: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    log_info "桌面快捷方式已创建"
}

# 主函数
main() {
    echo "========================================"
    echo "  银河麒麟智能问答助手 - 快速安装脚本"
    echo "========================================"
    echo
    
    check_root
    
    local kylin_version=$(detect_kylin_version)
    log_info "检测到麒麟版本: $kylin_version"
    
    # 根据版本安装依赖
    case $kylin_version in
        v10)
            install_deps_v10 || exit 1
            ;;
        v4)
            install_deps_v4 || exit 1
            ;;
        *)
            install_deps_generic || exit 1
            ;;
    esac
    
    # 验证依赖
    if ! verify_deps; then
        log_error "依赖验证失败，请检查安装"
        exit 1
    fi
    
    # 编译项目
    cd "$(dirname "$0")/.."
    build_project || exit 1
    
    # 安装应用程序
    install_app || exit 1
    
    echo
    log_success "安装完成！"
    echo
    echo "使用方法:"
    echo "1. 命令行启动: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    echo "2. 桌面快捷方式: 在应用程序菜单中找到'银河麒麟智能问答助手'"
    echo "3. 配置文件: /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json"
    echo
    echo "如有问题，请查看日志: /opt/kylin-qa-assistant/logs/kylin_qa.log"
    echo
}

# 执行主函数
main "$@"
