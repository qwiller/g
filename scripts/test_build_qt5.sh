#!/bin/bash

# Qt5构建测试脚本 - 验证源文件修复

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

echo "========================================"
echo "  Qt5构建测试 - 源文件验证"
echo "========================================"
echo

# 检查当前目录
if [ ! -f "CMakeLists.txt" ]; then
    log_error "请在项目根目录运行此脚本"
    exit 1
fi

# 验证源文件存在性
log_info "验证源文件存在性..."

REQUIRED_SOURCES=(
    "src/main.cpp"
    "src/core/application.cpp"
    "src/core/config_manager.cpp"
    "src/core/logger.cpp"
    "src/document/document_processor.cpp"
    "src/document/pdf_parser.cpp"
    "src/document/markdown_parser.cpp"
    "src/document/text_parser.cpp"
    "src/document/chunk_manager.cpp"
    "src/rag/vector_store.cpp"
    "src/rag/retriever.cpp"
    "src/rag/rag_engine.cpp"
)

MISSING_FILES=()

for file in "${REQUIRED_SOURCES[@]}"; do
    if [ -f "$file" ]; then
        log_success "✓ $file"
    else
        log_error "✗ $file"
        MISSING_FILES+=("$file")
    fi
done

if [ ${#MISSING_FILES[@]} -ne 0 ]; then
    log_error "发现 ${#MISSING_FILES[@]} 个缺失的源文件"
    exit 1
fi

log_success "所有必需的源文件都存在"

# 验证头文件存在性
log_info "验证头文件存在性..."

REQUIRED_HEADERS=(
    "include/core/application.h"
    "include/core/config_manager.h"
    "include/core/logger.h"
    "include/document/document_processor.h"
    "include/document/pdf_parser.h"
    "include/document/markdown_parser.h"
    "include/document/text_parser.h"
    "include/document/chunk_manager.h"
    "include/rag/vector_store.h"
    "include/rag/retriever.h"
    "include/rag/rag_engine.h"
)

MISSING_HEADERS=()

for file in "${REQUIRED_HEADERS[@]}"; do
    if [ -f "$file" ]; then
        log_success "✓ $file"
    else
        log_warning "? $file"
        MISSING_HEADERS+=("$file")
    fi
done

if [ ${#MISSING_HEADERS[@]} -ne 0 ]; then
    log_warning "发现 ${#MISSING_HEADERS[@]} 个缺失的头文件，但不影响编译"
fi

# 检查Qt5环境
log_info "检查Qt5环境..."

if pkg-config --exists Qt5Core; then
    QT5_VERSION=$(pkg-config --modversion Qt5Core)
    log_success "Qt5 Core: $QT5_VERSION"
else
    log_error "Qt5 Core未找到"
    exit 1
fi

if pkg-config --exists Qt5Widgets; then
    log_success "Qt5 Widgets: 可用"
else
    log_error "Qt5 Widgets未找到"
    exit 1
fi

# 清理并创建构建目录
log_info "准备构建环境..."
rm -rf build_test
mkdir build_test
cd build_test

# 配置CMake
log_info "配置CMake（Qt5）..."
if cmake -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=/tmp/kylin-qa-test \
         -DBUILD_TESTS=OFF \
         ..; then
    log_success "CMake配置成功"
else
    log_error "CMake配置失败"
    exit 1
fi

# 尝试编译
log_info "尝试编译..."
if make -j$(nproc) 2>&1 | tee build.log; then
    log_success "编译成功"
    
    # 检查可执行文件
    if [ -f "src/KylinQAAssistant" ]; then
        log_success "可执行文件生成成功"
        
        # 检查文件大小
        FILE_SIZE=$(stat -c%s "src/KylinQAAssistant")
        log_info "可执行文件大小: $((FILE_SIZE / 1024)) KB"
        
        # 检查依赖
        log_info "检查Qt5依赖..."
        QT5_DEPS=$(ldd src/KylinQAAssistant | grep -i qt5 | wc -l)
        if [ $QT5_DEPS -gt 0 ]; then
            log_success "检测到 $QT5_DEPS 个Qt5依赖库"
        else
            log_warning "未检测到Qt5依赖库"
        fi
        
        # 检查缺失的库
        MISSING_LIBS=$(ldd src/KylinQAAssistant | grep "not found" || true)
        if [ -z "$MISSING_LIBS" ]; then
            log_success "所有依赖库都已满足"
        else
            log_warning "存在缺失的依赖库:"
            echo "$MISSING_LIBS"
        fi
        
    else
        log_error "可执行文件生成失败"
        exit 1
    fi
    
else
    log_error "编译失败"
    echo
    log_info "编译错误日志:"
    tail -20 build.log
    exit 1
fi

# 清理测试构建
cd ..
rm -rf build_test

echo
echo "========================================"
echo "           测试完成"
echo "========================================"
echo
log_success "✅ 所有源文件存在"
log_success "✅ Qt5环境正常"
log_success "✅ CMake配置成功"
log_success "✅ 编译成功"
log_success "✅ 可执行文件生成"
echo
echo "项目已准备好进行完整构建！"
echo "运行: ./scripts/build_qt5_kylin.sh"
echo
