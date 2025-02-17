/**
 * @file rga_task.h
 * @author JiuT (1094316934@qq.com)
 * @brief rga 图像操作封装
 * @version 0.1
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <vector>

#include "im2d.h"
#include "rga_data.h"
#include "rga_err.h"

namespace JiuT_RGA {
class classRGAEasyUse {
 public:
  classRGAEasyUse(int width, int height, rga_format_e format);
  ~classRGAEasyUse();

  rga_err_e init();
  rga_err_e inputRGA(const uint8_t *data, uint32_t size);

  rga_buffer_t &getRGABuffer() { return rga_buffer_; }

  rga_err_e getBufferData(void *data, uint32_t size);

  std::tuple<int, int, int> getSize();

 private:
  int width_, height_;
  rga_format_e format_;
  DmaObject dma_obj_;
  DrmObject drm_obj_;
  rga_buffer_t rga_buffer_ = {0};
  rga_buffer_handle_t rga_handle_ = 0;

  std::vector<int> vtr_byte_align_16_ = {RK_FORMAT_BGR_888, RK_FORMAT_RGB_888};
  std::vector<int> vtr_byte_align_4_ = {RK_FORMAT_BGRA_8888, RK_FORMAT_RGBA_8888};
};
rga_err_e resize(classRGAEasyUse &src, classRGAEasyUse &dst);
rga_err_e crop(classRGAEasyUse &src, classRGAEasyUse &dst, int x, int y, int width, int height);
rga_err_e makeBorder(classRGAEasyUse &src, classRGAEasyUse &dst, int x, int y, int width, int height);
}  // namespace JiuT_RGA
