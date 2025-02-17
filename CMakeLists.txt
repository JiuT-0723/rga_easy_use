# 编译器路径
if($ENV{ARCH} MATCHES "x86_64")
  message(STATUS "System: $ENV{ARCH}")
  set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
  set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
endif()

cmake_minimum_required(VERSION 3.11)

project(jiut_rga VERSION 1.0.0 LANGUAGES C CXX)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(ARCHIVE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置编译器
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# submodules
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/rga_api/include
  ${CMAKE_CURRENT_SOURCE_DIR}/dma_api/include
)

# link_directories(PUBLIC ${CMAKE_CURRENT_LIST_DIR}/rga_api/lib)
add_library(${PROJECT_NAME} STATIC ${SRC_LIST}
  src/rga_task.cpp
  dma_api/dma_alloc.cpp
)

target_link_libraries(${PROJECT_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/rga_api/lib/librga.a
)