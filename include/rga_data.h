/**
 * @file rga_data.h
 * @author JiuT (1094316934@qq.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "rga.h"

class DmaObject {
 public:
  int dma_fd;
  size_t dma_buf_size;
  char *dma_buf;
};

class DrmObject {
 public:
  int drm_buffer_fd;
  int drm_buffer_handle;
  size_t actual_size;
  uint8_t *drm_buf;
};

enum class rga_format_e {
  BGR888 = RK_FORMAT_BGR_888,
  RBG888 = RK_FORMAT_RGB_888,
  BGRA8888 = RK_FORMAT_BGRA_8888,
  RGBA8888 = RK_FORMAT_RGBA_8888,
};