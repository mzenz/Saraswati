#include "Note.h"
#include <cmath>
#include <algorithm>
#include <ostream>

namespace {

const char* _notes[] { "C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B" };

// The frequency for midi key 0 (C-2)
const float C_2_FREQUENCY = 4.08789945782185f;

// Speed of sound in meters per second
const float SOUND_SPEED = 343.0f;

template<class T>
T clamp(T n, T min, T max) { return std::min(std::max(n, min), max); }

}

namespace mk {

const Note& Note::min() {
	return C_2;
}

const Note& Note::max() {
	return G8;
}

Note Note::fromFrequency(float frequency) {
	uint8_t key = 0;
	// avoid invalid inputs
	if (frequency > 0.0f) {
		key = clamp<long>(std::lroundf(12 * std::log2f(frequency / C_2_FREQUENCY)), 0, 127);
	}
	return Note(key);
}

Note::Note(uint8_t key) {
	set(key);
}

Note& Note::operator+=(int interval) {
	set(_key + interval);
	return *this;
}

Note& Note::operator++() {
	*this += 1;
	return *this;
}

Note& Note::operator--() {
	*this += -1;
	return *this;
}

char Note::pureNote() const {
	return *_notes[note()];
}

void Note::set(uint8_t key) {
	_key = clamp<uint8_t>(key, 0, 127);
}

void Note::set(uint8_t note, int octave) {
	set(12 * octave + clamp<uint8_t>(note, 0, 11) + 24);
}

float Note::frequency() const {
	// Use midi key code 0 = C-2 ~= 4.0879 Hz as reference
	// f = 4.0879 * 2 ^ (midi_key / 12)
	return C_2_FREQUENCY * std::exp2f(key() / 12.f);
}

float Note::period() const {
	return 1.0f / frequency();
}

float Note::waveLength() const {
	return period() * SOUND_SPEED;
}

int Note::operator-(const Note& other) const {
	return _key - other._key;
}

std::ostream& operator<<(std::ostream& s, const Note& note) {
	s << _notes[note.note()] << note.octave();
	return s;
}

} // namespace mk
