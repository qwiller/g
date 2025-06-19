#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
性能测试脚本
"""

import time
import requests
import statistics
import threading
from concurrent.futures import ThreadPoolExecutor
import os

class PerformanceTest:
    def __init__(self, base_url="http://localhost:8000"):
        self.base_url = base_url
        self.results = []
    
    def test_upload_performance(self, file_path, num_tests=5):
        """测试文档上传性能"""
        print(f"📊 测试文档上传性能 (文件: {os.path.basename(file_path)}, 次数: {num_tests})")
        
        upload_times = []
        
        for i in range(num_tests):
            start_time = time.time()
            
            try:
                with open(file_path, 'rb') as f:
                    files = {'file': (os.path.basename(file_path), f, 'application/octet-stream')}
                    response = requests.post(f"{self.base_url}/upload", files=files, timeout=60)
                
                end_time = time.time()
                upload_time = end_time - start_time
                
                if response.status_code == 200:
                    upload_times.append(upload_time)
                    print(f"  测试 {i+1}: {upload_time:.2f}秒 ✅")
                else:
                    print(f"  测试 {i+1}: 失败 ({response.status_code}) ❌")
                    
            except Exception as e:
                print(f"  测试 {i+1}: 异常 ({str(e)}) ❌")
        
        if upload_times:
            avg_time = statistics.mean(upload_times)
            min_time = min(upload_times)
            max_time = max(upload_times)
            
            print(f"📈 上传性能统计:")
            print(f"  平均时间: {avg_time:.2f}秒")
            print(f"  最快时间: {min_time:.2f}秒")
            print(f"  最慢时间: {max_time:.2f}秒")
            
            return {
                'avg': avg_time,
                'min': min_time,
                'max': max_time,
                'times': upload_times
            }
        
        return None
    
    def test_query_performance(self, questions, num_tests=3):
        """测试问答性能"""
        print(f"🤔 测试问答性能 (问题数: {len(questions)}, 每题测试: {num_tests}次)")
        
        all_times = []
        
        for question in questions:
            print(f"  问题: {question[:30]}...")
            question_times = []
            
            for i in range(num_tests):
                start_time = time.time()
                
                try:
                    response = requests.post(
                        f"{self.base_url}/ask",
                        json={"question": question},
                        timeout=60
                    )
                    
                    end_time = time.time()
                    query_time = end_time - start_time
                    
                    if response.status_code == 200:
                        question_times.append(query_time)
                        print(f"    测试 {i+1}: {query_time:.2f}秒 ✅")
                    else:
                        print(f"    测试 {i+1}: 失败 ({response.status_code}) ❌")
                        
                except Exception as e:
                    print(f"    测试 {i+1}: 异常 ({str(e)}) ❌")
            
            if question_times:
                avg_time = statistics.mean(question_times)
                print(f"    平均: {avg_time:.2f}秒")
                all_times.extend(question_times)
        
        if all_times:
            overall_avg = statistics.mean(all_times)
            overall_min = min(all_times)
            overall_max = max(all_times)
            
            print(f"📈 问答性能统计:")
            print(f"  总体平均: {overall_avg:.2f}秒")
            print(f"  最快响应: {overall_min:.2f}秒")
            print(f"  最慢响应: {overall_max:.2f}秒")
            
            return {
                'avg': overall_avg,
                'min': overall_min,
                'max': overall_max,
                'times': all_times
            }
        
        return None
    
    def test_concurrent_queries(self, question, num_concurrent=5, num_requests=10):
        """测试并发查询性能"""
        print(f"⚡ 测试并发查询性能 (并发数: {num_concurrent}, 总请求: {num_requests})")
        
        def make_request():
            start_time = time.time()
            try:
                response = requests.post(
                    f"{self.base_url}/ask",
                    json={"question": question},
                    timeout=60
                )
                end_time = time.time()
                return {
                    'time': end_time - start_time,
                    'status': response.status_code,
                    'success': response.status_code == 200
                }
            except Exception as e:
                end_time = time.time()
                return {
                    'time': end_time - start_time,
                    'status': 0,
                    'success': False,
                    'error': str(e)
                }
        
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=num_concurrent) as executor:
            futures = [executor.submit(make_request) for _ in range(num_requests)]
            results = [future.result() for future in futures]
        
        end_time = time.time()
        total_time = end_time - start_time
        
        successful_requests = [r for r in results if r['success']]
        failed_requests = [r for r in results if not r['success']]
        
        if successful_requests:
            response_times = [r['time'] for r in successful_requests]
            avg_response_time = statistics.mean(response_times)
            
            print(f"📈 并发性能统计:")
            print(f"  总耗时: {total_time:.2f}秒")
            print(f"  成功请求: {len(successful_requests)}/{num_requests}")
            print(f"  失败请求: {len(failed_requests)}")
            print(f"  平均响应时间: {avg_response_time:.2f}秒")
            print(f"  吞吐量: {len(successful_requests)/total_time:.2f} 请求/秒")
            
            return {
                'total_time': total_time,
                'successful': len(successful_requests),
                'failed': len(failed_requests),
                'avg_response_time': avg_response_time,
                'throughput': len(successful_requests)/total_time
            }
        
        return None
    
    def test_system_status(self, num_tests=10):
        """测试系统状态查询性能"""
        print(f"📊 测试系统状态查询性能 (次数: {num_tests})")
        
        status_times = []
        
        for i in range(num_tests):
            start_time = time.time()
            
            try:
                response = requests.get(f"{self.base_url}/status", timeout=10)
                end_time = time.time()
                status_time = end_time - start_time
                
                if response.status_code == 200:
                    status_times.append(status_time)
                    print(f"  测试 {i+1}: {status_time:.3f}秒 ✅")
                else:
                    print(f"  测试 {i+1}: 失败 ({response.status_code}) ❌")
                    
            except Exception as e:
                print(f"  测试 {i+1}: 异常 ({str(e)}) ❌")
        
        if status_times:
            avg_time = statistics.mean(status_times)
            min_time = min(status_times)
            max_time = max(status_times)
            
            print(f"📈 状态查询性能:")
            print(f"  平均时间: {avg_time:.3f}秒")
            print(f"  最快时间: {min_time:.3f}秒")
            print(f"  最慢时间: {max_time:.3f}秒")
            
            return {
                'avg': avg_time,
                'min': min_time,
                'max': max_time
            }
        
        return None

def main():
    """主测试函数"""
    print("🧪 银河麒麟智能问答助手 - 性能测试")
    print("=" * 50)
    
    tester = PerformanceTest()
    
    # 测试系统状态
    print("\n1. 系统状态查询测试")
    tester.test_system_status()
    
    # 测试文档上传（如果有测试文档）
    test_doc = "docs/kylin_system_intro.md"
    if os.path.exists(test_doc):
        print("\n2. 文档上传性能测试")
        tester.test_upload_performance(test_doc)
    
    # 测试问答性能
    print("\n3. 问答性能测试")
    test_questions = [
        "银河麒麟操作系统有什么特点？",
        "如何安装银河麒麟系统？",
        "麒麟系统支持哪些CPU架构？"
    ]
    tester.test_query_performance(test_questions)
    
    # 测试并发性能
    print("\n4. 并发查询测试")
    tester.test_concurrent_queries("银河麒麟操作系统有什么特点？", num_concurrent=3, num_requests=6)
    
    print("\n✅ 性能测试完成!")

if __name__ == "__main__":
    main()
