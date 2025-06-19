#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
向量存储模块 - 使用简单文本匹配进行检索
"""

import os
import json
import logging
import re
from typing import List, Dict, Any

logger = logging.getLogger(__name__)

class VectorStore:
    """简化的文档存储和检索"""
    
    def __init__(self, index_file: str = "../documents.json"):
        """
        初始化文档存储
        
        Args:
            index_file: 文档存储文件路径
        """
        self.index_file = index_file
        self.documents = []  # 存储文档内容和元数据
        
        # 尝试加载已有文档
        self._load_documents()
    
    def add_documents(self, chunks: List[Dict[str, Any]], source_name: str):
        """
        添加文档块到存储
        
        Args:
            chunks: 文档块列表
            source_name: 文档来源名称
        """
        try:
            logger.info(f"开始存储文档: {source_name}, 共 {len(chunks)} 个块")
            
            # 保存文档元数据
            for i, chunk in enumerate(chunks):
                doc_metadata = chunk['metadata'].copy()
                doc_metadata['source_name'] = source_name
                doc_metadata['doc_id'] = len(self.documents)
                
                self.documents.append({
                    'text': chunk['text'],
                    'metadata': doc_metadata
                })
            
            # 保存文档
            self._save_documents()
            
            logger.info(f"文档存储完成: {source_name}")
            
        except Exception as e:
            logger.error(f"文档存储失败: {str(e)}")
            raise
    
    def search(self, query: str, k: int = 5) -> List[Dict[str, Any]]:
        """
        搜索相关文档（使用简单文本匹配）
        
        Args:
            query: 查询文本
            k: 返回结果数量
            
        Returns:
            List[Dict]: 搜索结果列表
        """
        try:
            if not self.documents:
                logger.warning("文档库为空，无法搜索")
                return []
            
            # 改进的文本匹配搜索
            query_lower = query.lower()

            # 提取关键词
            query_keywords = self._extract_keywords(query_lower)

            # 计算每个文档的相关性分数
            results = []
            for doc in self.documents:
                text_lower = doc['text'].lower()
                score = 0.0

                # 1. 完整查询匹配（最高权重）
                if query_lower in text_lower:
                    score += 2.0

                # 2. 关键词匹配
                keyword_matches = 0
                for keyword in query_keywords:
                    if keyword in text_lower:
                        keyword_matches += 1
                        score += 0.8

                # 3. 关键词覆盖率奖励
                if query_keywords and keyword_matches > 0:
                    coverage = keyword_matches / len(query_keywords)
                    score += coverage * 1.0

                # 4. 语义相关性（基于共同词汇）
                text_words = set(re.findall(r'[\u4e00-\u9fff]+|[a-zA-Z]+', text_lower))
                query_words = set(re.findall(r'[\u4e00-\u9fff]+|[a-zA-Z]+', query_lower))

                if text_words and query_words:
                    common_words = text_words.intersection(query_words)
                    if common_words:
                        semantic_score = len(common_words) / len(query_words)
                        score += semantic_score * 0.5

                # 5. 文档质量调整（优先选择较长的文档片段）
                if len(doc['text']) > 200:
                    score *= 1.1

                # 只保留有意义的匹配结果
                if score >= 0.2:  # 适中的阈值
                    doc_copy = doc.copy()
                    doc_copy['score'] = min(score, 3.0)  # 限制最高分数
                    results.append(doc_copy)
            
            # 按分数排序并返回前k个结果
            results.sort(key=lambda x: x['score'], reverse=True)
            results = results[:k]
            
            logger.info(f"搜索完成，返回 {len(results)} 个结果")
            return results
            
        except Exception as e:
            logger.error(f"搜索失败: {str(e)}")
            return []

    def _extract_keywords(self, text: str) -> List[str]:
        """提取关键词"""
        # 移除常见停用词
        stop_words = {
            '的', '了', '在', '是', '我', '有', '和', '就', '不', '人', '都', '一', '一个', '上', '也', '很', '到', '说', '要', '去', '你', '会', '着', '没有', '看', '好', '自己', '这', '什么', '如何', '怎么',
            'the', 'a', 'an', 'and', 'or', 'but', 'in', 'on', 'at', 'to', 'for', 'of', 'with', 'by', 'is', 'are', 'was', 'were', 'be', 'been', 'have', 'has', 'had', 'do', 'does', 'did', 'what', 'how'
        }

        # 智能关键词提取
        keywords = []

        # 1. 提取重要的中文词组
        important_patterns = [
            r'银河麒麟[操作]*系统',
            r'麒麟[操作]*系统',
            r'操作系统',
            r'安全[性特]*[点征]*',
            r'稳定[性特]*[点征]*',
            r'兼容[性特]*[点征]*',
            r'安装[步骤过程方法]*',
            r'CPU架构',
            r'处理器',
            r'特[点征性]',
            r'功能',
            r'优[点势]',
            r'应用',
            r'支持'
        ]

        for pattern in important_patterns:
            matches = re.findall(pattern, text)
            keywords.extend(matches)

        # 2. 按标点分割提取词汇
        words = re.split(r'[，。！？\s\u3000]+', text)
        for word in words:
            if not word:
                continue

            # 提取中文词汇（2-8个字符）
            chinese_matches = re.findall(r'[\u4e00-\u9fff]{2,8}', word)
            keywords.extend(chinese_matches)

            # 提取英文单词
            english_matches = re.findall(r'[a-zA-Z]{2,}', word)
            keywords.extend(english_matches)

        # 过滤停用词和重复词
        filtered_keywords = []
        seen = set()
        for word in keywords:
            if word not in stop_words and word not in seen and len(word) >= 2:
                filtered_keywords.append(word)
                seen.add(word)

        return filtered_keywords

    def _save_documents(self):
        """保存文档数据"""
        try:
            # 保存文档数据
            with open(self.index_file, 'w', encoding='utf-8') as f:
                json.dump(self.documents, f, ensure_ascii=False, indent=2)
            
            logger.info("文档保存成功")
            
        except Exception as e:
            logger.error(f"文档保存失败: {str(e)}")
    
    def _load_documents(self):
        """加载已有的文档数据"""
        try:
            if os.path.exists(self.index_file):
                # 加载文档数据
                with open(self.index_file, 'r', encoding='utf-8') as f:
                    self.documents = json.load(f)
                
                logger.info(f"文档加载成功，共 {len(self.documents)} 个文档")
            else:
                logger.info("未找到已有文档，将创建新存储")
                self.documents = []
                
        except Exception as e:
            logger.error(f"文档加载失败: {str(e)}")
            # 重新初始化
            self.documents = []
    
    def get_vector_count(self) -> int:
        """获取文档数量"""
        return len(self.documents)
    
    def remove_document(self, source_name: str):
        """删除指定来源的文档"""
        try:
            # 过滤掉指定来源的文档
            remaining_docs = [doc for doc in self.documents if doc['metadata'].get('source_name') != source_name]
            
            if len(remaining_docs) < len(self.documents):
                # 更新文档列表
                self.documents = remaining_docs
                
                # 保存更新后的文档
                self._save_documents()
                logger.info(f"文档删除成功: {source_name}")
            
        except Exception as e:
            logger.error(f"文档删除失败: {str(e)}")
    
    def clear_all(self):
        """清空所有数据"""
        try:
            self.documents = []
            
            # 删除文件
            if os.path.exists(self.index_file):
                os.remove(self.index_file)
            
            logger.info("所有数据已清空")
            
        except Exception as e:
            logger.error(f"清空数据失败: {str(e)}")
