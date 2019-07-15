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

	BitDepth _bitDepth;
	uint8_t _sampleDepth;
	uint16_t _channels;
	double _sampleRate;
	uint32_t _frames;
};

} // namespace mk
