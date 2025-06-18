# 银河麒麟智能问答助手部署指南

本文档提供了在银河麒麟操作系统上部署智能问答助手的详细步骤。

## 🎯 部署目标

- 在银河麒麟桌面操作系统V10 SP1上成功部署应用程序
- 支持x86_64、ARM64、MIPS64架构
- 确保所有功能正常工作
- 提供用户友好的安装体验

## 📋 部署前检查

### 系统要求验证

```bash
# 检查操作系统版本
cat /etc/os-release

# 检查CPU架构
uname -m

# 检查可用内存
free -h

# 检查磁盘空间
df -h
```

### 网络连接测试

```bash
# 测试网络连接
ping -c 3 8.8.8.8

# 测试HTTPS连接
curl -I https://github.com

# 测试API连接
curl -I https://api.siliconflow.cn
```

## 🚀 快速部署

### 方法一：一键安装（推荐）

```bash
# 1. 克隆项目
git clone https://github.com/qwiller/g.git
cd g

# 2. 运行快速安装脚本
sudo ./scripts/quick_install_kylin.sh
```

### 方法二：分步部署

```bash
# 1. 故障排除（可选）
./scripts/troubleshoot_kylin.sh

# 2. 安装依赖
sudo ./scripts/install_dependencies_kylin.sh

# 3. 编译项目
./scripts/build_kylin.sh --all

# 4. 安装应用程序
sudo ./scripts/build_kylin.sh --install
```

## 🔧 手动部署

### 步骤1：环境准备

```bash
# 更新系统包
sudo apt update && sudo apt upgrade -y

# 安装基础工具
sudo apt install -y git wget curl
```

### 步骤2：安装开发依赖

#### 麒麟V10（基于Ubuntu/Debian）

```bash
# 安装编译工具
sudo apt install -y build-essential cmake pkg-config

# 安装Qt5开发包（麒麟系统标准配置）
sudo apt install -y \
    qtbase5-dev \
    qtbase5-dev-tools \
    qtdeclarative5-dev \
    qttools5-dev \
    qtmultimedia5-dev \
    qml-module-qtquick2 \
    qml-module-qtquick-controls2

# 安装其他依赖
sudo apt install -y \
    libpoppler-cpp-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    python3 \
    python3-pip
```

#### 麒麟V4（基于CentOS/RHEL）

```bash
# 安装编译工具
sudo yum groupinstall -y "Development Tools"
sudo yum install -y cmake pkg-config

# 安装Qt5开发包
sudo yum install -y \
    qt5-qtbase-devel \
    qt5-qtdeclarative-devel \
    qt5-qttools-devel

# 安装其他依赖
sudo yum install -y \
    poppler-cpp-devel \
    libcurl-devel \
    openssl-devel
```

### 步骤3：编译安装

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/kylin-qa-assistant \
      ..

# 编译
make -j$(nproc)

# 运行测试
make test

# 安装
sudo make install
```

### 步骤4：配置应用程序

```bash
# 复制配置文件
sudo cp config/app_config.json /opt/kylin-qa-assistant/etc/kylin-qa-assistant/

# 创建日志目录
sudo mkdir -p /opt/kylin-qa-assistant/logs
sudo chown $USER:$USER /opt/kylin-qa-assistant/logs

# 设置权限
sudo chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant
```

## 📦 打包部署

### 创建DEB包

```bash
# 在构建目录中
cd build
cpack -G DEB

# 安装DEB包
sudo dpkg -i kylin-qa-assistant-*.deb
```

### 创建RPM包

```bash
# 在构建目录中
cd build
cpack -G RPM

# 安装RPM包
sudo rpm -i kylin-qa-assistant-*.rpm
```

## 🔍 部署验证

### 功能测试

```bash
# 启动应用程序
/opt/kylin-qa-assistant/bin/KylinQAAssistant

# 检查日志
tail -f /opt/kylin-qa-assistant/logs/kylin_qa.log

# 测试配置加载
grep "初始化完成" /opt/kylin-qa-assistant/logs/kylin_qa.log
```

### 性能测试

```bash
# 检查内存使用
ps aux | grep KylinQAAssistant

# 检查CPU使用
top -p $(pgrep KylinQAAssistant)

# 检查磁盘使用
du -sh /opt/kylin-qa-assistant
```

## 🛠️ 故障排除

### 常见部署问题

#### 1. 依赖包缺失

```bash
# 运行诊断脚本
./scripts/troubleshoot_kylin.sh

# 检查特定依赖
pkg-config --exists Qt5Core && echo "Qt5 OK" || echo "Qt5 Missing"
```

#### 2. 编译错误

```bash
# 清理重新编译
rm -rf build
mkdir build && cd build
cmake .. 2>&1 | tee cmake.log
make 2>&1 | tee make.log
```

#### 3. 运行时错误

```bash
# 检查库依赖
ldd /opt/kylin-qa-assistant/bin/KylinQAAssistant

# 检查配置文件
cat /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json
```

#### 4. 权限问题

```bash
# 修复权限
sudo chown -R root:root /opt/kylin-qa-assistant
sudo chmod -R 755 /opt/kylin-qa-assistant
sudo chmod +x /opt/kylin-qa-assistant/bin/KylinQAAssistant
```

## 📋 部署检查清单

- [ ] 系统要求满足
- [ ] 网络连接正常
- [ ] 依赖包完整安装
- [ ] 编译成功无错误
- [ ] 测试用例通过
- [ ] 应用程序正常启动
- [ ] 配置文件正确加载
- [ ] 日志系统工作正常
- [ ] 桌面快捷方式创建
- [ ] 用户权限设置正确

## 🔄 更新部署

### 应用程序更新

```bash
# 停止应用程序
pkill KylinQAAssistant

# 备份配置
sudo cp /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json ~/app_config.backup

# 更新代码
git pull origin main

# 重新编译安装
./scripts/build_kylin.sh --all
sudo ./scripts/build_kylin.sh --install

# 恢复配置
sudo cp ~/app_config.backup /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json
```

### 配置更新

```bash
# 备份当前配置
sudo cp /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json ~/config.backup

# 更新配置
sudo nano /opt/kylin-qa-assistant/etc/kylin-qa-assistant/app_config.json

# 重启应用程序
/opt/kylin-qa-assistant/bin/KylinQAAssistant
```

## 📞 技术支持

如果在部署过程中遇到问题：

1. 查看详细日志：`/opt/kylin-qa-assistant/logs/kylin_qa.log`
2. 运行诊断脚本：`./scripts/troubleshoot_kylin.sh`
3. 查看GitHub Issues：https://github.com/qwiller/g/issues
4. 联系技术支持：support@kylinos.cn

## 📝 部署记录

建议在部署完成后记录以下信息：

- 部署时间：
- 系统版本：
- 应用版本：
- 部署方式：
- 遇到的问题：
- 解决方案：

---

**部署成功后，您就可以开始使用银河麒麟智能问答助手了！**
