//#include <algorithm>
#include "Note.h"
#include <cmath>
#include <algorithm>

namespace {

const char* _notes[] { "C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B" };

template<class T>
T clamp(T n, T min, T max) { return std::min(std::max(n, min), max); }

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
	// Use A4 = 440 Hz as reference
	// f = 440 * 2 ^ [(midi_key - 81 / 12)]
	return std::exp2f((key() - 81) / 12.f) * 440.f;
}

std::ostream& operator<<(std::ostream& s, const Note& note) {
	s << _notes[note.note()] << note.octave();
	return s;
}
