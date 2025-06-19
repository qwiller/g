#!/bin/bash
# 银河麒麟智能问答助手 - 超简单快速启动脚本

# 颜色定义
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}"
echo "🚀 银河麒麟智能问答助手 - 快速启动"
echo "=================================="
echo -e "${NC}"

echo -e "${BLUE}正在启动服务...${NC}"

# 检查虚拟环境
if [ ! -d "venv" ]; then
    echo -e "${YELLOW}⚠️  未找到虚拟环境，请先运行部署脚本: ./deploy.sh${NC}"
    exit 1
fi

# 激活虚拟环境
source venv/bin/activate

# 启动服务
echo -e "${BLUE}🔄 启动智能问答助手...${NC}"
python3 backend/main.py &

# 等待服务启动
sleep 3

# 检查服务
if curl -s http://localhost:8000/status > /dev/null; then
    echo -e "${GREEN}✅ 服务启动成功！${NC}"
    echo ""
    echo -e "${BLUE}🌐 访问地址: http://localhost:8000${NC}"
    echo -e "${BLUE}📖 使用说明:${NC}"
    echo "  1. 打开浏览器访问上述地址"
    echo "  2. 上传文档开始问答"
    echo "  3. 按 Ctrl+C 停止服务"
    echo ""
    
    # 自动打开浏览器（如果可能）
    if command -v xdg-open > /dev/null; then
        echo -e "${BLUE}🔄 正在打开浏览器...${NC}"
        xdg-open http://localhost:8000 2>/dev/null &
    fi
    
    # 等待用户停止
    echo -e "${YELLOW}按 Ctrl+C 停止服务${NC}"
    wait
else
    echo -e "${YELLOW}⚠️  服务启动可能失败，请检查错误信息${NC}"
fi
