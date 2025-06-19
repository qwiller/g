#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
银河麒麟智能问答助手 - 桌面版
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox, scrolledtext
import requests
import threading
import os

class KylinQAApp:
    def __init__(self, root):
        self.root = root
        self.root.title("银河麒麟智能问答助手")
        self.root.geometry("800x600")
        
        # API配置
        self.api_base = "http://localhost:8000"
        
        self.setup_ui()
        self.refresh_status()
    
    def setup_ui(self):
        """设置用户界面"""
        # 主框架
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 配置网格权重
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        main_frame.rowconfigure(2, weight=1)
        
        # 标题
        title_label = ttk.Label(main_frame, text="🤖 银河麒麟智能问答助手", 
                               font=("Arial", 16, "bold"))
        title_label.grid(row=0, column=0, columnspan=2, pady=(0, 20))
        
        # 左侧控制面板
        control_frame = ttk.LabelFrame(main_frame, text="控制面板", padding="10")
        control_frame.grid(row=1, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), padx=(0, 10))
        
        # 文档上传
        ttk.Label(control_frame, text="📁 文档管理").grid(row=0, column=0, sticky=tk.W, pady=(0, 10))
        
        upload_btn = ttk.Button(control_frame, text="上传文档", command=self.upload_document)
        upload_btn.grid(row=1, column=0, sticky=(tk.W, tk.E), pady=2)
        
        # 系统状态
        ttk.Label(control_frame, text="📊 系统状态").grid(row=2, column=0, sticky=tk.W, pady=(20, 10))
        
        self.status_text = tk.Text(control_frame, height=8, width=30)
        self.status_text.grid(row=3, column=0, sticky=(tk.W, tk.E), pady=2)
        
        refresh_btn = ttk.Button(control_frame, text="刷新状态", command=self.refresh_status)
        refresh_btn.grid(row=4, column=0, sticky=(tk.W, tk.E), pady=2)
        
        # 右侧对话区域
        chat_frame = ttk.LabelFrame(main_frame, text="智能问答", padding="10")
        chat_frame.grid(row=1, column=1, sticky=(tk.W, tk.E, tk.N, tk.S))
        chat_frame.columnconfigure(0, weight=1)
        chat_frame.rowconfigure(0, weight=1)
        
        # 对话历史
        self.chat_history = scrolledtext.ScrolledText(chat_frame, wrap=tk.WORD, 
                                                     state=tk.DISABLED, height=20)
        self.chat_history.grid(row=0, column=0, columnspan=2, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        
        # 问题输入
        ttk.Label(chat_frame, text="请输入问题:").grid(row=1, column=0, sticky=tk.W)
        
        self.question_entry = ttk.Entry(chat_frame, width=50)
        self.question_entry.grid(row=2, column=0, sticky=(tk.W, tk.E), padx=(0, 10))
        self.question_entry.bind('<Return>', lambda e: self.ask_question())
        
        ask_btn = ttk.Button(chat_frame, text="提问", command=self.ask_question)
        ask_btn.grid(row=2, column=1)
        
        # 初始欢迎消息
        self.add_message("助手", "👋 您好！我是银河麒麟智能问答助手。\n\n请先上传相关文档，然后我就可以基于文档内容回答您的问题了。")
    
    def add_message(self, sender, message):
        """添加消息到对话历史"""
        self.chat_history.config(state=tk.NORMAL)
        
        if sender == "用户":
            self.chat_history.insert(tk.END, f"🤔 {sender}: {message}\n\n")
        else:
            self.chat_history.insert(tk.END, f"💡 {sender}: {message}\n\n")
        
        self.chat_history.config(state=tk.DISABLED)
        self.chat_history.see(tk.END)
    
    def upload_document(self):
        """上传文档"""
        file_path = filedialog.askopenfilename(
            title="选择文档",
            filetypes=[
                ("支持的文档", "*.pdf *.md *.txt"),
                ("PDF文件", "*.pdf"),
                ("Markdown文件", "*.md"),
                ("文本文件", "*.txt"),
                ("所有文件", "*.*")
            ]
        )
        
        if file_path:
            # 在新线程中上传，避免界面卡顿
            threading.Thread(target=self._upload_file, args=(file_path,), daemon=True).start()
    
    def _upload_file(self, file_path):
        """实际上传文件的方法"""
        try:
            filename = os.path.basename(file_path)
            self.add_message("系统", f"正在上传文档: {filename}...")
            
            with open(file_path, 'rb') as f:
                files = {'file': (filename, f, 'application/octet-stream')}
                response = requests.post(f"{self.api_base}/upload", files=files, timeout=30)
            
            if response.status_code == 200:
                result = response.json()
                message = f"✅ 文档上传成功!\n"
                message += f"文件名: {result['filename']}\n"
                message += f"分块数量: {result['chunks_count']}\n"
                message += f"文件大小: {result['file_size']} 字节"
                self.add_message("系统", message)
                self.refresh_status()
            else:
                self.add_message("系统", f"❌ 上传失败: {response.text}")
                
        except Exception as e:
            self.add_message("系统", f"❌ 上传异常: {str(e)}")
    
    def ask_question(self):
        """提问"""
        question = self.question_entry.get().strip()
        if not question:
            return
        
        self.question_entry.delete(0, tk.END)
        self.add_message("用户", question)
        
        # 在新线程中处理问题，避免界面卡顿
        threading.Thread(target=self._process_question, args=(question,), daemon=True).start()
    
    def _process_question(self, question):
        """实际处理问题的方法"""
        try:
            self.add_message("系统", "🤔 正在思考中...")
            
            response = requests.post(
                f"{self.api_base}/ask",
                json={"question": question},
                timeout=60
            )
            
            if response.status_code == 200:
                result = response.json()
                answer = result['answer']
                sources = result['sources']
                confidence = result['confidence']
                
                # 格式化回答
                formatted_answer = f"{answer}\n\n"
                if sources:
                    formatted_answer += f"📚 参考来源 ({len(sources)}个):\n"
                    for i, source in enumerate(sources, 1):
                        formatted_answer += f"{i}. {source['source_name']} (相似度: {source['score']:.2f})\n"
                
                formatted_answer += f"\n🎯 置信度: {confidence:.2f}"
                
                self.add_message("助手", formatted_answer)
            else:
                self.add_message("助手", f"❌ 处理失败: {response.text}")
                
        except Exception as e:
            self.add_message("助手", f"❌ 处理异常: {str(e)}")
    
    def refresh_status(self):
        """刷新系统状态"""
        try:
            response = requests.get(f"{self.api_base}/status", timeout=10)
            
            if response.status_code == 200:
                status = response.json()
                
                status_info = f"状态: {status['status']}\n"
                status_info += f"文档数量: {status['documents_count']}\n"
                status_info += f"向量数量: {status['vector_count']}\n"
                status_info += f"平台: {status['system_info']['platform']}\n"
                status_info += f"AI SDK: {status['system_info']['ai_sdk']}"
                
                self.status_text.delete(1.0, tk.END)
                self.status_text.insert(1.0, status_info)
            else:
                self.status_text.delete(1.0, tk.END)
                self.status_text.insert(1.0, "❌ 无法获取状态")
                
        except Exception as e:
            self.status_text.delete(1.0, tk.END)
            self.status_text.insert(1.0, f"❌ 连接失败: {str(e)}")

def main():
    """主函数"""
    root = tk.Tk()
    app = KylinQAApp(root)
    
    try:
        root.mainloop()
    except KeyboardInterrupt:
        print("程序退出")

if __name__ == "__main__":
    main()
