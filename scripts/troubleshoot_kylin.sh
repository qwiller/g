#!/bin/bash

# 银河麒麟系统故障排除脚本

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

# 系统信息收集
collect_system_info() {
    echo "========================================"
    echo "           系统信息收集"
    echo "========================================"
    
    log_info "操作系统信息:"
    if [ -f /etc/os-release ]; then
        cat /etc/os-release
    fi
    echo
    
    if [ -f /etc/kylin-release ]; then
        log_info "麒麟版本信息:"
        cat /etc/kylin-release
        echo
    fi
    
    log_info "内核版本:"
    uname -a
    echo
    
    log_info "CPU架构:"
    lscpu | grep "Architecture\|CPU op-mode\|Model name"
    echo
    
    log_info "内存信息:"
    free -h
    echo
    
    log_info "磁盘空间:"
    df -h
    echo
}

# 检查编译工具
check_build_tools() {
    echo "========================================"
    echo "           编译工具检查"
    echo "========================================"
    
    local tools=("gcc" "g++" "cmake" "make" "git" "pkg-config")
    
    for tool in "${tools[@]}"; do
        if command -v "$tool" &> /dev/null; then
            local version=$($tool --version 2>/dev/null | head -n1)
            log_success "$tool: $version"
        else
            log_error "$tool: 未安装"
        fi
    done
    echo
}

# 检查Qt环境
check_qt_environment() {
    echo "========================================"
    echo "            Qt环境检查"
    echo "========================================"
    
    # 检查Qt6
    if pkg-config --exists Qt6Core 2>/dev/null; then
        local qt6_version=$(pkg-config --modversion Qt6Core)
        log_success "Qt6 Core: $qt6_version"
        
        local qt6_modules=("Qt6Widgets" "Qt6Qml" "Qt6Quick" "Qt6TextToSpeech")
        for module in "${qt6_modules[@]}"; do
            if pkg-config --exists "$module" 2>/dev/null; then
                local version=$(pkg-config --modversion "$module")
                log_success "$module: $version"
            else
                log_warning "$module: 未找到"
            fi
        done
    else
        log_warning "Qt6: 未找到"
    fi
    
    # 检查Qt5
    if pkg-config --exists Qt5Core 2>/dev/null; then
        local qt5_version=$(pkg-config --modversion Qt5Core)
        log_success "Qt5 Core: $qt5_version"
        
        local qt5_modules=("Qt5Widgets" "Qt5Qml" "Qt5Quick" "Qt5TextToSpeech")
        for module in "${qt5_modules[@]}"; do
            if pkg-config --exists "$module" 2>/dev/null; then
                local version=$(pkg-config --modversion "$module")
                log_success "$module: $version"
            else
                log_warning "$module: 未找到"
            fi
        done
    else
        log_warning "Qt5: 未找到"
    fi
    
    # 检查qmake
    if command -v qmake &> /dev/null; then
        local qmake_version=$(qmake --version)
        log_info "qmake版本: $qmake_version"
    fi
    
    echo
}

# 检查依赖库
check_dependencies() {
    echo "========================================"
    echo "           依赖库检查"
    echo "========================================"
    
    local libs=("poppler-cpp" "libcurl" "openssl")
    
    for lib in "${libs[@]}"; do
        if pkg-config --exists "$lib" 2>/dev/null; then
            local version=$(pkg-config --modversion "$lib")
            log_success "$lib: $version"
        else
            log_warning "$lib: 未找到"
        fi
    done
    
    # 检查其他库文件
    local lib_files=("/usr/lib/libjsoncpp.so" "/usr/lib/x86_64-linux-gnu/libjsoncpp.so" 
                     "/usr/lib/libspdlog.so" "/usr/lib/x86_64-linux-gnu/libspdlog.so")
    
    for lib_file in "${lib_files[@]}"; do
        if [ -f "$lib_file" ]; then
            log_success "找到库文件: $lib_file"
        fi
    done
    
    echo
}

