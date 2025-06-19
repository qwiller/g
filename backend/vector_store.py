#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
向量存储模块 - 使用简单文本匹配进行检索
"""

import os
import json
import logging
import re
from typing import List, Dict, Any, Tuple
import numpy as np

logger = logging.getLogger(__name__)

class VectorStore:
    """简化的文档存储和检索"""

    def __init__(self, index_file: str = "documents.json"):
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

            # 简单的关键词匹配搜索
            query_lower = query.lower()
            query_words = set(re.findall(r'\w+', query_lower))

            # 计算每个文档的相关性分数
            results = []
            for doc in self.documents:
                text_lower = doc['text'].lower()
                text_words = set(re.findall(r'\w+', text_lower))

                # 计算词汇重叠度
                common_words = query_words.intersection(text_words)
                if common_words:
                    # 简单的TF-IDF风格评分
                    score = len(common_words) / len(query_words)

                    # 如果查询词在文档中连续出现，给予额外分数
                    if query_lower in text_lower:
                        score += 0.5

                    doc_copy = doc.copy()
                    doc_copy['score'] = score
                    results.append(doc_copy)

            # 按分数排序并返回前k个结果
            results.sort(key=lambda x: x['score'], reverse=True)
            results = results[:k]

            logger.info(f"搜索完成，返回 {len(results)} 个结果")
            return results

        except Exception as e:
            logger.error(f"搜索失败: {str(e)}")
            return []

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
