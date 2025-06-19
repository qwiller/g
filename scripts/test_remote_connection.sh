#!/bin/bash

# 远程SSH连接测试脚本
# 用于测试连接到麒麟系统服务器

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

# 远程服务器配置
REMOTE_HOST="192.168.100.131"
REMOTE_USER="a"
REMOTE_PASSWORD="1234qwer"
REMOTE_PORT="22"
GDB_PORT="5555"
PROJECT_NAME="kylin-qa-assistant"
REMOTE_PROJECT_PATH="/home/$REMOTE_USER/$PROJECT_NAME"

echo "========================================"
echo "  远程SSH连接测试"
echo "========================================"
echo
echo "目标服务器: $REMOTE_HOST"
echo "用户名: $REMOTE_USER"
echo "端口: $REMOTE_PORT"
echo

# 测试网络连通性
test_network_connectivity() {
    log_info "测试网络连通性..."
    
    if ping -c 3 -W 3 "$REMOTE_HOST" >/dev/null 2>&1; then
        log_success "网络连通性正常"
    else
        log_error "无法ping通远程主机 $REMOTE_HOST"
        log_info "请检查："
        echo "  1. 网络连接是否正常"
        echo "  2. 远程主机是否开机"
        echo "  3. 防火墙设置"
        return 1
    fi
}

# 测试SSH端口
test_ssh_port() {
    log_info "测试SSH端口连通性..."
    
    if timeout 5 bash -c "</dev/tcp/$REMOTE_HOST/$REMOTE_PORT" 2>/dev/null; then
        log_success "SSH端口 $REMOTE_PORT 可访问"
    else
        log_error "SSH端口 $REMOTE_PORT 不可访问"
        log_info "请检查："
        echo "  1. SSH服务是否启动"
        echo "  2. 端口是否正确"
        echo "  3. 防火墙是否阻止连接"
        return 1
    fi
}

# 测试SSH登录
test_ssh_login() {
    log_info "测试SSH登录..."
    
    # 使用sshpass进行自动登录测试
    if command -v sshpass >/dev/null 2>&1; then
        if sshpass -p "$REMOTE_PASSWORD" ssh -o ConnectTimeout=10 -o StrictHostKeyChecking=no \
           "$REMOTE_USER@$REMOTE_HOST" "echo 'SSH连接成功'" 2>/dev/null; then
            log_success "SSH登录成功"
        else
            log_error "SSH登录失败"
            log_info "请检查用户名和密码是否正确"
            return 1
        fi
    else
        log_warning "sshpass未安装，将进行交互式登录测试"
        log_info "请手动输入密码进行测试..."
        
        if ssh -o ConnectTimeout=10 -o StrictHostKeyChecking=no \
           "$REMOTE_USER@$REMOTE_HOST" "echo 'SSH连接成功'"; then
            log_success "SSH登录成功"
        else
            log_error "SSH登录失败"
            return 1
        fi
    fi
}

# 检查远程系统信息
check_remote_system() {
    log_info "检查远程系统信息..."
    
    local ssh_cmd="ssh -o ConnectTimeout=10 -o StrictHostKeyChecking=no $REMOTE_USER@$REMOTE_HOST"
    
    if command -v sshpass >/dev/null 2>&1; then
        ssh_cmd="sshpass -p $REMOTE_PASSWORD $ssh_cmd"
    fi
    
    echo "系统信息："
    $ssh_cmd "uname -a" 2>/dev/null || log_warning "无法获取系统信息"
    
    echo
    echo "操作系统版本："
    $ssh_cmd "cat /etc/os-release 2>/dev/null | head -5" || log_warning "无法获取OS版本"
    
    echo
    echo "可用内存："
    $ssh_cmd "free -h" 2>/dev/null || log_warning "无法获取内存信息"
    
    echo
    echo "磁盘空间："
    $ssh_cmd "df -h /" 2>/dev/null || log_warning "无法获取磁盘信息"
}

