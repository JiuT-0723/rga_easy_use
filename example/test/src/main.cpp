#include <opencv2/opencv.hpp>
#include <vector>

#include "rga_easy_use.h"
int main() {
  cv::Mat src = cv::imread("test.jpg");
  JiuT_RGA::classRGAEasyUse rga_src(src.cols, src.rows, rga_format_e::BGR888);
  JiuT_RGA::classRGAEasyUse rga_makeBorder(720, 720, rga_format_e::BGR888);
  rga_src.init();
  rga_makeBorder.init();
  auto [width, height, size] = rga_makeBorder.getSize();
  rga_src.inputRGA(src.data, src.cols * src.rows * 3);
  JiuT_RGA::makeBorder(rga_src, rga_makeBorder, 30, 30, src.cols, src.rows);
  std::vector<uint8_t> data(size);
  rga_makeBorder.getBufferData((void*)data.data(), size);
  cv::Mat img(height, width, CV_8UC3, data.data());
  cv::imwrite("img.jpg", img);

  return 0;
}