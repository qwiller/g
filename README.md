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

## 📖 使用说明

### 访问应用

服务启动成功后，可通过以下方式访问：

- **Web界面**: http://localhost:8000
- **API文档**: http://localhost:8000/docs
- **健康检查**: http://localhost:8000/status

### Web界面使用

#### 1. 文档上传和管理

```
📁 支持的文档格式：
├── PDF文件 (.pdf)
├── Markdown文件 (.md)
├── 文本文件 (.txt)
└── 图片文件 (.jpg, .png, .gif, .bmp, .tiff, .webp)
```

**上传步骤：**
1. 点击"选择文件"按钮
2. 选择要上传的文档
3. 点击"上传文档"
4. 等待处理完成（会显示处理进度）

**文档处理说明：**
- PDF文件：自动提取文本内容
- 图片文件：使用OCR技术识别文字
- 大文档：自动分块处理，提高检索效率

#### 2. 智能问答功能

**基本问答：**
```
用户输入：银河麒麟操作系统有什么特点？
AI回答：根据文档内容，银河麒麟操作系统具有以下特点：
1. 安全可靠：采用多层安全防护机制
2. 自主可控：完全自主知识产权
3. 兼容性强：支持多种硬件平台
...
来源：kylin_system_intro.md (第1-3段)
```

**高级问答技巧：**
- 使用具体问题获得更准确答案
- 可以追问相关细节
- 支持多轮对话上下文

#### 3. 本地文件搜索

**搜索功能：**
1. 在"本地文件搜索"区域输入关键词
2. 点击"搜索"按钮
3. 查看搜索结果列表
4. 点击文件路径可查看详情

**搜索示例：**
```bash
搜索关键词：配置文件
搜索结果：
├── /home/user/.bashrc
├── /etc/nginx/nginx.conf
├── /opt/app/config.json
└── ...
```

#### 4. OCR文字识别

**使用方法：**
1. 上传包含文字的图片文件
2. 系统自动调用OCR识别文字
3. 识别结果会添加到文档库
4. 可以基于识别的文字进行问答

**支持的图片格式：**
- JPG/JPEG
- PNG
- GIF
- BMP
- TIFF
- WebP

### API接口使用

#### 核心API端点

```bash
# 上传文档
curl -X POST "http://localhost:8000/upload" \
  -H "Content-Type: multipart/form-data" \
  -F "file=@document.pdf"

# 智能问答
curl -X POST "http://localhost:8000/ask" \
  -H "Content-Type: application/json" \
  -d '{"question": "这个文档讲了什么？", "max_results": 3}'

# 系统状态
curl -X GET "http://localhost:8000/status"

# OCR识别
curl -X POST "http://localhost:8000/ocr" \
  -H "Content-Type: application/json" \
  -d '{"image_path": "/path/to/image.jpg"}'

# 本地搜索
curl -X POST "http://localhost:8000/local-search" \
  -H "Content-Type: application/json" \
  -d '{"query": "配置", "directory": "/home", "max_results": 10}'

# 播放音效
curl -X POST "http://localhost:8000/play-sound" \
  -H "Content-Type: application/json" \
  -d '{"sound_type": "success"}'
```

#### API响应示例

**问答接口响应：**
```json
{
  "answer": "银河麒麟操作系统是一款安全可靠的国产操作系统...",
  "sources": [
    {
      "content": "银河麒麟操作系统特点...",
      "metadata": {
        "source": "kylin_intro.md",
        "page": 1
      },
      "score": 0.95
    }
  ],
  "confidence": 0.92
}
```

**状态接口响应：**
```json
{
  "status": "running",
  "documents_count": 5,
  "vector_count": 25,
  "system_info": {
    "sdk_available": true,
    "ocr_enabled": true,
    "search_enabled": true,
    "sound_enabled": true
  }
}
```

### 桌面应用使用

```bash
# 启动桌面应用
python3 desktop_app.py
```

