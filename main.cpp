
// Project: midi_to_freq
//
// Created with cpp_boot.py
// Martin Zenzerovich
#include <iostream>
#include "Note.h"

using namespace std;

// Given a note, print its major scale
void printMajorScale(uint8_t note)
{
	Note n(note);
	cout  << endl;
	cout << "Major scale of " << n.pureNote() << " (note = " << n << ")" << endl;
	for (auto i = 0; i < 7; ++i) {
		cout << n.pureNote() << " ";
		switch(i) {
			case 2:
			case 6:
			++n;
			break;
		default:
			n += 2;
		}
	}
	cout << endl;
}

// for each note print the MIDI keys and frequency
void printFrequenciesOfAllMidiNotes() {
	cout << "Key\tNote\tFrequency" << endl;
	Note n(0);
	for (auto i = 0; i < 128; ++n, ++i) {
		cout << (int)n.key() << "\t" << n << "\t" << n.frequency() << " Hz" << endl;
	}
	cout << endl;
}

// print the MIDI keys for each note
void printAllNotes() {
	Note n = C_2;
	for (auto i = 0; i < 128; ++n, ++i) {
		cout << n << endl;
	}
	cout << endl;
}

// the intervals between several notes
void printSomeIntervals() {
	cout << "Interval between C3 and Db5 = " << Db5 - C3 << endl;
	cout << "Interval between C-2 and G8 = " << G8 - C_2 << endl;
	cout << "Interval between G8 and C-2 = " << C_2 - G8 << endl;
}

int main(int argc, char* argv[]) {
	printMajorScale(0);
	printFrequenciesOfAllMidiNotes();
	printAllNotes();
	printSomeIntervals();
}
