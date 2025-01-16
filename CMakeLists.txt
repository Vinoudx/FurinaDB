# CMakeList.txt: furinadb 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories("include/database" "include/sqlcompiler" "include" "gui")
set(CMAKE_PREFIX_PATH "D:/work/qt/6.8.1/msvc2022_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/furinadb/include/database/*.h" "${CMAKE_SOURCE_DIR}/furinadb/include/sqlcompiler/*.h" "${CMAKE_SOURCE_DIR}/furinadb/gui/*.h")

set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/furinadb/src/database/bufferpool.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/database/config.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/database/database.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/database/logger.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/database/page.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/database/table.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/furinadb.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/sqlcompiler/sqlmanager.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/gui/DatabaseDetailWindow.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/gui/LoginWindow.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/gui/MainWindow.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/gui/Table.cpp
    ${CMAKE_SOURCE_DIR}/furinadb/src/md5.cpp
)



# 将源代码添加到此项目的可执行文件。
add_executable (furinadb "main.cpp" ${SOURCE_FILES})

target_include_directories(furinadb PRIVATE ${CMAKE_SOURCE_DIR}/furinadb/include/database ${CMAKE_SOURCE_DIR}/furinadb/include/sqlcompiler ${CMAKE_SOURCE_DIR}/furinadb/gui)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET furinadb PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries("furinadb" PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

