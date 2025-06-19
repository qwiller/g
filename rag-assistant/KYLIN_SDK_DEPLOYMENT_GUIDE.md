# 银河麒麟AI SDK集成部署指南

## 概述

本指南详细介绍如何在银河麒麟操作系统上部署集成了AI SDK的智能问答助手，包括OCR文字识别、本地文件搜索和音效播放等功能。

## 系统要求

### 硬件要求
- CPU: x86_64架构，2核心以上
- 内存: 4GB以上（推荐8GB）
- 存储: 10GB可用空间
- 网络: 可选（用于远程AI服务）

### 软件要求
- 操作系统: 银河麒麟操作系统 V10 SP1/SP2/SP3
- Python: 3.8+
- Git: 用于代码管理

## 银河麒麟AI SDK安装

### 1. 安装SDK包

```bash
# 安装OCR功能包
sudo apt update
sudo apt install libkysdk-ocr libkysdk-ocr-dev

# 安装搜索功能包
sudo apt install libkysdk-search libkysdk-search-dev

# 安装音效功能包
sudo apt install libkysdk-soundeffects libkysdk-soundeffects-dev
```

### 2. 验证SDK安装

```bash
# 检查库文件是否存在
ls -la /usr/lib/libkysdk-*
ls -la /usr/include/kysdk/

# 检查头文件
ls -la /usr/include/kysdk/kysdk-system/
```

### 3. 设置环境变量

```bash
# 添加到 ~/.bashrc 或 ~/.profile
export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH
export KYSDK_PATH=/usr/include/kysdk
```

## 项目部署

### 1. 克隆项目

```bash
cd /home/$(whoami)
git clone https://github.com/qwiller/g.git kylin-qa-assistant
cd kylin-qa-assistant
```

### 2. 安装Python依赖

```bash
# 创建虚拟环境
python3 -m venv rag-assistant/venv
source rag-assistant/venv/bin/activate

# 安装依赖
cd rag-assistant
pip install -r requirements.txt

# 安装额外的SDK相关依赖
pip install ctypes-sh Pillow
```

### 3. 配置权限

```bash
# 设置执行权限
chmod +x *.sh
chmod +x backend/*.py

# 设置文件权限
sudo chown -R $(whoami):$(whoami) .
```

### 4. 创建必要目录

```bash
# 创建配置目录
mkdir -p config
mkdir -p uploads
mkdir -p logs

# 设置目录权限
chmod 755 config uploads logs
```

## 配置说明

### 1. AI配置文件

创建 `config/ai_settings.json`：

```json
{
  "ai_provider": "mixed",
  "kylin_sdk": {
    "enabled": true,
    "ocr_enabled": true,
    "search_enabled": true,
    "sound_enabled": true,
    "search_directories": ["/home", "/opt", "/usr/share/doc"]
  },
  "siliconflow": {
    "enabled": true,
    "api_key": "your-api-key-here",
    "api_base": "https://api.siliconflow.cn/v1",
    "model": "deepseek-ai/DeepSeek-V3"
  },
  "rag_settings": {
    "chunk_size": 500,
    "chunk_overlap": 50,
    "max_search_results": 3,
    "enable_local_search": true
  }
}
```

### 2. 环境配置

创建 `.env` 文件：

```bash
# 服务配置
HOST=0.0.0.0
PORT=8000
DEBUG=false

# 日志配置
LOG_LEVEL=INFO
LOG_FILE=logs/app.log

# SDK配置
KYLIN_SDK_ENABLED=true
OCR_ENABLED=true
SEARCH_ENABLED=true
SOUND_ENABLED=true
```

## 启动服务

### 1. 使用启动脚本

```bash
# 后台启动
./start_background.sh

# 前台启动（用于调试）
./start_service.sh
```

### 2. 手动启动

```bash
# 激活虚拟环境
source venv/bin/activate

# 启动服务
cd backend
python3 main.py
```

### 3. 检查服务状态

```bash
# 检查进程
ps aux | grep python | grep main.py

# 检查端口
netstat -tlnp | grep :8000

# 检查日志
tail -f logs/app.log
```

## 功能测试

### 1. 运行集成测试

```bash
# 确保服务已启动
python3 test_kylin_sdk_integration.py
```

### 2. 手动功能测试

