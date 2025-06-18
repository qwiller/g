# 源文件列表（仅包含已存在的文件）
set(SOURCES
    main.cpp
    core/application.cpp
    core/config_manager.cpp
    core/logger.cpp
    document/document_processor.cpp
    document/pdf_parser.cpp
    document/markdown_parser.cpp
    document/text_parser.cpp
    document/chunk_manager.cpp
    rag/vector_store.cpp
    rag/retriever.cpp
    rag/rag_engine.cpp
)

# 头文件列表
set(HEADERS
    ../include/core/application.h
    ../include/core/config_manager.h
    ../include/core/logger.h
    ../include/document/document_processor.h
    ../include/document/pdf_parser.h
    ../include/document/markdown_parser.h
    ../include/document/text_parser.h
    ../include/document/chunk_manager.h
    ../include/rag/vector_store.h
    ../include/rag/retriever.h
    ../include/rag/generator.h
    ../include/rag/rag_engine.h
    ../include/ai/kylin_ai_client.h
    ../include/ui/main_window.h
    ../include/ui/chat_widget.h
    ../include/ui/document_manager_widget.h
    ../include/ui/settings_dialog.h
    ../include/ui/voice_controller.h
)

# QML资源文件（使用Qt5）
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../resources/qml.qrc)
    qt5_add_resources(QML_RESOURCES ../resources/qml.qrc)
    add_executable(KylinQAAssistant ${SOURCES} ${HEADERS} ${QML_RESOURCES})
    message(STATUS "包含QML资源文件")
else()
    add_executable(KylinQAAssistant ${SOURCES} ${HEADERS})
    message(STATUS "未找到QML资源文件，使用基础配置")
endif()

# 链接库
target_link_libraries(KylinQAAssistant
    ${QT_LIBRARIES}
    ${POPPLER_LIBRARIES}
    curl
    jsoncpp
    pthread
)

# 尝试链接spdlog
find_package(spdlog QUIET)
if(spdlog_FOUND)
    target_link_libraries(KylinQAAssistant spdlog::spdlog)
else()
    # 如果找不到spdlog包，尝试直接链接库文件
    find_library(SPDLOG_LIB spdlog)
    if(SPDLOG_LIB)
        target_link_libraries(KylinQAAssistant ${SPDLOG_LIB})
    else()
        message(WARNING "spdlog库未找到，将使用内置日志功能")
    endif()
endif()

# 设置编译定义
target_compile_definitions(KylinQAAssistant PRIVATE
    QT_QML_DEBUG
    KYLIN_ARCH="${KYLIN_ARCH}"
)

# 设置包含目录
target_include_directories(KylinQAAssistant PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
