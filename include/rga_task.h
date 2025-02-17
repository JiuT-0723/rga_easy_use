/**
 * @file rga_task.h
 * @author JiuT (1094316934@qq.com)
 * @brief rga_task头文件
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
class classRGATask {
 public:
  classRGATask(int width, int height, rga_format_e format);
  ~classRGATask();

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

rga_err_e resize(classRGATask &src, classRGATask &dst);

rga_err_e crop(classRGATask &src, classRGATask &dst, int x, int y, int width, int height);

rga_err_e makeBorder(classRGATask &src, classRGATask &dst, int x, int y, int width, int height);
}  // namespace JiuT_RGA
