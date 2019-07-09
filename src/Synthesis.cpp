#include "Synthesis.h"
#include <iostream>
#include <cmath>

namespace {

constexpr float PI = 3.141592653589793f;
constexpr float PIx2 = 2 * PI;
constexpr double SILENCE = 1.0e-4; // ~-80dB

}

namespace mk {

float sampleSineWave(double time, double frequency) {
	return ::sin(frequency * PIx2 * time);
}

void sineWave(std::ostream& out, double duration, double frequency, double sampleRate) {
	if (duration <= 0 ||
		duration > MAX_DURATION_SECONDS ||
		sampleRate > SAMPLE_RATE_192K ||
		frequency < 0)
		return;

	const double step = 1.0 / sampleRate;
	for (double t = 0.0; t < duration; t += step) {
		out << t;
		out << "\t";
		out << sampleSineWave(t, frequency);
		out << std::endl;
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
		// decay envelope: start from maximum level
		level = max;
		ratio = SILENCE / max;
	}
	else {
		// attack envelope: start from minumum level (i.e. silence)
		level = SILENCE;
		ratio = max / SILENCE;
	}

	// avoid calling power function for each sample by calculating exponential factor (k)
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
