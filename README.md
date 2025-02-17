
# 瑞芯微图像处理  RGA硬件加速

由于rga操作涉及硬件, 因此有对齐要求. 初始化时会对width / height 进行对其操作, 不同format对其要求不同, 详见 <https://github.com/airockchip/librga/blob/main/docs/Rockchip_Developer_Guide_RGA_CN.md>

对齐后的参数可通过 `getSize()` 获取

```cpp
#include "rga_easy_use.h"

JiuT_RGA::classRGAEasyUse rga_src(src.cols, src.rows, rga_format_e::BGR888);
auto [width, height, size] = rga_src.getSize();
```

## 当前完成功能

```Bash
  1. resize
  2. crop
  3. makeBorder
```