桌面应用提供与Web界面相同的功能，但具有：
- 本地化界面体验
- 更好的文件管理
- 系统集成功能
- 离线使用能力

### 配置管理

#### AI服务配置

编辑 `config/ai_settings.json`：

```json
{
  "ai_provider": "mixed",
  "kylin_sdk": {
    "enabled": true,
    "ocr_enabled": true,
    "search_enabled": true,
    "sound_enabled": true
  },
  "siliconflow": {
    "enabled": true,
    "api_key": "your_api_key",
    "model": "deepseek-ai/DeepSeek-V3"
  }
}
```

#### 环境变量配置

编辑 `.env` 文件：

```bash
# SDK功能开关
KYLIN_SDK_ENABLED=true
OCR_ENABLED=true
SEARCH_ENABLED=true
SOUND_ENABLED=true

# 服务配置
APP_HOST=0.0.0.0
APP_PORT=8000
DEBUG=false

# AI服务
AI_PROVIDER=mixed
SILICONFLOW_API_KEY=your_key_here
```

## ❓ 常见问题解答

### 安装部署问题

#### Q1: 部署脚本执行失败，提示权限不足
**A:** 确保当前用户有sudo权限，并且脚本有执行权限：
```bash
# 添加执行权限
chmod +x deploy_with_sdk.sh

# 检查sudo权限
sudo -v

# 如果没有sudo权限，联系系统管理员添加
```

#### Q2: 银河麒麟SDK安装失败
**A:** 检查系统版本和软件源配置：
```bash
# 检查系统版本
cat /etc/os-release

# 更新软件源
sudo apt update

# 手动安装SDK包
sudo apt install libkysdk-ocr libkysdk-search libkysdk-soundeffects

# 如果包不存在，检查是否启用了正确的软件源
```

#### Q3: Python依赖安装失败
**A:** 尝试以下解决方案：
```bash
# 使用国内镜像源
pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple/

# 升级pip版本
pip install --upgrade pip

# 清理pip缓存
pip cache purge

# 如果某个包安装失败，单独安装
pip install fastapi uvicorn python-multipart
```

#### Q4: 端口8000被占用
**A:** 更改应用端口或停止占用进程：
```bash
# 查看端口占用
sudo netstat -tlnp | grep :8000
sudo lsof -i :8000

# 停止占用进程
sudo kill -9 <PID>

# 或者修改应用端口
export APP_PORT=8001
python3 backend/main.py
```

### 功能使用问题

#### Q5: OCR识别效果不好
**A:** 优化图片质量和格式：
```bash
# 支持的最佳图片格式
- PNG: 无损压缩，文字清晰
- JPG: 适中文件大小，质量良好
- 分辨率: 建议300DPI以上
- 文字大小: 12号字体以上效果更佳

# 图片预处理建议
- 确保文字清晰可见
- 避免倾斜和模糊
- 良好的光照条件
- 避免复杂背景
```

#### Q6: 文档上传失败
**A:** 检查文件格式和大小：
```bash
# 支持的文件格式
PDF: .pdf
文本: .txt, .md
图片: .jpg, .png, .gif, .bmp, .tiff, .webp

# 文件大小限制
单个文件: 建议不超过50MB
总存储: 根据磁盘空间而定

# 检查文件权限
ls -la your_file.pdf
chmod 644 your_file.pdf
```

#### Q7: 问答结果不准确
**A:** 优化文档质量和问题表述：
```bash
# 文档优化建议
- 确保文档内容完整清晰
- 避免扫描版PDF（文字不可选择）
- 使用结构化的文档格式
- 定期清理无关文档

# 问题优化建议
- 使用具体明确的问题
- 避免过于宽泛的询问
- 可以分步骤提问
- 利用上下文信息
```

#### Q8: 本地搜索无结果
**A:** 检查搜索路径和权限：
```bash
# 检查搜索目录权限
ls -la /home/user/
sudo find /home -name "*keyword*" -type f

# 扩大搜索范围
搜索目录设置为: /home, /opt, /usr/share/doc

# 使用不同的关键词
- 尝试文件名的一部分
- 使用文件扩展名搜索
- 尝试模糊匹配
```

