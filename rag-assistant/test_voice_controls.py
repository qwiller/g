#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试语音播放控制功能
"""

import requests
import time

def test_voice_controls():
    """测试语音播放控制功能"""
    print("🎤 测试银河麒麟智能问答助手 - 语音播放控制功能")
    print("=" * 60)
    
    base_url = "http://localhost:8000"
    
    # 1. 测试系统状态
    print("1. 测试系统状态...")
    try:
        response = requests.get(f"{base_url}/status")
        if response.status_code == 200:
            status = response.json()
            print(f"✅ 系统状态正常")
            print(f"   文档数量: {status['documents_count']}")
            print(f"   向量数量: {status['vector_count']}")
        else:
            print(f"❌ 系统状态异常: {response.status_code}")
            return
    except Exception as e:
        print(f"❌ 无法连接到服务: {e}")
        return
    
    # 2. 测试前端页面
    print("\n2. 测试前端页面...")
    try:
        response = requests.get(base_url)
        if response.status_code == 200:
            content = response.text
            
            # 检查关键功能是否存在
            checks = [
                ("语音播放按钮", "🔊 播放语音" in content),
                ("暂停按钮", "⏸️ 暂停" in content),
                ("继续按钮", "▶️ 继续" in content),
                ("语音输入按钮", "🎤" in content),
                ("语音控制函数", "speakText" in content),
                ("暂停函数", "pauseSpeech" in content),
                ("继续函数", "resumeSpeech" in content),
                ("Web Speech API", "speechSynthesis" in content),
            ]
            
            print("✅ 前端页面加载成功")
            print("   功能检查:")
            for name, exists in checks:
                status = "✅" if exists else "❌"
                print(f"   {status} {name}")
                
        else:
            print(f"❌ 前端页面加载失败: {response.status_code}")
            return
    except Exception as e:
        print(f"❌ 前端页面测试失败: {e}")
        return
    
    # 3. 测试问答功能（生成语音内容）
    print("\n3. 测试问答功能...")
    test_questions = [
        "银河麒麟操作系统有什么特点？",
        "麒麟系统支持哪些CPU架构？"
    ]
    
    for i, question in enumerate(test_questions, 1):
        print(f"\n   问题 {i}: {question}")
        try:
            response = requests.post(
                f"{base_url}/ask",
                json={"question": question},
                timeout=60
            )
            
            if response.status_code == 200:
                result = response.json()
                answer_length = len(result['answer'])
                sources_count = len(result['sources'])
                confidence = result['confidence']
                
                print(f"   ✅ 回答生成成功")
                print(f"      回答长度: {answer_length} 字符")
                print(f"      来源数量: {sources_count}")
                print(f"      置信度: {confidence:.2f}")
                print(f"      回答预览: {result['answer'][:100]}...")
                
                # 检查回答是否适合语音播放
                if answer_length > 50:
                    print(f"      🔊 回答长度适合语音播放")
                else:
                    print(f"      ⚠️  回答较短，语音播放效果可能不明显")
                    
            else:
                print(f"   ❌ 问答失败: {response.status_code}")
                print(f"      错误信息: {response.text}")
                
        except Exception as e:
            print(f"   ❌ 问答异常: {e}")
    
    # 4. 输出使用说明
    print("\n4. 语音播放控制功能使用说明:")
    print("   🌐 访问地址: http://localhost:8000")
    print("   📝 使用步骤:")
    print("      1. 在Web界面中提问")
    print("      2. 等待AI回答生成")
    print("      3. 点击回答下方的 '🔊 播放语音' 按钮")
    print("      4. 语音开始播放后，可以:")
    print("         - 点击 '⏸️ 暂停' 暂停播放")
    print("         - 点击 '▶️ 继续' 恢复播放")
    print("      5. 语音播放完毕后按钮自动重置")
    
    print("\n5. 浏览器兼容性:")
    print("   ✅ Chrome/Edge: 完全支持")
    print("   ✅ Firefox: 支持语音播放")
    print("   ✅ Safari: 支持语音播放")
    print("   ⚠️  注意: 某些浏览器可能需要用户交互后才能播放语音")
    
    print("\n6. 技术实现:")
    print("   🔧 Web Speech API - speechSynthesis")
    print("   🔧 语音合成 - SpeechSynthesisUtterance")
    print("   🔧 播放控制 - pause() / resume() / cancel()")
    print("   🔧 状态管理 - 播放中/暂停/停止")
    
    print("\n✅ 语音播放控制功能测试完成!")
    print("💡 提示: 请在浏览器中实际测试语音播放功能")

if __name__ == "__main__":
    test_voice_controls()
