#pragma once

#include <cstdint>
#include <iosfwd>

namespace mk {

static const double SAMPLE_RATE_22050 = 22050.0;
static const double SAMPLE_RATE_44100 = 44100.0;
static const double SAMPLE_RATE_48K = 48000.0;
static const double SAMPLE_RATE_96K = 96000.0;
static const double SAMPLE_RATE_192K = 192000.0;

static const double MAX_DURATION_SECONDS = 3600.0;

float sampleSineWave(float time, float frequency);

void sineWave(std::ostream& out,
			  double duration,
			  double frequency,
			  double sampleRate);

// prints out a general exponential envelope
// if startLevel < endLevel => attack envelope
// if startLevel > endLevel => decay envelope
void envelope(std::ostream& out,
			  double startTime,
			  double endTime,
			  double startLevel,
			  double endLevel,
			  double sampleRate);

} // namespace mk
