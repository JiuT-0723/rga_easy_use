#include "rga_task.h"

#include <cstring>
#include <memory>

#include "RgaUtils.h"
#include "dma_alloc.h"
#define CLAC_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

namespace JiuT_RGA {
classRGATask::classRGATask(int width, int height, rga_format_e format) : format_(format) {
  int byte_align = 1;
  if (vtr_byte_align_16_.end() != std::find(vtr_byte_align_16_.begin(), vtr_byte_align_16_.end(), static_cast<int>(format))) {
    byte_align = 16;
  }
  if (vtr_byte_align_4_.end() != std::find(vtr_byte_align_4_.begin(), vtr_byte_align_4_.end(), static_cast<int>(format))) {
    byte_align = 4;
  }
  width_ = CLAC_ALIGN(width, byte_align);
  height_ = CLAC_ALIGN(height, byte_align);
  printf("width_: %d, height_: %d\n", width_, height_);
}

classRGATask::~classRGATask() {
  if (rga_handle_) releasebuffer_handle(rga_handle_);
  if (dma_obj_.dma_buf) dma_buf_free(dma_obj_.dma_buf_size, &dma_obj_.dma_fd, dma_obj_.dma_buf);
}
rga_err_e classRGATask::init() {
  int rga_format = static_cast<int>(format_);
  dma_obj_.dma_buf_size = width_ * height_ * get_bpp_from_format(rga_format);
  printf("get_bpp_from_format(rga_format): %f\n", get_bpp_from_format(rga_format));
  auto ret = dma_buf_alloc(DMA_HEAP_DMA32_UNCACHED_PATH, dma_obj_.dma_buf_size, &dma_obj_.dma_fd, (void **)&dma_obj_.dma_buf);
  if (ret < 0) {
    return rga_err_e::RGA_DMA32_ALLOC_FAIL;
  }
  rga_handle_ = importbuffer_fd(dma_obj_.dma_fd, dma_obj_.dma_buf_size);
  rga_buffer_ = wrapbuffer_handle(rga_handle_, width_, height_, rga_format);
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e classRGATask::inputRGA(const uint8_t *data, uint32_t size) {
  if (size != dma_obj_.dma_buf_size) {
    throw std::length_error{"The input size does not match: need " + std::to_string(dma_obj_.dma_buf_size) + ", but got " + std::to_string(size)};
  }
  memcpy(dma_obj_.dma_buf, data, size);
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e classRGATask::getBufferData(void *data, uint32_t size) {
  if (size != dma_obj_.dma_buf_size) {
    throw std::length_error{"The output size does not match: need " + std::to_string(dma_obj_.dma_buf_size) + ", but got " + std::to_string(size)};
  }
  memcpy(data, dma_obj_.dma_buf, size);
  return rga_err_e::RGA_SUCCESS;
}

std::tuple<int, int, int> classRGATask::getSize() { return std::make_tuple(width_, height_, dma_obj_.dma_buf_size); }

rga_err_e resize(classRGATask &src, classRGATask &dst) {
  auto ret = imresize(src.getRGABuffer(), dst.getRGABuffer());
  if (ret < 0) {
    return rga_err_e::RGA_RESIZE_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e crop(classRGATask &src, classRGATask &dst, int x, int y, int width, int height) {
  im_rect rect = {x, y, width, height};
  auto ret = imcrop(src.getRGABuffer(), dst.getRGABuffer(), rect);
  if (ret < 0) {
    return rga_err_e::RGA_CROP_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}

rga_err_e makeBorder(classRGATask &src, classRGATask &dst, int x, int y, int width, int height) {
  im_rect rect = {x, y, width, height};
  auto ret = improcess(src.getRGABuffer(), dst.getRGABuffer(), {}, {}, rect, {}, IM_SYNC);
  if (ret < 0) {
    return rga_err_e::RGA_MAKE_BORDER_FAIL;
  }
  return rga_err_e::RGA_SUCCESS;
}
}  // namespace JiuT_RGA