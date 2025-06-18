#!/bin/bash

# 启动问题诊断脚本

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
echo "  银河麒麟智能问答助手 - 启动诊断工具"
echo "========================================"
echo

# 1. 检查安装状态
log_info "检查应用程序安装状态..."

if [ -f "/opt/kylin-qa-assistant/bin/KylinQAAssistant" ]; then
    log_success "可执行文件存在: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    
    # 检查权限
    PERMS=$(ls -la /opt/kylin-qa-assistant/bin/KylinQAAssistant | cut -d' ' -f1)
    log_info "文件权限: $PERMS"
    
    if [[ $PERMS == *"x"* ]]; then
        log_success "可执行权限正常"
    else
        log_error "缺少可执行权限"
        echo "修复命令: sudo chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    fi
else
    log_error "可执行文件不存在"
    echo "请重新安装应用程序"
    exit 1
fi

# 2. 检查桌面文件
log_info "检查桌面快捷方式..."

if [ -f "/usr/share/applications/kylin-qa-assistant.desktop" ]; then
    log_success "桌面文件存在"
    
    # 检查桌面文件内容
    log_info "桌面文件内容:"
    cat /usr/share/applications/kylin-qa-assistant.desktop
    echo
    
    # 验证桌面文件
    if command -v desktop-file-validate &> /dev/null; then
        if desktop-file-validate /usr/share/applications/kylin-qa-assistant.desktop 2>/dev/null; then
            log_success "桌面文件格式正确"
        else
            log_warning "桌面文件格式可能有问题"
            desktop-file-validate /usr/share/applications/kylin-qa-assistant.desktop
        fi
    fi
else
    log_error "桌面文件不存在"
    echo "修复命令: sudo ./scripts/fix_desktop_entry.sh"
fi

# 3. 检查动态库依赖
log_info "检查动态库依赖..."

MISSING_LIBS=$(ldd /opt/kylin-qa-assistant/bin/KylinQAAssistant 2>/dev/null | grep "not found" || true)

if [ -z "$MISSING_LIBS" ]; then
    log_success "所有动态库依赖都已满足"
else
    log_error "缺少以下动态库:"
    echo "$MISSING_LIBS"
    echo
    echo "请安装缺失的依赖包"
fi

# 4. 检查Qt环境
log_info "检查Qt环境..."

if pkg-config --exists Qt5Core 2>/dev/null; then
    QT5_VERSION=$(pkg-config --modversion Qt5Core)
    log_success "Qt5 Core: $QT5_VERSION"
elif pkg-config --exists Qt6Core 2>/dev/null; then
    QT6_VERSION=$(pkg-config --modversion Qt6Core)
    log_success "Qt6 Core: $QT6_VERSION"
else
    log_error "未找到Qt开发环境"
fi

# 检查Qt插件路径
QT_PLUGIN_PATHS=(
    "/usr/lib/x86_64-linux-gnu/qt5/plugins"
    "/usr/lib/qt5/plugins"
    "/usr/lib64/qt5/plugins"
    "/usr/lib/x86_64-linux-gnu/qt6/plugins"
    "/usr/lib/qt6/plugins"
)

log_info "检查Qt插件路径..."
for path in "${QT_PLUGIN_PATHS[@]}"; do
    if [ -d "$path" ]; then
        log_success "找到Qt插件路径: $path"
        break
    fi
done

# 5. 尝试启动应用程序
log_info "尝试启动应用程序..."

echo "正在测试应用程序启动（5秒超时）..."

# 设置环境变量
export QT_QPA_PLATFORM=xcb
export QT_DEBUG_PLUGINS=0

# 尝试启动并捕获输出
timeout 5 /opt/kylin-qa-assistant/bin/KylinQAAssistant --help 2>&1 | head -20

STARTUP_RESULT=$?

if [ $STARTUP_RESULT -eq 0 ]; then
    log_success "应用程序可以正常启动"
elif [ $STARTUP_RESULT -eq 124 ]; then
    log_info "应用程序启动超时（这可能是正常的GUI行为）"
else
    log_error "应用程序启动失败，退出码: $STARTUP_RESULT"
fi

# 6. 检查配置文件
log_info "检查配置文件..."

CONFIG_PATHS=(
    "/opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json"
    "/opt/kylin-qa-assistant/config/app_config.json"
    "./config/app_config.json"
)

CONFIG_FOUND=false
for config_path in "${CONFIG_PATHS[@]}"; do
    if [ -f "$config_path" ]; then
        log_success "找到配置文件: $config_path"
        CONFIG_FOUND=true
        
        # 验证JSON格式
        if command -v python3 &> /dev/null; then
            if python3 -m json.tool "$config_path" >/dev/null 2>&1; then
                log_success "配置文件JSON格式正确"
            else
                log_error "配置文件JSON格式错误"
            fi
        fi
        break
    fi
done

if [ "$CONFIG_FOUND" = false ]; then
    log_warning "未找到配置文件"
fi

# 7. 检查日志目录
log_info "检查日志目录..."

LOG_DIRS=(
    "/opt/kylin-qa-assistant/logs"
    "./logs"
)

for log_dir in "${LOG_DIRS[@]}"; do
    if [ -d "$log_dir" ]; then
        log_success "找到日志目录: $log_dir"
        
        # 检查权限
        if [ -w "$log_dir" ]; then
            log_success "日志目录可写"
        else
            log_warning "日志目录不可写"
            echo "修复命令: sudo chmod 755 $log_dir"
        fi
        break
    fi
done

# 8. 检查显示环境
log_info "检查显示环境..."

if [ -n "$DISPLAY" ]; then
    log_success "DISPLAY环境变量已设置: $DISPLAY"
else
    log_warning "DISPLAY环境变量未设置"
    echo "如果是远程连接，请使用: export DISPLAY=:0"
fi

if [ -n "$WAYLAND_DISPLAY" ]; then
    log_info "检测到Wayland显示服务器: $WAYLAND_DISPLAY"
fi

# 9. 生成修复建议
echo
echo "========================================"
echo "           修复建议"
echo "========================================"
echo

echo "基于诊断结果，建议按以下顺序尝试修复："
echo

echo "1. 修复权限和桌面快捷方式："
echo "   sudo ./scripts/fix_desktop_entry.sh"
echo

echo "2. 如果有缺失的动态库，安装依赖："
echo "   sudo ./scripts/install_dependencies_kylin.sh"
echo

echo "3. 尝试从命令行启动："
echo "   /opt/kylin-qa-assistant/bin/KylinQAAssistant"
echo

echo "4. 如果仍然无法启动，查看详细错误信息："
echo "   /opt/kylin-qa-assistant/bin/KylinQAAssistant 2>&1 | tee startup_error.log"
echo

echo "5. 检查系统日志："
echo "   journalctl -f | grep -i kylin"
echo

echo "6. 如果是权限问题，尝试以当前用户运行："
echo "   cd /path/to/project/build/src && ./KylinQAAssistant"
echo

echo "========================================"
echo "           诊断完成"
echo "========================================"
