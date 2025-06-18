# 银河麒麟智能问答助手

基于RAG（检索增强生成）技术的智能问答系统，专为银河麒麟桌面操作系统V10 SP1优化设计。

## 🌟 项目特色

- **🔍 智能文档解析**: 支持PDF、Markdown、文本文件的智能解析和分块
- **🧠 RAG技术**: 结合向量检索和大语言模型，提供准确的问答服务
- **🎤 多模态交互**: 集成语音识别和语音合成功能
- **🖥️ 原生界面**: 基于Qt5的稳定桌面界面（麒麟系统优化）
- **🔧 麒麟优化**: 专为麒麟系统优化，支持多CPU架构

## 🏗️ 技术架构

```
├── UI层（Qt6界面）
├── 业务逻辑层（RAG控制器）
├── 数据访问层（向量数据库、文档存储）
└── 基础设施层（麒麟AI SDK、日志、配置）
```

### 核心模块

1. **文档预处理引擎**: 智能解析PDF、Markdown、文本文件
2. **RAG核心引擎**: 向量检索 + 生成式AI
3. **Qt5桌面界面**: 稳定可靠的用户交互界面（麒麟系统原生支持）
4. **麒麟AI集成**: 语音识别、语音合成、文本向量化

## 🚀 快速开始

### 系统要求

- **操作系统**: 银河麒麟桌面操作系统V10 SP1
- **CPU架构**: x86_64 / ARM64 / MIPS64
- **内存**: 至少2GB RAM
- **磁盘空间**: 至少500MB可用空间
- **编译器**: GCC 8+ 或 Clang 10+

### 一键安装（推荐）

```bash
# 1. 克隆项目
git clone https://github.com/qwiller/g.git
cd g

# 2. 给脚本添加执行权限
chmod +x scripts/*.sh

# 3. 快速安装（自动检测麒麟版本并安装）
sudo ./scripts/quick_install_kylin.sh
```

### 分步安装

如果快速安装遇到问题，可以使用分步安装：

```bash
# 1. 故障排除（可选，用于诊断问题）
./scripts/troubleshoot_kylin.sh

# 2. 安装依赖
sudo ./scripts/install_dependencies_kylin.sh

# 3. 编译项目
./scripts/build_kylin.sh --all

# 4. 安装应用程序
sudo ./scripts/build_kylin.sh --install
```

### 手动编译

#### 麒麟V10系统（基于Ubuntu/Debian）

```bash
# 1. 安装依赖
sudo apt update
sudo apt install -y build-essential cmake git pkg-config

# 安装Qt5（麒麟V10通常使用Qt5）
sudo apt install -y qtbase5-dev qtdeclarative5-dev qttools5-dev \
    qtmultimedia5-dev qml-module-qtquick2 qml-module-qtquick-controls2

# 安装其他依赖
sudo apt install -y libpoppler-cpp-dev libcurl4-openssl-dev \
    python3 python3-pip

# 2. 创建构建目录
mkdir build && cd build

# 3. 配置CMake
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/kylin-qa-assistant ..

# 4. 编译
make -j$(nproc)

# 5. 安装
sudo make install
```

#### 麒麟V4系统（基于CentOS/RHEL）

```bash
# 1. 安装依赖
sudo yum groupinstall -y "Development Tools"
sudo yum install -y cmake git pkgconfig

# 安装Qt5
sudo yum install -y qt5-qtbase-devel qt5-qtdeclarative-devel \
    qt5-qttools-devel qt5-qtmultimedia-devel

# 安装其他依赖
sudo yum install -y poppler-cpp-devel libcurl-devel openssl-devel \
    python3 python3-pip

# 2-5. 编译步骤同上
```

## 📖 使用指南

### 启动应用程序

```bash
# 如果已安装到系统
/opt/kylin-qa-assistant/bin/KylinQAAssistant

# 或从构建目录运行
./build/src/KylinQAAssistant
```

### 基本操作

1. **添加文档**: 点击"打开文档"按钮，选择PDF、Markdown或文本文件
2. **提问**: 在输入框中输入问题，按Enter或点击发送
3. **语音交互**: 
   - 点击"语音输入"按钮开始录音
   - 启用"语音播报"自动朗读回答
4. **查看来源**: 点击回答下方的"查看来源"查看相关文档片段

### 快捷键

- `Ctrl+N`: 新建对话
- `Ctrl+O`: 打开文档
- `Ctrl+S`: 保存对话
- `Ctrl+Enter`: 发送消息
- `F1`: 语音输入

## ⚙️ 配置说明

### 配置文件位置

- 主配置文件: `config/app_config.json`
- 用户配置: `~/.config/kylin-qa-assistant/`
- 日志文件: `logs/kylin_qa.log`

### 主要配置项

```json
{
  "ai": {
    "api_endpoint": "https://api.siliconflow.cn/v1/chat/completions",
    "model": "deepseek-ai/DeepSeek-V3",
    "api_key": "your-api-key",
    "max_tokens": 2048,
    "temperature": 0.7
  },
  "rag": {
    "chunk_size": 500,
    "chunk_overlap": 100,
    "top_k": 5,
    "similarity_threshold": 0.7
  },
  "kylin_ai": {
    "speech_recognition": {
      "enabled": true,
      "language": "zh-CN"
    },
    "text_to_speech": {
      "enabled": true,
      "voice": "zh-CN-XiaoxiaoNeural"
    }
  }
}
```

