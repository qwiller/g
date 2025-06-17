#!/bin/bash

# 银河麒麟智能问答助手构建脚本
# 适用于银河麒麟桌面操作系统V10 SP1
# 支持架构: x86_64, aarch64, mips64

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
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

# 检查是否为麒麟系统
check_kylin_system() {
    log_info "检查操作系统..."
    
    if [ -f /etc/os-release ]; then
        if grep -qi "kylin" /etc/os-release; then
            log_success "检测到银河麒麟操作系统"
            return 0
        fi
    fi
    
    if [ -f /etc/kylin-release ]; then
        log_success "检测到银河麒麟操作系统"
        return 0
    fi
    
    log_warning "未检测到银河麒麟操作系统，某些功能可能无法正常工作"
    return 1
}

# 检查CPU架构
check_architecture() {
    local arch=$(uname -m)
    log_info "检测到CPU架构: $arch"
    
    case $arch in
        x86_64)
            log_success "支持的架构: x86_64"
            ;;
        aarch64)
            log_success "支持的架构: ARM64"
            ;;
        mips64)
            log_success "支持的架构: MIPS64"
            ;;
        *)
            log_warning "架构 $arch 可能不被完全支持"
            ;;
    esac
}

# 检查依赖包
check_dependencies() {
    log_info "检查系统依赖..."
    
    local missing_deps=()
    
    # 检查编译工具
    if ! command -v gcc &> /dev/null; then
        missing_deps+=("gcc")
    fi
    
    if ! command -v g++ &> /dev/null; then
        missing_deps+=("g++")
    fi
    
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    fi
    
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
    fi
    
    # 检查Qt6
    if ! pkg-config --exists Qt6Core; then
        missing_deps+=("qt6-base-dev")
    fi
    
    if ! pkg-config --exists Qt6Widgets; then
        missing_deps+=("qt6-base-dev")
    fi
    
    if ! pkg-config --exists Qt6Qml; then
        missing_deps+=("qt6-declarative-dev")
    fi
    
    # 检查其他依赖
    if ! pkg-config --exists poppler-cpp; then
        missing_deps+=("libpoppler-cpp-dev")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "缺少以下依赖包:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        log_info "请运行安装依赖脚本: ./scripts/install_dependencies_kylin.sh"
        return 1
    fi
    
    log_success "所有依赖检查通过"
    return 0
}

# 清理构建目录
clean_build() {
    log_info "清理构建目录..."
    if [ -d "build" ]; then
        rm -rf build
        log_success "构建目录已清理"
    fi
}

# 创建构建目录
create_build_dir() {
    log_info "创建构建目录..."
    mkdir -p build
    cd build
}

# 配置CMake
configure_cmake() {
    log_info "配置CMake..."
    
    local cmake_args=(
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=/opt/kylin-qa-assistant
        -DCMAKE_CXX_STANDARD=17
    )
    
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
    
    cmake "${cmake_args[@]}" ..
    
    if [ $? -eq 0 ]; then
        log_success "CMake配置完成"
    else
        log_error "CMake配置失败"
        return 1
    fi
}

# 编译项目
build_project() {
    log_info "开始编译项目..."
    
    local cpu_count=$(nproc)
    log_info "使用 $cpu_count 个CPU核心进行编译"
    
    make -j$cpu_count
    
    if [ $? -eq 0 ]; then
        log_success "项目编译完成"
    else
        log_error "项目编译失败"
        return 1
    fi
}

# 运行测试
run_tests() {
    log_info "运行测试..."
    
    if [ -f "tests/test_runner" ]; then
        ./tests/test_runner
        if [ $? -eq 0 ]; then
            log_success "所有测试通过"
        else
            log_error "测试失败"
            return 1
        fi
    else
        log_warning "未找到测试程序，跳过测试"
    fi
}

# 创建安装包
create_package() {
    log_info "创建安装包..."
    
    make package
    
    if [ $? -eq 0 ]; then
        log_success "安装包创建完成"
        ls -la *.deb *.rpm 2>/dev/null || true
    else
        log_error "安装包创建失败"
        return 1
    fi
}

# 安装应用程序
install_application() {
    log_info "安装应用程序..."
    
    if [ "$EUID" -ne 0 ]; then
        log_warning "需要root权限进行安装，请使用sudo运行"
        return 1
    fi
    
    make install
    
    if [ $? -eq 0 ]; then
        log_success "应用程序安装完成"
        log_info "安装路径: /opt/kylin-qa-assistant"
        log_info "可执行文件: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    else
        log_error "应用程序安装失败"
        return 1
    fi
}

# 显示帮助信息
show_help() {
    echo "银河麒麟智能问答助手构建脚本"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  -h, --help      显示帮助信息"
    echo "  -c, --clean     清理构建目录"
    echo "  -b, --build     编译项目"
    echo "  -t, --test      运行测试"
    echo "  -p, --package   创建安装包"
    echo "  -i, --install   安装应用程序（需要root权限）"
    echo "  -a, --all       执行完整构建流程（清理、编译、测试、打包）"
    echo ""
    echo "示例:"
    echo "  $0 --all        # 完整构建流程"
    echo "  $0 --build      # 仅编译"
    echo "  $0 --package    # 仅创建安装包"
}

# 主函数
main() {
    local action=""
    
    # 解析命令行参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--clean)
                action="clean"
                shift
                ;;
            -b|--build)
                action="build"
                shift
                ;;
            -t|--test)
                action="test"
                shift
                ;;
            -p|--package)
                action="package"
                shift
                ;;
            -i|--install)
                action="install"
                shift
                ;;
            -a|--all)
                action="all"
                shift
                ;;
            *)
                log_error "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # 如果没有指定动作，显示帮助
    if [ -z "$action" ]; then
        show_help
        exit 1
    fi
    
    # 检查系统环境
    check_kylin_system
    check_architecture
    
    # 执行相应动作
    case $action in
        clean)
            clean_build
            ;;
        build)
            check_dependencies || exit 1
            create_build_dir
            configure_cmake || exit 1
            build_project || exit 1
            ;;
        test)
            cd build 2>/dev/null || { log_error "构建目录不存在，请先编译项目"; exit 1; }
            run_tests || exit 1
            ;;
        package)
            cd build 2>/dev/null || { log_error "构建目录不存在，请先编译项目"; exit 1; }
            create_package || exit 1
            ;;
        install)
            cd build 2>/dev/null || { log_error "构建目录不存在，请先编译项目"; exit 1; }
            install_application || exit 1
            ;;
        all)
            check_dependencies || exit 1
            clean_build
            create_build_dir
            configure_cmake || exit 1
            build_project || exit 1
            run_tests || exit 1
            create_package || exit 1
            ;;
    esac
    
    log_success "构建脚本执行完成"
}

# 执行主函数
main "$@"
