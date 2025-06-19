#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试文档上传功能
"""

import requests
import os

def test_upload():
    """测试文档上传"""
    # 检查测试文档是否存在
    test_doc = "docs/kylin_system_intro.md"
    if not os.path.exists(test_doc):
        print(f"测试文档不存在: {test_doc}")
        return
    
    # 上传文档
    url = "http://localhost:8000/upload"
    
    with open(test_doc, 'rb') as f:
        files = {'file': (os.path.basename(test_doc), f, 'text/markdown')}
        
        try:
            response = requests.post(url, files=files)
            
            if response.status_code == 200:
                result = response.json()
                print("✅ 文档上传成功!")
                print(f"文件名: {result['filename']}")
                print(f"分块数量: {result['chunks_count']}")
                print(f"文件大小: {result['file_size']} 字节")
            else:
                print(f"❌ 上传失败: {response.status_code}")
                print(response.text)
                
        except Exception as e:
            print(f"❌ 上传异常: {str(e)}")

def test_status():
    """测试状态查询"""
    url = "http://localhost:8000/status"
    
    try:
        response = requests.get(url)
        
        if response.status_code == 200:
            result = response.json()
            print("📊 系统状态:")
            print(f"状态: {result['status']}")
            print(f"文档数量: {result['documents_count']}")
            print(f"向量数量: {result['vector_count']}")
            print(f"系统信息: {result['system_info']}")
        else:
            print(f"❌ 状态查询失败: {response.status_code}")
            
    except Exception as e:
        print(f"❌ 状态查询异常: {str(e)}")

def test_question():
    """测试问答功能"""
    url = "http://localhost:8000/ask"
    
    questions = [
        "银河麒麟操作系统有什么特点？",
        "如何安装银河麒麟系统？",
        "麒麟系统支持哪些CPU架构？"
    ]
    
    for question in questions:
        print(f"\n🤔 问题: {question}")
        
        try:
            response = requests.post(url, json={"question": question})
            
            if response.status_code == 200:
                result = response.json()
                print(f"💡 回答: {result['answer'][:200]}...")
                print(f"📚 来源数量: {len(result['sources'])}")
                print(f"🎯 置信度: {result['confidence']}")
            else:
                print(f"❌ 问答失败: {response.status_code}")
                print(response.text)
                
        except Exception as e:
            print(f"❌ 问答异常: {str(e)}")

if __name__ == "__main__":
    print("🧪 开始测试银河麒麟智能问答助手...")
    
    print("\n1. 测试文档上传...")
    test_upload()
    
    print("\n2. 测试系统状态...")
    test_status()
    
    print("\n3. 测试问答功能...")
    test_question()
    
    print("\n✅ 测试完成!")
