#!/bin/bash

# 银河麒麟系统依赖安装脚本
# 适用于银河麒麟桌面操作系统V10 SP1

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

# 检查是否为root用户
check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "此脚本需要root权限运行"
        log_info "请使用: sudo $0"
        exit 1
    fi
}

# 检测包管理器
detect_package_manager() {
    if command -v apt &> /dev/null; then
        echo "apt"
    elif command -v yum &> /dev/null; then
        echo "yum"
    elif command -v dnf &> /dev/null; then
        echo "dnf"
    else
        log_error "未检测到支持的包管理器 (apt/yum/dnf)"
        exit 1
    fi
}

# 更新包索引
update_package_index() {
    local pm=$1
    log_info "更新包索引..."
    
    case $pm in
        apt)
            apt update
            ;;
        yum)
            yum makecache
            ;;
        dnf)
            dnf makecache
            ;;
    esac
    
    log_success "包索引更新完成"
}

# 安装基础开发工具
install_build_tools() {
    local pm=$1
    log_info "安装基础开发工具..."
    
    case $pm in
        apt)
            apt install -y \
                build-essential \
                cmake \
                git \
                pkg-config \
                wget \
                curl
            ;;
        yum)
            yum groupinstall -y "Development Tools"
            yum install -y \
                cmake \
                git \
                pkgconfig \
                wget \
                curl
            ;;
        dnf)
            dnf groupinstall -y "Development Tools"
            dnf install -y \
                cmake \
                git \
                pkgconf-devel \
                wget \
                curl
            ;;
    esac
    
    log_success "基础开发工具安装完成"
}

# 检测Qt版本并安装相应开发包
detect_and_install_qt() {
    local pm=$1
    log_info "检测Qt版本并安装开发包..."

    # 首先尝试检测系统中可用的Qt版本
    local qt_version=""

    if pkg-config --exists Qt6Core 2>/dev/null; then
        qt_version="6"
        log_info "检测到Qt6已安装"
    elif pkg-config --exists Qt5Core 2>/dev/null; then
        qt_version="5"
        log_info "检测到Qt5，将安装Qt5开发包"
    else
        # 检查包管理器中可用的Qt版本
        case $pm in
            apt)
                if apt-cache search qt6-base-dev | grep -q qt6-base-dev; then
                    qt_version="6"
                elif apt-cache search qtbase5-dev | grep -q qtbase5-dev; then
                    qt_version="5"
                elif apt-cache search qt5-default | grep -q qt5-default; then
                    qt_version="5"
                fi
                ;;
        esac
    fi

    if [ -z "$qt_version" ]; then
        log_warning "无法检测Qt版本，尝试安装Qt5作为备选方案"
        qt_version="5"
    fi

    install_qt_packages "$pm" "$qt_version"
}

# 安装Qt开发包
install_qt_packages() {
    local pm=$1
    local qt_version=$2

    log_info "安装Qt${qt_version}开发包..."

    case $pm in
        apt)
            if [ "$qt_version" = "6" ]; then
                # 尝试安装Qt6包，如果失败则回退到Qt5
                if ! apt install -y \
                    qt6-base-dev \
                    qt6-base-dev-tools \
                    qt6-declarative-dev \
                    qt6-tools-dev \
                    qt6-multimedia-dev 2>/dev/null; then

                    log_warning "Qt6包安装失败，回退到Qt5"
                    install_qt5_packages "$pm"
                    return
                fi

                # 尝试安装可选的Qt6包
                apt install -y \
                    libqt6texttospeech6-dev \
                    qml6-module-qtquick \
                    qml6-module-qtquick-controls2 \
                    qml6-module-qtquick-layouts 2>/dev/null || log_warning "部分Qt6模块安装失败，但不影响核心功能"
            else
                install_qt5_packages "$pm"
            fi
            ;;
        yum)
            if [ "$qt_version" = "6" ]; then
                yum install -y \
                    qt6-qtbase-devel \
                    qt6-qtdeclarative-devel \
                    qt6-qttools-devel \
                    qt6-qtmultimedia-devel \
                    qt6-qtspeech-devel 2>/dev/null || install_qt5_packages_yum
            else
                install_qt5_packages_yum
            fi
            ;;
        dnf)
            if [ "$qt_version" = "6" ]; then
                dnf install -y \
                    qt6-qtbase-devel \
                    qt6-qtdeclarative-devel \
                    qt6-qttools-devel \
                    qt6-qtmultimedia-devel \
                    qt6-qtspeech-devel 2>/dev/null || install_qt5_packages_dnf
            else
                install_qt5_packages_dnf
            fi
            ;;
    esac

    log_success "Qt${qt_version}开发包安装完成"
}

