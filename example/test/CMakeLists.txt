
project(test VERSION 1.0.0 LANGUAGES C CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(ARCHIVE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include(${CROSS_AARCH64_DIR}/lib/aarch64-linux-gnu/cmake/opencv4/OpenCVConfig.cmake)

# find_package(OpenCV REQUIRED)

# submodules
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../rga_api/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../dma_api/include
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
  src/main.cpp)

target_link_libraries(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libjiut_rga.a
  ${OpenCV_LIBS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../rga_api/lib/librga.a
)