# 检查包管理器
check_package_manager() {
    echo "========================================"
    echo "          包管理器检查"
    echo "========================================"
    
    if command -v apt &> /dev/null; then
        log_success "APT: 可用"
        log_info "APT版本: $(apt --version)"
        
        # 检查源列表
        log_info "软件源配置:"
        if [ -f /etc/apt/sources.list ]; then
            grep -v "^#" /etc/apt/sources.list | grep -v "^$" | head -5
        fi
        
    elif command -v yum &> /dev/null; then
        log_success "YUM: 可用"
        log_info "YUM版本: $(yum --version | head -1)"
        
    elif command -v dnf &> /dev/null; then
        log_success "DNF: 可用"
        log_info "DNF版本: $(dnf --version)"
        
    else
        log_error "未找到支持的包管理器"
    fi
    
    echo
}

# 检查网络连接
check_network() {
    echo "========================================"
    echo "           网络连接检查"
    echo "========================================"
    
    # 检查基本网络连接
    if ping -c 1 8.8.8.8 &> /dev/null; then
        log_success "网络连接: 正常"
    else
        log_error "网络连接: 异常"
    fi
    
    # 检查DNS解析
    if nslookup github.com &> /dev/null; then
        log_success "DNS解析: 正常"
    else
        log_warning "DNS解析: 可能有问题"
    fi
    
    # 检查HTTPS连接
    if curl -s --connect-timeout 5 https://github.com &> /dev/null; then
        log_success "HTTPS连接: 正常"
    else
        log_warning "HTTPS连接: 可能有问题"
    fi
    
    echo
}

# 检查项目文件
check_project_files() {
    echo "========================================"
    echo "          项目文件检查"
    echo "========================================"
    
    local required_files=("CMakeLists.txt" "src/main.cpp" "config/app_config.json")
    
    for file in "${required_files[@]}"; do
        if [ -f "$file" ]; then
            log_success "找到文件: $file"
        else
            log_error "缺少文件: $file"
        fi
    done
    
    # 检查目录结构
    local required_dirs=("src" "include" "config" "scripts")
    
    for dir in "${required_dirs[@]}"; do
        if [ -d "$dir" ]; then
            log_success "找到目录: $dir"
        else
            log_error "缺少目录: $dir"
        fi
    done
    
    echo
}

# 生成修复建议
generate_suggestions() {
    echo "========================================"
    echo "           修复建议"
    echo "========================================"
    
    log_info "基于检查结果，以下是修复建议:"
    echo
    
    echo "1. 如果Qt包缺失，请运行:"
    echo "   sudo ./scripts/quick_install_kylin.sh"
    echo
    
    echo "2. 如果编译工具缺失，请安装:"
    echo "   sudo apt install build-essential cmake"
    echo "   或"
    echo "   sudo yum groupinstall 'Development Tools'"
    echo
    
    echo "3. 如果网络有问题，请检查:"
    echo "   - 防火墙设置"
    echo "   - 代理配置"
    echo "   - DNS设置"
    echo
    
    echo "4. 如果依赖库缺失，请运行完整安装脚本:"
    echo "   sudo ./scripts/install_dependencies_kylin.sh"
    echo
    
    echo "5. 如果仍有问题，请查看详细日志:"
    echo "   - 编译日志: build/CMakeFiles/CMakeError.log"
    echo "   - 运行日志: logs/kylin_qa.log"
    echo
}

# 主函数
main() {
    echo "========================================"
    echo "  银河麒麟智能问答助手 - 故障排除工具"
    echo "========================================"
    echo
    
    collect_system_info
    check_build_tools
    check_qt_environment
    check_dependencies
    check_package_manager
    check_network
    check_project_files
    generate_suggestions
    
    echo "========================================"
    echo "           检查完成"
    echo "========================================"
    echo
    echo "如果问题仍未解决，请将此输出保存并联系技术支持。"
    echo
}

# 执行主函数
main "$@"
