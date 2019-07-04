#include "Synthesis.h"
#include <ostream>
#include <cmath>

namespace {

constexpr float PI = 3.141592653589793f;
constexpr float PIx2 = 2 * PI;

}

namespace mk {

float sampleSineWave(float time, float frequency) {
	return ::sin(frequency * PIx2 * time);
}

void sineWave(std::ostream& out, float duration, float frequency, uint32_t sampleRate) {
	if (duration <= 0 ||
		duration > MAX_DURATION_SECONDS ||
		sampleRate > SAMPLE_RATE_192K ||
		frequency < 0)
		return;

	const double step = 1.0 / sampleRate;
	for (double t = 0.0; t < duration; t += step) {
		out << t << "\t" << sampleSineWave(t, frequency) << std::endl;
	}
}

} // namespace mk