# 安装Qt5包（APT）
install_qt5_packages() {
    local pm=$1
    log_info "安装Qt5开发包..."

    apt install -y \
        qtbase5-dev \
        qtbase5-dev-tools \
        qtdeclarative5-dev \
        qttools5-dev \
        qttools5-dev-tools \
        qtmultimedia5-dev \
        qml-module-qtquick2 \
        qml-module-qtquick-controls2 \
        qml-module-qtquick-layouts \
        libqt5texttospeech5-dev || {

        # 如果上述包也不可用，尝试更基础的包
        log_warning "标准Qt5包不可用，尝试安装基础包"
        apt install -y \
            qt5-default \
            qtbase5-dev \
            qtdeclarative5-dev \
            qttools5-dev 2>/dev/null || {

            log_error "无法安装Qt开发包，请手动安装"
            return 1
        }
    }
}

# 安装Qt5包（YUM）
install_qt5_packages_yum() {
    yum install -y \
        qt5-qtbase-devel \
        qt5-qtdeclarative-devel \
        qt5-qttools-devel \
        qt5-qtmultimedia-devel
}

# 安装Qt5包（DNF）
install_qt5_packages_dnf() {
    dnf install -y \
        qt5-qtbase-devel \
        qt5-qtdeclarative-devel \
        qt5-qttools-devel \
        qt5-qtmultimedia-devel
}

# 安装PDF处理库
install_pdf_libs() {
    local pm=$1
    log_info "安装PDF处理库..."
    
    case $pm in
        apt)
            apt install -y \
                libpoppler-cpp-dev \
                libpoppler-dev \
                poppler-utils
            ;;
        yum)
            yum install -y \
                poppler-cpp-devel \
                poppler-devel \
                poppler-utils
            ;;
        dnf)
            dnf install -y \
                poppler-cpp-devel \
                poppler-devel \
                poppler-utils
            ;;
    esac
    
    log_success "PDF处理库安装完成"
}

# 安装JSON处理库
install_json_libs() {
    local pm=$1
    log_info "安装JSON处理库..."
    
    case $pm in
        apt)
            apt install -y \
                libjsoncpp-dev \
                nlohmann-json3-dev
            ;;
        yum)
            yum install -y \
                jsoncpp-devel
            ;;
        dnf)
            dnf install -y \
                jsoncpp-devel
            ;;
    esac
    
    log_success "JSON处理库安装完成"
}

# 安装日志库
install_logging_libs() {
    local pm=$1
    log_info "安装日志库..."
    
    case $pm in
        apt)
            apt install -y \
                libspdlog-dev \
                libfmt-dev
            ;;
        yum)
            yum install -y \
                spdlog-devel \
                fmt-devel
            ;;
        dnf)
            dnf install -y \
                spdlog-devel \
                fmt-devel
            ;;
    esac
    
    log_success "日志库安装完成"
}

# 安装网络库
install_network_libs() {
    local pm=$1
    log_info "安装网络库..."
    
    case $pm in
        apt)
            apt install -y \
                libcurl4-openssl-dev \
                libssl-dev
            ;;
        yum)
            yum install -y \
                libcurl-devel \
                openssl-devel
            ;;
        dnf)
            dnf install -y \
                libcurl-devel \
                openssl-devel
            ;;
    esac
    
    log_success "网络库安装完成"
}

