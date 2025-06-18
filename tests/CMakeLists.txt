# 测试模块CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# 启用测试
enable_testing()

# 查找Qt5测试框架
find_package(Qt5 REQUIRED COMPONENTS Test)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# 测试源文件
set(TEST_SOURCES
    test_main.cpp
    test_config_manager.cpp
    test_document_processor.cpp
    test_pdf_parser.cpp
    test_markdown_parser.cpp
    test_text_parser.cpp
    test_chunk_manager.cpp
    test_vector_store.cpp
    test_rag_engine.cpp
)

# 需要测试的源文件（不包括main.cpp）
set(SOURCE_FILES
    ../src/core/config_manager.cpp
    ../src/core/logger.cpp
    ../src/document/document_processor.cpp
    ../src/document/pdf_parser.cpp
    ../src/document/markdown_parser.cpp
    ../src/document/text_parser.cpp
    ../src/document/chunk_manager.cpp
    ../src/rag/vector_store.cpp
    ../src/rag/retriever.cpp
    ../src/rag/generator.cpp
    ../src/rag/rag_engine.cpp
    ../src/ai/kylin_ai_client.cpp
)

# 创建测试可执行文件
add_executable(test_runner
    ${TEST_SOURCES}
    ${SOURCE_FILES}
)

# 链接库
target_link_libraries(test_runner
    Qt5::Test
    Qt5::Core
    Qt5::Widgets
    ${POPPLER_LIBRARIES}
    curl
    pthread
)

# 尝试链接可选库
find_package(spdlog QUIET)
if(spdlog_FOUND)
    target_link_libraries(test_runner spdlog::spdlog)
endif()

find_library(JSONCPP_LIB jsoncpp)
if(JSONCPP_LIB)
    target_link_libraries(test_runner ${JSONCPP_LIB})
endif()

# 设置编译定义
target_compile_definitions(test_runner PRIVATE
    QT_TESTCASE_BUILDDIR="${CMAKE_CURRENT_BINARY_DIR}"
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

# 添加测试
add_test(NAME AllTests COMMAND test_runner)

# 创建测试数据目录
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# 复制测试数据文件
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test_data/)
endif()

# 设置测试环境变量
set_tests_properties(AllTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
