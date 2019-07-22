#include "Synthesis.h"
#include "AIFF.h"
#include <iostream>
#include <cmath>

namespace {

constexpr float PI = 3.141592653589793;
constexpr float PIx2 = 2.0 * PI;
constexpr double SILENCE = 1.0e-4; // ~-80dB
constexpr double MAX_DURATION_SECONDS = 3600.0;

}

namespace mk {

Sine::Sine(double frequency)
	: _frequency(frequency)
{
}

double Sine::operator()(double time) const {
	return ::sin(_frequency * PIx2 * time);
}

ADEnvelope::ADEnvelope(double startLevel,
					   double peakLevel,
					   double endLevel,
					   double attackDuration,
					   double decayDuration)
	: _startLevel(startLevel)
	, _peakLevel(peakLevel)
	, _endLevel(endLevel)
	, _attackDuration(attackDuration)
	, _decayDuration(decayDuration)
{
}

double ADEnvelope::operator()(double time) const {
	if (time <= 0)
		return _startLevel;
	if (time >= _attackDuration + _decayDuration)
		return _endLevel;

	return 0.0;
}

void sineWaveToTextFile(std::ostream& out, double duration, double frequency, double sampleRate) {
	if (duration <= 0 ||
		duration > MAX_DURATION_SECONDS ||
		sampleRate > SAMPLE_RATE_192K ||
		frequency < 0)
		return;

	Sine sine(frequency);
	const double step = 1.0 / sampleRate;
	for (double t = 0.0; t < duration; t += step) {
		out << t;
		out << "\t";
		out << sine(t);
		out << std::endl;
	}
}

void sineWaveToAIFF(AIFF& file, double duration, double frequency, double sampleRate) {
	if (duration <= 0 ||
		duration > MAX_DURATION_SECONDS ||
		sampleRate > SAMPLE_RATE_192K ||
		frequency < 0)
		return;

	const double dt = 1.0 / sampleRate;
	for (double t = 0.0; t < duration; t += dt) {
		for (auto i = 0; i < file.channels(); ++i) {
			file << ::sin(frequency * PIx2 * t);
		}
	}
}

void exponentialEnvelope(std::ostream& out,
						 double startTime,
						 double endTime,
						 double startLevel,
						 double endLevel,
						 double sampleRate)
{
	const auto duration = endTime - startTime;
	if (duration <= 0 ||
		duration > MAX_DURATION_SECONDS ||
		sampleRate > SAMPLE_RATE_192K ||
		sampleRate <= 0.0)
		return;

	if (startLevel == endLevel) {
		std::cerr << "Warning: start level and end level are the same" << std::endl;
	}

	const double samples = duration * sampleRate;
	const double dt = 1.0 / sampleRate;
	const double max = ::abs(endLevel - startLevel) + SILENCE;

	double level, ratio;
	if (startLevel > endLevel) {
		// decay envelope
		level = max;
		ratio = SILENCE / max;
	}
	else {
		// attack envelope
		level = SILENCE;
		ratio = max / SILENCE;
	}

	const double k = ::pow(ratio, 1.0 / samples);
	double t = startTime;
	// offset values so that the minumum value coincides with the starting level
	const double offset = std::min(startLevel, endLevel) - SILENCE;

	// iterate samples + 1 times to cover entire sample range
	const size_t sampleCount = ::ceil(samples);
	for (size_t i = 0; i <= sampleCount; t += dt, ++i, level *= k) {
		out << t << '\t' << level + offset << std::endl;
	}
}

} // namespace mk
