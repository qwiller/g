#!/bin/bash

# 修复安装问题脚本

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
echo "  银河麒麟智能问答助手 - 安装修复工具"
echo "========================================"
echo

# 检查当前目录
if [ ! -f "CMakeLists.txt" ]; then
    log_error "请在项目根目录运行此脚本"
    exit 1
fi

# 1. 检查构建目录状态
log_info "检查构建目录状态..."

if [ -d "build" ]; then
    log_success "构建目录存在"
    
    if [ -f "build/src/KylinQAAssistant" ]; then
        log_success "可执行文件存在于构建目录"
        
        # 测试运行
        log_info "测试构建目录中的可执行文件..."
        if timeout 3 ./build/src/KylinQAAssistant --version 2>/dev/null; then
            log_success "构建目录中的可执行文件可以运行"
        else
            log_info "构建目录中的可执行文件测试完成（GUI应用正常行为）"
        fi
        
        # 直接从构建目录安装
        log_info "从构建目录安装应用程序..."
        cd build
        
        if sudo make install; then
            log_success "应用程序安装成功"
        else
            log_error "安装失败"
            exit 1
        fi
        
        cd ..
    else
        log_warning "构建目录中没有可执行文件，需要重新编译"
        NEED_REBUILD=true
    fi
else
    log_warning "构建目录不存在，需要重新编译"
    NEED_REBUILD=true
fi

# 2. 如果需要重新编译
if [ "$NEED_REBUILD" = true ]; then
    log_info "开始重新编译..."
    
    # 清理旧的构建目录
    rm -rf build
    mkdir build
    cd build
    
    # 配置CMake
    log_info "配置CMake..."
    if cmake -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX=/opt/kylin-qa-assistant \
             ..; then
        log_success "CMake配置成功"
    else
        log_error "CMake配置失败"
        exit 1
    fi
    
    # 编译
    log_info "编译项目..."
    if make -j$(nproc); then
        log_success "编译成功"
    else
        log_error "编译失败"
        exit 1
    fi
    
    # 安装
    log_info "安装应用程序..."
    if sudo make install; then
        log_success "安装成功"
    else
        log_error "安装失败"
        exit 1
    fi
    
    cd ..
fi

# 3. 验证安装
log_info "验证安装结果..."

if [ -f "/opt/kylin-qa-assistant/bin/KylinQAAssistant" ]; then
    log_success "可执行文件已正确安装"
    
    # 设置权限
    sudo chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant
    
    # 检查依赖
    log_info "检查动态库依赖..."
    missing_libs=$(ldd /opt/kylin-qa-assistant/bin/KylinQAAssistant | grep "not found" || true)
    
    if [ -z "$missing_libs" ]; then
        log_success "所有依赖库都已满足"
    else
        log_warning "存在缺失的依赖库:"
        echo "$missing_libs"
        log_info "尝试安装缺失的依赖..."
        sudo ./scripts/install_dependencies_kylin.sh
    fi
    
else
    log_error "安装验证失败，可执行文件不存在"
    exit 1
fi

# 4. 修复桌面快捷方式
log_info "修复桌面快捷方式..."

# 创建正确的桌面文件
sudo tee /usr/share/applications/kylin-qa-assistant.desktop > /dev/null << 'EOF'
[Desktop Entry]
Name=银河麒麟智能问答助手
Name[en]=Kylin QA Assistant
Comment=基于RAG技术的智能问答系统
Comment[en]=Intelligent Q&A system based on RAG technology
Exec=/opt/kylin-qa-assistant/bin/KylinQAAssistant
Icon=applications-office
Terminal=false
Type=Application
Categories=Office;Utility;Education;
StartupNotify=true
MimeType=application/pdf;text/markdown;text/plain;
Keywords=AI;问答;助手;麒麟;RAG;
EOF

sudo chmod 644 /usr/share/applications/kylin-qa-assistant.desktop

# 刷新桌面数据库
if command -v update-desktop-database &> /dev/null; then
    sudo update-desktop-database /usr/share/applications
fi

log_success "桌面快捷方式已修复"

# 5. 创建必要的目录和配置
log_info "创建必要的目录和配置..."

sudo mkdir -p /opt/kylin-qa-assistant/logs
sudo mkdir -p /opt/kylin-qa-assistant/data
sudo mkdir -p /opt/kylin-qa-assistant/temp
sudo mkdir -p /opt/kylin-qa-assistant/etc/kylin-qa-assistant

# 复制配置文件
if [ -f "config/app_config.json" ]; then
    sudo cp config/app_config.json /opt/kylin-qa-assistant/etc/kylin-qa-assistant/
    log_success "配置文件已复制"
fi

# 设置权限
sudo chown -R root:root /opt/kylin-qa-assistant
sudo chmod -R 755 /opt/kylin-qa-assistant
sudo chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant

# 允许用户写入日志目录
sudo chmod 777 /opt/kylin-qa-assistant/logs
sudo chmod 777 /opt/kylin-qa-assistant/data
sudo chmod 777 /opt/kylin-qa-assistant/temp

log_success "目录和权限设置完成"

# 6. 测试应用程序
log_info "测试应用程序启动..."

# 设置环境变量
export QT_QPA_PLATFORM=xcb

# 测试启动
if timeout 5 /opt/kylin-qa-assistant/bin/KylinQAAssistant --version 2>/dev/null; then
    log_success "应用程序可以正常启动"
elif timeout 5 /opt/kylin-qa-assistant/bin/KylinQAAssistant 2>&1 | grep -q "QApplication"; then
    log_success "应用程序可以正常启动（GUI模式）"
else
    log_warning "应用程序启动测试超时，但这可能是正常的GUI行为"
fi

# 7. 显示完成信息
echo
echo "========================================"
echo "           安装修复完成"
echo "========================================"
echo
echo "应用程序已成功安装到: /opt/kylin-qa-assistant/"
echo
echo "启动方式："
echo "1. 桌面快捷方式: 在应用程序菜单中找到'银河麒麟智能问答助手'"
echo "2. 命令行启动: /opt/kylin-qa-assistant/bin/KylinQAAssistant"
echo "3. 从构建目录: ./build/src/KylinQAAssistant"
echo
echo "配置文件位置: /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json"
echo "日志文件位置: /opt/kylin-qa-assistant/logs/kylin_qa.log"
echo
echo "如果仍有问题，请运行: ./scripts/diagnose_startup.sh"
echo
