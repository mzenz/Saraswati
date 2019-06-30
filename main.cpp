
// Project: midi_to_freq
//
// Created with cpp_boot.py
// Martin Zenzerovich
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

// convert a MIDI Note number to the equal-tempered frequency in hertz
// C3 note (i.e. "middle C") is MIDI code #60

namespace {

// the ratio between two adjacent semitones (r = the 12th root of 2; r^12 = 2)
//const float _semitone_ratio = 1.059463094359295f;
const char* _notes[] { "C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B" };

template<class T>
T clamp(T n, T min, T max) { return std::max(std::min(n, max), min); }

}

struct Note {
	Note(uint8_t note, int octave = 4)
		: _note(note % 12)
		, _octave(clamp(octave, -2, 8))
	{
	}

	/// Translates the note by a given interval
	Note& operator+=(int interval) {
		_note += interval % 12;
		_octave += interval / 12;
		return *this;
	}

	/// Returns pure note
	char pureNote() const {
		return *_notes[_note];
	}

	/// Returns MIDI key
	uint8_t key() const {
		return 12 * _octave + _note + 24;
	}

	float frequency() const {
		// f = 440 Hz * 2 ^ [(key - 81 / 12)]
		return std::exp2f((key() - 81) / 12.f) * 440.f;
	}

	friend std::ostream& operator<<(std::ostream& s, const Note& note) {
		s << _notes[note._note] << note._octave;
		return s;
	}

private:
	uint8_t _note;
	int _octave;
};

struct Key {
	Key(uint8_t key) : _key(key) {}
	
	uint8_t _key;
};

int main(int argc, char* argv[]) {

	// Given a note, print its major scale

	Note note(2);
	cout  << endl;
	cout << "Major scale of " << note.pureNote() << " (note = " << note << ")" << endl;
	for (auto i = 0; i < 7; ++i) {
		cout << note << " ";
		switch(i) {
			case 2:
			case 6:
			note += 1;
			break;
		default:
			note += 2;
		}
	}
	cout << endl;

	// Print the MIDI keys for each note

	cout << "Key\tNote\tFrequency" << endl;
	for (auto octave = -2; octave < 9; ++octave) {
		for (auto note = 0; note < 12; ++note) {
			Note n(note,octave);
			cout << (int)n.key() << "\t" << n << "\t" << n.frequency() << " Hz" << endl;
		}
	}
	cout << endl;
}
