# 银河麒麟智能问答助手 - 小白部署指南

## 📋 部署概述

本指南将帮助您在银河麒麟操作系统上快速部署银河麒麟智能问答助手。即使您是编程初学者，也能按照本指南轻松完成部署。

## 🎯 部署目标

部署完成后，您将获得：
- 🌐 Web版智能问答系统 (http://localhost:8000)
- 🖥️ 桌面版问答应用
- 📚 支持PDF、Markdown、文本文档上传
- 🎤 语音播放和控制功能
- 🤖 基于AI的智能问答

## 📋 系统要求

### 硬件要求
- **CPU**: 双核1GHz以上处理器
- **内存**: 4GB RAM（推荐8GB）
- **存储**: 2GB可用磁盘空间
- **网络**: 互联网连接（用于AI服务）

### 软件要求
- **操作系统**: 银河麒麟V10/V4.0.2或更高版本
- **Python**: 3.8或更高版本（通常系统自带）
- **浏览器**: Chrome、Firefox、Edge等现代浏览器

## 🚀 快速部署（推荐）

### 方法一：一键自动部署

1. **打开终端**
   ```bash
   # 按 Ctrl+Alt+T 打开终端
   ```

2. **下载项目**
   ```bash
   # 克隆项目到本地
   git clone https://github.com/qwiller/g.git
   cd g/rag-assistant
   ```

3. **执行一键部署**
   ```bash
   # 给部署脚本执行权限
   chmod +x deploy.sh
   
   # 运行一键部署脚本
   ./deploy.sh
   ```

4. **等待部署完成**
   - 脚本会自动安装所有依赖
   - 创建系统服务
   - 启动应用程序
   - 整个过程约需5-10分钟

5. **验证部署**
   ```bash
   # 检查服务状态
   sudo systemctl status kylin-qa-assistant
   
   # 如果显示 "active (running)" 则部署成功
   ```

6. **访问应用**
   - 打开浏览器访问：http://localhost:8000
   - 看到欢迎界面即表示部署成功

## 🔧 手动部署（详细步骤）

如果自动部署失败，请按照以下步骤手动部署：

### 步骤1：准备环境

1. **更新系统包**
   ```bash
   sudo apt update
   sudo apt upgrade -y
   ```

2. **安装必要的系统依赖**
   ```bash
   sudo apt install -y python3 python3-pip python3-venv python3-tk git curl
   ```

3. **验证Python版本**
   ```bash
   python3 --version
   # 应该显示 Python 3.8.x 或更高版本
   ```

### 步骤2：下载项目

1. **克隆项目**
   ```bash
   # 在用户主目录下创建项目目录
   cd ~
   git clone https://github.com/qwiller/g.git
   cd g/rag-assistant
   ```

2. **查看项目结构**
   ```bash
   ls -la
   # 应该看到 backend/, frontend/, docs/ 等目录
   ```

### 步骤3：安装Python依赖

1. **创建虚拟环境**
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   ```

2. **安装依赖包**
   ```bash
   pip install --upgrade pip
   pip install -r requirements.txt
   ```

3. **验证安装**
   ```bash
   pip list | grep fastapi
   # 应该显示 fastapi 相关包
   ```

### 步骤4：启动服务

1. **启动后端服务**
   ```bash
   # 确保在 rag-assistant 目录下
   python3 backend/main.py
   ```

2. **验证服务启动**
   - 看到 "🚀 启动银河麒麟智能问答助手..." 表示启动成功
   - 服务运行在 http://localhost:8000

3. **测试服务**
   ```bash
   # 新开一个终端窗口测试
   curl http://localhost:8000/status
   # 应该返回JSON格式的状态信息
   ```

### 步骤5：访问应用

1. **打开Web界面**
   - 打开浏览器
   - 访问：http://localhost:8000
   - 看到智能问答助手界面

2. **测试桌面应用**
   ```bash
   # 在新终端中运行桌面版
   cd ~/g/rag-assistant
   source venv/bin/activate
   python3 desktop_app.py
   ```

## 📚 使用指南

### 首次使用

1. **上传文档**
   - 在Web界面左侧点击"选择文件"
   - 选择PDF、Markdown或文本文件
   - 等待上传和处理完成

2. **开始问答**
   - 在右侧问答区域输入问题
   - 点击"发送"按钮
   - 等待AI生成回答

3. **语音功能**
   - 点击回答下方的"🔊 播放语音"
   - 使用"⏸️ 暂停"和"▶️ 继续"控制播放

### 常用操作

```bash
# 启动服务
sudo systemctl start kylin-qa-assistant

# 停止服务
sudo systemctl stop kylin-qa-assistant

# 重启服务
sudo systemctl restart kylin-qa-assistant

# 查看服务状态
sudo systemctl status kylin-qa-assistant

# 查看服务日志
sudo journalctl -u kylin-qa-assistant -f
```

## 🔧 故障排除

### 常见问题及解决方案

#### 问题1：端口8000被占用
```bash
# 查看占用端口的进程
sudo netstat -tlnp | grep :8000

# 杀死占用进程（替换PID为实际进程ID）
sudo kill -9 PID

# 重新启动服务
sudo systemctl restart kylin-qa-assistant
```

#### 问题2：Python依赖安装失败
```bash
# 更新pip
pip install --upgrade pip

# 使用国内镜像源
pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple/

# 如果还是失败，逐个安装
pip install fastapi uvicorn python-multipart pypdf pycryptodome markdown numpy requests python-dotenv
```

#### 问题3：权限问题
```bash
# 给当前用户添加必要权限
sudo usermod -a -G audio,video $USER

# 重新登录或重启系统使权限生效
```

#### 问题4：服务无法启动
```bash
# 检查配置文件
sudo systemctl cat kylin-qa-assistant

# 手动启动查看错误信息
cd ~/g/rag-assistant
source venv/bin/activate
python3 backend/main.py
```

#### 问题5：浏览器无法访问
```bash
# 检查防火墙设置
sudo ufw status

# 如果防火墙开启，允许8000端口
sudo ufw allow 8000

# 检查服务是否真正运行
curl http://localhost:8000/status
```

### 日志查看

```bash
# 查看应用日志
sudo journalctl -u kylin-qa-assistant -n 50

# 实时查看日志
sudo journalctl -u kylin-qa-assistant -f

# 查看错误日志
sudo journalctl -u kylin-qa-assistant --since "1 hour ago" -p err
```

## 🔄 更新和维护

### 更新应用

```bash
# 停止服务
sudo systemctl stop kylin-qa-assistant

# 更新代码
cd ~/g
git pull origin main

# 更新依赖
cd rag-assistant
source venv/bin/activate
pip install -r requirements.txt --upgrade

# 重启服务
sudo systemctl start kylin-qa-assistant
```

### 备份数据

```bash
# 备份文档数据
cp ~/g/rag-assistant/documents.json ~/backup/
cp -r ~/g/rag-assistant/uploads/ ~/backup/

# 备份配置
sudo cp /etc/systemd/system/kylin-qa-assistant.service ~/backup/
```

### 卸载应用

```bash
# 停止并禁用服务
sudo systemctl stop kylin-qa-assistant
sudo systemctl disable kylin-qa-assistant

# 删除服务文件
sudo rm /etc/systemd/system/kylin-qa-assistant.service
sudo systemctl daemon-reload

# 删除应用目录
rm -rf ~/g

# 删除虚拟环境
rm -rf ~/.local/share/virtualenvs/rag-assistant-*
```

## 📞 技术支持

### 获取帮助

- **GitHub Issues**: https://github.com/qwiller/g/issues
- **项目文档**: 查看项目目录下的README.md
- **功能说明**: 查看VOICE_CONTROLS_FEATURE.md

### 反馈问题

提交问题时请包含：
1. 操作系统版本：`cat /etc/kylin-release`
2. Python版本：`python3 --version`
3. 错误日志：`sudo journalctl -u kylin-qa-assistant -n 20`
4. 详细的操作步骤

## ✅ 部署检查清单

部署完成后，请检查以下项目：

- [ ] 服务状态正常：`sudo systemctl status kylin-qa-assistant`
- [ ] Web界面可访问：http://localhost:8000
- [ ] 文档上传功能正常
- [ ] 问答功能正常
- [ ] 语音播放功能正常
- [ ] 桌面应用可启动
- [ ] 系统服务自启动：`sudo systemctl is-enabled kylin-qa-assistant`

## 🎉 部署完成

恭喜！您已经成功在银河麒麟操作系统上部署了智能问答助手。

现在您可以：
1. 上传相关文档
2. 开始智能问答
3. 体验语音播放功能
4. 使用桌面版应用

享受AI助手带来的便利吧！🚀