#### 测试Web界面
```bash
# 在浏览器中访问
firefox http://localhost:8000
```

#### 测试API接口
```bash
# 检查SDK状态
curl http://localhost:8000/sdk-info

# 检查系统状态
curl http://localhost:8000/status

# 测试本地搜索
curl -X POST http://localhost:8000/local-search \
  -H "Content-Type: application/json" \
  -d '{"query": "test", "directory": "/home", "max_results": 5}'
```

## 故障排除

### 1. SDK库未找到

**问题**: 提示找不到SDK库文件

**解决方案**:
```bash
# 检查库文件位置
find /usr -name "libkysdk*" 2>/dev/null

# 更新库缓存
sudo ldconfig

# 设置环境变量
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
```

### 2. 权限问题

**问题**: 无法访问某些目录或文件

**解决方案**:
```bash
# 检查当前用户权限
id

# 添加用户到必要的组
sudo usermod -a -G audio,video $(whoami)

# 重新登录使权限生效
```

### 3. 端口占用

**问题**: 8000端口被占用

**解决方案**:
```bash
# 查找占用进程
sudo netstat -tlnp | grep :8000

# 杀死占用进程
sudo kill -9 <PID>

# 或使用其他端口
export PORT=8001
```

### 4. OCR功能异常

**问题**: OCR识别失败或返回空结果

**解决方案**:
```bash
# 检查图片文件权限
ls -la /path/to/image

# 检查图片格式支持
file /path/to/image

# 测试简单图片
python3 -c "
from rag-assistant.backend.kylin_sdk_wrapper import get_kylin_sdk
sdk = get_kylin_sdk()
print(sdk.is_available())
"
```

## 性能优化

### 1. 系统优化

```bash
# 增加文件描述符限制
echo "* soft nofile 65536" | sudo tee -a /etc/security/limits.conf
echo "* hard nofile 65536" | sudo tee -a /etc/security/limits.conf

# 优化内存使用
echo "vm.swappiness=10" | sudo tee -a /etc/sysctl.conf
```

### 2. 应用优化

- 调整 `chunk_size` 和 `chunk_overlap` 参数
- 限制搜索目录范围
- 设置合适的 `max_search_results`

### 3. 监控和日志

```bash
# 设置日志轮转
sudo tee /etc/logrotate.d/kylin-qa-assistant << EOF
/home/*/kylin-qa-assistant/rag-assistant/logs/*.log {
    daily
    missingok
    rotate 7
    compress
    notifempty
    create 644 $(whoami) $(whoami)
}
EOF
```

## 安全配置

### 1. 防火墙设置

```bash
# 开放必要端口
sudo ufw allow 8000/tcp

# 限制访问来源（可选）
sudo ufw allow from 192.168.1.0/24 to any port 8000
```

### 2. 服务用户

```bash
# 创建专用用户（推荐）
sudo useradd -r -s /bin/false kylin-qa
sudo chown -R kylin-qa:kylin-qa /home/*/kylin-qa-assistant
```

## 维护和更新

### 1. 定期维护

```bash
# 清理日志
find logs/ -name "*.log" -mtime +7 -delete

# 清理临时文件
find uploads/ -name "*.tmp" -delete

# 更新依赖
pip install --upgrade -r requirements.txt
```

### 2. 备份配置

```bash
# 备份配置文件
tar -czf config_backup_$(date +%Y%m%d).tar.gz config/

# 备份数据
tar -czf data_backup_$(date +%Y%m%d).tar.gz uploads/ logs/
```

## 支持和帮助

### 1. 日志分析

- 应用日志: `logs/app.log`
- 系统日志: `/var/log/syslog`
- SDK日志: 检查应用日志中的SDK相关信息

### 2. 常用命令

```bash
# 查看服务状态
./status.sh

# 重启服务
./stop_service.sh && ./start_background.sh

# 查看实时日志
tail -f logs/app.log

# 测试SDK功能
python3 test_kylin_sdk_integration.py
```

### 3. 获取帮助

- 查看项目文档: `README.md`
- 运行测试脚本获取详细信息
- 检查GitHub Issues页面

---

**注意**: 本指南基于银河麒麟操作系统V10系列，其他版本可能需要适当调整。如遇到问题，请参考故障排除部分或联系技术支持。
