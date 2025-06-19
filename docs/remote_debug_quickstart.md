# VS Code Remote-SSH 麒麟系统调试快速开始指南

## 🚀 快速开始（5分钟配置）

### 步骤1：安装VS Code扩展

在VS Code中安装以下扩展：
```
1. Remote - SSH
2. Remote - SSH: Editing Configuration Files  
3. C/C++
4. CMake Tools
```

### 步骤2：配置SSH连接

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 输入 "Remote-SSH: Open SSH Configuration File"
3. 选择用户配置文件 (`%USERPROFILE%\.ssh\config`)
4. 添加以下配置：

```ssh-config
Host kylin-dev
    HostName 192.168.100.131
    User a
    Port 22
    PreferredAuthentications password
    StrictHostKeyChecking no
    LocalForward 5555 localhost:5555
```

### 步骤3：连接到远程服务器

1. 按 `Ctrl+Shift+P`
2. 输入 "Remote-SSH: Connect to Host"
3. 选择 "kylin-dev"
4. 输入密码：`1234qwer`
5. 等待VS Code连接并安装远程组件

### 步骤4：设置远程环境

连接成功后，在远程终端中运行：
```bash
# 下载并运行环境配置脚本
wget -O setup.sh https://raw.githubusercontent.com/your-repo/scripts/setup_remote_kylin_env.sh
chmod +x setup.sh
./setup.sh
```

### 步骤5：打开项目

1. 在VS Code中按 `Ctrl+K Ctrl+O`
2. 选择远程目录：`/home/a/kylin-qa-assistant`
3. VS Code会自动加载项目配置

### 步骤6：开始调试

1. 按 `Ctrl+Shift+P` → "Tasks: Run Task" → "Build Project"
2. 设置断点
3. 按 `F5` 开始调试

## 📋 详细配置步骤

### 1. Windows环境准备

#### 1.1 安装必要工具

```powershell
# 使用Chocolatey安装（推荐）
choco install git vscode openssh

# 或手动下载安装：
# - Git for Windows: https://git-scm.com/download/win
# - VS Code: https://code.visualstudio.com/
```

#### 1.2 配置SSH客户端

```powershell
# 生成SSH密钥（可选，推荐）
ssh-keygen -t rsa -b 4096 -f ~/.ssh/kylin_dev_key

# 复制公钥到远程服务器
type ~/.ssh/kylin_dev_key.pub | ssh a@192.168.100.131 "mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys"
```

### 2. VS Code Remote-SSH配置

#### 2.1 SSH配置文件

位置：`%USERPROFILE%\.ssh\config`

```ssh-config
# 完整配置
Host kylin-dev
    HostName 192.168.100.131
    User a
    Port 22
    
    # 使用密钥认证（推荐）
    IdentityFile ~/.ssh/kylin_dev_key
    PreferredAuthentications publickey,password
    
    # 或使用密码认证
    # PreferredAuthentications password
    
    # 连接优化
    Compression yes
    ServerAliveInterval 60
    ControlMaster auto
    ControlPath ~/.ssh/master-%r@%h:%p
    ControlPersist 10m
    
    # 开发环境设置
    StrictHostKeyChecking no
    UserKnownHostsFile /dev/null
    
    # 端口转发
    LocalForward 5555 localhost:5555
    LocalForward 8080 localhost:8080
```

#### 2.2 连接到远程服务器

1. **方法1：通过命令面板**
   - `Ctrl+Shift+P` → "Remote-SSH: Connect to Host" → "kylin-dev"

2. **方法2：通过Remote Explorer**
   - 点击左侧Remote Explorer图标
   - 在SSH TARGETS下找到kylin-dev
   - 点击连接图标

3. **方法3：通过状态栏**
   - 点击左下角绿色状态栏
   - 选择"Connect to Host" → "kylin-dev"

### 3. 远程环境配置

#### 3.1 自动配置（推荐）

```bash
# 在远程服务器上运行
curl -sSL https://raw.githubusercontent.com/your-repo/scripts/setup_remote_kylin_env.sh | bash
```