### 性能优化问题

#### Q9: 系统响应速度慢
**A:** 优化系统资源和配置：
```bash
# 检查系统资源
htop
free -h
df -h

# 优化建议
- 增加内存到8GB以上
- 使用SSD存储
- 关闭不必要的后台服务
- 定期清理临时文件

# 应用优化
- 减少同时处理的文档数量
- 调整AI模型参数
- 使用本地AI减少网络延迟
```

#### Q10: 服务经常崩溃
**A:** 检查日志和系统稳定性：
```bash
# 查看应用日志
tail -f app.log
sudo journalctl -u kylin-qa-assistant -f

# 查看系统日志
sudo dmesg | tail
sudo journalctl -xe

# 常见解决方案
- 重启服务: sudo systemctl restart kylin-qa-assistant
- 检查内存使用: free -h
- 更新系统: sudo apt update && sudo apt upgrade
- 重启系统: sudo reboot
```

### 网络和安全问题

#### Q11: 无法访问远程AI服务
**A:** 检查网络连接和API配置：
```bash
# 测试网络连接
ping api.siliconflow.cn
curl -I https://api.siliconflow.cn

# 检查API配置
cat config/ai_settings.json
echo $SILICONFLOW_API_KEY

# 使用本地AI模式
export AI_PROVIDER=kylin_sdk
```

#### Q12: 防火墙阻止访问
**A:** 配置防火墙规则：
```bash
# 检查防火墙状态
sudo ufw status

# 开放应用端口
sudo ufw allow 8000/tcp

# 或者临时关闭防火墙（不推荐）
sudo ufw disable
```

## 🔧 故障排除

### 诊断工具

#### 系统诊断脚本
```bash
#!/bin/bash
# 创建诊断脚本 diagnose.sh

echo "=== 银河麒麟智能问答助手诊断报告 ==="
echo "时间: $(date)"
echo

echo "1. 系统信息:"
cat /etc/os-release | grep -E "(NAME|VERSION)"
uname -a
echo

echo "2. Python环境:"
python3 --version
pip --version
which python3
echo

echo "3. SDK库检查:"
ls -la /usr/lib/libkysdk-* 2>/dev/null || echo "未找到SDK库文件"
echo

echo "4. 服务状态:"
systemctl is-active kylin-qa-assistant 2>/dev/null || echo "服务未运行"
ps aux | grep -E "(python.*main.py|uvicorn)" | grep -v grep
echo

echo "5. 端口检查:"
netstat -tlnp | grep :8000 || echo "端口8000未被占用"
echo

echo "6. 磁盘空间:"
df -h | grep -E "(/$|/home)"
echo

echo "7. 内存使用:"
free -h
echo

echo "=== 诊断完成 ==="
```

#### 日志分析
```bash
# 查看最近的错误日志
tail -100 app.log | grep -i error

# 查看系统服务日志
sudo journalctl -u kylin-qa-assistant --since "1 hour ago"

# 查看Python错误
python3 -c "
import sys
sys.path.append('backend')
try:
    from main import app
    print('应用导入成功')
except Exception as e:
    print(f'应用导入失败: {e}')
"
```

### 快速修复命令

```bash
# 重置服务
sudo systemctl stop kylin-qa-assistant
sudo systemctl start kylin-qa-assistant

# 重建虚拟环境
rm -rf venv
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# 清理缓存和临时文件
rm -rf __pycache__/
rm -rf backend/__pycache__/
rm -f app.log app.pid

# 重新部署
./deploy_with_sdk.sh --force
```

## 📁 项目结构

