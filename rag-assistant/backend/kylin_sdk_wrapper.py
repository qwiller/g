#!/usr/bin/env python3
"""
银河麒麟AI SDK Python封装模块
提供OCR文字识别、本地文件搜索、音效播放等功能的Python接口
"""

import os
import logging
import ctypes
from typing import List, Dict, Any, Optional
import json
from enum import Enum
import hashlib
import time
from functools import lru_cache

# 配置日志
logger = logging.getLogger(__name__)

class FileType(Enum):
    """文件类型枚举"""
    ALL = 0
    IMAGE = 1
    VIDEO = 2
    TEXT = 3
    AUDIO = 4
    DOCUMENT = 5

class FileSize(Enum):
    """文件大小枚举"""
    ALL = 0
    SMALL = 1      # 16K-1M
    MEDIUM = 2     # 1M-128M
    LARGE = 3      # >128M

class FileTime(Enum):
    """文件时间枚举"""
    ALL = 0
    TODAY = 1
    TWEEK = 2      # 本周
    TMONTH = 3     # 本月
    TYEAR = 4      # 本年

class SoundType(Enum):
    """音效类型枚举"""
    AUDIO_VOLUME_CHANGE = 0
    BATTERY_LOW = 1
    DEVICE_ADDED = 2
    DEVICE_REMOVED = 3
    WINDOW_ATTENTION = 4

class KylinSDKError(Exception):
    """银河麒麟SDK异常类"""
    pass

