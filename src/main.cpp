#include <opencv2/opencv.hpp>
#include <vector>

#include "rga_easy_use.h"

using namespace JiuT_RGA;
using enum rga_err_e;
int main() {
  cv::Mat src = cv::imread("test.jpg");
  classRGAEasyUse rga_src;
  rga_src.init(src.cols, src.rows, rga_format_e::BGR888);
  classRGAEasyUse rga_makeBorder;
  rga_makeBorder.init(src.cols + 60, src.rows + 60, rga_format_e::BGR888);

  auto [width, height, size] = rga_makeBorder.getSize();
  rga_src.inputData(src.data, src.cols * src.rows * 3);
  makeBorder(rga_src, rga_makeBorder, 30, 30, src.cols, src.rows);
  std::vector<uint8_t> data(size);
  rga_makeBorder.getData((void*)data.data(), size);
  cv::Mat img(height, width, CV_8UC3, data.data());
  cv::imwrite("img.jpg", img);
  return 0;
}