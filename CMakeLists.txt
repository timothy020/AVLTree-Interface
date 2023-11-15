# CMakeList.txt: YJS 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project ("YJS")

# Enable Hot Reload for MSVC compilers if supported.


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(common1 STATIC 
    "src/struct/YList.cpp"
    "src/struct/BPlusTree.cpp"
    "src/struct/YListNew.cpp"
    "src/struct/AVLTree.cpp"
)


add_library(common2 STATIC 
    "src/server/doc.cpp"
    "src/server/util.cpp"
)

target_link_libraries(common2 common1)

# 将源代码添加到此项目的可执行文件。
add_executable (YJS "test/YJS.cpp")

target_link_libraries(YJS common2)

# TODO: 如有需要，请添加测试并安装目标。

set(SRC_LIST ./src/struct/test.cpp ./src/struct/AVLTree.cpp)
add_executable (test ${SRC_LIST})