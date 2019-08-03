#include "Util.h"
#include <fstream>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include <cmath>

namespace {

} // namespace

namespace mk {

float amplitude2dB(float amplitude) {
	return 20.0 * ::log10f(clamp(::fabs(amplitude), 1.0e-4f, 1.0f));
}

float dB2Amplitude(float loudness) {
	return ::exp(clamp(loudness, std::numeric_limits<float>::min(), 0.0f) / 20.0f);
}

SampleInfo::SampleInfo()
	: frame(0)
	, channel(0)
	, amplitude(0.0)
{
}

double SampleInfo::loudness() const {
	return amplitude2dB(amplitude);
}

void audioToText(const std::string& audioFilePath, const std::string& textFilePath) {
	if (audioFilePath == textFilePath) {
		std::cerr << "soundToText(): audio and text file paths can't be the same: " << audioFilePath << std::endl;
		return;
	}

	// open audio file for reading
	SF_INFO info;
	SNDFILE* in = sf_open(audioFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "soundToText(): Failed to open audio file: " << audioFilePath << std::endl;
		return;
	}

	// open text file for writing
	std::ofstream o(textFilePath);
	if (o.bad()) {
		std::cerr << "soundToText(): Failed to open text file: " << textFilePath << std::endl;
		return;
	}
	o.precision(std::numeric_limits<double>::max_digits10);

	const sf_count_t frames = info.frames;
	const int channels = info.channels;
	std::vector<float> samples((size_t)channels);

	// iterate audio file's frames
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "soundToText(): Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(in) << std::endl;
			return;
		}

		o << i << '\t';

		// iterate audio frame's samples
		for (auto j = 0; j < channels; ++j) {
			o << samples[j] << '\t';
		}
		o << '\n';
	}

	sf_close(in);
}

bool scanMax(const std::string& inputFilePath, SampleInfo& max) {
	// open audio file in read mode
	SF_INFO info;
	SNDFILE* in = sf_open(inputFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "scanMax(): Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	const sf_count_t frames = info.frames;
	const int channels = info.channels;
	std::vector<float> samples((size_t)channels);
	max.amplitude = 0.0;
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "scanMax(): Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(in) << std::endl;
			return false;
		}

		for (auto j = 0; j < channels; ++j) {
			if (::abs(max.amplitude) < ::abs(samples[j])) {
				max.amplitude = samples[j];
				max.frame = i;
				max.channel = j;
			}
		}
	}

	sf_close(in);
	return true;
}

void normalize(const std::string& inputFilePath, const std::string& outputFilePath, float peakValue) {
	if (inputFilePath == outputFilePath) {
		std::cerr << "normalize(): Input and output file can't be the same: " << inputFilePath << std::endl;
		return;
	}

	SampleInfo max;
	if (!scanMax(inputFilePath, max)) {
		std::cerr << "normalize(): Failed to scan input file: " << inputFilePath << std::endl;
		return;
	}

	// open input file 
	SF_INFO info;
	SNDFILE* in = sf_open(inputFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "normalize(): Failed to open input file: " << inputFilePath << std::endl;
		return;
	}

	const sf_count_t frames = info.frames;
	const int channels = info.channels;

	// open output file in write mode
	SNDFILE* out = sf_open(outputFilePath.c_str(), SFM_WRITE, &info);
	if (!out) {
		std::cerr << "normalize(): Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << sf_strerror(out) << std::endl;
		sf_close(in);
		return;
	}

	const float gain = 1.0 / max.amplitude;
	std::cout << "normalize(): Gain = " << gain << std::endl;

	std::vector<float> samples((size_t)channels);
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "normalize(): Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(in) << std::endl;
			sf_close(in);
			sf_close(out);
			return;
		}

		// normalize audio frame
		for (auto j = 0; j < channels; ++j) {
			samples[j] *= gain;
		}

		if(sf_write_float(out, &samples[0], channels) != channels) {
			std::cerr << "normalize(): Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(out) << std::endl;
			sf_close(in);
			sf_close(out);
			return;
		}
	}

	sf_close(in);
	sf_close(out);
}

} // namespace mk
