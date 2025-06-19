# VS Code Remote-SSH 麒麟系统开发配置指南

## 1. 安装必要的VS Code扩展

在VS Code中安装以下扩展：

```
1. Remote - SSH (ms-vscode-remote.remote-ssh)
2. Remote - SSH: Editing Configuration Files (ms-vscode-remote.remote-ssh-edit)
3. C/C++ (ms-vscode.cpptools)
4. C/C++ Extension Pack (ms-vscode.cpptools-extension-pack)
5. CMake Tools (ms-vscode.cmake-tools)
6. Remote Explorer (ms-vscode.remote-explorer)
```

安装方法：
- 打开VS Code
- 按 `Ctrl+Shift+X` 打开扩展面板
- 搜索并安装上述扩展

## 2. SSH配置文件设置

### 2.1 创建SSH配置文件

位置：`%USERPROFILE%\.ssh\config`

```ssh-config
# 麒麟开发服务器配置
Host kylin-dev
    HostName 192.168.100.131
    User a
    Port 22
    # 密码认证（临时使用）
    PreferredAuthentications password
    # 连接优化
    Compression yes
    ServerAliveInterval 60
    ServerAliveCountMax 3
    # 控制连接复用
    ControlMaster auto
    ControlPath ~/.ssh/master-%r@%h:%p
    ControlPersist 10m
    # 跳过主机密钥检查（开发环境）
    StrictHostKeyChecking no
    UserKnownHostsFile /dev/null
    # 端口转发配置
    LocalForward 5555 localhost:5555
    LocalForward 8080 localhost:8080
```

### 2.2 创建SSH密钥（推荐）

```powershell
# 在PowerShell中执行
ssh-keygen -t rsa -b 4096 -C "your-email@example.com" -f ~/.ssh/kylin_dev_key

# 复制公钥到远程服务器
type ~/.ssh/kylin_dev_key.pub | ssh a@192.168.100.131 "mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys"
```

更新SSH配置使用密钥：
```ssh-config
Host kylin-dev
    HostName 192.168.100.131
    User a
    Port 22
    IdentityFile ~/.ssh/kylin_dev_key
    PreferredAuthentications publickey,password
    # ... 其他配置保持不变
```

## 3. VS Code Remote-SSH连接

### 3.1 连接到远程服务器

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 输入 "Remote-SSH: Connect to Host"
3. 选择 "kylin-dev" 或输入 "a@192.168.100.131"
4. 输入密码 "1234qwer"
5. 等待VS Code连接并安装远程服务器组件

### 3.2 验证连接

连接成功后，VS Code左下角会显示 "SSH: kylin-dev"

## 4. 远程开发环境配置

### 4.1 在远程服务器安装扩展

连接到远程服务器后，需要在远程安装以下扩展：
- C/C++
- CMake Tools
- GitLens (可选)

VS Code会自动提示安装，或手动在扩展面板中安装。

### 4.2 配置远程工作区

1. 在远程服务器创建项目目录：
```bash
mkdir -p ~/kylin-qa-assistant
cd ~/kylin-qa-assistant
```

2. 在VS Code中打开远程文件夹：
   - `Ctrl+Shift+P` → "File: Open Folder"
   - 选择 `/home/a/kylin-qa-assistant`

## 5. 调试配置

### 5.1 配置文件结构

远程项目目录结构：
```
/home/a/kylin-qa-assistant/
├── .vscode/
│   ├── launch.json
│   ├── tasks.json
│   ├── settings.json
│   └── c_cpp_properties.json
├── src/
├── include/
├── CMakeLists.txt
└── ...
```

## 6. 工作流程

### 6.1 开发流程

1. **连接远程服务器**：
   - `Ctrl+Shift+P` → "Remote-SSH: Connect to Host" → "kylin-dev"

2. **打开项目**：
   - `Ctrl+K Ctrl+O` → 选择远程项目目录

3. **编辑代码**：
   - 直接在VS Code中编辑远程文件

4. **编译项目**：
   - `Ctrl+Shift+P` → "Tasks: Run Task" → "Build Project"

5. **调试程序**：
   - 设置断点
   - 按 `F5` 开始调试

### 6.2 文件同步

VS Code Remote-SSH会自动同步文件，无需手动操作。

## 7. 故障排除

### 7.1 常见问题

1. **连接超时**：
   - 检查网络连接
   - 确认SSH服务运行：`sudo systemctl status ssh`

2. **权限问题**：
   - 确保用户有项目目录权限
   - 检查SSH密钥权限：`chmod 600 ~/.ssh/kylin_dev_key`

3. **扩展安装失败**：
   - 检查远程服务器网络连接
   - 手动下载扩展包

4. **调试连接失败**：
   - 确认GDB服务器运行
   - 检查端口转发配置

### 7.2 性能优化

1. **启用连接复用**：
   - SSH配置中的 ControlMaster 设置

2. **压缩传输**：
   - SSH配置中的 Compression yes

3. **保持连接**：
   - ServerAliveInterval 设置

## 8. 安全建议

1. **使用SSH密钥**：
   - 替代密码认证
   - 定期更换密钥

2. **限制网络访问**：
   - 配置防火墙规则
   - 使用VPN连接

3. **定期更新**：
   - 保持VS Code和扩展最新
   - 更新远程系统安全补丁
