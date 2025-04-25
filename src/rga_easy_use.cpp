/**
 * @file rga_easy_use.cpp
 * @author JiuT (1094316934@qq.com)
 * @brief rga 图像操作封装
 * @version 0.2
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

#define NUM_UP(x, y) (x > y ? x : y)
using enum rga_err_e;
namespace JiuT_RGA {
inline static int __clip(int x, int width, int max) { return (x + width) < max ? width : max - x; }

classRGAEasyUse::classRGAEasyUse() {}

classRGAEasyUse::~classRGAEasyUse() {
  if (rga_handle_) releasebuffer_handle(rga_handle_);
  if (dma_obj_.dma_buf) dma_buf_free(dma_obj_.dma_buf_size, &dma_obj_.dma_fd, dma_obj_.dma_buf);
  is_init_ = false;
}

rga_err_e classRGAEasyUse::init(int width, int height, rga_format_e format) {
  int byte_align = 1;
  if (vtr_byte_align_8_.end() != std::find(vtr_byte_align_8_.begin(), vtr_byte_align_8_.end(), static_cast<int>(format))) {
    byte_align = 8;
  } else if (vtr_byte_align_4_.end() != std::find(vtr_byte_align_4_.begin(), vtr_byte_align_4_.end(), static_cast<int>(format))) {
    byte_align = 4;
  } else {
    return RGA_FORMAT_UNSUPPORT;
  }
  width_ = CLAC_ALIGN(width, byte_align);
  height_ = CLAC_ALIGN(height, byte_align);

  int rga_format = static_cast<int>(format);
  dma_obj_.dma_buf_size = width_ * height_ * get_bpp_from_format(rga_format);
  auto ret = dma_buf_alloc(DMA_HEAP_DMA32_UNCACHED_PATH, dma_obj_.dma_buf_size, &dma_obj_.dma_fd, (void **)&dma_obj_.dma_buf);
  if (ret < 0 || dma_obj_.dma_buf == nullptr) {
    return RGA_DMA32_ALLOC_FAIL;
  }

  memset(dma_obj_.dma_buf, 0, dma_obj_.dma_buf_size);

  rga_handle_ = importbuffer_fd(dma_obj_.dma_fd, dma_obj_.dma_buf_size);
  rga_buffer_ = wrapbuffer_handle(rga_handle_, width_, height_, rga_format);
  is_init_ = true;
  return RGA_SUCCESS;
}

rga_err_e classRGAEasyUse::init(int fd, int width, int height, rga_format_e format) {
  int byte_align = 1;
  if (vtr_byte_align_8_.end() != std::find(vtr_byte_align_8_.begin(), vtr_byte_align_8_.end(), static_cast<int>(format))) {
    byte_align = 8;
  } else if (vtr_byte_align_4_.end() != std::find(vtr_byte_align_4_.begin(), vtr_byte_align_4_.end(), static_cast<int>(format))) {
    byte_align = 4;
  } else {
    return RGA_FORMAT_UNSUPPORT;
  }
  width_ = CLAC_ALIGN(width, byte_align);
  height_ = CLAC_ALIGN(height, byte_align);

  int rga_format = static_cast<int>(format);
  dma_obj_.dma_buf_size = width_ * height_ * get_bpp_from_format(rga_format);
  auto ret = dma_buf_alloc(DMA_HEAP_DMA32_UNCACHED_PATH, dma_obj_.dma_buf_size, &fd, (void **)&dma_obj_.dma_buf);
  if (ret < 0) {
    return RGA_DMA32_ALLOC_FAIL;
  }

  memset(dma_obj_.dma_buf, 0, dma_obj_.dma_buf_size);

  rga_handle_ = importbuffer_fd(fd, dma_obj_.dma_buf_size);
  rga_buffer_ = wrapbuffer_handle(rga_handle_, width_, height_, rga_format);
  is_init_ = true;
  return RGA_SUCCESS;
}

rga_err_e classRGAEasyUse::inputData(const void *data, uint32_t size) {
  if (!is_init_) {
    return RGA_FAIL;
  }
  if (size > dma_obj_.dma_buf_size) {
    return RGA_FAIL;
  }
  if (data == nullptr) {
    return RGA_INPUT_DATA_NULL;
  }
  memcpy(dma_obj_.dma_buf, data, size);
  return RGA_SUCCESS;
}

rga_err_e classRGAEasyUse::getData(void *data, uint32_t size) {
  if (!is_init_) {
    return RGA_FAIL;
  }
  if (size > dma_obj_.dma_buf_size) {
    return RGA_FAIL;
  }
  if (data == nullptr) {
    return RGA_OUTPUT_CONTAINER_NULL;
  }
  memcpy(data, dma_obj_.dma_buf, size);
  return RGA_SUCCESS;
}

std::tuple<int, int, int> classRGAEasyUse::getSize() { return std::make_tuple(width_, height_, dma_obj_.dma_buf_size); }

rga_err_e resize(classRGAEasyUse &src, classRGAEasyUse &dst) {
  auto ret = imresize(src.getRGABuffer(), dst.getRGABuffer());
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e crop(classRGAEasyUse &src, classRGAEasyUse &dst, int x, int y, int width, int height) {
  im_rect rect = {NUM_UP(x, 0), NUM_UP(y, 0), width, height};
  auto ret = imcrop(src.getRGABuffer(), dst.getRGABuffer(), rect);
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e makeBorder(classRGAEasyUse &src, classRGAEasyUse &dst, int left, int upper, int width, int height) {
  auto [fg_width, fg_height, fg_size] = src.getSize();
  auto [bg_width, bg_height, bg_size] = dst.getSize();
  if (left > bg_width || upper > bg_height) {
    return RGA_OVER_BG;
  }
  im_rect rect = {NUM_UP(left, 0), NUM_UP(upper, 0), width, height};
  auto ret = improcess(src.getRGABuffer(), dst.getRGABuffer(), {}, {}, rect, {}, IM_SYNC);
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e rectangle(classRGAEasyUse &src, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t line_width) {
  auto [img_width, img_height, img_size] = src.getSize();
  im_rect rect = {NUM_UP(x, line_width), NUM_UP(y, line_width), __clip(x, width, img_width), __clip(y, height, img_height)};
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imrectangle(src.getRGABuffer(), rect, color, line_width);
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e rectangleGroup(classRGAEasyUse &src, std::vector<std::array<int, 4>> &point_group, uint8_t r, uint8_t g, uint8_t b, uint8_t line_width) {
  auto [img_width, img_height, img_size] = src.getSize();
  std::vector<im_rect> rects;
  int size = point_group.size();
  rects.reserve(size);
  for (auto &point : point_group) {
    if (point[0] > img_width || point[1] > img_height) {
      size--;
      continue;
    }
    im_rect rect = {NUM_UP(point[0], line_width), NUM_UP(point[1], line_width), __clip(point[0], point[2], img_width), __clip(point[1], point[3], img_height)};
    rects.insert(rects.end(), rect);
  }
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imrectangleArray(src.getRGABuffer(), rects.data(), size, color, line_width);
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e rotate(classRGAEasyUse &src, classRGAEasyUse &dst, rga_rotate_e degree) {
  auto ret = imrotate(src.getRGABuffer(), dst.getRGABuffer(), static_cast<int>(degree));
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e filp(classRGAEasyUse &src, classRGAEasyUse &dst, rga_flip_e flip) {
  auto ret = imflip(src.getRGABuffer(), dst.getRGABuffer(), static_cast<int>(flip));
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e fill(classRGAEasyUse &src, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
  auto [img_width, img_height, img_size] = src.getSize();
  if (x > img_width || y > img_height) {
    return RGA_OVER_BG;
  }
  im_rect rect = {NUM_UP(x, 0), NUM_UP(y, 0), __clip(x, width, img_width), __clip(y, height, img_height)};
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imfill(src.getRGABuffer(), rect, color);
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

rga_err_e fillGroup(classRGAEasyUse &src, std::vector<std::array<int, 4>> &point_group, uint8_t r, uint8_t g, uint8_t b) {
  auto [img_width, img_height, img_size] = src.getSize();
  std::vector<im_rect> rects;
  int size = point_group.size();
  rects.reserve(size);
  for (auto &point : point_group) {
    if (point[0] > img_width || point[1] > img_height) {
      size--;
      continue;
    }
    im_rect rect = {NUM_UP(point[0], 0), NUM_UP(point[1], 0), __clip(point[0], point[2], img_width), __clip(point[1], point[3], img_height)};
    rects.insert(rects.end(), rect);
  }
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imfillArray(src.getRGABuffer(), rects.data(), size, color);
  if (ret < 0) {
    return RGA_FAIL;
  }
  return RGA_SUCCESS;
}

}  // namespace JiuT_RGA