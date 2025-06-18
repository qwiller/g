#!/bin/bash

# 修复桌面快捷方式脚本

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

# 修复可执行文件权限
fix_executable_permissions() {
    log_info "修复可执行文件权限..."
    
    if [ -f "/opt/kylin-qa-assistant/bin/KylinQAAssistant" ]; then
        chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant
        chown root:root /opt/kylin-qa-assistant/bin/KylinQAAssistant
        log_success "可执行文件权限已修复"
    else
        log_error "可执行文件不存在: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
        return 1
    fi
}

# 创建正确的桌面文件
create_desktop_file() {
    log_info "创建桌面快捷方式..."
    
    cat > /usr/share/applications/kylin-qa-assistant.desktop << 'EOF'
[Desktop Entry]
Name=银河麒麟智能问答助手
Name[en]=Kylin QA Assistant
Comment=基于RAG技术的智能问答系统
Comment[en]=Intelligent Q&A system based on RAG technology
Exec=/opt/kylin-qa-assistant/bin/KylinQAAssistant
Icon=/opt/kylin-qa-assistant/share/kylin-qa-assistant/resources/app_icon.png
Terminal=false
Type=Application
Categories=Office;Utility;Education;
StartupNotify=true
MimeType=application/pdf;text/markdown;text/plain;
Keywords=AI;问答;助手;麒麟;RAG;
EOF

    # 设置桌面文件权限
    chmod 644 /usr/share/applications/kylin-qa-assistant.desktop
    
    log_success "桌面快捷方式已创建"
}

# 创建启动脚本（解决环境变量问题）
create_launcher_script() {
    log_info "创建启动脚本..."
    
    cat > /opt/kylin-qa-assistant/bin/kylin-qa-launcher.sh << 'EOF'
#!/bin/bash

# 银河麒麟智能问答助手启动脚本

# 设置环境变量
export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/lib/x86_64-linux-gnu/qt5/plugins
export LD_LIBRARY_PATH=/opt/kylin-qa-assistant/lib:$LD_LIBRARY_PATH

# 切换到应用程序目录
cd /opt/kylin-qa-assistant

# 创建必要的目录
mkdir -p logs
mkdir -p data
mkdir -p temp

# 检查配置文件
if [ ! -f "etc/kylin-qa-assistant/app_config.json" ]; then
    echo "配置文件不存在，正在创建默认配置..."
    mkdir -p etc/kylin-qa-assistant
    cp /opt/kylin-qa-assistant/share/kylin-qa-assistant/config/app_config.json etc/kylin-qa-assistant/ 2>/dev/null || true
fi

# 启动应用程序
exec /opt/kylin-qa-assistant/bin/KylinQAAssistant "$@"
EOF

    chmod +x /opt/kylin-qa-assistant/bin/kylin-qa-launcher.sh
    
    log_success "启动脚本已创建"
}

# 更新桌面文件使用启动脚本
update_desktop_file() {
    log_info "更新桌面文件..."
    
    sed -i 's|Exec=/opt/kylin-qa-assistant/bin/KylinQAAssistant|Exec=/opt/kylin-qa-assistant/bin/kylin-qa-launcher.sh|' \
        /usr/share/applications/kylin-qa-assistant.desktop
    
    log_success "桌面文件已更新"
}

# 刷新桌面数据库
refresh_desktop_database() {
    log_info "刷新桌面数据库..."
    
    if command -v update-desktop-database &> /dev/null; then
        update-desktop-database /usr/share/applications
        log_success "桌面数据库已刷新"
    else
        log_warning "update-desktop-database命令不可用"
    fi
}

# 测试应用程序启动
test_application() {
    log_info "测试应用程序启动..."
    
    # 测试直接启动
    if timeout 5 /opt/kylin-qa-assistant/bin/KylinQAAssistant --version 2>/dev/null; then
        log_success "应用程序可以正常启动"
    else
        log_warning "应用程序启动测试失败，但这可能是正常的（GUI应用程序）"
    fi
    
    # 检查依赖
    log_info "检查动态库依赖..."
    missing_libs=$(ldd /opt/kylin-qa-assistant/bin/KylinQAAssistant | grep "not found" || true)
    
    if [ -z "$missing_libs" ]; then
        log_success "所有动态库依赖都已满足"
    else
        log_error "缺少以下动态库:"
        echo "$missing_libs"
        return 1
    fi
}

# 创建用户桌面快捷方式
create_user_desktop_shortcut() {
    log_info "为当前用户创建桌面快捷方式..."
    
    # 获取真实用户（即使使用sudo）
    REAL_USER=${SUDO_USER:-$USER}
    USER_HOME=$(eval echo ~$REAL_USER)
    
    if [ "$REAL_USER" != "root" ] && [ -d "$USER_HOME" ]; then
        USER_DESKTOP="$USER_HOME/Desktop"
        
        if [ -d "$USER_DESKTOP" ]; then
            cp /usr/share/applications/kylin-qa-assistant.desktop "$USER_DESKTOP/"
            chown $REAL_USER:$REAL_USER "$USER_DESKTOP/kylin-qa-assistant.desktop"
            chmod +x "$USER_DESKTOP/kylin-qa-assistant.desktop"
            log_success "用户桌面快捷方式已创建"
        else
            log_warning "用户桌面目录不存在: $USER_DESKTOP"
        fi
    fi
}

# 显示使用说明
show_usage_info() {
    echo
    echo "========================================"
    echo "           修复完成"
    echo "========================================"
    echo
    echo "应用程序启动方式："
    echo "1. 桌面快捷方式: 双击桌面上的'银河麒麟智能问答助手'图标"
    echo "2. 应用程序菜单: 在应用程序菜单中找到'银河麒麟智能问答助手'"
    echo "3. 命令行启动: /opt/kylin-qa-assistant/bin/kylin-qa-launcher.sh"
    echo "4. 直接启动: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    echo
    echo "如果仍然无法启动，请查看日志："
    echo "- 应用程序日志: /opt/kylin-qa-assistant/logs/kylin_qa.log"
    echo "- 系统日志: journalctl -f"
    echo
    echo "故障排除："
    echo "- 运行诊断脚本: ./scripts/troubleshoot_kylin.sh"
    echo "- 检查依赖: ldd /opt/kylin-qa-assistant/bin/KylinQAAssistant"
    echo
}

# 主函数
main() {
    echo "========================================"
    echo "  银河麒麟智能问答助手 - 桌面修复工具"
    echo "========================================"
    echo
    
    check_root
    
    fix_executable_permissions || exit 1
    create_desktop_file
    create_launcher_script
    update_desktop_file
    refresh_desktop_database
    create_user_desktop_shortcut
    test_application
    
    show_usage_info
}

# 执行主函数
main "$@"