# 检查开发环境
check_dev_environment() {
    log_info "检查远程开发环境..."
    
    local ssh_cmd="ssh -o ConnectTimeout=10 -o StrictHostKeyChecking=no $REMOTE_USER@$REMOTE_HOST"
    
    if command -v sshpass >/dev/null 2>&1; then
        ssh_cmd="sshpass -p $REMOTE_PASSWORD $ssh_cmd"
    fi
    
    echo "检查编译工具："
    $ssh_cmd "which gcc g++ cmake make 2>/dev/null" || log_warning "部分编译工具缺失"
    
    echo
    echo "检查Qt5环境："
    $ssh_cmd "pkg-config --exists Qt5Core && echo 'Qt5 Core: 可用' || echo 'Qt5 Core: 不可用'" 2>/dev/null
    $ssh_cmd "pkg-config --exists Qt5Widgets && echo 'Qt5 Widgets: 可用' || echo 'Qt5 Widgets: 不可用'" 2>/dev/null
    
    echo
    echo "检查调试工具："
    $ssh_cmd "which gdb gdbserver 2>/dev/null" || log_warning "调试工具缺失"
}

# 创建SSH配置
create_ssh_config() {
    log_info "创建SSH配置文件..."
    
    local ssh_config_dir="$HOME/.ssh"
    local ssh_config_file="$ssh_config_dir/config"
    
    # 创建.ssh目录
    mkdir -p "$ssh_config_dir"
    chmod 700 "$ssh_config_dir"
    
    # 备份现有配置
    if [ -f "$ssh_config_file" ]; then
        cp "$ssh_config_file" "$ssh_config_file.backup.$(date +%Y%m%d_%H%M%S)"
    fi
    
    # 添加远程主机配置
    cat >> "$ssh_config_file" << EOF

# 麒麟开发服务器配置
Host kylin-dev-server
    HostName $REMOTE_HOST
    User $REMOTE_USER
    Port $REMOTE_PORT
    Compression yes
    ServerAliveInterval 60
    ControlMaster auto
    ControlPath ~/.ssh/master-%r@%h:%p
    ControlPersist 10m
    StrictHostKeyChecking no

EOF

    chmod 600 "$ssh_config_file"
    log_success "SSH配置已添加到 $ssh_config_file"
    log_info "现在可以使用: ssh kylin-dev-server"
}

