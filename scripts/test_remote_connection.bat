@echo off
REM 远程SSH连接测试脚本 (Windows版本)
REM 用于测试连接到麒麟系统服务器

setlocal enabledelayedexpansion

REM 远程服务器配置
set REMOTE_HOST=192.168.100.131
set REMOTE_USER=a
set REMOTE_PASSWORD=1234qwer
set REMOTE_PORT=22
set GDB_PORT=5555
set PROJECT_NAME=kylin-qa-assistant
set REMOTE_PROJECT_PATH=/home/%REMOTE_USER%/%PROJECT_NAME%

echo ========================================
echo           远程SSH连接测试
echo ========================================
echo.
echo 目标服务器: %REMOTE_HOST%
echo 用户名: %REMOTE_USER%
echo 端口: %REMOTE_PORT%
echo.

REM 测试网络连通性
echo [INFO] 测试网络连通性...
ping -n 3 %REMOTE_HOST% >nul 2>&1
if %errorlevel% equ 0 (
    echo [SUCCESS] 网络连通性正常
) else (
    echo [ERROR] 无法ping通远程主机 %REMOTE_HOST%
    echo 请检查：
    echo   1. 网络连接是否正常
    echo   2. 远程主机是否开机
    echo   3. 防火墙设置
    goto :error
)

REM 测试SSH连接
echo [INFO] 测试SSH连接...
echo 请输入密码: %REMOTE_PASSWORD%

REM 使用plink进行SSH测试（需要安装PuTTY）
where plink >nul 2>&1
if %errorlevel% equ 0 (
    echo y | plink -ssh -P %REMOTE_PORT% -pw %REMOTE_PASSWORD% %REMOTE_USER%@%REMOTE_HOST% "echo 'SSH连接成功'" 2>nul
    if !errorlevel! equ 0 (
        echo [SUCCESS] SSH连接成功
    ) else (
        echo [ERROR] SSH连接失败
        echo 请检查用户名和密码是否正确
        goto :error
    )
) else (
    echo [WARNING] plink未找到，请安装PuTTY或使用其他SSH客户端测试
    echo 手动测试命令: ssh %REMOTE_USER%@%REMOTE_HOST% -p %REMOTE_PORT%
)

REM 创建SSH配置文件
echo [INFO] 创建SSH配置...

if not exist "%USERPROFILE%\.ssh" mkdir "%USERPROFILE%\.ssh"

REM 创建SSH配置
echo. >> "%USERPROFILE%\.ssh\config"
echo # 麒麟开发服务器配置 >> "%USERPROFILE%\.ssh\config"
echo Host kylin-dev-server >> "%USERPROFILE%\.ssh\config"
echo     HostName %REMOTE_HOST% >> "%USERPROFILE%\.ssh\config"
echo     User %REMOTE_USER% >> "%USERPROFILE%\.ssh\config"
echo     Port %REMOTE_PORT% >> "%USERPROFILE%\.ssh\config"
echo     Compression yes >> "%USERPROFILE%\.ssh\config"
echo     ServerAliveInterval 60 >> "%USERPROFILE%\.ssh\config"
echo. >> "%USERPROFILE%\.ssh\config"

echo [SUCCESS] SSH配置已添加

REM 创建部署脚本
echo [INFO] 创建部署脚本...

