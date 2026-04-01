cmake_minimum_required(VERSION 3.16)
project(XMLToDocxConverter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 确保源文件使用 UTF-8 编码
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Xml)

# Find PkgConfig
find_package(PkgConfig)

# Find QScintilla: try pkg-config first, then manual search
if(PkgConfig_FOUND)
    pkg_check_modules(QSCINTILLA qscintilla2-qt6)
endif()
if(NOT QSCINTILLA_FOUND)
    find_path(QSCINTILLA_INCLUDE_DIR Qsci/qsciscintilla.h
        HINTS
            /opt/homebrew/include  # macOS Apple Silicon
            /usr/local/include     # macOS Intel / generic
            /usr/include
            $ENV{MSYSTEM_PREFIX}/include)
    find_library(QSCINTILLA_LIB NAMES qscintilla2_qt6
        HINTS
            /opt/homebrew/lib
            /usr/local/lib
            /usr/lib
            $ENV{MSYSTEM_PREFIX}/lib)
    if(QSCINTILLA_INCLUDE_DIR AND QSCINTILLA_LIB)
        set(QSCINTILLA_INCLUDE_DIRS ${QSCINTILLA_INCLUDE_DIR})
        set(QSCINTILLA_LIBRARIES ${QSCINTILLA_LIB})
    else()
        message(FATAL_ERROR "QScintilla2 for Qt6 not found. "
            "Install it: brew install qscintilla2 (macOS) / "
            "apt install libqscintilla2-qt6-dev (Debian/Ubuntu)")
    endif()
endif()

# Find libzip: try pkg-config first, then manual search
if(PkgConfig_FOUND)
    pkg_check_modules(LIBZIP libzip)
endif()
if(NOT LIBZIP_FOUND)
    find_path(LIBZIP_INCLUDE_DIR zip.h
        HINTS
            /opt/homebrew/include
            /usr/local/include
            /usr/include
            $ENV{MSYSTEM_PREFIX}/include)
    find_library(LIBZIP_LIB NAMES zip
        HINTS
            /opt/homebrew/lib
            /usr/local/lib
            /usr/lib
            $ENV{MSYSTEM_PREFIX}/lib)
    if(LIBZIP_INCLUDE_DIR AND LIBZIP_LIB)
        set(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR})
        set(LIBZIP_LIBRARIES ${LIBZIP_LIB})
    else()
        message(FATAL_ERROR "libzip not found. "
            "Install it: brew install libzip (macOS) / "
            "apt install libzip-dev (Debian/Ubuntu)")
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${QSCINTILLA_INCLUDE_DIRS}
    ${LIBZIP_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/XMLParser.cpp
    src/HTMLParser.cpp
    src/DOCXGenerator.cpp
    src/SampleGenerator.cpp
    src/Logger.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/XMLParser.h
    include/HTMLParser.h
    include/DOCXGenerator.h
    include/SampleGenerator.h
    include/DocumentModel.h
    include/Logger.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add library search paths from pkg-config
target_link_directories(${PROJECT_NAME} PRIVATE
    ${QSCINTILLA_LIBRARY_DIRS}
    ${LIBZIP_LIBRARY_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Xml
    ${QSCINTILLA_LIBRARIES}
    ${LIBZIP_LIBRARIES}
)

# Test runner executable
add_executable(${PROJECT_NAME}_test_runner
    src/test_runner.cpp
    src/XMLParser.cpp
    src/DOCXGenerator.cpp
    src/SampleGenerator.cpp
    src/Logger.cpp
)

target_link_libraries(${PROJECT_NAME}_test_runner
    Qt6::Core
    Qt6::Xml
    ${LIBZIP_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}_test_runner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${LIBZIP_INCLUDE_DIRS}
)

# Test target
add_custom_target(test
    COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
    COMMAND ${PROJECT_NAME}_test_runner
    DEPENDS ${PROJECT_NAME}_test_runner
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running test suite"
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY samples/ DESTINATION share/${PROJECT_NAME}/samples)
