cmake_minimum_required(VERSION 3.10)
project(DITTO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenCV_DIR C:/opencv/build)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
# add_executable(${PROJECT_NAME} main.cpp utils.cpp scratch.cpp)
 add_executable(${PROJECT_NAME} test.cpp utils.cpp scratch.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
