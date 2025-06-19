#!/usr/bin/env python3
"""
AI配置管理模块
支持本地AI（银河麒麟SDK）和远程AI服务的配置和切换
"""

import os
import json
import logging
from typing import Dict, Any, Optional
from enum import Enum

logger = logging.getLogger(__name__)

class AIProvider(Enum):
    """AI服务提供商枚举"""
    KYLIN_SDK = "kylin_sdk"      # 银河麒麟本地SDK
    SILICONFLOW = "siliconflow"  # 硅基流动API
    MIXED = "mixed"              # 混合模式

class AIConfig:
    """AI配置管理类"""
    
    def __init__(self, config_file: str = "config/ai_settings.json"):
        """
        初始化AI配置
        
        Args:
            config_file: 配置文件路径
        """
        self.config_file = config_file
        self.config = self._load_config()
        
    def _load_config(self) -> Dict[str, Any]:
        """加载配置文件"""
        default_config = {
            "ai_provider": AIProvider.MIXED.value,
            "kylin_sdk": {
                "enabled": True,
                "ocr_enabled": True,
                "search_enabled": True,
                "sound_enabled": True,
                "search_directories": ["/home", "/opt", "/usr/share/doc"]
            },
            "siliconflow": {
                "enabled": True,
                "api_key": "sk-owsayozifrzvaxuxvyvywmyzcceokwatdbolevdnfnbwlurp",
                "api_base": "https://api.siliconflow.cn/v1",
                "model": "deepseek-ai/DeepSeek-V3",
                "max_tokens": 2000,
                "temperature": 0.7
            },
            "rag_settings": {
                "chunk_size": 500,
                "chunk_overlap": 50,
                "max_search_results": 3,
                "enable_local_search": True,
                "confidence_threshold": 0.3
            }
        }
        
        try:
            if os.path.exists(self.config_file):
                with open(self.config_file, 'r', encoding='utf-8') as f:
                    loaded_config = json.load(f)
                    # 合并默认配置和加载的配置
                    return self._merge_config(default_config, loaded_config)
            else:
                # 创建默认配置文件
                self._save_config(default_config)
                return default_config
                
        except Exception as e:
            logger.error(f"加载配置文件失败: {e}")
            return default_config
    
    def _merge_config(self, default: Dict[str, Any], loaded: Dict[str, Any]) -> Dict[str, Any]:
        """合并配置"""
        result = default.copy()
        for key, value in loaded.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key].update(value)
            else:
                result[key] = value
        return result
    
    def _save_config(self, config: Dict[str, Any] = None):
        """保存配置文件"""
        try:
            config_to_save = config or self.config
            
            # 确保配置目录存在
            os.makedirs(os.path.dirname(self.config_file), exist_ok=True)
            
            with open(self.config_file, 'w', encoding='utf-8') as f:
                json.dump(config_to_save, f, indent=2, ensure_ascii=False)
                
            logger.info(f"配置已保存到: {self.config_file}")
            
        except Exception as e:
            logger.error(f"保存配置文件失败: {e}")
    
    def get_ai_provider(self) -> AIProvider:
        """获取当前AI服务提供商"""
        provider_str = self.config.get("ai_provider", AIProvider.MIXED.value)
        try:
            return AIProvider(provider_str)
        except ValueError:
            logger.warning(f"无效的AI提供商: {provider_str}，使用默认值")
            return AIProvider.MIXED
    
    def set_ai_provider(self, provider: AIProvider):
        """设置AI服务提供商"""
        self.config["ai_provider"] = provider.value
        self._save_config()
        logger.info(f"AI服务提供商已设置为: {provider.value}")
    
    def is_kylin_sdk_enabled(self) -> bool:
        """检查银河麒麟SDK是否启用"""
        return self.config.get("kylin_sdk", {}).get("enabled", True)
    
    def is_siliconflow_enabled(self) -> bool:
        """检查硅基流动API是否启用"""
        return self.config.get("siliconflow", {}).get("enabled", True)
    
    def get_kylin_sdk_config(self) -> Dict[str, Any]:
        """获取银河麒麟SDK配置"""
        return self.config.get("kylin_sdk", {})
    
    def get_siliconflow_config(self) -> Dict[str, Any]:
        """获取硅基流动API配置"""
        return self.config.get("siliconflow", {})
    
    def get_rag_settings(self) -> Dict[str, Any]:
        """获取RAG设置"""
        return self.config.get("rag_settings", {})
    
    def update_kylin_sdk_config(self, **kwargs):
        """更新银河麒麟SDK配置"""
        if "kylin_sdk" not in self.config:
            self.config["kylin_sdk"] = {}
        
        self.config["kylin_sdk"].update(kwargs)
        self._save_config()
        logger.info("银河麒麟SDK配置已更新")
    
    def update_siliconflow_config(self, **kwargs):
        """更新硅基流动API配置"""
        if "siliconflow" not in self.config:
            self.config["siliconflow"] = {}
        
        self.config["siliconflow"].update(kwargs)
        self._save_config()
        logger.info("硅基流动API配置已更新")
    
    def update_rag_settings(self, **kwargs):
        """更新RAG设置"""
        if "rag_settings" not in self.config:
            self.config["rag_settings"] = {}
        
        self.config["rag_settings"].update(kwargs)
        self._save_config()
        logger.info("RAG设置已更新")
    
    def get_search_directories(self) -> list:
        """获取搜索目录列表"""
        return self.config.get("kylin_sdk", {}).get("search_directories", ["/home"])
    
    def add_search_directory(self, directory: str):
        """添加搜索目录"""
        directories = self.get_search_directories()
        if directory not in directories:
            directories.append(directory)
            self.update_kylin_sdk_config(search_directories=directories)
    
    def remove_search_directory(self, directory: str):
        """移除搜索目录"""
        directories = self.get_search_directories()
        if directory in directories:
            directories.remove(directory)
            self.update_kylin_sdk_config(search_directories=directories)
    
    def get_config_summary(self) -> Dict[str, Any]:
        """获取配置摘要"""
        return {
            "ai_provider": self.get_ai_provider().value,
            "kylin_sdk_enabled": self.is_kylin_sdk_enabled(),
            "siliconflow_enabled": self.is_siliconflow_enabled(),
            "ocr_enabled": self.config.get("kylin_sdk", {}).get("ocr_enabled", False),
            "search_enabled": self.config.get("kylin_sdk", {}).get("search_enabled", False),
            "sound_enabled": self.config.get("kylin_sdk", {}).get("sound_enabled", False),
            "search_directories_count": len(self.get_search_directories()),
            "rag_settings": self.get_rag_settings()
        }
    
    def reset_to_defaults(self):
        """重置为默认配置"""
        self.config = self._load_config()
        logger.info("配置已重置为默认值")