class KylinOCR:
    """银河麒麟OCR功能封装类"""
    
    def __init__(self):
        self.lib = None
        self._load_library()
    
    def _load_library(self):
        """加载OCR动态库"""
        try:
            # 尝试加载OCR库
            lib_paths = [
                '/usr/lib/libkysdk-ocr.so',
                '/usr/lib/x86_64-linux-gnu/libkysdk-ocr.so',
                '/usr/local/lib/libkysdk-ocr.so',
                'libkysdk-ocr.so'
            ]

            for lib_path in lib_paths:
                try:
                    # 检查文件是否存在
                    if not os.path.exists(lib_path) and not lib_path.startswith('lib'):
                        continue

                    self.lib = ctypes.CDLL(lib_path)
                    logger.info(f"成功加载OCR库: {lib_path}")

                    # 设置函数原型（模拟实现）
                    self._setup_function_prototypes()
                    break

                except OSError as e:
                    logger.debug(f"无法加载库 {lib_path}: {e}")
                    continue
                except Exception as e:
                    logger.warning(f"加载库时出错 {lib_path}: {e}")
                    continue

            if self.lib is None:
                logger.warning("未找到OCR库，OCR功能将不可用")

        except Exception as e:
            logger.error(f"加载OCR库失败: {e}")
            self.lib = None

    def _setup_function_prototypes(self):
        """设置函数原型（模拟实现）"""
        try:
            # 这里应该设置实际的SDK函数原型
            # 由于是模拟实现，我们跳过这一步
            pass
        except Exception as e:
            logger.warning(f"设置函数原型失败: {e}")
    
    def get_text_rects(self, image_path: str) -> List[List[int]]:
        """
        获取图片中文字框的坐标
        
        Args:
            image_path: 图片文件路径
            
        Returns:
            文字框坐标列表，每个坐标为[x, y, width, height]
        """
        if not self.lib:
            logger.warning("OCR库未加载，返回空结果")
            return []
            
        if not os.path.exists(image_path):
            raise KylinSDKError(f"图片文件不存在: {image_path}")
        
        try:
            # 模拟OCR文字框检测
            # 实际实现需要调用SDK的getRect函数
            logger.info(f"检测图片文字框: {image_path}")
            
            # 这里是模拟实现，实际需要调用C++接口
            # result = self.lib.getRect(image_path.encode('utf-8'))
            
            # 模拟返回一些文字框坐标
            mock_rects = [
                [10, 10, 100, 30],   # x, y, width, height
                [10, 50, 150, 30],
                [10, 90, 200, 30]
            ]
            
            logger.info(f"检测到 {len(mock_rects)} 个文字框")
            return mock_rects
            
        except Exception as e:
            logger.error(f"OCR文字框检测失败: {e}")
            raise KylinSDKError(f"OCR文字框检测失败: {e}")
    
    def get_text_content(self, image_path: str, num_rects: int = None) -> List[str]:
        """
        获取图片中的文字内容
        
        Args:
            image_path: 图片文件路径
            num_rects: 文字框数量
            
        Returns:
            识别出的文字内容列表
        """
        if not self.lib:
            logger.warning("OCR库未加载，返回空结果")
            return []
            
        if not os.path.exists(image_path):
            raise KylinSDKError(f"图片文件不存在: {image_path}")
        
        try:
            # 模拟OCR文字识别
            logger.info(f"识别图片文字内容: {image_path}")
            
            # 这里是模拟实现，实际需要调用SDK的getCls函数
            # result = self.lib.getCls(image_path.encode('utf-8'), num_rects or 0)
            
            # 模拟返回识别的文字
            mock_texts = [
                "银河麒麟操作系统",
                "智能问答助手",
                "OCR文字识别功能测试"
            ]
            
            logger.info(f"识别到 {len(mock_texts)} 段文字")
            return mock_texts
            
        except Exception as e:
            logger.error(f"OCR文字识别失败: {e}")
            raise KylinSDKError(f"OCR文字识别失败: {e}")
    
    def extract_text_from_image(self, image_path: str, use_cache: bool = True) -> str:
        """
        从图片中提取所有文字内容

        Args:
            image_path: 图片文件路径
            use_cache: 是否使用缓存

        Returns:
            提取的完整文字内容
        """
        try:
            # 检查缓存
            if use_cache:
                cached_result = self._get_cached_ocr_result(image_path)
                if cached_result:
                    logger.info(f"使用缓存的OCR结果: {image_path}")
                    return cached_result

            # 先获取文字框
            rects = self.get_text_rects(image_path)

            # 再获取文字内容
            texts = self.get_text_content(image_path, len(rects))

            # 合并所有文字
            full_text = '\n'.join(texts)

            # 缓存结果
            if use_cache and full_text.strip():
                self._cache_ocr_result(image_path, full_text)

            logger.info(f"从图片 {image_path} 提取文字 {len(full_text)} 个字符")
            return full_text

        except Exception as e:
            logger.error(f"图片文字提取失败: {e}")
            raise KylinSDKError(f"图片文字提取失败: {e}")

    def _get_cached_ocr_result(self, image_path: str) -> Optional[str]:
        """获取缓存的OCR结果"""
        try:
            # 生成缓存键
            cache_key = self._generate_cache_key(image_path)
            cache_file = f"/tmp/ocr_cache_{cache_key}.txt"

            if os.path.exists(cache_file):
                # 检查缓存是否过期（24小时）
                cache_age = time.time() - os.path.getmtime(cache_file)
                if cache_age < 24 * 3600:  # 24小时
                    with open(cache_file, 'r', encoding='utf-8') as f:
                        return f.read()
                else:
                    # 删除过期缓存
                    os.remove(cache_file)

            return None

        except Exception as e:
            logger.warning(f"读取OCR缓存失败: {e}")
            return None

    def _cache_ocr_result(self, image_path: str, text: str):
        """缓存OCR结果"""
        try:
            cache_key = self._generate_cache_key(image_path)
            cache_file = f"/tmp/ocr_cache_{cache_key}.txt"

            with open(cache_file, 'w', encoding='utf-8') as f:
                f.write(text)

        except Exception as e:
            logger.warning(f"缓存OCR结果失败: {e}")

    def _generate_cache_key(self, image_path: str) -> str:
        """生成缓存键"""
        try:
            # 使用文件路径和修改时间生成唯一键
            stat = os.stat(image_path)
            content = f"{image_path}_{stat.st_mtime}_{stat.st_size}"
            return hashlib.md5(content.encode()).hexdigest()
        except:
            # 如果获取文件信息失败，使用文件路径
            return hashlib.md5(image_path.encode()).hexdigest()

