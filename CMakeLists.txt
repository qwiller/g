cmake_minimum_required(VERSION 3.16)
project(KylinQAAssistant VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 检测麒麟系统架构
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(KYLIN_ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(KYLIN_ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "mips64")
    set(KYLIN_ARCH "mips64")
else()
    message(FATAL_ERROR "不支持的架构: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "检测到麒麟系统架构: ${KYLIN_ARCH}")

# 查找Qt5（专为麒麟系统优化）
message(STATUS "配置Qt5开发环境（麒麟系统专用）")

# 查找Qt5核心组件
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# 查找Qt5可选组件
find_package(Qt5 COMPONENTS Qml Quick QuickControls2 TextToSpeech QUIET)

# 设置Qt版本
set(QT_VERSION_MAJOR 5)

# 设置Qt5库列表
set(QT_LIBRARIES Qt5::Core Qt5::Widgets)

# 添加可选组件
if(Qt5Qml_FOUND)
    list(APPEND QT_LIBRARIES Qt5::Qml)
    message(STATUS "Qt5 Qml: 已找到")
else()
    message(WARNING "Qt5 Qml: 未找到，QML功能将被禁用")
endif()

if(Qt5Quick_FOUND)
    list(APPEND QT_LIBRARIES Qt5::Quick)
    message(STATUS "Qt5 Quick: 已找到")
else()
    message(WARNING "Qt5 Quick: 未找到，Quick功能将被禁用")
endif()

if(Qt5QuickControls2_FOUND)
    list(APPEND QT_LIBRARIES Qt5::QuickControls2)
    message(STATUS "Qt5 QuickControls2: 已找到")
else()
    message(WARNING "Qt5 QuickControls2: 未找到，部分UI功能可能受限")
endif()

if(Qt5TextToSpeech_FOUND)
    list(APPEND QT_LIBRARIES Qt5::TextToSpeech)
    message(STATUS "Qt5 TextToSpeech: 已找到")
else()
    message(WARNING "Qt5 TextToSpeech: 未找到，语音功能将被禁用")
endif()

message(STATUS "使用Qt5 (版本: ${Qt5Core_VERSION})")
message(STATUS "Qt5安装路径: ${Qt5Core_DIR}")

# 查找其他依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler-cpp)

# 设置包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${POPPLER_INCLUDE_DIRS}
)

# 设置库目录
link_directories(${POPPLER_LIBRARY_DIRS})

# 添加子目录
add_subdirectory(src)

# 添加测试（可选）
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# 设置安装目录
set(CMAKE_INSTALL_PREFIX "/opt/kylin-qa-assistant")

# 安装规则
install(TARGETS KylinQAAssistant
    RUNTIME DESTINATION bin
)

install(DIRECTORY resources/
    DESTINATION share/kylin-qa-assistant/resources
)

install(FILES config/app_config.json
    DESTINATION etc/kylin-qa-assistant
)

# 创建桌面文件
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources/kylin-qa-assistant.desktop.in)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/kylin-qa-assistant.desktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/kylin-qa-assistant.desktop
        @ONLY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kylin-qa-assistant.desktop
        DESTINATION share/applications
    )
else()
    message(WARNING "桌面文件模板不存在，跳过桌面文件创建")
endif()

# CPack配置
set(CPACK_PACKAGE_NAME "kylin-qa-assistant")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "银河麒麟智能问答助手")
set(CPACK_PACKAGE_VENDOR "麒麟软件")
set(CPACK_GENERATOR "DEB;RPM")

# DEB包配置
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-base-dev, libpoppler-cpp-dev, libspdlog-dev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "麒麟软件 <support@kylinos.cn>")

# RPM包配置
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase-devel, poppler-cpp-devel, spdlog-devel")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")

include(CPack)
