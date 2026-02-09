cmake_minimum_required(VERSION 3.16)
project(XMLToDocxConverter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Xml)

# Find QScintilla
find_package(PkgConfig REQUIRED)
pkg_check_modules(QSCINTILLA REQUIRED qscintilla2-qt6)

# Find libzip
pkg_check_modules(LIBZIP REQUIRED libzip)

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

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Xml
    ${QSCINTILLA_LIBRARIES}
    ${LIBZIP_LIBRARIES}
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY samples/ DESTINATION share/${PROJECT_NAME}/samples)
