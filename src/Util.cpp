#include "Util.h"
#include <fstream>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include <cmath>

namespace {

struct SNDFILE_RAII {
	SNDFILE_RAII(const std::string& filePath)
		: file(sf_open(filePath.c_str(), SFM_READ, &info))
		, samples(static_cast<size_t>(info.channels))
	{
	}

	SNDFILE_RAII(const std::string& filePath, const SF_INFO& other)
		: info(other)
		, file(sf_open(filePath.c_str(), SFM_WRITE, &info))
		, samples(static_cast<size_t>(info.channels))
	{
	}

	~SNDFILE_RAII() {
		sf_close(file);
	}

	bool valid() const { return file != nullptr; }
	
	const char* error() const { return sf_strerror(file); }
	
	bool fetchNextFrame() {
		return sf_read_float(file, &samples[0], info.channels) == info.channels;
	}

	bool writeFrame(const std::vector<float>& s) {
		return s.size() == samples.size() && sf_write_float(file, &s[0], info.channels) == info.channels;
	}

	SF_INFO info;
	SNDFILE* file;
	std::vector<float> samples;
};

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
	SNDFILE_RAII f(audioFilePath);
	if (!f.valid()) {
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

	// iterate audio file's frames
	for (sf_count_t i = 0; i < f.info.frames; ++i) {
		if(!f.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << f.error() << std::endl;
			return false;
		}

		o << i << '\t';

		// iterate audio frame's samples
		for (auto j = 0; j < f.info.channels; ++j) {
			o << f.samples[j] << '\t';
		}
		o << '\n';
	}

	return true;
}

bool scanMax(const std::string& inputFilePath, SampleInfo& max) {
	// open audio file in read mode
	SNDFILE_RAII f(inputFilePath);
	if (!f.valid()) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	max.amplitude = 0.0;
	for (sf_count_t i = 0; i < f.info.frames; ++i) {
		if(!f.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << f.error() << std::endl;
			return false;
		}

		for (auto j = 0; j < f.info.channels; ++j) {
			if (::abs(max.amplitude) < ::abs(f.samples[j])) {
				max.amplitude = f.samples[j];
				max.frame = i;
				max.channel = j;
			}
		}
	}

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
	const double gain = ::abs(peakAmplitude / max.amplitude);

	// open input file
	SNDFILE_RAII in(inputFilePath);
	if (!in.valid()) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	// open output file in write mode
	SNDFILE_RAII out(outputFilePath, in.info);
	if (!out.valid()) {
		std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << out.error() << std::endl;
		return false;
	}

	for (sf_count_t i = 0; i < in.info.frames; ++i) {
		if(!in.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << in.error() << std::endl;
			return false;
		}

		// normalize audio frame
		for (auto j = 0; j < in.info.channels; ++j) {
			in.samples[j] *= gain;
		}

		if (!out.writeFrame(in.samples)) {
			std::cerr << "Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << out.error() << std::endl;
			return false;
		}
	}

	return true;
}

bool amplify(const std::string& inputFilePath, const std::string& outputFilePath, float gain) {
	if (inputFilePath == outputFilePath) {
		std::cerr << "Input and output file can't be the same: " << inputFilePath << std::endl;
		return false;
	}

	const double ratio = loudnessToAmplitude(gain);
	if (std::isnan(ratio)) {
		std::cerr << "Gain factor is too small" << std::endl;
		return false;
	}

	// open input file
	SNDFILE_RAII in(inputFilePath);
	if (!in.valid()) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	// open output file in write mode
	SNDFILE_RAII out(outputFilePath, in.info);
	if (!out.valid()) {
		std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << out.error() << std::endl;
		return false;
	}

	for (sf_count_t i = 0; i < in.info.frames; ++i) {
		if(!in.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << in.error() << std::endl;
			return false;
		}

		// apply gain to audio frame while avoiding dynamic range overflow
		for (auto j = 0; j < in.info.channels; ++j) {
			in.samples[j] = clamp(ratio * in.samples[j], -1.0, 1.0);
		}

		if(!out.writeFrame(in.samples)) {
			std::cerr << "Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << out.error() << std::endl;
			return false;
		}
	}

	return true;
}

bool invertPhase(const std::string& inputFilePath, const std::string& outputFilePath) {
	if (inputFilePath == outputFilePath) {
		std::cerr << "Input and output file can't be the same: " << inputFilePath << std::endl;
		return false;
	}

	// open input file
	SNDFILE_RAII in(inputFilePath);
	if (!in.valid()) {
		std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
		return false;
	}

	// open output file in write mode
	SNDFILE_RAII out(outputFilePath, in.info);
	if (!out.valid()) {
		std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << out.error() << std::endl;
		return false;
	}

	for (sf_count_t i = 0; i < in.info.frames; ++i) {
		if(!in.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << in.error() << std::endl;
			return false;
		}

		// apply gain to audio frame while avoiding dynamic range overflow
		for (auto j = 0; j < in.info.channels; ++j) {
			in.samples[j] *= -1.0;
		}

		if(!out.writeFrame(in.samples)) {
			std::cerr << "Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << out.error() << std::endl;
			return false;
		}
	}

	return true;
}

bool mix(const std::string& inputFilePath1,
		 const std::string& inputFilePath2,
		 const std::string& outputFilePath,
		 double gain1,
		 double gain2) {
	if (inputFilePath1 == inputFilePath2) {
		std::cerr << "Input files can't be the same: " << inputFilePath1 << std::endl;
		return false;
	}

	// open 1st input file in read mode
	SNDFILE_RAII in1(inputFilePath1);
	if (!in1.valid()) {
		std::cerr << "Failed to open input file: " << inputFilePath1 << std::endl;
		return false;
	}

	// open 2nd input file in read mode
	SNDFILE_RAII in2(inputFilePath2);
	if (!in2.valid()) {
		std::cerr << "Failed to open input file: " << inputFilePath2 << std::endl;
		return false;
	}

	if (in1.info.samplerate != in2.info.samplerate ||
		in1.info.format     != in2.info.format) {
		std::cerr << "Format error: can only mix audio files with matching sample rate and bit depth" << std::endl;
		return false;
	}

	// open output file in write mode
	SF_INFO outInfo = in1.info;
	outInfo.channels = std::max(in1.info.channels, in2.info.channels);
	outInfo.frames = std::max(in1.info.frames, in2.info.frames);

	SNDFILE_RAII out(outputFilePath, outInfo);
	if (!out.valid()) {
		std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
		std::cerr << "Error: " << out.error() << std::endl;
		return false;
	}

	for (sf_count_t i = 0; i < outInfo.frames; ++i) {
		if(!in1.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << in1.error() << std::endl;
			return false;
		}

		if(!in2.fetchNextFrame()) {
			std::cerr << "Failed to read audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << in2.error() << std::endl;
			return false;
		}

		// normalize audio frame
		for (auto j = 0; j < out.info.channels; ++j) {
			const float s1 = j < in1.samples.size() ? in1.samples[j] : 0.0f;
			const float s2 = j < in2.samples.size() ? in2.samples[j] : 0.0f;
			out.samples[j] = clamp(loudnessToAmplitude(gain1) * s1 + loudnessToAmplitude(gain2) * s2, -1.0, 1.0);
		}

		if(!out.writeFrame(out.samples)) {
			std::cerr << "Failed to write audio frame @ pos " << i << std::endl;
			std::cerr << "Error: " << out.error() << std::endl;
			return false;
		}
	}

	return true;
}

} // namespace mk
