# Windows PowerShell 自动化部署和调试脚本
# 用于VS Code Remote-SSH开发工作流

param(
    [string]$Action = "deploy",  # deploy, debug, clean, setup
    [string]$RemoteHost = "192.168.100.131",
    [string]$RemoteUser = "a",
    [string]$RemotePassword = "1234qwer",
    [string]$ProjectPath = "/home/a/kylin-qa-assistant",
    [int]$GdbPort = 5555
)

# 颜色输出函数
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    
    $colors = @{
        "Red" = [ConsoleColor]::Red
        "Green" = [ConsoleColor]::Green
        "Yellow" = [ConsoleColor]::Yellow
        "Blue" = [ConsoleColor]::Blue
        "Cyan" = [ConsoleColor]::Cyan
        "White" = [ConsoleColor]::White
    }
    
    Write-Host $Message -ForegroundColor $colors[$Color]
}

function Write-Info { param([string]$Message) Write-ColorOutput "[INFO] $Message" "Blue" }
function Write-Success { param([string]$Message) Write-ColorOutput "[SUCCESS] $Message" "Green" }
function Write-Warning { param([string]$Message) Write-ColorOutput "[WARNING] $Message" "Yellow" }
function Write-Error { param([string]$Message) Write-ColorOutput "[ERROR] $Message" "Red" }

Write-ColorOutput "========================================" "Cyan"
Write-ColorOutput "  VS Code Remote-SSH 自动化工具" "Cyan"
Write-ColorOutput "========================================" "Cyan"
Write-Host ""

# 检查必要工具
function Test-Prerequisites {
    Write-Info "检查必要工具..."
    
    $tools = @("ssh", "scp", "rsync")
    $missing = @()
    
    foreach ($tool in $tools) {
        if (!(Get-Command $tool -ErrorAction SilentlyContinue)) {
            $missing += $tool
        }
    }
    
    if ($missing.Count -gt 0) {
        Write-Error "缺少必要工具: $($missing -join ', ')"
        Write-Info "请安装Git for Windows或WSL以获得这些工具"
        return $false
    }
    
    Write-Success "所有必要工具已安装"
    return $true
}

# 测试远程连接
function Test-RemoteConnection {
    Write-Info "测试远程连接..."
    
    # 测试网络连通性
    if (!(Test-NetConnection -ComputerName $RemoteHost -Port 22 -InformationLevel Quiet)) {
        Write-Error "无法连接到远程主机 $RemoteHost:22"
        return $false
    }
    
    # 测试SSH登录
    $testCommand = "echo 'Connection test successful'"
    $result = ssh -o ConnectTimeout=10 -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" $testCommand 2>$null
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "SSH连接测试成功"
        return $true
    } else {
        Write-Error "SSH连接测试失败"
        return $false
    }
}

# 设置远程环境
function Setup-RemoteEnvironment {
    Write-Info "设置远程环境..."
    
    # 上传环境配置脚本
    $setupScript = "scripts/setup_remote_kylin_env.sh"
    if (Test-Path $setupScript) {
        Write-Info "上传环境配置脚本..."
        scp -o StrictHostKeyChecking=no $setupScript "$RemoteUser@$RemoteHost:/tmp/"
        
        # 执行环境配置
        Write-Info "执行远程环境配置..."
        ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "chmod +x /tmp/setup_remote_kylin_env.sh && /tmp/setup_remote_kylin_env.sh"
        
        if ($LASTEXITCODE -eq 0) {
            Write-Success "远程环境配置完成"
        } else {
            Write-Error "远程环境配置失败"
            return $false
        }
    } else {
        Write-Warning "环境配置脚本不存在: $setupScript"
    }
    
    return $true
}

# 部署代码
function Deploy-Code {
    Write-Info "部署代码到远程服务器..."
    
    # 创建远程项目目录
    ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "mkdir -p $ProjectPath"
    
    # 同步代码
    Write-Info "同步项目文件..."
    
    $excludePatterns = @(
        "--exclude=build*",
        "--exclude=.git",
        "--exclude=*.o",
        "--exclude=*.so",
        "--exclude=*.exe",
        "--exclude=.vscode/settings.json"
    )
    
    $rsyncArgs = @(
        "-avz",
        "--delete",
        "--progress"
    ) + $excludePatterns + @(
        "./",
        "$RemoteUser@$RemoteHost`:$ProjectPath/"
    )
    
    & rsync $rsyncArgs
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "代码同步完成"
    } else {
        Write-Error "代码同步失败"
        return $false
    }
    
    # 远程编译
    Write-Info "开始远程编译..."
    ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "cd $ProjectPath && chmod +x *.sh && ./build.sh"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "远程编译完成"
        return $true
    } else {
        Write-Error "远程编译失败"
        return $false
    }
}