# 全局配置实例
_ai_config = None

def get_ai_config() -> AIConfig:
    """获取全局AI配置实例"""
    global _ai_config
    if _ai_config is None:
        _ai_config = AIConfig()
    return _ai_config

def reload_ai_config():
    """重新加载AI配置"""
    global _ai_config
    _ai_config = None
    return get_ai_config()

# 便捷函数
def get_current_ai_provider() -> AIProvider:
    """获取当前AI服务提供商"""
    return get_ai_config().get_ai_provider()

def is_kylin_sdk_available() -> bool:
    """检查银河麒麟SDK是否可用"""
    config = get_ai_config()
    if not config.is_kylin_sdk_enabled():
        return False
    
    try:
        # 尝试导入SDK
        from .kylin_sdk_wrapper import get_kylin_sdk
        sdk = get_kylin_sdk()
        return any(sdk.is_available().values())
    except ImportError:
        return False
    except Exception as e:
        logger.warning(f"检查SDK可用性失败: {e}")
        return False

def should_use_local_search() -> bool:
    """是否应该使用本地搜索"""
    config = get_ai_config()
    provider = config.get_ai_provider()
    
    if provider == AIProvider.KYLIN_SDK:
        return True
    elif provider == AIProvider.MIXED:
        return config.get_rag_settings().get("enable_local_search", True)
    else:
        return False
