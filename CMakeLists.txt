# 编译器路径
# if($ENV{ARCH} MATCHES "x86_64")
  # message(STATUS "System: $ENV{ARCH} ")
set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
# endif()

cmake_minimum_required(VERSION 3.11)

project(rga_easy_use VERSION 1.0.0 LANGUAGES C CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(ARCHIVE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置编译器
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(SYSROOT /home/jiut/develop/sysroot_3588)

include(${SYSROOT}/usr/lib/aarch64-linux-gnu/cmake/opencv4/OpenCVConfig.cmake)
link_directories(PUBLIC ${SYSROOT}/usr/lib)
link_directories(PUBLIC ${SYSROOT}/usr/lib/aarch64-linux-gnu/lapack)
link_directories(PUBLIC ${SYSROOT}/usr/lib/aarch64-linux-gnu/blas)
# submodules
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/rga_api/include
  ${CMAKE_CURRENT_SOURCE_DIR}/dma_api/include
  ${OpenCV_INCLUDE_DIRS}
)

# link_directories(PUBLIC ${CMAKE_CURRENT_LIST_DIR}/rga_api/lib)
# add_library(${PROJECT_NAME} STATIC ${SRC_LIST}
#   src/rga_easy_use.cpp
#   dma_api/dma_alloc.cpp
#   ${OpenCV_INCLUDE_DIRS}
#   src/main.cpp
# )
add_executable(${PROJECT_NAME}
  src/rga_easy_use.cpp
  dma_api/dma_alloc.cpp
  ${OpenCV_INCLUDE_DIRS}
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/rga_api/lib/librga.so
  ${OpenCV_LIBS}
  # jiut_rga
)