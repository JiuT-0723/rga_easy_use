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
inline static int __clip(int x, int width, int max) { return (x + width) < max ? width : max - x; }

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

rga_err_e crop(classRGAEasyUse &src, classRGAEasyUse &dst, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  im_rect rect = {x, y, width, height};
  auto ret = imcrop(src.getRGABuffer(), dst.getRGABuffer(), rect);
  if (ret < 0) {
    return rga_err_e::RGA_CROP_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e makeBorder(classRGAEasyUse &src, classRGAEasyUse &dst, uint16_t left, uint16_t upper) {
  auto [fg_width, fg_height, fg_size] = src.getSize();
  auto [bg_width, bg_height, bg_size] = dst.getSize();
  if (left > bg_width || upper > bg_height) {
    return rga_err_e::RGA_MAKE_BORDER_FAIL;
  }
  im_rect rect = {left, upper, fg_width, fg_height};
  auto ret = improcess(src.getRGABuffer(), dst.getRGABuffer(), {}, {}, rect, {}, IM_SYNC);
  if (ret < 0) {
    return rga_err_e::RGA_MAKE_BORDER_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e rectangle(classRGAEasyUse &src, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b, uint8_t line_width) {
  auto [img_width, img_height, img_size] = src.getSize();
  im_rect rect = {x, y, __clip(x, width, img_width), __clip(y, height, img_height)};
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imrectangle(src.getRGABuffer(), rect, color, line_width);
  if (ret < 0) {
    return rga_err_e::RGA_RECTANGLE_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e rectangleGroup(classRGAEasyUse &src, std::vector<std::array<uint16_t, 4>> &point_group, uint8_t r, uint8_t g, uint8_t b, uint8_t line_width) {
  auto [img_width, img_height, img_size] = src.getSize();
  std::vector<im_rect> rects;
  rects.reserve(point_group.size());
  int size = point_group.size();
  rects.reserve(img_size);
  for (auto &point : point_group) {
    if (point[0] > img_width || point[1] > img_height) {
      size--;
      continue;
    }
    im_rect rect = {point[0], point[1], __clip(point[0], point[2], img_width), __clip(point[1], point[3], img_height)};
    rects.insert(rects.end(), rect);
  }
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imrectangleArray(src.getRGABuffer(), rects.data(), size, color, line_width);
  if (ret < 0) {
    return rga_err_e::RGA_RECTANGLE_GROUP_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e rotate(classRGAEasyUse &src, classRGAEasyUse &dst, rga_rotate_e degree) {
  auto ret = imrotate(src.getRGABuffer(), dst.getRGABuffer(), static_cast<int>(degree));
  if (ret < 0) {
    return rga_err_e::RGA_ROTATE_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e filp(classRGAEasyUse &src, classRGAEasyUse &dst, rga_flip_e flip) {
  auto ret = imflip(src.getRGABuffer(), dst.getRGABuffer(), static_cast<int>(flip));
  if (ret < 0) {
    return rga_err_e::RGA_FLIP_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e fill(classRGAEasyUse &src, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b) {
  auto [img_width, img_height, img_size] = src.getSize();
  if (x > img_width || y > img_height) {
    return rga_err_e::RGA_FILL_FAIL;
  }
  im_rect rect = {x, y, __clip(x, width, img_width), __clip(y, height, img_height)};
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imfill(src.getRGABuffer(), rect, color);
  if (ret < 0) {
    return rga_err_e::RGA_FILL_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e fillGroup(classRGAEasyUse &src, std::vector<std::array<uint16_t, 4>> &point_group, uint8_t r, uint8_t g, uint8_t b) {
  auto [img_width, img_height, img_size] = src.getSize();
  std::vector<im_rect> rects;
  int size = point_group.size();
  rects.reserve(img_size);
  for (auto &point : point_group) {
    if (point[0] > img_width || point[1] > img_height) {
      size--;
      continue;
    }
    im_rect rect = {point[0], point[1], __clip(point[0], point[2], img_width), __clip(point[1], point[3], img_height)};
    rects.insert(rects.end(), rect);
  }
  uint32_t color = (b << 16) | (g << 8) | r;
  auto ret = imfillArray(src.getRGABuffer(), rects.data(), size, color);
  if (ret < 0) {
    return rga_err_e::RGA_FILL_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

}  // namespace JiuT_RGA