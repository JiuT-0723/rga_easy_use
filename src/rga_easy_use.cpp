/**
 * @file rga_easy_use.cpp
 * @author JiuT (1094316934@qq.com)
 * @brief rga 图像操作封装
 * @version 0.1
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "rga_easy_use.h"

#include <cstring>
#include <memory>

#include "RgaUtils.h"
#include "dma_alloc.h"
#define CLAC_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

namespace JiuT_RGA {
classRGAEasyUse::classRGAEasyUse(int width, int height, rga_format_e format) : format_(format) {
  int byte_align = 1;
  if (vtr_byte_align_16_.end() != std::find(vtr_byte_align_16_.begin(), vtr_byte_align_16_.end(), static_cast<int>(format))) {
    byte_align = 16;
  } else if (vtr_byte_align_4_.end() != std::find(vtr_byte_align_4_.begin(), vtr_byte_align_4_.end(), static_cast<int>(format))) {
    byte_align = 4;
  } else {
    throw std::invalid_argument{"This format is not supported at this time"};
  }
  width_ = CLAC_ALIGN(width, byte_align);
  height_ = CLAC_ALIGN(height, byte_align);

  int rga_format = static_cast<int>(format_);
  dma_obj_.dma_buf_size = width_ * height_ * get_bpp_from_format(rga_format);
  auto ret = dma_buf_alloc(DMA_HEAP_DMA32_UNCACHED_PATH, dma_obj_.dma_buf_size, &dma_obj_.dma_fd, (void **)&dma_obj_.dma_buf);
  if (ret < 0) {
    throw std::runtime_error{"dma_buf_alloc failed"};
  }
  rga_handle_ = importbuffer_fd(dma_obj_.dma_fd, dma_obj_.dma_buf_size);
  rga_buffer_ = wrapbuffer_handle(rga_handle_, width_, height_, rga_format);
  is_init_ = true;
}

classRGAEasyUse::~classRGAEasyUse() {
  if (rga_handle_) releasebuffer_handle(rga_handle_);
  if (dma_obj_.dma_buf) dma_buf_free(dma_obj_.dma_buf_size, &dma_obj_.dma_fd, dma_obj_.dma_buf);
}

rga_err_e classRGAEasyUse::inputData(const uint8_t *data, uint32_t size) {
  if (!is_init_) {
    throw std::runtime_error{"rga is not initialized"};
  }

  if (size > dma_obj_.dma_buf_size) {
    throw std::length_error{"The input size does not match: need <=" + std::to_string(dma_obj_.dma_buf_size) + ", but got " + std::to_string(size)};
  }
  memcpy(dma_obj_.dma_buf, data, size);
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e classRGAEasyUse::getData(void *data, uint32_t size) {
  if (!is_init_) {
    throw std::runtime_error{"rga is not initialized"};
  }

  if (size > dma_obj_.dma_buf_size) {
    throw std::length_error{"The output size does not match: need " + std::to_string(dma_obj_.dma_buf_size) + ", but got " + std::to_string(size)};
  }
  memcpy(data, dma_obj_.dma_buf, size);
  return rga_err_e::RGA_SUCCESS;
}

std::tuple<int, int, int> classRGAEasyUse::getSize() { return std::make_tuple(width_, height_, dma_obj_.dma_buf_size); }

rga_err_e resize(classRGAEasyUse &src, classRGAEasyUse &dst) {
  auto ret = imresize(src.getRGABuffer(), dst.getRGABuffer());
  if (ret < 0) {
    return rga_err_e::RGA_RESIZE_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e crop(classRGAEasyUse &src, classRGAEasyUse &dst, int x, int y, int width, int height) {
  im_rect rect = {x, y, width, height};
  auto ret = imcrop(src.getRGABuffer(), dst.getRGABuffer(), rect);
  if (ret < 0) {
    return rga_err_e::RGA_CROP_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e makeBorder(classRGAEasyUse &src, classRGAEasyUse &dst, int x, int y, int width, int height) {
  im_rect rect = {x, y, width, height};
  auto ret = improcess(src.getRGABuffer(), dst.getRGABuffer(), {}, {}, rect, {}, IM_SYNC);
  if (ret < 0) {
    return rga_err_e::RGA_MAKE_BORDER_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e rectangle(classRGAEasyUse &src, int x, int y, int width, int height, int r, int g, int b, int line_width) {
  im_rect rect = {x, y, width, height};
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imrectangle(src.getRGABuffer(), rect, color, line_width);
  if (ret < 0) {
    return rga_err_e::RGA_RECTANGLE_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

}  // namespace JiuT_RGA