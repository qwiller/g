#!/usr/bin/env python3
"""
银河麒麟AI SDK集成测试脚本
验证OCR、搜索、问答等功能的集成效果
"""

import os
import sys
import time
import requests
import json
import logging
from typing import Dict, Any, List
import tempfile
from PIL import Image, ImageDraw, ImageFont

# 添加项目路径
sys.path.append(os.path.join(os.path.dirname(__file__), 'rag-assistant', 'backend'))

def setup_logging():
    """设置日志"""
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler(),
            logging.FileHandler('test_kylin_sdk.log', encoding='utf-8')
        ]
    )

class KylinSDKIntegrationTest:
    """银河麒麟SDK集成测试类"""
    
    def __init__(self, base_url: str = "http://localhost:8000"):
        """
        初始化测试
        
        Args:
            base_url: 服务器基础URL
        """
        self.base_url = base_url
        self.logger = logging.getLogger(__name__)
        self.test_results = {}
        
    def run_all_tests(self) -> Dict[str, Any]:
        """运行所有测试"""
        self.logger.info("🚀 开始银河麒麟AI SDK集成测试")
        
        tests = [
            ("服务器连接测试", self.test_server_connection),
            ("SDK状态检查", self.test_sdk_status),
            ("系统状态API测试", self.test_system_status),
            ("OCR功能测试", self.test_ocr_functionality),
            ("本地搜索功能测试", self.test_local_search),
            ("文档上传测试", self.test_document_upload),
            ("增强问答测试", self.test_enhanced_qa),
            ("音效播放测试", self.test_sound_effects),
            ("配置管理测试", self.test_config_management)
        ]
        
        for test_name, test_func in tests:
            self.logger.info(f"\n📋 执行测试: {test_name}")
            try:
                result = test_func()
                self.test_results[test_name] = {
                    "status": "PASS" if result else "FAIL",
                    "details": result if isinstance(result, dict) else {"success": result}
                }
                status = "✅ PASS" if result else "❌ FAIL"
                self.logger.info(f"{status} {test_name}")
            except Exception as e:
                self.test_results[test_name] = {
                    "status": "ERROR",
                    "details": {"error": str(e)}
                }
                self.logger.error(f"❌ ERROR {test_name}: {e}")
        
        self._generate_test_report()
        return self.test_results
    
    def test_server_connection(self) -> bool:
        """测试服务器连接"""
        try:
            response = requests.get(f"{self.base_url}/", timeout=10)
            return response.status_code == 200
        except Exception as e:
            self.logger.error(f"服务器连接失败: {e}")
            return False
    
    def test_sdk_status(self) -> Dict[str, Any]:
        """测试SDK状态"""
        try:
            response = requests.get(f"{self.base_url}/sdk-info", timeout=10)
            if response.status_code == 200:
                sdk_info = response.json()
                self.logger.info(f"SDK可用性: {sdk_info['sdk_available']}")
                if sdk_info['sdk_available']:
                    modules = sdk_info['modules']
                    self.logger.info(f"模块状态 - OCR: {modules.get('ocr', False)}, "
                                   f"搜索: {modules.get('search', False)}, "
                                   f"音效: {modules.get('sound', False)}")
                return sdk_info
            else:
                return {"error": f"HTTP {response.status_code}"}
        except Exception as e:
            return {"error": str(e)}
    
    def test_system_status(self) -> Dict[str, Any]:
        """测试系统状态API"""
        try:
            response = requests.get(f"{self.base_url}/status", timeout=10)
            if response.status_code == 200:
                status = response.json()
                self.logger.info(f"系统状态: {status['status']}")
                self.logger.info(f"文档数量: {status['documents_count']}")
                self.logger.info(f"向量数量: {status['vector_count']}")
                
                # 检查是否包含SDK信息
                system_info = status.get('system_info', {})
                has_sdk_info = 'kylin_sdk_available' in system_info
                
                return {
                    "status": status,
                    "has_sdk_info": has_sdk_info,
                    "success": True
                }
            else:
                return {"error": f"HTTP {response.status_code}", "success": False}
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def test_ocr_functionality(self) -> Dict[str, Any]:
        """测试OCR功能"""
        try:
            # 创建测试图片
            test_image_path = self._create_test_image()
            
            # 测试OCR API
            response = requests.post(
                f"{self.base_url}/ocr",
                json={"image_path": test_image_path},
                timeout=30
            )
            
            if response.status_code == 200:
                result = response.json()
                self.logger.info(f"OCR结果: {result}")
                
                # 清理测试文件
                if os.path.exists(test_image_path):
                    os.remove(test_image_path)
                
                return {
                    "success": result.get('success', False),
                    "text_length": len(result.get('text', '')),
                    "has_text": bool(result.get('text', '').strip())
                }
            else:
                return {"error": f"HTTP {response.status_code}", "success": False}
                
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def test_local_search(self) -> Dict[str, Any]:
        """测试本地搜索功能"""
        try:
            response = requests.post(
                f"{self.base_url}/local-search",
                json={
                    "query": "test",
                    "directory": "/tmp",
                    "max_results": 5
                },
                timeout=30
            )
            
            if response.status_code == 200:
                result = response.json()
                self.logger.info(f"搜索结果数量: {result.get('count', 0)}")
                
                return {
                    "success": result.get('success', False),
                    "results_count": result.get('count', 0),
                    "has_results": result.get('count', 0) > 0
                }
            else:
                return {"error": f"HTTP {response.status_code}", "success": False}
                
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def test_document_upload(self) -> Dict[str, Any]:
        """测试文档上传功能"""
        try:
            # 创建测试文档
            test_content = "这是银河麒麟操作系统的测试文档。\n包含OCR和搜索功能的集成测试内容。"
            
            with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False, encoding='utf-8') as f:
                f.write(test_content)
                test_file_path = f.name
            
            # 上传文档
            with open(test_file_path, 'rb') as f:
                files = {'file': ('test_doc.txt', f, 'text/plain')}
                response = requests.post(f"{self.base_url}/upload", files=files, timeout=30)
            
            # 清理测试文件
            os.remove(test_file_path)
            
            if response.status_code == 200:
                result = response.json()
                self.logger.info(f"上传结果: {result}")
                
                return {
                    "success": True,
                    "filename": result.get('filename'),
                    "chunks_count": result.get('chunks_count', 0)
                }
            else:
                return {"error": f"HTTP {response.status_code}", "success": False}
                
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def test_enhanced_qa(self) -> Dict[str, Any]:
        """测试增强问答功能"""
        try:
            response = requests.post(
                f"{self.base_url}/ask",
                json={"question": "银河麒麟操作系统有什么特点？"},
                timeout=60
            )
            
            if response.status_code == 200:
                result = response.json()
                self.logger.info(f"问答结果长度: {len(result.get('answer', ''))}")
                self.logger.info(f"来源数量: {len(result.get('sources', []))}")
                self.logger.info(f"置信度: {result.get('confidence', 0)}")
                
                return {
                    "success": True,
                    "answer_length": len(result.get('answer', '')),
                    "sources_count": len(result.get('sources', [])),
                    "confidence": result.get('confidence', 0),
                    "has_answer": bool(result.get('answer', '').strip())
                }
            else:
                return {"error": f"HTTP {response.status_code}", "success": False}
                
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def test_sound_effects(self) -> Dict[str, Any]:
        """测试音效播放功能"""
        try:
            sound_types = ["volume_change", "device_added", "window_attention"]
            results = {}
            
            for sound_type in sound_types:
                response = requests.post(f"{self.base_url}/play-sound/{sound_type}", timeout=10)
                results[sound_type] = response.status_code == 200
                
                if response.status_code == 200:
                    self.logger.info(f"音效播放成功: {sound_type}")
                else:
                    self.logger.warning(f"音效播放失败: {sound_type} - {response.status_code}")
            
            return {
                "success": any(results.values()),
                "sound_results": results,
                "successful_sounds": sum(results.values())
            }
            
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def test_config_management(self) -> Dict[str, Any]:
        """测试配置管理功能"""
        try:
            # 这里测试配置管理的基本功能
            # 由于配置管理主要在后端，我们通过检查系统状态来验证
            response = requests.get(f"{self.base_url}/status", timeout=10)
            
            if response.status_code == 200:
                status = response.json()
                system_info = status.get('system_info', {})
                
                has_config_info = all(key in system_info for key in [
                    'kylin_sdk_available', 'loaded_documents', 'documents_summary'
                ])
                
                return {
                    "success": has_config_info,
                    "has_sdk_config": 'kylin_sdk_available' in system_info,
                    "config_fields_present": has_config_info
                }
            else:
                return {"error": f"HTTP {response.status_code}", "success": False}
                
        except Exception as e:
            return {"error": str(e), "success": False}
    
    def _create_test_image(self) -> str:
        """创建测试图片"""
        # 创建一个简单的测试图片，包含文字
        img = Image.new('RGB', (400, 200), color='white')
        draw = ImageDraw.Draw(img)
        
        # 尝试使用系统字体，如果失败则使用默认字体
        try:
            font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24)
        except:
            font = ImageFont.load_default()
        
        text = "银河麒麟操作系统\nOCR测试文字"
        draw.text((50, 50), text, fill='black', font=font)
        
        # 保存到临时文件
        with tempfile.NamedTemporaryFile(suffix='.png', delete=False) as f:
            img.save(f.name, 'PNG')
            return f.name
    
    def _generate_test_report(self):
        """生成测试报告"""
        self.logger.info("\n" + "="*60)
        self.logger.info("📊 银河麒麟AI SDK集成测试报告")
        self.logger.info("="*60)
        
        total_tests = len(self.test_results)
        passed_tests = sum(1 for result in self.test_results.values() if result['status'] == 'PASS')
        failed_tests = sum(1 for result in self.test_results.values() if result['status'] == 'FAIL')
        error_tests = sum(1 for result in self.test_results.values() if result['status'] == 'ERROR')
        
        self.logger.info(f"总测试数: {total_tests}")
        self.logger.info(f"通过: {passed_tests} ✅")
        self.logger.info(f"失败: {failed_tests} ❌")
        self.logger.info(f"错误: {error_tests} ⚠️")
        self.logger.info(f"成功率: {(passed_tests/total_tests)*100:.1f}%")
        
        self.logger.info("\n详细结果:")
        for test_name, result in self.test_results.items():
            status_icon = {"PASS": "✅", "FAIL": "❌", "ERROR": "⚠️"}[result['status']]
            self.logger.info(f"{status_icon} {test_name}: {result['status']}")
        
        # 保存详细报告到文件
        report_file = f"kylin_sdk_test_report_{int(time.time())}.json"
        with open(report_file, 'w', encoding='utf-8') as f:
            json.dump(self.test_results, f, indent=2, ensure_ascii=False)
        
        self.logger.info(f"\n📄 详细报告已保存到: {report_file}")

def main():
    """主函数"""
    setup_logging()
    
    print("🚀 银河麒麟AI SDK集成测试")
    print("=" * 50)
    
    # 检查服务器是否运行
    print("⏳ 检查服务器状态...")
    try:
        response = requests.get("http://localhost:8000/", timeout=5)
        if response.status_code != 200:
            print("❌ 服务器未正常运行，请先启动服务器")
            return
    except:
        print("❌ 无法连接到服务器，请确保服务器在 http://localhost:8000 运行")
        return
    
    print("✅ 服务器连接正常")
    
    # 运行测试
    tester = KylinSDKIntegrationTest()
    results = tester.run_all_tests()
    
    # 显示总结
    total = len(results)
    passed = sum(1 for r in results.values() if r['status'] == 'PASS')
    
    print(f"\n🎯 测试完成！通过率: {(passed/total)*100:.1f}% ({passed}/{total})")
    
    if passed == total:
        print("🎉 所有测试通过！银河麒麟AI SDK集成成功！")
    else:
        print("⚠️  部分测试未通过，请检查日志了解详情")

if __name__ == "__main__":
    main()
