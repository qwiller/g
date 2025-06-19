# 🤖 银河麒麟智能问答助手 - 小白快速上手指南

## 📖 这是什么？

银河麒麟智能问答助手是一个智能文档问答系统，可以：
- 📚 上传PDF、Word、文本文档
- 🤖 基于文档内容智能回答问题
- 🎤 语音播放回答内容
- 🌐 提供Web界面和桌面应用

## 🎯 5分钟快速体验

### 第一步：下载项目
```bash
# 打开终端（按 Ctrl+Alt+T）
git clone https://github.com/qwiller/g.git
cd g/rag-assistant
```

### 第二步：一键部署
```bash
# 运行自动部署脚本
chmod +x deploy.sh
./deploy.sh
```
> 💡 脚本会自动安装所有需要的软件，大约需要5-10分钟

### 第三步：开始使用
```bash
# 打开浏览器访问
http://localhost:8000
```

## 🚀 超简单启动方式

如果已经部署过，下次使用只需：
```bash
cd g/rag-assistant
./quick_start.sh
```

## 📱 使用方法

### 1. 上传文档
- 点击左侧"选择文件"按钮
- 选择PDF、Markdown或文本文件
- 等待上传完成（会显示"✅ 文件上传成功"）

### 2. 开始问答
- 在右侧输入框输入问题
- 点击"发送"按钮
- 等待AI生成回答

### 3. 语音播放
- 点击回答下方的"🔊 播放语音"
- 使用"⏸️ 暂停"和"▶️ 继续"控制播放

## 🔧 常用命令

```bash
# 启动服务
sudo systemctl start kylin-qa-assistant

# 停止服务
sudo systemctl stop kylin-qa-assistant

# 查看服务状态
sudo systemctl status kylin-qa-assistant

# 查看日志（如果有问题）
sudo journalctl -u kylin-qa-assistant -f
```

## ❓ 遇到问题怎么办？

### 问题1：端口被占用
```bash
# 查看谁在使用8000端口
sudo netstat -tlnp | grep :8000

# 重启服务
sudo systemctl restart kylin-qa-assistant
```

### 问题2：服务启动失败
```bash
# 查看详细错误信息
sudo journalctl -u kylin-qa-assistant -n 20

# 手动启动查看错误
cd ~/g/rag-assistant
source venv/bin/activate
python3 backend/main.py
```

### 问题3：浏览器无法访问
```bash
# 检查服务是否运行
curl http://localhost:8000/status

# 检查防火墙
sudo ufw status
sudo ufw allow 8000
```

### 问题4：文档上传失败
- 确保文件格式正确（PDF、MD、TXT）
- 检查文件大小不要太大（建议<50MB）
- 查看浏览器控制台错误信息

## 📞 获取帮助

- 📖 详细部署指南：查看 `DEPLOYMENT_GUIDE.md`
- 🎤 语音功能说明：查看 `VOICE_CONTROLS_FEATURE.md`
- 🐛 问题反馈：https://github.com/qwiller/g/issues

## 🎉 快速测试

部署完成后，可以用这个问题测试：

**问题**：银河麒麟操作系统有什么特点？

**期望结果**：AI会基于上传的文档回答关于麒麟系统的特点

## 💡 小贴士

1. **首次使用**：需要先上传相关文档才能获得准确回答
2. **文档格式**：支持PDF、Markdown(.md)、文本(.txt)文件
3. **网络要求**：需要互联网连接来使用AI服务
4. **浏览器推荐**：Chrome、Firefox、Edge等现代浏览器
5. **语音功能**：首次使用可能需要允许浏览器播放音频

---

**🚀 现在就开始体验银河麒麟智能问答助手吧！**
