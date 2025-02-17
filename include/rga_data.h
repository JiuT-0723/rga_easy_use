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