# 安装麒麟AI SDK（如果可用）
install_kylin_ai_sdk() {
    log_info "检查麒麟AI SDK..."
    
    # 检查是否存在麒麟AI SDK包
    local pm=$1
    local sdk_available=false
    
    case $pm in
        apt)
            if apt-cache search kylin-ai-sdk | grep -q kylin-ai-sdk; then
                apt install -y kylin-ai-sdk kylin-ai-sdk-dev
                sdk_available=true
            fi
            ;;
        yum)
            if yum search kylin-ai-sdk 2>/dev/null | grep -q kylin-ai-sdk; then
                yum install -y kylin-ai-sdk kylin-ai-sdk-devel
                sdk_available=true
            fi
            ;;
        dnf)
            if dnf search kylin-ai-sdk 2>/dev/null | grep -q kylin-ai-sdk; then
                dnf install -y kylin-ai-sdk kylin-ai-sdk-devel
                sdk_available=true
            fi
            ;;
    esac
    
    if [ "$sdk_available" = true ]; then
        log_success "麒麟AI SDK安装完成"
    else
        log_warning "麒麟AI SDK不可用，将使用备用方案"
    fi
}

# 安装Python依赖（用于某些工具脚本）
install_python_deps() {
    local pm=$1
    log_info "安装Python依赖..."
    
    case $pm in
        apt)
            apt install -y \
                python3 \
                python3-pip \
                python3-dev
            ;;
        yum)
            yum install -y \
                python3 \
                python3-pip \
                python3-devel
            ;;
        dnf)
            dnf install -y \
                python3 \
                python3-pip \
                python3-devel
            ;;
    esac
    
    # 安装Python包
    pip3 install --upgrade pip
    pip3 install requests numpy
    
    log_success "Python依赖安装完成"
}

# 验证安装
verify_installation() {
    log_info "验证安装..."

    local missing_tools=()
    local warnings=()

    # 检查编译工具
    command -v gcc >/dev/null || missing_tools+=("gcc")
    command -v g++ >/dev/null || missing_tools+=("g++")
    command -v cmake >/dev/null || missing_tools+=("cmake")
    command -v make >/dev/null || missing_tools+=("make")

    # 检查Qt（优先Qt6，备选Qt5）
    if pkg-config --exists Qt6Core 2>/dev/null; then
        log_info "Qt6 Core: ✓"
        pkg-config --exists Qt6Widgets || warnings+=("Qt6Widgets")
        pkg-config --exists Qt6Qml || warnings+=("Qt6Qml")
    elif pkg-config --exists Qt5Core 2>/dev/null; then
        log_info "Qt5 Core: ✓ (备选方案)"
        pkg-config --exists Qt5Widgets || warnings+=("Qt5Widgets")
        pkg-config --exists Qt5Qml || warnings+=("Qt5Qml")
    else
        missing_tools+=("Qt开发包")
    fi

    # 检查其他库
    if pkg-config --exists poppler-cpp 2>/dev/null; then
        log_info "Poppler: ✓"
    else
        missing_tools+=("poppler-cpp")
    fi

    # 检查可选依赖
    command -v python3 >/dev/null || warnings+=("python3")

    # 输出结果
    if [ ${#missing_tools[@]} -eq 0 ]; then
        log_success "核心依赖验证通过"

        if [ ${#warnings[@]} -gt 0 ]; then
            log_warning "以下可选组件缺失，但不影响基本功能:"
            for warning in "${warnings[@]}"; do
                echo "  - $warning"
            done
        fi

        return 0
    else
        log_error "以下核心依赖缺失:"
        for tool in "${missing_tools[@]}"; do
            echo "  - $tool"
        done
        return 1
    fi
}

# 主函数
main() {
    log_info "开始安装银河麒麟智能问答助手依赖..."
    
    check_root
    
    local pm=$(detect_package_manager)
    log_info "检测到包管理器: $pm"
    
    update_package_index "$pm"
    install_build_tools "$pm"
    detect_and_install_qt "$pm"
    install_pdf_libs "$pm"
    install_json_libs "$pm"
    install_logging_libs "$pm"
    install_network_libs "$pm"
    install_kylin_ai_sdk "$pm"
    install_python_deps "$pm"
    
    if verify_installation; then
        log_success "所有依赖安装完成！"
        log_info "现在可以运行构建脚本: ./scripts/build_kylin.sh --all"
    else
        log_error "依赖安装不完整，请检查错误信息"
        exit 1
    fi
}

# 执行主函数
main "$@"
