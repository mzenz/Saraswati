#pragma once

#include <algorithm>

namespace mk {

template<class T>
T clamp(T n, T min, T max) { return std::min(std::max(n, min), max); }

float amplitude2dB(float amplitude);

float dB2Amplitude(float loudness);

void audioToText(const std::string& audioFilePath, const std::string& textFilePath);

void normalize(const std::string& inputFilePath, const std::string& outputFilePath, float peakValue = 0.0);

} // namespace mk