REM 创建PowerShell部署脚本
echo # 部署到远程麒麟服务器脚本 > deploy_to_remote.ps1
echo. >> deploy_to_remote.ps1
echo $REMOTE_HOST = "%REMOTE_HOST%" >> deploy_to_remote.ps1
echo $REMOTE_USER = "%REMOTE_USER%" >> deploy_to_remote.ps1
echo $REMOTE_PATH = "%REMOTE_PROJECT_PATH%" >> deploy_to_remote.ps1
echo. >> deploy_to_remote.ps1
echo Write-Host "正在同步代码到远程服务器..." >> deploy_to_remote.ps1
echo. >> deploy_to_remote.ps1
echo # 使用WSL的rsync进行同步 >> deploy_to_remote.ps1
echo wsl rsync -avz --delete --exclude 'build*' --exclude '.git' --progress ./ $REMOTE_USER@$REMOTE_HOST`:$REMOTE_PATH/ >> deploy_to_remote.ps1
echo. >> deploy_to_remote.ps1
echo Write-Host "代码同步完成！" >> deploy_to_remote.ps1
echo. >> deploy_to_remote.ps1
echo # 远程编译 >> deploy_to_remote.ps1
echo Write-Host "开始远程编译..." >> deploy_to_remote.ps1
echo ssh $REMOTE_USER@$REMOTE_HOST "cd $REMOTE_PATH && chmod +x scripts/*.sh && ./scripts/build_qt5_kylin.sh" >> deploy_to_remote.ps1
echo. >> deploy_to_remote.ps1
echo Write-Host "部署完成！" >> deploy_to_remote.ps1

REM 创建调试启动脚本
echo # 启动远程调试脚本 > start_remote_debug.ps1
echo. >> start_remote_debug.ps1
echo $REMOTE_HOST = "%REMOTE_HOST%" >> start_remote_debug.ps1
echo $REMOTE_USER = "%REMOTE_USER%" >> start_remote_debug.ps1
echo $REMOTE_PATH = "%REMOTE_PROJECT_PATH%" >> start_remote_debug.ps1
echo $GDB_PORT = "%GDB_PORT%" >> start_remote_debug.ps1
echo. >> start_remote_debug.ps1
echo Write-Host "启动远程GDB服务器..." >> start_remote_debug.ps1
echo. >> start_remote_debug.ps1
echo # 启动远程GDB服务器 >> start_remote_debug.ps1
echo Start-Process -NoNewWindow ssh -ArgumentList "$REMOTE_USER@$REMOTE_HOST", "cd $REMOTE_PATH && pkill gdbserver 2>/dev/null || true && sleep 1 && gdbserver :$GDB_PORT ./build/src/KylinQAAssistant" >> start_remote_debug.ps1
echo. >> start_remote_debug.ps1
echo Write-Host "GDB服务器已启动" >> start_remote_debug.ps1
echo Write-Host "调试连接地址: %REMOTE_HOST%:%GDB_PORT%" >> start_remote_debug.ps1
echo Write-Host "在IDE中配置远程调试连接到此地址" >> start_remote_debug.ps1

REM 创建批处理版本的部署脚本
echo @echo off > deploy_to_remote.bat
echo echo 正在部署到远程麒麟服务器... >> deploy_to_remote.bat
echo. >> deploy_to_remote.bat
echo set REMOTE_HOST=%REMOTE_HOST% >> deploy_to_remote.bat
echo set REMOTE_USER=%REMOTE_USER% >> deploy_to_remote.bat
echo set REMOTE_PATH=%REMOTE_PROJECT_PATH% >> deploy_to_remote.bat
echo. >> deploy_to_remote.bat
echo REM 使用WSL进行文件同步 >> deploy_to_remote.bat
echo wsl rsync -avz --delete --exclude 'build*' --exclude '.git' --progress ./ %%REMOTE_USER%%@%%REMOTE_HOST%%:%%REMOTE_PATH%%/ >> deploy_to_remote.bat
echo. >> deploy_to_remote.bat
echo echo 代码同步完成！ >> deploy_to_remote.bat
echo. >> deploy_to_remote.bat
echo REM 远程编译 >> deploy_to_remote.bat
echo echo 开始远程编译... >> deploy_to_remote.bat
echo ssh %%REMOTE_USER%%@%%REMOTE_HOST%% "cd %%REMOTE_PATH%% && chmod +x scripts/*.sh && ./scripts/build_qt5_kylin.sh" >> deploy_to_remote.bat
echo. >> deploy_to_remote.bat
echo echo 部署完成！ >> deploy_to_remote.bat
echo pause >> deploy_to_remote.bat

echo [SUCCESS] 部署脚本已创建

REM 更新VS Code配置
echo [INFO] 更新VS Code配置...

if not exist ".vscode" mkdir ".vscode"

REM 创建远程调试配置
echo { > .vscode\launch_remote.json
echo     "version": "0.2.0", >> .vscode\launch_remote.json
echo     "configurations": [ >> .vscode\launch_remote.json
echo         { >> .vscode\launch_remote.json
echo             "name": "Remote Debug (Kylin Server)", >> .vscode\launch_remote.json
echo             "type": "cppdbg", >> .vscode\launch_remote.json
echo             "request": "launch", >> .vscode\launch_remote.json
echo             "program": "%REMOTE_PROJECT_PATH%/build/src/KylinQAAssistant", >> .vscode\launch_remote.json
echo             "args": [], >> .vscode\launch_remote.json
echo             "stopAtEntry": false, >> .vscode\launch_remote.json
echo             "cwd": "%REMOTE_PROJECT_PATH%", >> .vscode\launch_remote.json
echo             "environment": [ >> .vscode\launch_remote.json
echo                 { >> .vscode\launch_remote.json
echo                     "name": "QT_QPA_PLATFORM", >> .vscode\launch_remote.json
echo                     "value": "offscreen" >> .vscode\launch_remote.json
echo                 } >> .vscode\launch_remote.json
echo             ], >> .vscode\launch_remote.json
echo             "externalConsole": false, >> .vscode\launch_remote.json
echo             "MIMode": "gdb", >> .vscode\launch_remote.json
echo             "miDebuggerPath": "/usr/bin/gdb", >> .vscode\launch_remote.json
echo             "miDebuggerServerAddress": "%REMOTE_HOST%:%GDB_PORT%", >> .vscode\launch_remote.json
echo             "setupCommands": [ >> .vscode\launch_remote.json
echo                 { >> .vscode\launch_remote.json
echo                     "description": "Enable pretty-printing for gdb", >> .vscode\launch_remote.json
echo                     "text": "-enable-pretty-printing", >> .vscode\launch_remote.json
echo                     "ignoreFailures": true >> .vscode\launch_remote.json
echo                 } >> .vscode\launch_remote.json
echo             ] >> .vscode\launch_remote.json
echo         } >> .vscode\launch_remote.json
echo     ] >> .vscode\launch_remote.json
echo } >> .vscode\launch_remote.json

echo [SUCCESS] VS Code远程调试配置已创建

goto :success

:error
echo.
echo [ERROR] 连接测试失败！
echo 请检查网络连接和服务器配置
pause
exit /b 1

:success
echo.
echo ========================================
echo           连接测试完成
echo ========================================
echo.
echo 📋 连接信息:
echo   服务器地址: %REMOTE_HOST%
echo   用户名: %REMOTE_USER%
echo   SSH端口: %REMOTE_PORT%
echo   GDB端口: %GDB_PORT%
echo   项目路径: %REMOTE_PROJECT_PATH%
echo.
echo 🚀 使用方法:
echo   1. 快速连接: ssh kylin-dev-server
echo   2. 部署代码: deploy_to_remote.bat 或 deploy_to_remote.ps1
echo   3. 启动调试: start_remote_debug.ps1
echo.
echo 🔧 VS Code调试:
echo   使用配置: Remote Debug (Kylin Server)
echo   配置文件: .vscode\launch_remote.json
echo.
echo 📁 创建的文件:
echo   deploy_to_remote.bat - 批处理部署脚本
echo   deploy_to_remote.ps1 - PowerShell部署脚本
echo   start_remote_debug.ps1 - 调试启动脚本
echo   .vscode\launch_remote.json - VS Code调试配置
echo.
pause
