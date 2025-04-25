/**
 * @file rga_task.h
 * @author JiuT (1094316934@qq.com)
 * @brief rga 图像操作封装
 * @version 0.2
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <array>
#include <vector>

#include "im2d.h"
#include "rga_data.h"
#include "rga_err.h"
namespace JiuT_RGA {
class classRGAEasyUse {
 public:
  classRGAEasyUse();
  ~classRGAEasyUse();

  rga_err_e init(int fd, int width, int height, rga_format_e format);
  rga_err_e init(int width, int height, rga_format_e format);
  rga_err_e inputData(const void *data, uint32_t size);

  rga_buffer_t &getRGABuffer() { return rga_buffer_; }
  rga_err_e getData(void *data, uint32_t size);

  std::tuple<int, int, int> getSize();

 private:
  int width_, height_;
  DmaObject dma_obj_;
  DrmObject drm_obj_;
  rga_buffer_t rga_buffer_ = {0};
  rga_buffer_handle_t rga_handle_ = 0;
  int fd_ = -1;
  bool is_init_ = false;

  std::vector<int> vtr_byte_align_8_ = {RK_FORMAT_BGR_888, RK_FORMAT_RGB_888};
  std::vector<int> vtr_byte_align_4_ = {RK_FORMAT_BGRA_8888, RK_FORMAT_RGBA_8888, RK_FORMAT_YCbCr_420_SP, RK_FORMAT_YCbCr_422_SP};
};
/**
 * @brief 缩放
 *
 * @param src
 * @param dst
 * @return rga_err_e
 */
rga_err_e resize(classRGAEasyUse &src, classRGAEasyUse &dst);

/**
 * @brief 裁剪
 *
 * @param src 输入图像
 * @param dst 输出图像
 * @param x 左上角 x 坐标
 * @param y 左上角 y 坐标
 * @param width 裁剪区域宽度
 * @param height 裁剪区域高度
 * @return rga_err_e
 */
rga_err_e crop(classRGAEasyUse &src, classRGAEasyUse &dst, int x, int y, int width, int height);

/**
 * @brief 扩展边框
 *
 * @param src 输入图像
 * @param dst 输出图像
 * @param left 左边框宽度
 * @param upper 上边框宽度
 * @param width 输入图像宽度
 * @param height 输入图像高度
 * @return rga_err_e
 */
rga_err_e makeBorder(classRGAEasyUse &src, classRGAEasyUse &dst, int left, int upper, int width, int height);

/**
 * @brief 绘制矩形
 *
 * @param src 输入图像
 * @param x 矩形框左上角 x 坐标
 * @param y 矩形框左上角 y 坐标
 * @param width 矩形框宽度
 * @param height 矩形框高度
 * @param r 矩形框颜色 r 通道
 * @param g 矩形框颜色 g 通道
 * @param b 矩形框颜色 b 通道
 * @param line_width 线条宽度
 * @return rga_err_e
 */
rga_err_e rectangle(classRGAEasyUse &src, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t line_width = 2);

/**
 * @brief 绘制一组矩形框
 *
 * @param src 输入图像
 * @param point_group 矩形框组 [x, y, width, height]
 * @param r 矩形框颜色 r 通道
 * @param g 矩形框颜色 g 通道
 * @param b 矩形框颜色 b 通道
 * @param line_width 线条宽度
 * @return rga_err_e
 */
rga_err_e rectangleGroup(classRGAEasyUse &src, std::vector<std::array<int, 4>> &point_group, uint8_t r, uint8_t g, uint8_t b, uint8_t line_width = 2);

/**
 * @brief 旋转
 *
 * @param src 输入图像
 * @param dst 输出图像
 * @param degree 旋转角度 90, 180, 270
 * @return rga_err_e
 */
rga_err_e rotate(classRGAEasyUse &src, classRGAEasyUse &dst, rga_rotate_e degree);

/**
 * @brief 镜像翻转
 *
 * @param src 输入图像
 * @param dst 输出图像
 * @param flip 翻转方式 0: 水平翻转, 1: 垂直翻转, 2: 水平垂直翻转
 * @return rga_err_e
 */
rga_err_e filp(classRGAEasyUse &src, classRGAEasyUse &dst, rga_flip_e flip);

/**
 * @brief 指定区域颜色填充 （矩形）
 *
 * @param src 输入图像
 * @param x
 * @param y
 * @param width
 * @param height
 * @param r
 * @param g
 * @param b
 * @return rga_err_e
 */
rga_err_e fill(classRGAEasyUse &src, int x, int y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief 指定区域颜色填充 （矩形组）
 *
 * @param src
 * @param point_group
 * @param r
 * @param g
 * @param b
 * @return rga_err_e
 */
rga_err_e fillGroup(classRGAEasyUse &src, std::vector<std::array<int, 4>> &point_group, uint8_t r, uint8_t g, uint8_t b);

}  // namespace JiuT_RGA
