cmake_minimum_required(VERSION 3.20)
project(demo VERSION 0.1.0 LANGUAGES C CXX)

# 将poco作为子目录添加进工程
add_subdirectory(poco)

# 构建二进制应用
add_executable(demo main.cpp)

# 添加Poco::Net依赖
target_link_libraries(demo PRIVATE Poco::Net)


