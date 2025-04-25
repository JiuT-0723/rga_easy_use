/**
 * @file rga_err.h
 * @author JiuT (1094316934@qq.com)
 * @brief
 * @version 0.2
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

enum class rga_err_e {
  RGA_SUCCESS = 0,
  RGA_FAIL = -1,
  RGA_FORMAT_UNSUPPORT = -2,
  RGA_DMA32_ALLOC_FAIL = -3,
  RGA_OVER_BG = -4,
  RGA_INPUT_DATA_NULL = -5,
  RGA_OUTPUT_CONTAINER_NULL = -6,
};
