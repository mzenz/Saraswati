#pragma once

#include <fstream>
#include <string>

namespace mk {

enum class BitDepth {
	BitDepth16 = 16,
	BitDepth24 = 24,
};

class AIFF
{
public:
	AIFF(const std::string& filePath, BitDepth bitDepth, uint16_t channels, double sampleRate);
	~AIFF();

	BitDepth bitDepth() const { return _bitDepth; }

	uint16_t channels() const { return _channels; }

	double sampleRate() const { return _sampleRate; }

	AIFF& operator<<(double sample);

private:
	std::ofstream _f;

	const BitDepth _bitDepth;
	const uint16_t _channels;
	const double _sampleRate;
	uint8_t _sampleDepth;
	uint32_t _samples;
};

} // namespace mk