## 🔧 开发指南

### 项目结构

```
kylin-qa-assistant/
├── CMakeLists.txt              # 主CMake配置
├── README.md                   # 项目说明
├── config/                     # 配置文件
│   └── app_config.json
├── include/                    # 头文件
│   ├── core/                   # 核心模块
│   ├── document/               # 文档处理
│   ├── rag/                    # RAG引擎
│   ├── ai/                     # AI集成
│   └── ui/                     # 用户界面
├── src/                        # 源代码
│   ├── main.cpp                # 主程序入口
│   ├── core/                   # 核心实现
│   ├── document/               # 文档处理实现
│   ├── rag/                    # RAG引擎实现
│   ├── ai/                     # AI集成实现
│   └── ui/                     # 界面实现
├── scripts/                    # 构建脚本
│   ├── build_kylin.sh          # 构建脚本
│   └── install_dependencies_kylin.sh
├── tests/                      # 测试代码
├── resources/                  # 资源文件
└── docs/                       # 文档
```

### 编码规范

- 使用C++17标准
- 遵循Google C++代码风格
- 关键函数添加详细中文注释
- 使用智能指针管理内存
- 异常安全的错误处理

### 添加新功能

1. 在`include/`目录添加头文件
2. 在`src/`目录添加实现文件
3. 更新`CMakeLists.txt`
4. 编写单元测试
5. 更新文档

## 🧪 测试

### 运行测试

```bash
# 编译测试
./scripts/build_kylin.sh --test

# 或手动运行
cd build
make test
```

### 测试覆盖

- 单元测试: 核心模块功能测试
- 集成测试: 模块间交互测试
- 性能测试: 响应时间和内存使用测试

## 📦 部署

### 创建安装包

```bash
# 创建DEB包（适用于基于Debian的麒麟系统）
./scripts/build_kylin.sh --package

# 创建RPM包（适用于基于RedHat的麒麟系统）
cd build
cpack -G RPM
```

### 系统集成

应用程序安装后会自动：
- 创建桌面快捷方式
- 注册文件关联
- 添加到应用程序菜单

## 🐛 故障排除

### 自动诊断工具

```bash
# 运行故障排除脚本，自动检测系统环境和依赖
./scripts/troubleshoot_kylin.sh
```

### 常见问题及解决方案

#### 1. Qt包无法找到

**问题**: `E: 无法定位软件包 qt6-base-dev`

**解决方案**:
```bash
# 使用快速安装脚本，会自动检测并安装合适的Qt版本
sudo ./scripts/quick_install_kylin.sh

# 或手动安装Qt5（麒麟系统通常使用Qt5）
sudo apt install qtbase5-dev qtdeclarative5-dev
```

#### 2. 编译错误

**问题**: CMake配置失败或编译错误

**解决方案**:
```bash
# 1. 清理构建目录
rm -rf build

# 2. 检查依赖
./scripts/troubleshoot_kylin.sh

# 3. 重新安装依赖
sudo ./scripts/install_dependencies_kylin.sh

# 4. 重新编译
./scripts/build_kylin.sh --all
```

#### 3. 运行时崩溃

**问题**: 应用程序启动后崩溃

**解决方案**:
```bash
# 查看详细日志
tail -f logs/kylin_qa.log

# 检查配置文件
cat config/app_config.json

# 在终端中运行以查看错误信息
./build/src/KylinQAAssistant
```

#### 4. 语音功能不可用

**问题**: 语音识别或合成功能无法使用

**解决方案**:
- 检查麒麟AI SDK是否安装
- 确认音频设备权限
- 查看配置文件中的语音设置

#### 5. 网络连接问题

**问题**: 无法连接AI服务

**解决方案**:
```bash
# 检查网络连接
ping 8.8.8.8

# 检查API配置
grep "api_endpoint" config/app_config.json

# 测试HTTPS连接
curl -I https://api.siliconflow.cn
```

### 日志级别

- `DEBUG`: 详细调试信息
- `INFO`: 一般运行信息
- `WARNING`: 警告信息
- `ERROR`: 错误信息
- `CRITICAL`: 严重错误

### 获取帮助

```bash
# 查看实时日志
tail -f logs/kylin_qa.log

# 检查系统信息
./scripts/troubleshoot_kylin.sh

# 验证安装
./scripts/build_kylin.sh --test
```

### 技术支持

如果问题仍未解决：

1. 运行 `./scripts/troubleshoot_kylin.sh` 收集系统信息
2. 查看 `logs/kylin_qa.log` 获取详细错误信息
3. 在GitHub Issues中提交问题报告
4. 联系技术支持: support@kylinos.cn

## 🤝 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork项目
2. 创建功能分支: `git checkout -b feature/new-feature`
3. 提交更改: `git commit -am 'Add new feature'`
4. 推送分支: `git push origin feature/new-feature`
5. 创建Pull Request

## 📄 许可证

本项目采用MIT许可证 - 详见 [LICENSE](LICENSE) 文件

## 👥 团队

- **开发团队**: 麒麟软件AI团队
- **技术支持**: support@kylinos.cn
- **项目主页**: https://github.com/your-username/kylin-qa-assistant

## 🙏 致谢

感谢以下开源项目的支持：
- Qt6 - 跨平台应用程序框架
- Poppler - PDF处理库
- spdlog - 高性能日志库
- nlohmann/json - JSON处理库

---

**银河麒麟智能问答助手** - 让AI助力您的工作效率！
