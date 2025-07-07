cmake_minimum_required(VERSION 3.31)
project(ParkingManagementSystem_cProject)

set(CMAKE_CXX_STANDARD 20)

# 查找GTK3包
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# 包含GTK3头文件目录
include_directories(${GTK3_INCLUDE_DIRS})

# 添加编译选项和链接选项
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# 添加可执行文件
add_executable(ParkingManagementSystem_cProject main.cpp)

# 链接GTK3库
target_link_libraries(ParkingManagementSystem_cProject ${GTK3_LIBRARIES})
