# 银河麒麟智能问答助手 (Kylin AI Assistant)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python Version](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)
[![Kylin OS](https://img.shields.io/badge/OS-银河麒麟-red.svg)](https://www.kylinos.cn/)
[![FastAPI](https://img.shields.io/badge/FastAPI-0.104+-green.svg)](https://fastapi.tiangolo.com/)

> 🚀 专为银河麒麟操作系统设计的智能文档问答系统，集成本地AI SDK，支持OCR识别、文件搜索、智能问答等功能

## 📖 项目简介

银河麒麟智能问答助手是一个基于RAG（检索增强生成）技术的智能文档问答系统，专门为银河麒麟操作系统优化设计。该系统集成了银河麒麟AI SDK，提供OCR文字识别、本地文件搜索、系统音效播放等本地化功能，同时支持远程AI服务，为用户提供强大的文档处理和智能问答能力。

### 🌟 核心特性

- **🔍 智能文档问答**: 基于RAG技术，支持PDF、Markdown、文本、图片等多种格式文档
- **🖼️ OCR文字识别**: 集成银河麒麟OCR SDK，支持从图片中提取文字内容
- **📁 本地文件搜索**: 利用银河麒麟搜索SDK，快速搜索本地文件系统
- **🔊 系统音效播放**: 集成音效SDK，提供丰富的用户交互反馈
- **🌐 双界面支持**: Web界面 + 桌面应用，满足不同使用场景
- **🤖 混合AI服务**: 本地AI（银河麒麟SDK）+ 远程AI（SiliconFlow）智能切换
- **⚡ 快速部署**: 一键部署脚本，支持开发和生产环境
- **🛡️ 麒麟优化**: 专为银河麒麟操作系统和国产CPU架构优化

### 🎯 主要用途

- **文档管理**: 智能处理和检索各类文档资料
- **知识问答**: 基于文档内容的智能问答服务
- **办公助手**: 提高文档处理和信息检索效率
- **学习工具**: 辅助学习和研究工作
- **系统集成**: 作为其他应用的AI能力组件

## 🏗️ 技术栈

### 后端技术
- **Python 3.8+**: 主要开发语言
- **FastAPI**: 现代化Web框架，提供高性能API服务
- **Uvicorn**: ASGI服务器，支持异步处理
- **银河麒麟AI SDK**: 本地AI能力支持
  - OCR文字识别SDK
  - 本地文件搜索SDK
  - 系统音效播放SDK

### 前端技术
- **HTML5/CSS3/JavaScript**: 现代Web技术栈
- **响应式设计**: 适配不同屏幕尺寸
- **原生JavaScript**: 无框架依赖，轻量高效

### AI服务
- **本地AI**: 银河麒麟AI SDK
- **远程AI**: SiliconFlow API (DeepSeek-V3模型)
- **RAG引擎**: 自研检索增强生成系统
- **向量存储**: 基于文本相似度的智能检索

### 核心依赖
```
fastapi>=0.104.1          # Web框架
uvicorn>=0.24.0           # ASGI服务器
python-multipart>=0.0.6   # 文件上传支持
pypdf>=3.0.0              # PDF处理
markdown>=3.5.1           # Markdown处理
numpy>=1.24.3             # 数值计算
requests>=2.31.0          # HTTP客户端
python-dotenv>=1.0.0      # 环境变量管理
```

## 💻 系统要求

### 硬件要求
- **CPU**: x86_64架构，2核心以上（推荐4核心）
- **内存**: 4GB以上（推荐8GB）
- **存储**: 10GB可用空间
- **网络**: 可选（用于远程AI服务和软件包下载）

### 软件要求
- **操作系统**: 银河麒麟操作系统 V10 SP1/SP2/SP3 （必需）
- **Python**: 3.8+ 版本
- **Git**: 用于代码管理和部署
- **包管理器**: apt（系统自带）

### 银河麒麟SDK要求
```bash
# 必需的SDK包
libkysdk-ocr              # OCR文字识别功能
libkysdk-search           # 本地文件搜索功能
libkysdk-soundeffects     # 系统音效播放功能

# 开发包（可选，用于二次开发）
libkysdk-ocr-dev
libkysdk-search-dev
libkysdk-soundeffects-dev
```

## 🚀 安装部署指南

### 方式一：一键自动部署（推荐）

适合初学者和快速部署场景，脚本会自动处理所有依赖安装和配置。

```bash
# 1. 克隆项目
git clone https://github.com/qwiller/g.git
cd g

# 2. 进入项目目录
cd rag-assistant

# 3. 运行一键部署脚本
chmod +x deploy_with_sdk.sh
./deploy_with_sdk.sh
```

部署脚本会自动完成以下操作：
- ✅ 检查系统环境和权限
- ✅ 安装银河麒麟AI SDK
- ✅ 创建Python虚拟环境
- ✅ 安装项目依赖
- ✅ 配置系统服务
- ✅ 启动智能问答助手

### 方式二：手动分步部署

适合有经验的用户或需要自定义配置的场景。

#### 步骤1：准备系统环境

```bash
# 更新系统软件包
sudo apt update && sudo apt upgrade -y

# 安装基础依赖
sudo apt install -y python3 python3-pip python3-venv python3-tk git curl
```

#### 步骤2：安装银河麒麟AI SDK

```bash
# 安装OCR功能包
sudo apt install -y libkysdk-ocr libkysdk-ocr-dev

# 安装搜索功能包
sudo apt install -y libkysdk-search libkysdk-search-dev

# 安装音效功能包
sudo apt install -y libkysdk-soundeffects libkysdk-soundeffects-dev

# 更新动态库缓存
sudo ldconfig

# 验证SDK安装
ls -la /usr/lib/libkysdk-*
```

#### 步骤3：获取项目代码

```bash
# 克隆项目仓库
git clone https://github.com/qwiller/g.git
cd g/rag-assistant

# 或者下载压缩包
# wget https://github.com/qwiller/g/archive/main.zip
# unzip main.zip && cd g-main/rag-assistant
```

#### 步骤4：创建Python环境

```bash
# 创建虚拟环境
python3 -m venv venv

# 激活虚拟环境
source venv/bin/activate

# 升级pip
pip install --upgrade pip
```

#### 步骤5：安装Python依赖

```bash
# 安装项目依赖
pip install -r requirements.txt

# 如果网络较慢，可使用国内镜像源
pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple/
```

#### 步骤6：配置环境变量

```bash
# 创建环境配置文件
cat > .env << EOF
# 银河麒麟SDK配置
KYLIN_SDK_ENABLED=true
OCR_ENABLED=true
SEARCH_ENABLED=true
SOUND_ENABLED=true

# AI服务配置
AI_PROVIDER=mixed
SILICONFLOW_API_KEY=your_api_key_here

# 应用配置
APP_HOST=0.0.0.0
APP_PORT=8000
DEBUG=false
EOF
```

#### 步骤7：验证安装

```bash
# 测试SDK功能
python3 -c "
from backend.kylin_sdk_wrapper import get_kylin_sdk
sdk = get_kylin_sdk()
print('SDK状态:', sdk.is_available())
print('SDK信息:', sdk.get_sdk_info())
"

# 测试应用启动
python3 backend/main.py --help
```

### 方式三：快速体验部署

适合快速体验和测试功能。

```bash
# 克隆项目
git clone https://github.com/qwiller/g.git
cd g/rag-assistant

# 快速启动（自动处理依赖）
chmod +x quick_start.sh
./quick_start.sh
```

## 🎮 启动服务

### 开发模式启动

```bash
# 进入项目目录
cd rag-assistant

# 激活虚拟环境
source venv/bin/activate

# 前台启动（可看到实时日志）
python3 backend/main.py

# 或使用启动脚本
./start_service.sh
```

### 生产模式启动

```bash
# 后台启动服务
./start_background.sh

# 使用systemd服务（推荐）
sudo systemctl start kylin-qa-assistant
sudo systemctl enable kylin-qa-assistant  # 开机自启

# 查看服务状态
sudo systemctl status kylin-qa-assistant
```

### 服务管理命令

```bash
# 启动服务
./start_background.sh
sudo systemctl start kylin-qa-assistant

# 停止服务
./stop_service.sh
sudo systemctl stop kylin-qa-assistant

# 重启服务
sudo systemctl restart kylin-qa-assistant

# 查看日志
tail -f app.log
sudo journalctl -u kylin-qa-assistant -f
```