# 创建部署脚本
create_deployment_scripts() {
    log_info "创建部署脚本..."
    
    # 创建代码同步脚本
    cat > "deploy_to_remote.sh" << EOF
#!/bin/bash

# 部署到远程麒麟服务器脚本

REMOTE_HOST="$REMOTE_HOST"
REMOTE_USER="$REMOTE_USER"
REMOTE_PATH="$REMOTE_PROJECT_PATH"

echo "正在同步代码到远程服务器..."

# 同步代码（排除构建目录和Git文件）
rsync -avz --delete \\
    --exclude 'build*' \\
    --exclude '.git' \\
    --exclude '*.o' \\
    --exclude '*.so' \\
    --progress \\
    ./ \$REMOTE_USER@\$REMOTE_HOST:\$REMOTE_PATH/

echo "代码同步完成！"

# 远程编译
echo "开始远程编译..."
ssh \$REMOTE_USER@\$REMOTE_HOST "
    cd \$REMOTE_PATH
    chmod +x scripts/*.sh
    ./scripts/build_qt5_kylin.sh
"

echo "部署完成！"
EOF

    chmod +x "deploy_to_remote.sh"
    
    # 创建远程调试启动脚本
    cat > "start_remote_debug.sh" << EOF
#!/bin/bash

# 启动远程调试脚本

REMOTE_HOST="$REMOTE_HOST"
REMOTE_USER="$REMOTE_USER"
REMOTE_PATH="$REMOTE_PROJECT_PATH"
GDB_PORT="$GDB_PORT"

echo "启动远程GDB服务器..."

# 启动远程GDB服务器
ssh \$REMOTE_USER@\$REMOTE_HOST "
    cd \$REMOTE_PATH
    pkill gdbserver 2>/dev/null || true
    sleep 1
    gdbserver :\$GDB_PORT ./build/src/KylinQAAssistant
" &

echo "GDB服务器已启动"
echo "调试连接地址: $REMOTE_HOST:$GDB_PORT"
echo "在IDE中配置远程调试连接到此地址"
EOF

    chmod +x "start_remote_debug.sh"
    
    log_success "部署脚本已创建"
}

# 更新VS Code配置
update_vscode_config() {
    log_info "更新VS Code配置..."
    
    # 更新launch.json
    if [ -f ".vscode/launch.json" ]; then
        # 备份现有配置
        cp ".vscode/launch.json" ".vscode/launch.json.backup"
        
        # 添加远程调试配置
        cat > ".vscode/launch_remote.json" << EOF
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Remote Debug (Kylin Server)",
            "type": "cppdbg",
            "request": "launch",
            "program": "$REMOTE_PROJECT_PATH/build/src/KylinQAAssistant",
            "args": [],
            "stopAtEntry": false,
            "cwd": "$REMOTE_PROJECT_PATH",
            "environment": [
                {
                    "name": "QT_QPA_PLATFORM",
                    "value": "offscreen"
                }
            ],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "miDebuggerServerAddress": "$REMOTE_HOST:$GDB_PORT",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "Deploy to Remote Kylin"
        }
    ]
}
EOF
        
        log_success "VS Code远程调试配置已创建: .vscode/launch_remote.json"
    fi
}

# 安装必要工具
install_tools() {
    log_info "检查并安装必要工具..."
    
    # 检查sshpass
    if ! command -v sshpass >/dev/null 2>&1; then
        log_warning "sshpass未安装，尝试安装..."
        
        if command -v apt >/dev/null 2>&1; then
            sudo apt update && sudo apt install -y sshpass
        elif command -v yum >/dev/null 2>&1; then
            sudo yum install -y sshpass
        elif command -v brew >/dev/null 2>&1; then
            brew install sshpass
        else
            log_warning "无法自动安装sshpass，请手动安装"
        fi
    fi
    
    # 检查rsync
    if ! command -v rsync >/dev/null 2>&1; then
        log_warning "rsync未安装，尝试安装..."
        
        if command -v apt >/dev/null 2>&1; then
            sudo apt update && sudo apt install -y rsync
        elif command -v yum >/dev/null 2>&1; then
            sudo yum install -y rsync
        fi
    fi
}

# 显示连接信息
show_connection_info() {
    echo
    echo "========================================"
    echo "           连接测试完成"
    echo "========================================"
    echo
    echo "📋 连接信息:"
    echo "  服务器地址: $REMOTE_HOST"
    echo "  用户名: $REMOTE_USER"
    echo "  SSH端口: $REMOTE_PORT"
    echo "  GDB端口: $GDB_PORT"
    echo "  项目路径: $REMOTE_PROJECT_PATH"
    echo
    echo "🚀 使用方法:"
    echo "  1. 快速连接: ssh kylin-dev-server"
    echo "  2. 部署代码: ./deploy_to_remote.sh"
    echo "  3. 启动调试: ./start_remote_debug.sh"
    echo
    echo "🔧 VS Code调试:"
    echo "  使用配置: Remote Debug (Kylin Server)"
    echo "  配置文件: .vscode/launch_remote.json"
    echo
    echo "📁 项目文件:"
    echo "  部署脚本: deploy_to_remote.sh"
    echo "  调试脚本: start_remote_debug.sh"
    echo "  SSH配置: ~/.ssh/config"
    echo
}

# 主函数
main() {
    install_tools
    
    if test_network_connectivity && test_ssh_port && test_ssh_login; then
        log_success "SSH连接测试通过！"
        
        check_remote_system
        echo
        check_dev_environment
        echo
        
        create_ssh_config
        create_deployment_scripts
        update_vscode_config
        
        show_connection_info
    else
        log_error "SSH连接测试失败！"
        exit 1
    fi
}

# 执行主函数
main "$@"