```
g/
├── README.md                           # 项目说明文档
├── KYLIN_SDK_INTEGRATION_SUMMARY.md    # SDK集成总结
├── requirements.txt                    # 根目录依赖文件
├── rag-assistant/                      # 主应用目录
│   ├── README.md                       # 应用说明
│   ├── requirements.txt                # 应用依赖
│   ├── deploy_with_sdk.sh             # SDK集成部署脚本
│   ├── deploy.sh                      # 标准部署脚本
│   ├── quick_start.sh                 # 快速启动脚本
│   ├── start_service.sh               # 前台启动脚本
│   ├── start_background.sh            # 后台启动脚本
│   ├── stop_service.sh                # 停止服务脚本
│   ├── backend/                       # 后端代码
│   │   ├── main.py                    # FastAPI主程序
│   │   ├── document_processor.py      # 文档处理模块
│   │   ├── vector_store.py            # 向量存储模块
│   │   ├── rag_engine.py              # RAG引擎
│   │   └── kylin_sdk_wrapper.py       # 银河麒麟SDK封装
│   ├── frontend/                      # 前端代码
│   │   └── index.html                 # Web界面
│   ├── config/                        # 配置文件
│   │   ├── ai_config.py               # AI配置管理
│   │   └── ai_settings.json           # AI设置文件
│   ├── docs/                          # 文档目录
│   ├── uploads/                       # 上传文件存储
│   ├── desktop_app.py                 # 桌面应用
│   └── venv/                          # Python虚拟环境
├── backend/                           # 备用后端代码
├── frontend/                          # 备用前端代码
├── config/                            # 全局配置
├── docs/                              # 项目文档
├── scripts/                           # 部署和工具脚本
│   ├── setup_remote_kylin_env.sh      # 远程环境设置
│   ├── deploy_to_remote.ps1           # Windows部署脚本
│   └── test_remote_connection.sh      # 连接测试脚本
└── test_*.py                          # 测试文件
```

### 核心模块说明

#### 后端模块 (backend/)
- **main.py**: FastAPI应用主程序，定义所有API端点
- **document_processor.py**: 文档处理引擎，支持PDF、Markdown、图片等格式
- **vector_store.py**: 向量存储和检索系统
- **rag_engine.py**: RAG（检索增强生成）核心引擎
- **kylin_sdk_wrapper.py**: 银河麒麟AI SDK的Python封装

#### 前端模块 (frontend/)
- **index.html**: 响应式Web界面，包含所有功能模块

#### 配置模块 (config/)
- **ai_config.py**: AI服务配置管理类
- **ai_settings.json**: AI服务配置文件

#### 部署脚本
- **deploy_with_sdk.sh**: 完整SDK集成部署
- **deploy.sh**: 标准部署脚本
- **quick_start.sh**: 快速体验脚本

## 🤝 贡献指南

我们欢迎所有形式的贡献！无论您是初学者还是专家，都可以为项目做出贡献。

### 贡献方式

#### 1. 报告问题 (Issues)
- 🐛 **Bug报告**: 发现问题请创建Issue
- 💡 **功能建议**: 提出新功能想法
- 📖 **文档改进**: 指出文档不清楚的地方
- ❓ **使用问题**: 遇到使用困难可以提问

**Issue模板：**
```markdown
## 问题描述
简要描述遇到的问题

## 环境信息
- 操作系统: 银河麒麟 V10 SP3
- Python版本: 3.8.10
- 项目版本: v1.0.0

## 复现步骤
1. 执行命令 xxx
2. 点击按钮 xxx
3. 出现错误 xxx

## 期望结果
描述期望的正确行为

## 实际结果
描述实际发生的情况

## 附加信息
- 错误日志
- 截图
- 其他相关信息
```

#### 2. 提交代码 (Pull Requests)

**开发流程：**
```bash
# 1. Fork项目到您的GitHub账户

# 2. 克隆您的Fork
git clone https://github.com/YOUR_USERNAME/g.git
cd g

# 3. 创建功能分支
git checkout -b feature/your-feature-name

# 4. 进行开发
# 编写代码、测试、文档

# 5. 提交更改
git add .
git commit -m "feat: 添加新功能描述"

# 6. 推送到您的Fork
git push origin feature/your-feature-name

# 7. 创建Pull Request
```

