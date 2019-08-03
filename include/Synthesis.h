#pragma once

#include <cstdint>
#include <iosfwd>

namespace mk {

constexpr double SAMPLE_RATE_22050 = 22050.0;
constexpr double SAMPLE_RATE_44100 = 44100.0;
constexpr double SAMPLE_RATE_48K = 48000.0;
constexpr double SAMPLE_RATE_96K = 96000.0;
constexpr double SAMPLE_RATE_192K = 192000.0;

struct AudioModule {
	virtual double operator()(double time) const = 0;
};

struct SineWave : public AudioModule {
	SineWave(double frequency);

	double operator()(double time) const override;

	double _frequency;
};

struct SawWave : public AudioModule {
	SawWave(double frequency);

	double operator()(double time) const override;

	double _frequency;
	double _amplitude;
	double _offset;
	double _phase;
	double _polarity;
};

struct ADEnvelope : public AudioModule {
	ADEnvelope(double startLevel, double peakLevel, double endLevel,
			   double attackDuration, double decayDuration);

	double operator()(double time) const override;

	double _startLevel;
	double _peakLevel;
	double _endLevel;
	double _attackDuration;
	double _decayDuration;
};

// TODO: move to Util.h
/// Prints out a general exponential envelope for the given time range
/// where values increase or decrease from startLevel to endLevel.
/// If startLevel < endLevel => attack envelope
/// If startLevel > endLevel => decay envelope
/// If startLevel == endLevel => constant envelope (i.e. no change)
void exponentialEnvelope(std::ostream& out,
						 double startTime,
						 double endTime,
						 double startLevel,
						 double endLevel,
						 double sampleRate);

} // namespace mk
