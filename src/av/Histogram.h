#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace mxp {

template <typename T>
struct Histogram {
  T r[256];
  T g[256];
  T b[256];

  Histogram() {
    memset(r, 0, 256 * sizeof(T));
    memset(g, 0, 256 * sizeof(T));
    memset(b, 0, 256 * sizeof(T));
  }
};

inline void generateHistogram(const AVFrame* pFrame, Histogram<int>& histogram) {
  for(auto i = 0; i < pFrame->height; ++i) {
    auto pixelIndex = i * pFrame->linesize[0];
    for(auto j = 0; j < pFrame->width * 3; j += 3) {
      ++histogram.r[*pFrame->data[pixelIndex + j]];
      ++histogram.g[*pFrame->data[pixelIndex + j + 1]];
      ++histogram.b[*pFrame->data[pixelIndex + j + 2]];
    }
  }
}

}
