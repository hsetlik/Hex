#include "HexHeader.h"

namespace SampleRate {
static std::atomic<double> dSampleRate = 44100.0;
void set(double sr) {
  dSampleRate.store(sr);
}
double get() {
  return dSampleRate.load();
}
}  // namespace SampleRate