**代码规范：**
- 使用Python PEP 8编码规范
- 添加适当的注释和文档字符串
- 编写单元测试
- 确保所有测试通过
- 更新相关文档

#### 3. 改进文档
- 修正错别字和语法错误
- 添加使用示例
- 翻译文档到其他语言
- 改进安装和部署指南

#### 4. 测试和反馈
- 在不同环境下测试应用
- 报告兼容性问题
- 提供性能优化建议
- 分享使用经验

### 开发环境设置

```bash
# 1. 克隆项目
git clone https://github.com/qwiller/g.git
cd g/rag-assistant

# 2. 创建开发环境
python3 -m venv dev-env
source dev-env/bin/activate

# 3. 安装开发依赖
pip install -r requirements.txt
pip install pytest black flake8 mypy

# 4. 运行测试
python -m pytest tests/

# 5. 代码格式化
black backend/
flake8 backend/

# 6. 类型检查
mypy backend/
```

### 提交信息规范

使用约定式提交 (Conventional Commits) 格式：

```
<类型>[可选的作用域]: <描述>

[可选的正文]

[可选的脚注]
```

**类型说明：**
- `feat`: 新功能
- `fix`: 修复bug
- `docs`: 文档更新
- `style`: 代码格式化
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具的变动

**示例：**
```
feat(ocr): 添加图片批量识别功能

- 支持同时处理多张图片
- 添加进度显示
- 优化内存使用

Closes #123
```

### 社区准则

- 🤝 **友善互助**: 尊重所有贡献者，营造友好的社区氛围
- 📚 **知识分享**: 乐于分享经验和知识，帮助他人成长
- 🔍 **质量优先**: 注重代码质量和用户体验
- 🌱 **持续学习**: 保持开放心态，不断学习新技术
- 🎯 **目标导向**: 专注于解决实际问题，创造价值

## 📄 许可证

本项目采用 [MIT许可证](https://opensource.org/licenses/MIT) 开源。

### MIT许可证条款

```
MIT License

Copyright (c) 2024 银河麒麟智能问答助手项目

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### 许可证说明

✅ **允许的使用方式：**
- 商业使用
- 修改代码
- 分发软件
- 私人使用
- 专利使用

📋 **必须遵守的条件：**
- 包含许可证和版权声明
- 说明对原始代码的修改

❌ **不承担的责任：**
- 不提供责任担保
- 不提供质量保证

### 第三方许可证

本项目使用了以下开源软件：

- **FastAPI**: MIT许可证
- **Uvicorn**: BSD许可证
- **NumPy**: BSD许可证
- **Requests**: Apache 2.0许可证
- **银河麒麟AI SDK**: 商业许可证（需要单独获取）

## 🙏 致谢

感谢以下项目和组织的支持：

- **银河麒麟操作系统**: 提供优秀的国产操作系统平台
- **银河麒麟AI SDK**: 提供强大的本地AI能力
- **FastAPI**: 现代化的Python Web框架
- **SiliconFlow**: 提供高质量的AI API服务
- **开源社区**: 提供丰富的开源工具和库

特别感谢所有为项目做出贡献的开发者和用户！

## 📞 联系我们

- **GitHub Issues**: [提交问题和建议](https://github.com/qwiller/g/issues)
- **项目主页**: https://github.com/qwiller/g
- **邮箱**: xwiller@gmail.com

---

<div align="center">

**🌟 如果这个项目对您有帮助，请给我们一个Star！**

[![GitHub stars](https://img.shields.io/github/stars/qwiller/g.svg?style=social&label=Star)](https://github.com/qwiller/g/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/qwiller/g.svg?style=social&label=Fork)](https://github.com/qwiller/g/network)

**让我们一起构建更好的银河麒麟AI生态！**

</div>