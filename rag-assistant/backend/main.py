#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
银河麒麟智能问答助手 - FastAPI主程序
"""

from fastapi import FastAPI, File, UploadFile, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from pydantic import BaseModel
import os
import json
import logging
from typing import List, Dict, Any

from document_processor import DocumentProcessor
from vector_store import VectorStore
from rag_engine import RAGEngine

# 配置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# 创建FastAPI应用
app = FastAPI(
    title="银河麒麟智能问答助手",
    description="基于RAG技术的智能文档问答系统",
    version="1.0.0"
)

# 配置CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# 初始化组件
doc_processor = DocumentProcessor()
vector_store = VectorStore()
rag_engine = RAGEngine(vector_store)

# 数据模型
class QuestionRequest(BaseModel):
    question: str
    max_results: int = 3

class AnswerResponse(BaseModel):
    answer: str
    sources: List[Dict[str, Any]]
    confidence: float

class StatusResponse(BaseModel):
    status: str
    documents_count: int
    vector_count: int
    system_info: Dict[str, Any]

# 确保上传目录存在
UPLOAD_DIR = "uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)

@app.get("/")
async def root():
    """根路径，返回前端页面"""
    return FileResponse("../frontend/index.html")

@app.post("/upload")
async def upload_document(file: UploadFile = File(...)):
    """上传并处理文档"""
    try:
        # 检查文件类型
        if not file.filename.lower().endswith(('.pdf', '.md', '.txt')):
            raise HTTPException(status_code=400, detail="不支持的文件格式，仅支持PDF、Markdown和文本文件")
        
        # 保存文件
        file_path = os.path.join(UPLOAD_DIR, file.filename)
        with open(file_path, "wb") as buffer:
            content = await file.read()
            buffer.write(content)
        
        logger.info(f"文件已保存: {file_path}")
        
        # 处理文档
        chunks = doc_processor.process_document(file_path)
        logger.info(f"文档分块完成，共 {len(chunks)} 个块")
        
        # 添加到向量存储
        vector_store.add_documents(chunks, file.filename)
        logger.info(f"向量化完成，已添加到存储")
        
        return {
            "message": "文档上传并处理成功",
            "filename": file.filename,
            "chunks_count": len(chunks),
            "file_size": len(content)
        }
        
    except Exception as e:
        logger.error(f"文档处理失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"文档处理失败: {str(e)}")

@app.post("/ask", response_model=AnswerResponse)
async def ask_question(request: QuestionRequest):
    """提问接口"""
    try:
        if not request.question.strip():
            raise HTTPException(status_code=400, detail="问题不能为空")
        
        logger.info(f"收到问题: {request.question}")
        
        # 使用RAG引擎生成回答
        result = rag_engine.generate_answer(request.question, max_results=request.max_results)
        
        logger.info(f"回答生成完成")
        
        return AnswerResponse(
            answer=result["answer"],
            sources=result["sources"],
            confidence=result["confidence"]
        )
        
    except Exception as e:
        logger.error(f"问答处理失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"问答处理失败: {str(e)}")

@app.get("/status", response_model=StatusResponse)
async def get_status():
    """获取系统状态"""
    try:
        docs_count = len(os.listdir(UPLOAD_DIR)) if os.path.exists(UPLOAD_DIR) else 0
        vector_count = vector_store.get_vector_count()
        
        return StatusResponse(
            status="运行中",
            documents_count=docs_count,
            vector_count=vector_count,
            system_info={
                "python_version": "3.8+",
                "platform": "银河麒麟操作系统",
                "ai_sdk": "硅基流动API"
            }
        )
        
    except Exception as e:
        logger.error(f"状态查询失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"状态查询失败: {str(e)}")

if __name__ == "__main__":
    import uvicorn
    print("🚀 启动银河麒麟智能问答助手...")
    print("📖 访问地址: http://localhost:8000")
    print("📚 API文档: http://localhost:8000/docs")
    
    uvicorn.run(
        "main:app",
        host="0.0.0.0",
        port=8000,
        reload=True,
        log_level="info"
    )
