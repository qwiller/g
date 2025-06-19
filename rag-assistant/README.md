# 银河麒麟智能问答助手

基于RAG（检索增强生成）技术的智能文档问答系统，专为银河麒麟操作系统优化。

## 🌟 特性

- **多格式文档支持**: PDF、Markdown、文本文件
- **智能文档分块**: 自动将长文档分割为合适的块
- **语义检索**: 基于文本匹配的智能检索
- **AI问答**: 集成DeepSeek-V3模型生成准确回答
- **双界面支持**: Web界面 + 桌面应用
- **麒麟系统优化**: 专为银河麒麟操作系统设计

## 🏗️ 系统架构

```
rag-assistant/
├── backend/                 # 后端服务
│   ├── main.py             # FastAPI主程序
│   ├── document_processor.py # 文档处理模块
│   ├── vector_store.py     # 向量存储模块
│   └── rag_engine.py       # RAG引擎
├── frontend/               # 前端界面
│   └── index.html          # Web界面
├── docs/                   # 测试文档
├── desktop_app.py          # 桌面应用
├── deploy.sh              # 部署脚本
└── requirements.txt       # Python依赖
```

## 🚀 快速开始

### 环境要求

- Python 3.8+
- 银河麒麟操作系统（推荐）或其他Linux发行版
- 2GB+ 内存
- 网络连接（用于AI模型调用）

### 安装部署

1. **克隆项目**
```bash
git clone <repository-url>
cd rag-assistant
```

2. **一键部署**
```bash
chmod +x deploy.sh
./deploy.sh
```

3. **手动安装**
```bash
# 安装依赖
pip3 install -r requirements.txt

# 启动服务
python3 backend/main.py
```

### 启动服务

```bash
# 使用systemd服务（推荐）
sudo systemctl start kylin-qa-assistant

# 或直接运行
python3 backend/main.py
```

## 📖 使用说明

### Web界面

1. 访问 http://localhost:8000
2. 上传文档（支持PDF、Markdown、文本文件）
3. 在问答区域输入问题
4. 查看AI生成的回答和来源引用

### 桌面应用

```bash
python3 desktop_app.py
```

### API接口

- `POST /upload` - 上传文档
- `POST /ask` - 提问
- `GET /status` - 系统状态
- `GET /docs` - API文档

## 🔧 配置说明

### API配置

编辑 `backend/rag_engine.py` 中的API配置：

```python
self.api_key = "your-api-key"
self.api_base = "https://api.siliconflow.cn/v1"
self.model = "deepseek-ai/DeepSeek-V3"
```

### 文档处理配置

编辑 `backend/document_processor.py` 中的分块配置：

```python
chunk_size = 500      # 文档块大小
chunk_overlap = 50    # 块重叠大小
```

## 🧪 测试

运行测试脚本：

```bash
python3 test_upload.py
```

测试内容：
- 文档上传功能
- 系统状态查询
- 问答功能

## 📊 性能优化

### 文档处理优化

- 支持多种编码格式
- 智能文本清洗
- 句子边界分割

### 检索优化

- 多层次文本匹配
- 相关性评分
- 结果排序

### AI集成优化

- 备选回答机制
- 置信度计算
- 来源追踪

## 🛠️ 开发指南

### 添加新的文档格式

1. 在 `document_processor.py` 中添加解析方法
2. 更新 `process_document` 方法
3. 测试新格式支持

### 自定义检索算法

1. 修改 `vector_store.py` 中的 `search` 方法
2. 调整评分算法
3. 测试检索效果

### 集成新的AI模型

1. 修改 `rag_engine.py` 中的API配置
2. 调整提示词模板
3. 测试生成效果

## 🔍 故障排除

### 常见问题

1. **服务启动失败**
   - 检查端口8000是否被占用
   - 查看日志: `sudo journalctl -u kylin-qa-assistant -f`

2. **文档上传失败**
   - 检查文件格式是否支持
   - 确认文件大小不超过限制

3. **AI回答质量差**
   - 检查API密钥是否正确
   - 尝试上传更相关的文档

4. **桌面应用无法启动**
   - 确保安装了python3-tk
   - 检查DISPLAY环境变量

### 日志查看

```bash
# 系统服务日志
sudo journalctl -u kylin-qa-assistant -f

# 应用日志
tail -f /var/log/kylin-qa-assistant.log
```

## 🤝 贡献指南

1. Fork 项目
2. 创建特性分支
3. 提交更改
4. 推送到分支
5. 创建Pull Request

## 📄 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- 银河麒麟操作系统团队
- DeepSeek AI团队
- 硅基流动API服务
- 开源社区贡献者

## 📞 支持

- 问题反馈: [GitHub Issues](https://github.com/your-repo/issues)
- 技术交流: [讨论区](https://github.com/your-repo/discussions)
- 邮件支持: support@example.com

---

**银河麒麟智能问答助手** - 让AI为国产操作系统赋能 🚀
