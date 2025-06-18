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

# 查找Qt（优先Qt6，备选Qt5）
find_package(Qt6 COMPONENTS Core Widgets Qml Quick QuickControls2 TextToSpeech)

if(NOT Qt6_FOUND)
    message(STATUS "Qt6未找到，尝试使用Qt5")
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Qml Quick QuickControls2 TextToSpeech)
    set(QT_VERSION_MAJOR 5)
    set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::TextToSpeech)
else()
    set(QT_VERSION_MAJOR 6)
    set(QT_LIBRARIES Qt6::Core Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::TextToSpeech)
endif()

message(STATUS "使用Qt${QT_VERSION_MAJOR}")

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
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/kylin-qa-assistant.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/kylin-qa-assistant.desktop
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kylin-qa-assistant.desktop
    DESTINATION share/applications
)

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
