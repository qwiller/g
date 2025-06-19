#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
文档处理模块 - 支持PDF、Markdown、文本文档
"""

import os
import re
import logging
from typing import List, Dict, Any
import PyPDF2
import markdown
from io import StringIO

logger = logging.getLogger(__name__)

class DocumentProcessor:
    """文档处理器"""
    
    def __init__(self, chunk_size: int = 500, chunk_overlap: int = 50):
        """
        初始化文档处理器
        
        Args:
            chunk_size: 文档块大小（字符数）
            chunk_overlap: 块之间的重叠字符数
        """
        self.chunk_size = chunk_size
        self.chunk_overlap = chunk_overlap
        
    def process_document(self, file_path: str) -> List[Dict[str, Any]]:
        """
        处理文档，返回分块结果
        
        Args:
            file_path: 文档文件路径
            
        Returns:
            List[Dict]: 文档块列表，每个块包含text、metadata等信息
        """
        try:
            # 根据文件扩展名选择处理方法
            ext = os.path.splitext(file_path)[1].lower()
            
            if ext == '.pdf':
                text = self._extract_pdf_text(file_path)
            elif ext in ['.md', '.markdown']:
                text = self._extract_markdown_text(file_path)
            elif ext == '.txt':
                text = self._extract_text_file(file_path)
            else:
                raise ValueError(f"不支持的文件格式: {ext}")
            
            # 清洗文本
            cleaned_text = self._clean_text(text)
            
            # 分块
            chunks = self._split_text_into_chunks(cleaned_text)
            
            # 生成元数据
            result = []
            for i, chunk in enumerate(chunks):
                result.append({
                    'text': chunk,
                    'metadata': {
                        'source': os.path.basename(file_path),
                        'chunk_id': i,
                        'total_chunks': len(chunks),
                        'file_type': ext,
                        'chunk_size': len(chunk)
                    }
                })
            
            logger.info(f"文档处理完成: {file_path}, 共 {len(result)} 个块")
            return result
            
        except Exception as e:
            logger.error(f"文档处理失败: {file_path}, 错误: {str(e)}")
            raise
    
    def _extract_pdf_text(self, file_path: str) -> str:
        """提取PDF文本"""
        try:
            text = ""
            with open(file_path, 'rb') as file:
                pdf_reader = PyPDF2.PdfReader(file)
                for page in pdf_reader.pages:
                    text += page.extract_text() + "\n"
            return text
        except Exception as e:
            logger.error(f"PDF文本提取失败: {str(e)}")
            raise
    
    def _extract_markdown_text(self, file_path: str) -> str:
        """提取Markdown文本"""
        try:
            with open(file_path, 'r', encoding='utf-8') as file:
                md_content = file.read()
            
            # 将Markdown转换为纯文本
            html = markdown.markdown(md_content)
            # 简单去除HTML标签
            text = re.sub(r'<[^>]+>', '', html)
            # 处理HTML实体
            text = text.replace('&nbsp;', ' ').replace('&lt;', '<').replace('&gt;', '>')
            
            return text
        except Exception as e:
            logger.error(f"Markdown文本提取失败: {str(e)}")
            raise
    
    def _extract_text_file(self, file_path: str) -> str:
        """提取纯文本文件内容"""
        try:
            # 尝试多种编码
            encodings = ['utf-8', 'gbk', 'gb2312', 'latin-1']
            
            for encoding in encodings:
                try:
                    with open(file_path, 'r', encoding=encoding) as file:
                        return file.read()
                except UnicodeDecodeError:
                    continue
            
            # 如果所有编码都失败，使用二进制模式并忽略错误
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
                return file.read()
                
        except Exception as e:
            logger.error(f"文本文件读取失败: {str(e)}")
            raise
    
    def _clean_text(self, text: str) -> str:
        """清洗文本"""
        # 移除多余的空白字符
        text = re.sub(r'\s+', ' ', text)
        
        # 移除特殊字符（保留中文、英文、数字、基本标点）
        text = re.sub(r'[^\u4e00-\u9fff\w\s.,!?;:()[\]{}"\'-]', '', text)
        
        # 移除多余的换行
        text = re.sub(r'\n\s*\n', '\n\n', text)
        
        return text.strip()
    
    def _split_text_into_chunks(self, text: str) -> List[str]:
        """将文本分割成块"""
        if len(text) <= self.chunk_size:
            return [text]
        
        chunks = []
        start = 0
        
        while start < len(text):
            end = start + self.chunk_size
            
            # 如果不是最后一块，尝试在句子边界分割
            if end < len(text):
                # 寻找句号、问号、感叹号等句子结束标记
                sentence_end = max(
                    text.rfind('。', start, end),
                    text.rfind('！', start, end),
                    text.rfind('？', start, end),
                    text.rfind('.', start, end),
                    text.rfind('!', start, end),
                    text.rfind('?', start, end)
                )
                
                if sentence_end > start + self.chunk_size * 0.5:  # 确保块不会太小
                    end = sentence_end + 1
            
            chunk = text[start:end].strip()
            if chunk:
                chunks.append(chunk)
            
            # 计算下一个块的起始位置（考虑重叠）
            start = max(start + self.chunk_size - self.chunk_overlap, end)
        
        return chunks
    
    def get_document_info(self, file_path: str) -> Dict[str, Any]:
        """获取文档基本信息"""
        try:
            stat = os.stat(file_path)
            return {
                'filename': os.path.basename(file_path),
                'size': stat.st_size,
                'modified_time': stat.st_mtime,
                'file_type': os.path.splitext(file_path)[1].lower()
            }
        except Exception as e:
            logger.error(f"获取文档信息失败: {str(e)}")
            return {}
