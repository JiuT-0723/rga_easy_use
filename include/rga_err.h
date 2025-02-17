/**
 * @file rga_err.h
 * @author JiuT (1094316934@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

enum class rga_err_e {
  RGA_SUCCESS = 0,
  RGA_DMA32_ALLOC_FAIL = -1,
  RGA_INPUT_FAIL = -2,
  RGA_RESIZE_FAIL = -3,
  RGA_CROP_FAIL = -4,
  RGA_MAKE_BORDER_FAIL = -5,
};