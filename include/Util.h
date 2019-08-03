#pragma once

#include <algorithm>
#include <string>

namespace mk {

struct SampleInfo {
	SampleInfo();

	double loudness() const;

	uint32_t frame;
	uint16_t channel;
	double amplitude;
};

template<class T> T clamp(T n, T min, T max) { return std::min(std::max(n, min), max); }

double amplitudeToLoudness(double amplitude);

double loudnessToAmplitude(double loudness);

bool audioToText(const std::string& audioFilePath,
				 const std::string& textFilePath);

bool scanMax(const std::string& inputFilePath, SampleInfo& max);

bool normalize(const std::string& inputFilePath,
			   const std::string& outputFilePath,
			   float peakLoudness = 0.0);

bool amplify(const std::string& inputFilePath,
			 const std::string& outputFilePath,
			 float gain);

} // namespace mk