# 启动调试会话
function Start-DebugSession {
    Write-Info "启动远程调试会话..."
    
    # 检查可执行文件是否存在
    $checkExe = ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "test -f $ProjectPath/build/src/KylinQAAssistant && echo 'exists' || echo 'not found'"
    
    if ($checkExe -ne "exists") {
        Write-Error "可执行文件不存在，请先部署和编译项目"
        return $false
    }
    
    # 停止现有的GDB服务器
    Write-Info "停止现有的GDB服务器..."
    ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "pkill gdbserver 2>/dev/null || true"
    Start-Sleep -Seconds 2
    
    # 启动GDB服务器
    Write-Info "启动GDB服务器 (端口: $GdbPort)..."
    
    $gdbCommand = "cd $ProjectPath && ./start_gdbserver.sh $GdbPort"
    
    # 在后台启动GDB服务器
    Start-Process -NoNewWindow -FilePath "ssh" -ArgumentList @(
        "-o", "StrictHostKeyChecking=no",
        "$RemoteUser@$RemoteHost",
        $gdbCommand
    )
    
    Start-Sleep -Seconds 3
    
    # 验证GDB服务器是否启动
    $gdbCheck = ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "pgrep gdbserver >/dev/null && echo 'running' || echo 'not running'"
    
    if ($gdbCheck -eq "running") {
        Write-Success "GDB服务器启动成功"
        Write-Info "调试连接地址: $RemoteHost`:$GdbPort"
        Write-Info "在VS Code中按F5开始调试"
        return $true
    } else {
        Write-Error "GDB服务器启动失败"
        return $false
    }
}

# 清理远程构建
function Clean-RemoteBuild {
    Write-Info "清理远程构建文件..."
    
    ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "cd $ProjectPath && ./clean.sh"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "远程构建清理完成"
    } else {
        Write-Error "远程构建清理失败"
    }
}

# 显示状态信息
function Show-Status {
    Write-Info "获取远程状态信息..."
    
    Write-ColorOutput "远程服务器状态:" "Cyan"
    ssh -o StrictHostKeyChecking=no "$RemoteUser@$RemoteHost" "
        echo '系统信息:'
        uname -a
        echo ''
        echo '磁盘使用:'
        df -h /
        echo ''
        echo '内存使用:'
        free -h
        echo ''
        echo '项目目录:'
        ls -la $ProjectPath 2>/dev/null || echo '项目目录不存在'
        echo ''
        echo '构建状态:'
        if [ -f $ProjectPath/build/src/KylinQAAssistant ]; then
            echo '✓ 可执行文件存在'
            ls -lh $ProjectPath/build/src/KylinQAAssistant
        else
            echo '✗ 可执行文件不存在'
        fi
        echo ''
        echo '进程状态:'
        pgrep gdbserver >/dev/null && echo '✓ GDB服务器运行中' || echo '✗ GDB服务器未运行'
    "
}

# 创建VS Code配置
function Create-VSCodeConfig {
    Write-Info "创建VS Code配置文件..."
    
    # 确保.vscode目录存在
    if (!(Test-Path ".vscode")) {
        New-Item -ItemType Directory -Path ".vscode" -Force | Out-Null
    }
    
    # 创建SSH配置
    $sshConfigPath = "$env:USERPROFILE\.ssh\config"
    $sshConfigDir = Split-Path $sshConfigPath -Parent
    
    if (!(Test-Path $sshConfigDir)) {
        New-Item -ItemType Directory -Path $sshConfigDir -Force | Out-Null
    }
    
    $sshConfig = @"

# 麒麟开发服务器配置
Host kylin-dev
    HostName $RemoteHost
    User $RemoteUser
    Port 22
    PreferredAuthentications password
    StrictHostKeyChecking no
    LocalForward $GdbPort localhost:$GdbPort
    LocalForward 8080 localhost:8080

"@
    
    Add-Content -Path $sshConfigPath -Value $sshConfig
    Write-Success "SSH配置已添加到 $sshConfigPath"
    
    Write-Success "VS Code配置完成"
}

# 主函数
function Main {
    if (!(Test-Prerequisites)) {
        exit 1
    }
    
    switch ($Action.ToLower()) {
        "setup" {
            if (Test-RemoteConnection) {
                Setup-RemoteEnvironment
                Create-VSCodeConfig
            }
        }
        "deploy" {
            if (Test-RemoteConnection) {
                Deploy-Code
            }
        }
        "debug" {
            if (Test-RemoteConnection) {
                Start-DebugSession
            }
        }
        "clean" {
            if (Test-RemoteConnection) {
                Clean-RemoteBuild
            }
        }
        "status" {
            if (Test-RemoteConnection) {
                Show-Status
            }
        }
        "all" {
            if (Test-RemoteConnection) {
                Deploy-Code
                Start-DebugSession
            }
        }
        default {
            Write-Error "未知操作: $Action"
            Write-Info "支持的操作: setup, deploy, debug, clean, status, all"
            exit 1
        }
    }
}

# 显示使用帮助
function Show-Help {
    Write-ColorOutput @"
使用方法:
  .\deploy_and_debug.ps1 -Action <操作> [选项]

操作:
  setup   - 初始化远程环境和VS Code配置
  deploy  - 部署代码并编译
  debug   - 启动调试会话
  clean   - 清理远程构建文件
  status  - 显示远程状态
  all     - 部署并启动调试

选项:
  -RemoteHost     远程主机地址 (默认: 192.168.100.131)
  -RemoteUser     远程用户名 (默认: a)
  -ProjectPath    远程项目路径 (默认: /home/a/kylin-qa-assistant)
  -GdbPort        GDB调试端口 (默认: 5555)

示例:
  .\deploy_and_debug.ps1 -Action setup
  .\deploy_and_debug.ps1 -Action deploy
  .\deploy_and_debug.ps1 -Action debug
  .\deploy_and_debug.ps1 -Action all

"@ "Yellow"
}

# 检查是否需要显示帮助
if ($args -contains "-h" -or $args -contains "--help" -or $args -contains "help") {
    Show-Help
    exit 0
}

# 执行主函数
Main
