#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试PDF修复功能
"""

import requests
import os

def test_pdf_upload():
    """测试PDF上传功能"""
    print("🧪 测试PDF上传功能...")
    
    # 测试Markdown文件（作为对照）
    md_file = "docs/kylin_system_intro.md"
    if os.path.exists(md_file):
        print(f"\n1. 测试Markdown文件: {md_file}")
        try:
            with open(md_file, 'rb') as f:
                files = {'file': (os.path.basename(md_file), f, 'text/markdown')}
                response = requests.post("http://localhost:8000/upload", files=files)
            
            if response.status_code == 200:
                result = response.json()
                print(f"✅ Markdown上传成功: {result['chunks_count']} 个块")
            else:
                print(f"❌ Markdown上传失败: {response.text}")
        except Exception as e:
            print(f"❌ Markdown上传异常: {str(e)}")
    
    # 检查是否有PDF文件可以测试
    pdf_files = []
    for root, dirs, files in os.walk("."):
        for file in files:
            if file.lower().endswith('.pdf'):
                pdf_files.append(os.path.join(root, file))
    
    if pdf_files:
        print(f"\n2. 发现PDF文件: {len(pdf_files)} 个")
        for pdf_file in pdf_files[:3]:  # 只测试前3个
            print(f"\n测试PDF文件: {pdf_file}")
            try:
                with open(pdf_file, 'rb') as f:
                    files = {'file': (os.path.basename(pdf_file), f, 'application/pdf')}
                    response = requests.post("http://localhost:8000/upload", files=files)
                
                if response.status_code == 200:
                    result = response.json()
                    print(f"✅ PDF上传成功: {result['chunks_count']} 个块")
                else:
                    print(f"❌ PDF上传失败: {response.text}")
            except Exception as e:
                print(f"❌ PDF上传异常: {str(e)}")
    else:
        print("\n2. 未找到PDF文件进行测试")
    
    # 测试系统状态
    print(f"\n3. 检查系统状态...")
    try:
        response = requests.get("http://localhost:8000/status")
        if response.status_code == 200:
            status = response.json()
            print(f"📊 系统状态:")
            print(f"  文档数量: {status['documents_count']}")
            print(f"  向量数量: {status['vector_count']}")
            print(f"  系统状态: {status['status']}")
        else:
            print(f"❌ 状态查询失败: {response.status_code}")
    except Exception as e:
        print(f"❌ 状态查询异常: {str(e)}")

def test_question():
    """测试问答功能"""
    print(f"\n4. 测试问答功能...")
    
    question = "银河麒麟操作系统有什么特点？"
    try:
        response = requests.post(
            "http://localhost:8000/ask",
            json={"question": question}
        )
        
        if response.status_code == 200:
            result = response.json()
            print(f"🤔 问题: {question}")
            print(f"💡 回答: {result['answer'][:200]}...")
            print(f"📚 来源数量: {len(result['sources'])}")
            print(f"🎯 置信度: {result['confidence']}")
        else:
            print(f"❌ 问答失败: {response.text}")
    except Exception as e:
        print(f"❌ 问答异常: {str(e)}")

if __name__ == "__main__":
    print("🔧 银河麒麟智能问答助手 - PDF修复测试")
    print("=" * 50)
    
    test_pdf_upload()
    test_question()
    
    print("\n✅ 测试完成!")
    print("\n💡 提示:")
    print("- 如果PDF上传仍然失败，可能是PDF文件使用了不支持的加密")
    print("- 可以尝试使用其他PDF文件或转换为文本格式")
    print("- 系统已支持Markdown和文本文件，功能完全正常")
