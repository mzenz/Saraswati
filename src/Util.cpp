#include "Util.h"
#include <fstream>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include <cmath>

namespace {

} // namespace

namespace mk {

double amplitudeToLoudness(double amplitude) {
	return 20.0 * ::log10f(clamp(::abs(amplitude), 1.0e-4, 1.0));
}

double loudnessToAmplitude(double loudness) {
	return pow(10.0, loudness / 20.0);
}

SampleInfo::SampleInfo()
	: frame(0)
	, channel(0)
	, amplitude(0.0)
{
}

double SampleInfo::loudness() const {
	return amplitudeToLoudness(amplitude);
}

bool audioToText(const std::string& audioFilePath, const std::string& textFilePath) {
	if (audioFilePath == textFilePath) {
		std::cerr << "Audio and text file paths can't be the same: " << audioFilePath << std::endl;
		return false;
	}

	// open audio file for reading
	SF_INFO info;
	SNDFILE* in = sf_open(audioFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "Failed to open audio file: " << audioFilePath << std::endl;
		return false;
	}

	// open text file for writing
	std::ofstream o(textFilePath);
	if (o.bad()) {
		std::cerr << "Failed to open text file: " << textFilePath << std::endl;
		return false;
	}
	o.precision(std::numeric_limits<double>::max_digits10);

	const sf_count_t frames = info.frames;
	const int channels = info.channels;
	std::vector<float> samples((size_t)channels);

	// iterate audio file's frames
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(in) << std::endl;
			return false;
		}

		o << i << '\t';

		// iterate audio frame's samples
		for (auto j = 0; j < channels; ++j) {
			o << samples[j] << '\t';
		}
		o << '\n';
	}

	sf_close(in);
	
	return true;
}

bool scanMax(const std::string& inputFilePath, SampleInfo& max) {
	// open audio file in read mode
	SF_INFO info;
	SNDFILE* in = sf_open(inputFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	const sf_count_t frames = info.frames;
	const int channels = info.channels;
	std::vector<float> samples((size_t)channels);
	max.amplitude = 0.0;
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
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

bool normalize(const std::string& inputFilePath, const std::string& outputFilePath, float peakLoudness) {
	if (inputFilePath == outputFilePath) {
		std::cerr << "Input and output file can't be the same: " << inputFilePath << std::endl;
		return false;
	}

	if (peakLoudness > 0.0) {
		std::cerr << "Peak value is out of range: " << peakLoudness << ", max peak value is 0 dB" << std::endl;
		return false;
	}

	const float peakAmplitude = clamp(loudnessToAmplitude(peakLoudness), 0.0, 1.0);

	if (std::isnan(peakAmplitude)) {
		std::cerr << "Peak amplitude is too small" << std::endl;
		return false;
	}

	// get input file's peak amplitude
	SampleInfo max;
	if (!scanMax(inputFilePath, max)) {
		std::cerr << "Failed to scan input file: " << inputFilePath << std::endl;
		return false;
	}

	// calculate gain factor
	const float gain = ::abs(peakAmplitude / max.amplitude);

	// open input file
	SF_INFO info;
	SNDFILE* in = sf_open(inputFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	const sf_count_t frames = info.frames;
	const int channels = info.channels;

	// open output file in write mode
	SNDFILE* out = sf_open(outputFilePath.c_str(), SFM_WRITE, &info);
	if (!out) {
		std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << sf_strerror(out) << std::endl;
		sf_close(in);
		return false;
	}

	std::vector<float> samples((size_t)channels);
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(in) << std::endl;
			sf_close(in);
			sf_close(out);
			return false;
		}

		// normalize audio frame
		for (auto j = 0; j < channels; ++j) {
			samples[j] *= gain;
		}

		if(sf_write_float(out, &samples[0], channels) != channels) {
			std::cerr << "Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(out) << std::endl;
			sf_close(in);
			sf_close(out);
			return false;
		}
	}

	sf_close(in);
	sf_close(out);
	
	return true;
}

bool amplify(const std::string& inputFilePath, const std::string& outputFilePath, float gain) {
	if (inputFilePath == outputFilePath) {
		std::cerr << "Input and output file can't be the same: " << inputFilePath << std::endl;
		return false;
	}

	const float ratio = loudnessToAmplitude(gain);
	if (std::isnan(ratio)) {
		std::cerr << "Gain factor is too small" << std::endl;
		return false;
	}

	// open input file
	SF_INFO info;
	SNDFILE* in = sf_open(inputFilePath.c_str(), SFM_READ, &info);
	if (!in) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	const sf_count_t frames = info.frames;
	const int channels = info.channels;

	// open output file in write mode
	SNDFILE* out = sf_open(outputFilePath.c_str(), SFM_WRITE, &info);
	if (!out) {
		std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << sf_strerror(out) << std::endl;
		sf_close(in);
		return false;
	}

	std::vector<float> samples((size_t)channels);
	for (sf_count_t i = 0; i < frames; ++i) {
		if(sf_read_float(in, &samples[0], channels) != channels) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(in) << std::endl;
			sf_close(in);
			sf_close(out);
			return false;
		}

		// normalize audio frame
		for (auto j = 0; j < channels; ++j) {
			samples[j] *= ratio;
		}

		if(sf_write_float(out, &samples[0], channels) != channels) {
			std::cerr << "Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << sf_strerror(out) << std::endl;
			sf_close(in);
			sf_close(out);
			return false;
		}
	}

	sf_close(in);
	sf_close(out);
	
	return true;
}

} // namespace mk
