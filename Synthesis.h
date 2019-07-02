#pragma once

#include <cstdint>
#include <ostream>

static const uint32_t SAMPLE_RATE_22050 = 22050;
static const uint32_t SAMPLE_RATE_44100 = 44100;
static const uint32_t SAMPLE_RATE_48K = 48000;
static const uint32_t SAMPLE_RATE_96K = 96000;
static const uint32_t SAMPLE_RATE_192K = 192000;

static const double MAX_DURATION_SECONDS = 3600;

float sampleSineWave(float time, float frequency);

void sineWave(std::ostream& out, float duration, float frequency, uint32_t sampleRate);