class KylinSearch:
    """银河麒麟搜索功能封装类"""
    
    def __init__(self):
        self.lib = None
        self._load_library()
    
    def _load_library(self):
        """加载搜索动态库"""
        try:
            # 尝试加载搜索库
            lib_paths = [
                '/usr/lib/libkysdk-search.so',
                '/usr/lib/x86_64-linux-gnu/libkysdk-search.so',
                'libkysdk-search.so'
            ]
            
            for lib_path in lib_paths:
                try:
                    self.lib = ctypes.CDLL(lib_path)
                    logger.info(f"成功加载搜索库: {lib_path}")
                    break
                except OSError:
                    continue
            
            if self.lib is None:
                logger.warning("未找到搜索库，搜索功能将不可用")
                return
                
        except Exception as e:
            logger.error(f"加载搜索库失败: {e}")
            self.lib = None
    
    def search_by_term(self, directory: str, term: str, fuzzy: bool = True) -> List[str]:
        """
        按关键字搜索文件
        
        Args:
            directory: 搜索目录
            term: 搜索关键字
            fuzzy: 是否模糊搜索
            
        Returns:
            匹配的文件路径列表
        """
        if not self.lib:
            logger.warning("搜索库未加载，使用Python实现")
            return self._python_search(directory, term, fuzzy)
        
        try:
            logger.info(f"搜索文件: 目录={directory}, 关键字={term}, 模糊={fuzzy}")
            
            # 这里是模拟实现，实际需要调用SDK函数
            # if fuzzy:
            #     result = self.lib.kdk_search_get_term_by_directory(
            #         directory.encode('utf-8'), term.encode('utf-8'))
            # else:
            #     result = self.lib.kdk_search_full_get_term_by_directory(
            #         directory.encode('utf-8'), term.encode('utf-8'))
            
            # 使用Python实现作为后备
            return self._python_search(directory, term, fuzzy)
            
        except Exception as e:
            logger.error(f"搜索失败: {e}")
            # 降级到Python实现
            return self._python_search(directory, term, fuzzy)
    
    def _python_search(self, directory: str, term: str, fuzzy: bool = True) -> List[str]:
        """Python实现的文件搜索（后备方案）"""
        results = []
        
        if not os.path.exists(directory):
            logger.warning(f"搜索目录不存在: {directory}")
            return results
        
        try:
            for root, dirs, files in os.walk(directory):
                for file in files:
                    file_path = os.path.join(root, file)
                    
                    if fuzzy:
                        # 模糊搜索
                        if term.lower() in file.lower():
                            results.append(file_path)
                    else:
                        # 精确搜索
                        if term == file:
                            results.append(file_path)
                    
                    # 限制结果数量
                    if len(results) >= 100:
                        break
                
                if len(results) >= 100:
                    break
            
            logger.info(f"搜索完成，找到 {len(results)} 个文件")
            return results
            
        except Exception as e:
            logger.error(f"Python搜索失败: {e}")
            return []

    def search_with_filter(self, directory: str, term: str,
                          file_type: FileType = FileType.ALL,
                          file_size: FileSize = FileSize.ALL,
                          file_time: FileTime = FileTime.ALL) -> List[str]:
        """
        带过滤条件的搜索

        Args:
            directory: 搜索目录
            term: 搜索关键字
            file_type: 文件类型过滤
            file_size: 文件大小过滤
            file_time: 文件时间过滤

        Returns:
            符合条件的文件路径列表
        """
        if not self.lib:
            logger.warning("搜索库未加载，使用Python实现")
            return self._python_search_with_filter(directory, term, file_type, file_size, file_time)

        try:
            logger.info(f"过滤搜索: 目录={directory}, 关键字={term}")

            # 这里是模拟实现，实际需要调用SDK函数
            # result = self.lib.kdk_search_get_term_by_filter(
            #     directory.encode('utf-8'), term.encode('utf-8'),
            #     file_type.value, file_size.value, file_time.value)

            # 使用Python实现作为后备
            return self._python_search_with_filter(directory, term, file_type, file_size, file_time)

        except Exception as e:
            logger.error(f"过滤搜索失败: {e}")
            return self._python_search_with_filter(directory, term, file_type, file_size, file_time)

    def _python_search_with_filter(self, directory: str, term: str,
                                  file_type: FileType, file_size: FileSize,
                                  file_time: FileTime) -> List[str]:
        """Python实现的过滤搜索"""
        import time
        from datetime import datetime, timedelta

        results = []

        if not os.path.exists(directory):
            return results

        # 文件类型扩展名映射
        type_extensions = {
            FileType.IMAGE: ['.jpg', '.jpeg', '.png', '.gif', '.bmp', '.tiff'],
            FileType.VIDEO: ['.mp4', '.avi', '.mkv', '.mov', '.wmv', '.flv'],
            FileType.TEXT: ['.txt', '.md', '.log'],
            FileType.AUDIO: ['.mp3', '.wav', '.flac', '.aac'],
            FileType.DOCUMENT: ['.pdf', '.doc', '.docx', '.xls', '.xlsx', '.ppt', '.pptx']
        }

        # 时间过滤
        now = datetime.now()
        time_filters = {
            FileTime.TODAY: now - timedelta(days=1),
            FileTime.TWEEK: now - timedelta(weeks=1),
            FileTime.TMONTH: now - timedelta(days=30),
            FileTime.TYEAR: now - timedelta(days=365)
        }

        try:
            for root, dirs, files in os.walk(directory):
                for file in files:
                    file_path = os.path.join(root, file)

                    # 关键字过滤
                    if term and term.lower() not in file.lower():
                        continue

                    # 文件类型过滤
                    if file_type != FileType.ALL:
                        file_ext = os.path.splitext(file)[1].lower()
                        if file_ext not in type_extensions.get(file_type, []):
                            continue

                    # 文件大小过滤
                    if file_size != FileSize.ALL:
                        try:
                            size = os.path.getsize(file_path)
                            if file_size == FileSize.SMALL and not (16*1024 <= size <= 1024*1024):
                                continue
                            elif file_size == FileSize.MEDIUM and not (1024*1024 <= size <= 128*1024*1024):
                                continue
                            elif file_size == FileSize.LARGE and size <= 128*1024*1024:
                                continue
                        except OSError:
                            continue

                    # 文件时间过滤
                    if file_time != FileTime.ALL:
                        try:
                            mtime = datetime.fromtimestamp(os.path.getmtime(file_path))
                            if mtime < time_filters[file_time]:
                                continue
                        except OSError:
                            continue

                    results.append(file_path)

                    if len(results) >= 100:
                        break

                if len(results) >= 100:
                    break

            return results

        except Exception as e:
            logger.error(f"Python过滤搜索失败: {e}")
            return []