#### 3.2 手动配置

```bash
# 更新系统
sudo apt update && sudo apt upgrade -y

# 安装开发工具
sudo apt install -y build-essential cmake git pkg-config

# 安装Qt5
sudo apt install -y qtbase5-dev qtdeclarative5-dev qttools5-dev

# 安装项目依赖
sudo apt install -y libpoppler-cpp-dev libcurl4-openssl-dev libjsoncpp-dev

# 安装调试工具
sudo apt install -y gdb gdbserver

# 创建项目目录
mkdir -p ~/kylin-qa-assistant
cd ~/kylin-qa-assistant
```

### 4. 项目配置

#### 4.1 打开远程项目

1. 连接到远程服务器后
2. `File` → `Open Folder`
3. 选择 `/home/a/kylin-qa-assistant`
4. VS Code会自动检测并加载项目配置

#### 4.2 安装远程扩展

连接后，VS Code会提示安装以下扩展到远程服务器：
- C/C++
- CMake Tools
- GitLens（可选）

### 5. 调试配置

#### 5.1 构建项目

```bash
# 方法1：使用VS Code任务
# Ctrl+Shift+P → "Tasks: Run Task" → "Build Project"

# 方法2：使用终端
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

#### 5.2 启动调试

1. **直接调试**：
   - 设置断点
   - 按 `F5` 或选择 "Remote Debug (Kylin SSH)"

2. **GDB服务器调试**：
   - 运行任务 "Start GDB Server"
   - 选择 "Remote GDB Server Debug" 配置
   - 按 `F5` 开始调试

### 6. 工作流程

#### 6.1 日常开发流程

```
1. 连接远程服务器
   ↓
2. 编辑代码（自动同步）
   ↓
3. 构建项目 (Ctrl+Shift+B)
   ↓
4. 设置断点
   ↓
5. 开始调试 (F5)
   ↓
6. 调试和测试
```

#### 6.2 自动化脚本

使用PowerShell自动化脚本：

```powershell
# 一键部署和调试
.\scripts\deploy_and_debug.ps1 -Action all

# 仅部署
.\scripts\deploy_and_debug.ps1 -Action deploy

# 仅启动调试
.\scripts\deploy_and_debug.ps1 -Action debug
```

## 🔧 故障排除

### 常见问题

#### 1. 连接超时
```bash
# 检查网络连接
ping 192.168.100.131

# 检查SSH服务
ssh -v a@192.168.100.131
```

#### 2. 权限问题
```bash
# 修复SSH密钥权限
chmod 600 ~/.ssh/kylin_dev_key
chmod 700 ~/.ssh

# 修复项目目录权限
sudo chown -R a:a /home/a/kylin-qa-assistant
```

#### 3. 编译错误
```bash
# 检查依赖
pkg-config --exists Qt5Core
pkg-config --list-all | grep -i qt

# 重新安装依赖
sudo apt install --reinstall qtbase5-dev
```

#### 4. 调试连接失败
```bash
# 检查GDB服务器
ps aux | grep gdbserver
netstat -tlnp | grep 5555

# 重启GDB服务器
pkill gdbserver
./start_gdbserver.sh 5555
```

### 性能优化

#### 1. SSH连接优化
- 启用连接复用 (ControlMaster)
- 使用压缩传输 (Compression yes)
- 配置保活机制 (ServerAliveInterval)

#### 2. VS Code优化
```json
{
    "files.watcherExclude": {
        "**/build/**": true
    },
    "search.exclude": {
        "**/build": true
    }
}
```

#### 3. 编译优化
```bash
# 使用ccache加速编译
sudo apt install ccache
export PATH="/usr/lib/ccache:$PATH"

# 并行编译
make -j$(nproc)
```

## 📚 参考资源

- [VS Code Remote-SSH官方文档](https://code.visualstudio.com/docs/remote/ssh)
- [GDB调试指南](https://sourceware.org/gdb/documentation/)
- [Qt5开发文档](https://doc.qt.io/qt-5/)
- [CMake使用指南](https://cmake.org/documentation/)
