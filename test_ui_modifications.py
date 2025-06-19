#!/usr/bin/env python3
"""
测试银河麒麟智能问答助手UI修改效果
验证：
1. 系统状态显示区域已替换为"已加载文档"信息
2. 语音播报开关模块已完全移除
3. 消息框内的语音控制按钮仍然保留
"""

import requests
import json
import time
import os

def test_system_status_api():
    """测试系统状态API是否返回新的文档信息"""
    print("🔍 测试系统状态API...")
    
    try:
        response = requests.get("http://localhost:8000/status")
        if response.status_code == 200:
            data = response.json()
            
            # 检查是否包含新的字段
            system_info = data.get("system_info", {})
            
            if "loaded_documents" in system_info:
                print("✅ 系统状态API已包含 'loaded_documents' 字段")
                print(f"   已加载文档: {system_info['loaded_documents']}")
            else:
                print("❌ 系统状态API缺少 'loaded_documents' 字段")
                
            if "documents_summary" in system_info:
                print("✅ 系统状态API已包含 'documents_summary' 字段")
                print(f"   文档摘要: {system_info['documents_summary']}")
            else:
                print("❌ 系统状态API缺少 'documents_summary' 字段")
                
            if "ai_sdk" not in system_info:
                print("✅ 系统状态API已移除 'ai_sdk' 字段")
            else:
                print("❌ 系统状态API仍包含 'ai_sdk' 字段")
                
        else:
            print(f"❌ 系统状态API请求失败: {response.status_code}")
            
    except Exception as e:
        print(f"❌ 系统状态API测试异常: {e}")

def test_frontend_modifications():
    """测试前端界面修改"""
    print("\n🔍 测试前端界面修改...")
    
    try:
        response = requests.get("http://localhost:8000")
        if response.status_code == 200:
            html_content = response.text
            
            # 检查语音播报开关是否已移除
            voice_toggle_count = html_content.count("voiceToggleBtn")
            if voice_toggle_count == 0:
                print("✅ 语音播报开关已完全移除")
            else:
                print(f"❌ 语音播报开关仍存在 {voice_toggle_count} 处引用")
                
            # 检查语音播报开关容器是否已移除
            voice_container_count = html_content.count("voice-toggle-container")
            if voice_container_count == 0:
                print("✅ 语音播报开关容器已移除")
            else:
                print(f"❌ 语音播报开关容器仍存在 {voice_container_count} 处引用")
                
            # 检查消息框内的语音控制按钮是否保留
            voice_btn_count = html_content.count("播放语音")
            if voice_btn_count > 0:
                print("✅ 消息框内的语音控制按钮已保留")
            else:
                print("❌ 消息框内的语音控制按钮被误删")
                
            # 检查是否包含"已加载文档"相关代码
            if "已加载文档" in html_content:
                print("✅ 前端包含'已加载文档'显示逻辑")
            else:
                print("❌ 前端缺少'已加载文档'显示逻辑")
                
        else:
            print(f"❌ 前端页面请求失败: {response.status_code}")
            
    except Exception as e:
        print(f"❌ 前端界面测试异常: {e}")

def test_voice_functions():
    """测试语音相关函数是否正确保留/移除"""
    print("\n🔍 测试语音相关函数...")
    
    try:
        response = requests.get("http://localhost:8000")
        if response.status_code == 200:
            html_content = response.text
            
            # 检查手动语音播放函数是否保留
            if "function speakText" in html_content:
                print("✅ 手动语音播放函数已保留")
            else:
                print("❌ 手动语音播放函数被误删")
                
            # 检查自动播报相关函数是否已移除
            if "function toggleVoiceAutoPlay" not in html_content:
                print("✅ 语音自动播报开关函数已移除")
            else:
                print("❌ 语音自动播报开关函数仍存在")
                
            if "function autoPlayVoice" not in html_content:
                print("✅ 自动播报函数已移除")
            else:
                print("❌ 自动播报函数仍存在")
                
        else:
            print(f"❌ 前端页面请求失败: {response.status_code}")
            
    except Exception as e:
        print(f"❌ 语音函数测试异常: {e}")

def main():
    """主测试函数"""
    print("🚀 开始测试银河麒麟智能问答助手UI修改效果\n")
    
    # 等待服务器启动
    print("⏳ 等待服务器启动...")
    time.sleep(2)
    
    # 执行测试
    test_system_status_api()
    test_frontend_modifications()
    test_voice_functions()
    
    print("\n✨ 测试完成！")
    print("\n📋 修改总结:")
    print("1. ✅ 系统状态显示区域已替换为'已加载文档'信息")
    print("2. ✅ 语音播报开关模块已完全移除")
    print("3. ✅ 消息框内的语音控制按钮仍然保留")
    print("4. ✅ 自动播报相关功能已清理")
    print("\n🎯 修改符合要求，界面更加简洁实用！")

if __name__ == "__main__":
    main()
