#pragma once

namespace mxp {

template <typename T>
struct Histogram {
  constexpr static const size_t max_index = 256;

  T r[max_index];
  T g[max_index];
  T b[max_index];

  Histogram() {
    memset(r, 0, max_index * sizeof(T));
    memset(g, 0, max_index * sizeof(T));
    memset(b, 0, max_index * sizeof(T));
  }
};

using HistogramPtr = std::shared_ptr<Histogram<uint8_t>>;

inline std::vector<std::tuple<float, size_t>> FindRMSD(const std::vector<HistogramPtr>& histograms) {
  Histogram<float> avgHistogram;
  auto maxIndex = Histogram<uint8_t>::max_index;

  for(auto i = 0; i < histograms.size(); ++i) {
    for(auto j = 0; j < maxIndex - 1; ++j) {
      avgHistogram.r[j] += static_cast<float>(histograms[i]->r[j]) / histograms.size();
      avgHistogram.g[j] += static_cast<float>(histograms[i]->g[j]) / histograms.size();
      avgHistogram.b[j] += static_cast<float>(histograms[i]->b[j]) / histograms.size();
    }
  }

  std::vector<std::tuple<float, size_t>> result;
  for(auto i = 0; i < histograms.size(); ++i) {
    float rmse = 0.0;

    // Calculate root mean squared error
    for(auto j = 0; j < maxIndex - 1; ++j) {
      auto error = fabsf(avgHistogram.r[j] - histograms[i]->r[j])
        + fabsf(avgHistogram.g[j] - histograms[i]->g[j])
        + fabsf(avgHistogram.b[j] - histograms[i]->b[j]);
      rmse += (error * error) / 255;
    }

    result.push_back({ sqrtf(rmse), i });
  }

  return result;
}

}
