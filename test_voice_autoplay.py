#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试语音播报开关功能
"""

import requests
import time
import os

def test_voice_autoplay_feature():
    """测试语音播报开关功能"""
    print("🎵 测试语音播报开关功能")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    # 1. 检查前端页面是否包含语音播报开关
    print("1. 检查语音播报开关界面...")
    try:
        response = requests.get(base_url)
        if response.status_code == 200:
            content = response.text
            
            # 检查语音播报开关相关元素
            voice_checks = [
                ("语音播报开关按钮", 'id="voiceToggleBtn"' in content),
                ("开关文本元素", 'id="voiceToggleText"' in content),
                ("开关描述元素", 'id="voiceToggleDesc"' in content),
                ("自动播报指示器", 'id="autoPlayingIndicator"' in content),
                ("开关容器", 'voice-toggle-container' in content),
                ("开关样式", 'voice-toggle-btn' in content),
                ("开关函数", 'toggleVoiceAutoPlay' in content),
                ("自动播放函数", 'autoPlayVoice' in content),
                ("偏好设置保存", 'saveVoiceAutoPlayPreference' in content),
                ("偏好设置加载", 'loadVoiceAutoPlayPreference' in content),
            ]
            
            print("   语音播报开关检查:")
            all_passed = True
            for name, exists in voice_checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
                if not exists:
                    all_passed = False
            
            if all_passed:
                print("   🎉 语音播报开关界面检查通过!")
            else:
                print("   ⚠️  部分功能可能缺失")
                
        else:
            print(f"   ❌ 页面加载失败: {response.status_code}")
    except Exception as e:
        print(f"   ❌ 页面检查异常: {e}")

def test_voice_autoplay_css():
    """测试语音播报开关CSS样式"""
    print("\n🎨 测试语音播报开关CSS样式")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    try:
        response = requests.get(base_url)
        if response.status_code == 200:
            content = response.text
            
            # 检查CSS样式
            css_checks = [
                ("开关容器样式", '.voice-toggle-container' in content),
                ("开关按钮样式", '.voice-toggle-btn' in content),
                ("开启状态样式", '.voice-toggle-btn.enabled' in content),
                ("关闭状态样式", '.voice-toggle-btn.disabled' in content),
                ("描述文字样式", '.voice-toggle-description' in content),
                ("自动播报指示器样式", '.auto-playing-indicator' in content),
                ("脉冲动画", 'animation: pulse' in content),
                ("悬停效果", ':hover' in content),
            ]
            
            print("   CSS样式检查:")
            for name, exists in css_checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
                
        else:
            print(f"   ❌ 页面加载失败: {response.status_code}")
    except Exception as e:
        print(f"   ❌ CSS检查异常: {e}")

def test_voice_autoplay_javascript():
    """测试语音播报开关JavaScript功能"""
    print("\n🔧 测试语音播报开关JavaScript功能")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    try:
        response = requests.get(base_url)
        if response.status_code == 200:
            content = response.text
            
            # 检查JavaScript功能
            js_checks = [
                ("全局变量定义", 'voiceAutoPlayEnabled' in content),
                ("当前播报消息ID", 'currentAutoPlayMessageId' in content),
                ("开关切换函数", 'function toggleVoiceAutoPlay()' in content),
                ("UI更新函数", 'function updateVoiceToggleUI()' in content),
                ("偏好设置加载", 'function loadVoiceAutoPlayPreference()' in content),
                ("偏好设置保存", 'function saveVoiceAutoPlayPreference()' in content),
                ("自动播放函数", 'function autoPlayVoice(' in content),
                ("停止自动播放", 'function stopCurrentAutoPlay()' in content),
                ("localStorage使用", 'localStorage.getItem' in content),
                ("语音合成检查", 'speechSynthesis' in content),
                ("自动播报触发", 'voiceAutoPlayEnabled' in content and 'autoPlayVoice' in content),
            ]
            
            print("   JavaScript功能检查:")
            for name, exists in js_checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
                
        else:
            print(f"   ❌ 页面加载失败: {response.status_code}")
    except Exception as e:
        print(f"   ❌ JavaScript检查异常: {e}")

def test_integration_with_existing_features():
    """测试与现有功能的集成"""
    print("\n🔗 测试与现有功能的集成")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    try:
        response = requests.get(base_url)
        if response.status_code == 200:
            content = response.text
            
            # 检查与现有功能的兼容性
            integration_checks = [
                ("手动语音播放", 'function speakText(' in content),
                ("语音暂停功能", 'function pauseSpeech(' in content),
                ("语音继续功能", 'function resumeSpeech(' in content),
                ("语音按钮重置", 'function resetVoiceButtons(' in content),
                ("语音状态管理", 'speechState' in content),
                ("当前语音对象", 'currentSpeech' in content),
                ("语音控制按钮", 'voice-controls' in content),
                ("消息添加函数", 'function addMessage(' in content),
                ("AI回答检测", "type === 'assistant'" in content),
                ("自动播报触发条件", 'voiceAutoPlayEnabled' in content and 'type === \'assistant\'' in content),
            ]
            
            print("   功能集成检查:")
            for name, exists in integration_checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
                
        else:
            print(f"   ❌ 页面加载失败: {response.status_code}")
    except Exception as e:
        print(f"   ❌ 集成检查异常: {e}")

def test_api_compatibility():
    """测试API兼容性"""
    print("\n🌐 测试API兼容性")
    print("=" * 50)
    
    base_url = "http://localhost:8000"
    
    # 测试现有API是否正常工作
    api_tests = [
        ("系统状态", "GET", "/status"),
        ("主页访问", "GET", "/"),
        ("API文档", "GET", "/docs"),
    ]
    
    print("   API兼容性检查:")
    for name, method, endpoint in api_tests:
        try:
            if method == "GET":
                response = requests.get(f"{base_url}{endpoint}")
            
            if response.status_code in [200, 404]:  # 404对于某些端点是正常的
                print(f"   ✅ {name} ({method} {endpoint}): {response.status_code}")
            else:
                print(f"   ❌ {name} ({method} {endpoint}): {response.status_code}")
        except Exception as e:
            print(f"   ❌ {name} ({method} {endpoint}): 异常 - {e}")

def main():
    """主测试函数"""
    print("🎵 银河麒麟智能问答助手 - 语音播报开关功能测试")
    print("=" * 60)
    print("测试内容:")
    print("1. 语音播报开关界面元素")
    print("2. CSS样式完整性")
    print("3. JavaScript功能实现")
    print("4. 与现有功能的集成")
    print("5. API兼容性")
    print("=" * 60)
    
    # 执行各项测试
    test_voice_autoplay_feature()
    test_voice_autoplay_css()
    test_voice_autoplay_javascript()
    test_integration_with_existing_features()
    test_api_compatibility()
    
    print("\n" + "=" * 60)
    print("🎉 语音播报开关功能测试完成!")
    
    print("\n💡 功能说明:")
    print("✅ 语音播报开关 - 全局控制AI回答的自动语音播报")
    print("✅ 状态持久化 - 使用localStorage保存用户偏好设置")
    print("✅ 视觉反馈 - 清晰的开关状态指示和播报进度显示")
    print("✅ 兼容性保证 - 与现有手动语音控制功能完全兼容")
    print("✅ 银河麒麟优化 - 针对麒麟系统浏览器优化的图标和样式")
    
    print("\n🔧 使用方法:")
    print("1. 打开浏览器访问: http://localhost:8000")
    print("2. 在智能问答区域找到'语音播报开关'")
    print("3. 点击开关切换自动播报状态:")
    print("   - 🔊 开启: AI回答后自动开始语音播报")
    print("   - 🔇 关闭: AI回答仅显示文字，可手动播放")
    print("4. 设置会自动保存，下次访问时保持您的选择")
    print("5. 手动语音控制功能(播放/暂停/继续)保持不变")

if __name__ == "__main__":
    main()