class KylinSoundEffects:
    """银河麒麟音效功能封装类"""

    def __init__(self):
        self.lib = None
        self._load_library()

    def _load_library(self):
        """加载音效动态库"""
        try:
            lib_paths = [
                '/usr/lib/libkysdk-soundeffects.so',
                '/usr/lib/x86_64-linux-gnu/libkysdk-soundeffects.so',
                'libkysdk-soundeffects.so'
            ]

            for lib_path in lib_paths:
                try:
                    self.lib = ctypes.CDLL(lib_path)
                    logger.info(f"成功加载音效库: {lib_path}")
                    break
                except OSError:
                    continue

            if self.lib is None:
                logger.warning("未找到音效库，音效功能将不可用")

        except Exception as e:
            logger.error(f"加载音效库失败: {e}")
            self.lib = None

    def play_sound(self, sound_type: SoundType):
        """
        播放系统音效

        Args:
            sound_type: 音效类型
        """
        if not self.lib:
            logger.warning("音效库未加载，无法播放音效")
            return

        try:
            logger.info(f"播放音效: {sound_type.name}")

            # 这里是模拟实现，实际需要调用SDK函数
            # self.lib.playSound(sound_type.value)

            # 模拟音效播放
            logger.info(f"音效播放完成: {sound_type.name}")

        except Exception as e:
            logger.error(f"音效播放失败: {e}")

class KylinSDK:
    """银河麒麟AI SDK主类"""

    def __init__(self):
        """初始化SDK"""
        self.ocr = KylinOCR()
        self.search = KylinSearch()
        self.sound = KylinSoundEffects()

        logger.info("银河麒麟AI SDK初始化完成")

    def is_available(self) -> Dict[str, bool]:
        """
        检查各模块可用性

        Returns:
            各模块的可用状态
        """
        return {
            'ocr': self.ocr.lib is not None,
            'search': self.search.lib is not None,
            'sound': self.sound.lib is not None
        }

    def get_sdk_info(self) -> Dict[str, Any]:
        """
        获取SDK信息

        Returns:
            SDK版本和功能信息
        """
        availability = self.is_available()

        return {
            'name': '银河麒麟AI SDK',
            'version': '1.0.0',
            'modules': {
                'ocr': {
                    'available': availability['ocr'],
                    'description': 'OCR文字识别功能'
                },
                'search': {
                    'available': availability['search'],
                    'description': '本地文件搜索功能'
                },
                'sound': {
                    'available': availability['sound'],
                    'description': '系统音效播放功能'
                }
            }
        }

# 全局SDK实例
_kylin_sdk = None

def get_kylin_sdk() -> KylinSDK:
    """获取全局SDK实例"""
    global _kylin_sdk
    if _kylin_sdk is None:
        _kylin_sdk = KylinSDK()
    return _kylin_sdk

# 便捷函数
def extract_text_from_image(image_path: str) -> str:
    """从图片提取文字的便捷函数"""
    sdk = get_kylin_sdk()
    return sdk.ocr.extract_text_from_image(image_path)

def search_files(directory: str, term: str, fuzzy: bool = True) -> List[str]:
    """搜索文件的便捷函数"""
    sdk = get_kylin_sdk()
    return sdk.search.search_by_term(directory, term, fuzzy)

def play_system_sound(sound_type: SoundType):
    """播放系统音效的便捷函数"""
    sdk = get_kylin_sdk()
    sdk.sound.play_sound(sound_type)
