#include "Synthesis.h"
#include <ostream>
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

void envelope(std::ostream& out,
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

	const double samples = duration * sampleRate;
	const double dt = 1.0 / sampleRate;

	// k = (min / max)^(1 / r * dT)
	double k = ::pow(endLevel / startLevel, 1.0 / samples);
	auto level = startLevel;
	for (double t = startTime; t < endTime; t += dt) {
		out << t;
		out << "\t";
		out << level;
		out << std::endl;

		level *= k;
	}
}

} // namespace mk
