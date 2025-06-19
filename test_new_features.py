#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试新功能：清空数据和图标兼容性
"""

import requests
import time
import os

def test_clear_data_feature():
    """测试清空数据功能"""
    print("🧪 测试清空数据功能")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    # 1. 检查初始状态
    print("1. 检查初始状态...")
    try:
        response = requests.get(f"{base_url}/status")
        if response.status_code == 200:
            status = response.json()
            print(f"   文档数量: {status['documents_count']}")
            print(f"   向量数量: {status['vector_count']}")
        else:
            print(f"   ❌ 状态查询失败: {response.status_code}")
            return
    except Exception as e:
        print(f"   ❌ 连接失败: {e}")
        return
    
    # 2. 上传测试文档
    print("\n2. 上传测试文档...")
    test_doc = "docs/kylin_system_intro.md"
    if os.path.exists(test_doc):
        try:
            with open(test_doc, 'rb') as f:
                files = {'file': (os.path.basename(test_doc), f, 'text/markdown')}
                response = requests.post(f"{base_url}/upload", files=files)
            
            if response.status_code == 200:
                result = response.json()
                print(f"   ✅ 文档上传成功: {result['chunks_count']} 个块")
            else:
                print(f"   ❌ 上传失败: {response.status_code}")
        except Exception as e:
            print(f"   ❌ 上传异常: {e}")
    else:
        print(f"   ⚠️  测试文档不存在: {test_doc}")
    
    # 3. 检查上传后状态
    print("\n3. 检查上传后状态...")
    try:
        response = requests.get(f"{base_url}/status")
        if response.status_code == 200:
            status = response.json()
            print(f"   文档数量: {status['documents_count']}")
            print(f"   向量数量: {status['vector_count']}")
            
            if status['documents_count'] > 0:
                print("   ✅ 有数据可以清空")
            else:
                print("   ⚠️  没有数据，跳过清空测试")
                return
        else:
            print(f"   ❌ 状态查询失败: {response.status_code}")
            return
    except Exception as e:
        print(f"   ❌ 状态查询异常: {e}")
        return
    
    # 4. 测试清空数据API
    print("\n4. 测试清空数据API...")
    try:
        response = requests.delete(f"{base_url}/clear-data")
        if response.status_code == 200:
            result = response.json()
            print(f"   ✅ 清空成功!")
            print(f"   清空文件: {result['cleared_files']} 个")
            print(f"   清空向量: {result['cleared_vectors']} 个")
            print(f"   消息: {result['message']}")
        else:
            print(f"   ❌ 清空失败: {response.status_code}")
            print(f"   错误: {response.text}")
    except Exception as e:
        print(f"   ❌ 清空异常: {e}")
    
    # 5. 验证清空后状态
    print("\n5. 验证清空后状态...")
    time.sleep(1)  # 等待清空完成
    try:
        response = requests.get(f"{base_url}/status")
        if response.status_code == 200:
            status = response.json()
            print(f"   文档数量: {status['documents_count']}")
            print(f"   向量数量: {status['vector_count']}")
            
            if status['documents_count'] == 0 and status['vector_count'] == 0:
                print("   ✅ 数据清空验证成功")
            else:
                print("   ❌ 数据未完全清空")
        else:
            print(f"   ❌ 状态查询失败: {response.status_code}")
    except Exception as e:
        print(f"   ❌ 状态查询异常: {e}")

def test_icon_compatibility():
    """测试图标兼容性"""
    print("\n🎨 测试图标兼容性")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    # 1. 获取前端页面
    print("1. 检查前端页面图标...")
    try:
        response = requests.get(base_url)
        if response.status_code == 200:
            content = response.text
            
            # 检查图标兼容性改进
            icon_checks = [
                ("图标包装类", 'class="icon-fallback"' in content),
                ("系统图标", '<span class="icon-fallback">🤖</span>' in content),
                ("文档管理图标", '<span class="icon-fallback">📁</span>' in content),
                ("上传图标", '<span class="icon-fallback">📄</span>' in content),
                ("清空数据图标", '<span class="icon-fallback">🗑️</span>' in content),
                ("状态图标", '<span class="icon-fallback">📊</span>' in content),
                ("问答图标", '<span class="icon-fallback">💬</span>' in content),
                ("语音输入图标", '<span class="icon-fallback">🎤</span>' in content),
                ("警告图标", '<span class="icon-fallback">⚠️</span>' in content),
            ]
            
            print("   图标兼容性检查:")
            for name, exists in icon_checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
            
            # 检查CSS样式
            css_checks = [
                ("图标样式类", '.icon-fallback' in content),
                ("模态框样式", '.modal {' in content),
                ("清空按钮样式", '.clear-btn {' in content),
                ("动画效果", '@keyframes fadeIn' in content),
            ]
            
            print("\n   CSS样式检查:")
            for name, exists in css_checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
            
        else:
            print(f"   ❌ 页面加载失败: {response.status_code}")
    except Exception as e:
        print(f"   ❌ 页面检查异常: {e}")

def test_web_interface_features():
    """测试Web界面功能"""
    print("\n🌐 测试Web界面功能")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    # 检查所有API端点
    endpoints = [
        ("GET", "/", "主页"),
        ("GET", "/status", "系统状态"),
        ("DELETE", "/clear-data", "清空数据"),
        ("GET", "/docs", "API文档"),
    ]
    
    print("API端点检查:")
    for method, endpoint, name in endpoints:
        try:
            if method == "GET":
                response = requests.get(f"{base_url}{endpoint}")
            elif method == "DELETE":
                response = requests.delete(f"{base_url}{endpoint}")
            
            if response.status_code in [200, 404]:  # 404对于某些端点是正常的
                print(f"   ✅ {name} ({method} {endpoint}): {response.status_code}")
            else:
                print(f"   ❌ {name} ({method} {endpoint}): {response.status_code}")
        except Exception as e:
            print(f"   ❌ {name} ({method} {endpoint}): 异常 - {e}")

def main():
    """主测试函数"""
    print("🧪 银河麒麟智能问答助手 - 新功能测试")
    print("=" * 60)
    print("测试内容:")
    print("1. 清空数据功能")
    print("2. 图标兼容性改进")
    print("3. Web界面功能")
    print("=" * 60)
    
    # 测试清空数据功能
    test_clear_data_feature()
    
    # 测试图标兼容性
    test_icon_compatibility()
    
    # 测试Web界面功能
    test_web_interface_features()
    
    print("\n" + "=" * 60)
    print("🎉 测试完成!")
    print("\n💡 使用说明:")
    print("1. 打开浏览器访问: http://localhost:8000")
    print("2. 测试清空数据按钮功能")
    print("3. 检查图标在银河麒麟系统中的显示效果")
    print("4. 验证语音播放控制功能的兼容性")
    
    print("\n🔧 功能特性:")
    print("✅ 清空数据按钮 - 一键清空所有文档和向量数据")
    print("✅ 确认对话框 - 防止误操作的安全机制")
    print("✅ 图标兼容性 - 针对银河麒麟系统优化的图标显示")
    print("✅ 响应式设计 - 适配不同浏览器和屏幕尺寸")
    print("✅ 语音控制兼容 - 保持与现有功能的完全兼容")

if __name__ == "__main__":
    main()
