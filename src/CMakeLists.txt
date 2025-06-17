# 源文件列表
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
    rag/generator.cpp
    rag/rag_engine.cpp
    ai/kylin_ai_client.cpp
    ui/main_window.cpp
    ui/chat_widget.cpp
    ui/document_manager_widget.cpp
    ui/settings_dialog.cpp
    ui/voice_controller.cpp
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

# QML资源文件
qt6_add_resources(QML_RESOURCES
    ../resources/qml.qrc
)

# 创建可执行文件
qt6_add_executable(KylinQAAssistant
    ${SOURCES}
    ${HEADERS}
    ${QML_RESOURCES}
)

# 链接库
target_link_libraries(KylinQAAssistant
    Qt6::Core
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::TextToSpeech
    ${POPPLER_LIBRARIES}
    curl
    jsoncpp
    spdlog::spdlog
    pthread
)

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
