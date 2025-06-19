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
import shutil
from typing import List, Dict, Any

from document_processor import DocumentProcessor
from vector_store import VectorStore
from rag_engine import RAGEngine

# 导入银河麒麟SDK
try:
    from kylin_sdk_wrapper import get_kylin_sdk, KylinSDKError, SoundType
    KYLIN_SDK_AVAILABLE = True
except ImportError:
    KYLIN_SDK_AVAILABLE = False
    logging.warning("银河麒麟SDK不可用，部分功能将受限")

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

class ClearDataResponse(BaseModel):
    message: str
    cleared_files: int
    cleared_vectors: int

class SDKInfoResponse(BaseModel):
    sdk_available: bool
    sdk_info: Dict[str, Any]
    modules: Dict[str, bool]

class OCRRequest(BaseModel):
    image_path: str

class OCRResponse(BaseModel):
    success: bool
    text: str
    error: str = None

class LocalSearchRequest(BaseModel):
    query: str
    directory: str = "/home"
    max_results: int = 10

class LocalSearchResponse(BaseModel):
    success: bool
    results: List[str]
    count: int
    error: str = None



# 确保上传目录存在
UPLOAD_DIR = "uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)

@app.get("/")
async def root():
    """根路径，返回前端页面"""
    # 获取当前文件的目录，然后构建前端文件的绝对路径
    current_dir = os.path.dirname(os.path.abspath(__file__))
    frontend_path = os.path.join(os.path.dirname(current_dir), "frontend", "index.html")
    return FileResponse(frontend_path)

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

        # 获取已加载文档列表
        loaded_documents = []
        if os.path.exists(UPLOAD_DIR):
            for filename in os.listdir(UPLOAD_DIR):
                if os.path.isfile(os.path.join(UPLOAD_DIR, filename)):
                    loaded_documents.append(filename)

        return StatusResponse(
            status="运行中",
            documents_count=docs_count,
            vector_count=vector_count,
            system_info={
                "python_version": "3.8+",
                "platform": "银河麒麟操作系统",
                "loaded_documents": loaded_documents,
                "documents_summary": f"{len(loaded_documents)}个文档" if loaded_documents else "暂无文档",
                "kylin_sdk_available": KYLIN_SDK_AVAILABLE,
                "sdk_modules": _get_sdk_modules_status() if KYLIN_SDK_AVAILABLE else {}
            }
        )
        
    except Exception as e:
        logger.error(f"状态查询失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"状态查询失败: {str(e)}")

@app.delete("/clear-data", response_model=ClearDataResponse)
async def clear_all_data():
    """清空所有数据"""
    try:
        logger.info("开始清空所有数据...")

        # 统计清空前的数据
        files_count = len(os.listdir(UPLOAD_DIR)) if os.path.exists(UPLOAD_DIR) else 0
        vectors_count = vector_store.get_vector_count()

        # 清空向量存储
        vector_store.clear_all()
        logger.info("向量数据已清空")

        # 清空上传文件目录
        if os.path.exists(UPLOAD_DIR):
            shutil.rmtree(UPLOAD_DIR)
            os.makedirs(UPLOAD_DIR, exist_ok=True)
            logger.info("上传文件已清空")

        logger.info(f"数据清空完成，清空了 {files_count} 个文件和 {vectors_count} 个向量")

        return ClearDataResponse(
            message="所有数据已成功清空",
            cleared_files=files_count,
            cleared_vectors=vectors_count
        )

    except Exception as e:
        logger.error(f"数据清空失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"数据清空失败: {str(e)}")

def _get_sdk_modules_status() -> Dict[str, bool]:
    """获取SDK模块状态"""
    if not KYLIN_SDK_AVAILABLE:
        return {}

    try:
        sdk = get_kylin_sdk()
        return sdk.is_available()
    except Exception as e:
        logger.error(f"获取SDK状态失败: {e}")
        return {}

@app.get("/sdk-info", response_model=SDKInfoResponse)
async def get_sdk_info():
    """获取银河麒麟SDK信息"""
    try:
        if not KYLIN_SDK_AVAILABLE:
            return SDKInfoResponse(
                sdk_available=False,
                sdk_info={"message": "银河麒麟SDK不可用"},
                modules={}
            )

        sdk = get_kylin_sdk()
        sdk_info = sdk.get_sdk_info()
        modules_status = sdk.is_available()

        return SDKInfoResponse(
            sdk_available=True,
            sdk_info=sdk_info,
            modules=modules_status
        )

    except Exception as e:
        logger.error(f"获取SDK信息失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"获取SDK信息失败: {str(e)}")

@app.post("/ocr", response_model=OCRResponse)
async def ocr_extract_text(request: OCRRequest):
    """OCR文字识别接口"""
    try:
        if not KYLIN_SDK_AVAILABLE:
            return OCRResponse(
                success=False,
                text="",
                error="银河麒麟SDK不可用"
            )

        if not os.path.exists(request.image_path):
            return OCRResponse(
                success=False,
                text="",
                error=f"图片文件不存在: {request.image_path}"
            )

        sdk = get_kylin_sdk()
        text = sdk.ocr.extract_text_from_image(request.image_path)

        return OCRResponse(
            success=True,
            text=text,
            error=None
        )

    except Exception as e:
        logger.error(f"OCR识别失败: {str(e)}")
        return OCRResponse(
            success=False,
            text="",
            error=str(e)
        )

@app.post("/local-search", response_model=LocalSearchResponse)
async def local_file_search(request: LocalSearchRequest):
    """本地文件搜索接口"""
    try:
        if not KYLIN_SDK_AVAILABLE:
            return LocalSearchResponse(
                success=False,
                results=[],
                count=0,
                error="银河麒麟SDK不可用"
            )

        sdk = get_kylin_sdk()
        results = sdk.search.search_by_term(
            directory=request.directory,
            term=request.query,
            fuzzy=True
        )

        # 限制结果数量
        limited_results = results[:request.max_results]

        return LocalSearchResponse(
            success=True,
            results=limited_results,
            count=len(limited_results),
            error=None
        )

    except Exception as e:
        logger.error(f"本地搜索失败: {str(e)}")
        return LocalSearchResponse(
            success=False,
            results=[],
            count=0,
            error=str(e)
        )

@app.post("/play-sound/{sound_type}")
async def play_system_sound(sound_type: str):
    """播放系统音效接口"""
    try:
        if not KYLIN_SDK_AVAILABLE:
            raise HTTPException(status_code=503, detail="银河麒麟SDK不可用")

        # 音效类型映射
        sound_map = {
            "volume_change": SoundType.AUDIO_VOLUME_CHANGE,
            "battery_low": SoundType.BATTERY_LOW,
            "device_added": SoundType.DEVICE_ADDED,
            "device_removed": SoundType.DEVICE_REMOVED,
            "window_attention": SoundType.WINDOW_ATTENTION
        }

        if sound_type not in sound_map:
            raise HTTPException(status_code=400, detail=f"不支持的音效类型: {sound_type}")

        sdk = get_kylin_sdk()
        sdk.sound.play_sound(sound_map[sound_type])

        return {"message": f"音效播放成功: {sound_type}"}

    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"音效播放失败: {str(e)}")
        raise HTTPException(status_code=500, detail=f"音效播放失败: {str(e)}")

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
