#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RAG引擎 - 检索增强生成
"""

import logging
import requests
import json
from typing import List, Dict, Any
from vector_store import VectorStore

logger = logging.getLogger(__name__)

class RAGEngine:
    """RAG引擎，结合检索和生成"""
    
    def __init__(self, vector_store: VectorStore):
        """
        初始化RAG引擎
        
        Args:
            vector_store: 向量存储实例
        """
        self.vector_store = vector_store
        
        # 硅基流动API配置
        self.api_key = "sk-owsayozifrzvaxuxvyvywmyzcceokwatdbolevdnfnbwlurp"
        self.api_base = "https://api.siliconflow.cn/v1"
        self.model = "deepseek-ai/DeepSeek-V3"
        
        # 提示词模板
        self.system_prompt = """你是银河麒麟操作系统的智能助手，专门回答与麒麟系统相关的问题。

请基于提供的文档内容回答用户问题，要求：
1. 回答要准确、专业、有条理
2. 优先使用提供的文档内容
3. 如果文档内容不足，可以结合你的知识补充
4. 明确指出信息来源
5. 使用中文回答

文档内容：
{context}

用户问题：{question}

请提供详细的回答："""
    
    def generate_answer(self, question: str, max_results: int = 3) -> Dict[str, Any]:
        """
        生成问题的回答
        
        Args:
            question: 用户问题
            max_results: 最大检索结果数
            
        Returns:
            Dict: 包含answer、sources、confidence的结果
        """
        try:
            logger.info(f"开始处理问题: {question}")
            
            # 1. 检索相关文档
            search_results = self.vector_store.search(question, k=max_results)
            
            if not search_results:
                return {
                    "answer": "抱歉，我没有找到相关的文档内容来回答您的问题。请尝试上传相关文档或换个问题。",
                    "sources": [],
                    "confidence": 0.0
                }
            
            # 2. 构建上下文
            context = self._build_context(search_results)
            
            # 3. 生成回答
            answer = self._generate_with_llm(question, context)
            
            # 4. 计算置信度
            confidence = self._calculate_confidence(search_results)
            
            # 5. 整理来源信息
            sources = self._format_sources(search_results)
            
            result = {
                "answer": answer,
                "sources": sources,
                "confidence": confidence
            }
            
            logger.info("问题处理完成")
            return result
            
        except Exception as e:
            logger.error(f"RAG处理失败: {str(e)}")
            return {
                "answer": f"处理问题时出现错误: {str(e)}",
                "sources": [],
                "confidence": 0.0
            }
    
    def _build_context(self, search_results: List[Dict[str, Any]]) -> str:
        """构建上下文文本"""
        context_parts = []
        
        for i, result in enumerate(search_results, 1):
            text = result['text']
            source = result['metadata'].get('source', '未知来源')
            
            context_parts.append(f"文档片段 {i} (来源: {source}):\n{text}\n")
        
        return "\n".join(context_parts)
    
    def _generate_with_llm(self, question: str, context: str) -> str:
        """使用大语言模型生成回答"""
        try:
            # 构建提示词
            prompt = self.system_prompt.format(context=context, question=question)
            
            # 调用硅基流动API
            headers = {
                "Authorization": f"Bearer {self.api_key}",
                "Content-Type": "application/json"
            }
            
            data = {
                "model": self.model,
                "messages": [
                    {
                        "role": "user",
                        "content": prompt
                    }
                ],
                "max_tokens": 2000,
                "temperature": 0.7,
                "stream": False
            }
            
            logger.info("调用LLM API...")
            response = requests.post(
                f"{self.api_base}/chat/completions",
                headers=headers,
                json=data,
                timeout=30
            )
            
            if response.status_code == 200:
                result = response.json()
                answer = result['choices'][0]['message']['content']
                logger.info("LLM回答生成成功")
                return answer.strip()
            else:
                logger.error(f"API调用失败: {response.status_code}, {response.text}")
                return self._fallback_answer(question, context)
                
        except Exception as e:
            logger.error(f"LLM调用失败: {str(e)}")
            return self._fallback_answer(question, context)
    
    def _fallback_answer(self, question: str, context: str) -> str:
        """备选回答方案（当API不可用时）"""
        # 简单的关键词匹配回答
        context_lower = context.lower()
        question_lower = question.lower()
        
        # 提取关键词
        keywords = [word for word in question_lower.split() if len(word) > 1]
        
        # 查找相关句子
        sentences = context.split('。')
        relevant_sentences = []
        
        for sentence in sentences:
            sentence_lower = sentence.lower()
            if any(keyword in sentence_lower for keyword in keywords):
                relevant_sentences.append(sentence.strip())
        
        if relevant_sentences:
            answer = "根据文档内容，我找到以下相关信息：\n\n"
            answer += "\n".join(f"• {sentence}" for sentence in relevant_sentences[:3])
            answer += "\n\n请注意：这是基于关键词匹配的简化回答，建议查看完整文档获取更准确信息。"
        else:
            answer = "抱歉，在提供的文档中没有找到直接相关的信息。建议您：\n"
            answer += "1. 尝试使用不同的关键词重新提问\n"
            answer += "2. 上传更多相关文档\n"
            answer += "3. 查看文档的完整内容"
        
        return answer
    
    def _calculate_confidence(self, search_results: List[Dict[str, Any]]) -> float:
        """计算回答的置信度"""
        if not search_results:
            return 0.0
        
        # 基于搜索结果的分数计算置信度
        scores = [result.get('score', 0.0) for result in search_results]
        avg_score = sum(scores) / len(scores)
        
        # 归一化到0-1范围
        confidence = min(max(avg_score, 0.0), 1.0)
        
        # 如果有多个高质量结果，提高置信度
        high_quality_count = sum(1 for score in scores if score > 0.7)
        if high_quality_count >= 2:
            confidence = min(confidence * 1.2, 1.0)
        
        return round(confidence, 2)
    
    def _format_sources(self, search_results: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """格式化来源信息"""
        sources = []
        
        for result in search_results:
            metadata = result['metadata']
            source_info = {
                'source_name': metadata.get('source_name', '未知来源'),
                'chunk_id': metadata.get('chunk_id', 0),
                'score': result.get('score', 0.0),
                'text_preview': result['text'][:200] + "..." if len(result['text']) > 200 else result['text']
            }
            sources.append(source_info)
        
        return sources
