#include "Note.h"
#include "Scale.h"
#include "Synthesis.h"
#include "AIFF.h"
#include "IEEEExtended.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

using namespace std;
using namespace mk;

// Given a note, print its major scale
void majorScale() {
	// major scales
	const std::vector<Note> scaleC { C_2, D_2, E_2, F_2, G_2, A_2, B_2 };
	const std::vector<Note> scaleD { D0, E0, Gb0, G0, A0, B0, Db1 };
	const std::vector<Note> scaleE { E1, Gb1, Ab1, A1, B1, Db2, Eb2 };
	const std::vector<Note> scaleF { F2, G2, A2, Bb2, C3, D3, E3 };
	const std::vector<Note> scaleG { G3, A3, B3, C4, D4, E4, Gb4 };
	const std::vector<Note> scaleA { A4, B4, Db5, D5, E5, Gb5, Ab5 };
	const std::vector<Note> scaleB { B5, Db6, Eb6, E6, Gb6, Ab6, Bb6 };
	const std::vector<Note> scaleD8 { D8, E8, Gb8, G8 };

	assert(scaleC == majorScale(C_2));
	assert(scaleD == majorScale(D0));
	assert(scaleE == majorScale(E1));
	assert(scaleF == majorScale(F2));
	assert(scaleG == majorScale(G3));
	assert(scaleA == majorScale(A4));
	assert(scaleB == majorScale(B5));
	// test for scales that fall out of the playable note range (i.e. > 127)
	assert(scaleD8 == majorScale(D8));
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

void frequencyToNote() {
	std::vector<std::pair<float, Note>> frequencies {
		{   -10.f, C_2  },
		{	 2.f, C_2  },
		{	 4.f, C_2  },
		{	 5.f, Eb_2 },
		{	40.f, Eb1  },
		{	41.f, E1   },
		{	50.f, G1   },
		{   400.f, G4   },
		{   666.f, E5   },
		{  1200.f, D6   },
		{  6000.f, Gb8  },
		{ 22000.f, G8   },
	};

	for (auto f : frequencies) {
		Note n = Note::fromFrequency(f.first);
		cout << "Closest note to frequency = " << f.first << " Hz: " << n << " (f = " << n.frequency() << " Hz)" << endl;
		assert(n == f.second);
	}
}

void waveLength() {
	Note n = C4;
	cout << "Note\tPeriod\t\tWavelength" << endl << endl;
	for (auto i = 0; i < 12; ++i) {
		cout << n << "\t" << n.period() << " s\t" << n.waveLength() << endl;
		++n;
	}
}

void doubleToExtended() {
	double values[] {
		0.,
		22050.,
		44100.,
		48000.,
		96000.,
		192000.
	};
	for (auto v : values) {
		IeeeExtended n(v);
		cout << v << " = 0x" << n.toString() << endl;
	}
}

void exponentialEnvelopes() {
	{
		// decay envelope
		std::ofstream o("decay.txt");
		o.precision(std::numeric_limits<double>::max_digits10);
		exponentialEnvelope(o, 0, 1, 1, -1, SAMPLE_RATE_22050);
	}
	{
		// attack envelope
		std::ofstream o("attack.txt");
		o.precision(std::numeric_limits<double>::max_digits10);
		exponentialEnvelope(o, 0, 1, -1, 1, SAMPLE_RATE_22050);
	}
	{
		// attack-decay envelope
		std::ofstream o("ad.txt");
		o.precision(std::numeric_limits<double>::max_digits10);

		const auto sampleRate = 100.0;
		exponentialEnvelope(o, 0.0, 0.3, 1.0e-4, 1.0, sampleRate);
		exponentialEnvelope(o, 0.3, 2.0, 1.0, 1.0e-4, sampleRate);
	}
}

void writeSineWaveToFile()
{
	ofstream o("plot/sine_10Hz@22050Hz.txt");
	o.precision(std::numeric_limits<double>::max_digits10);

	SineWave sine(10.0);
	const double step = 1.0 / SAMPLE_RATE_22050;
	for (double t = 0.0; t < 1.0; t += step) {
		o << t;
		o << "\t";
		o << sine(t);
		o << std::endl;
	}
}

void writeSineWaveToAIFF() {
	const auto sampleRate = SAMPLE_RATE_48K;
	const double frequency = 440.0;
	const auto channels = 1;
	const double duration = 1.0;

	mk::AIFF aiff("synthesis/sine_440Hz@48KHz.aiff", BitDepth::BitDepth16, channels, sampleRate);

	mk::SineWave sineWave(frequency);
	const double dt = 1.0 / sampleRate;
	for (double t = 0.0; t < duration; t += dt) {
		aiff << sineWave(t);
	}
}

void writeSawWaveToAIFF() {
	const auto sampleRate = SAMPLE_RATE_48K;
	const double frequency = 440.0;
	const auto channels = 1;
	const double duration = 1.0;
	const double volume = .1;

	mk::SawWave sawWave(frequency);
	sawWave._amplitude = volume;
	mk::AIFF aiff("synthesis/ascending_saw_440Hz@48KHz.aif", BitDepth::BitDepth16, channels, sampleRate);
	const double dt = 1.0 / sampleRate;
	for (double t = 0.0; t < duration; t += dt) {
		for (auto i = 0; i < aiff.channels(); ++i) {
			aiff << sawWave(t);
		}
	}

	sawWave._polarity = -1.0;
	mk::AIFF aiff2("synthesis/descending_saw_440Hz@48KHz.aif", BitDepth::BitDepth16, duration, sampleRate);
	for (double t = 0.0; t < duration; t += dt) {
		for (auto i = 0; i < aiff.channels(); ++i) {
			aiff2 << sawWave(t);
		}
	}
}

void normalizeAudio() {
	mk::normalize("reference/wu-tang.aiff", "reference/wu-tang_normalized_0dB.aiff");
	mk::normalize("reference/wu-tang.aiff", "reference/wu-tang_normalized_-3dB.aiff", -3.0);
	mk::normalize("reference/wu-tang.aiff", "reference/wu-tang_normalized_-12dB.aiff", -12.0);
}

void dumpAudioToText() {
	mk::audioToText("reference/wu-tang.aiff", "reference/wu-tang.txt");
	mk::audioToText("reference/wu-tang_normalized_0dB.aiff", "reference/wu-tang_normalized_0dB.txt");
	mk::audioToText("reference/wu-tang_normalized_-3dB.aiff", "reference/wu-tang_normalized_-3dB.txt");
	mk::audioToText("reference/wu-tang_normalized_-12dB.aiff", "reference/wu-tang_normalized_-12dB.txt");
}

void printMaxSample() {
	SampleInfo max;
	mk::scanMax("reference/wu-tang.aiff", max);
	std::cout << "scanMax(): " << std::endl;
	std::cout << "frame: " << max.frame << std::endl;
	std::cout << "channel: " << max.channel << std::endl;
	std::cout << "amplitude: " << max.amplitude << std::endl;
	std::cout << "loudness: " << max.loudness() << " dB" << std::endl;
}

int main(int argc, char* argv[]) {
	cout.precision(std::numeric_limits<double>::max_digits10);

	majorScale();
	printFrequenciesOfAllMidiNotes();
	printAllNotes();
	printSomeIntervals();
	frequencyToNote();
	waveLength();
	doubleToExtended();
	exponentialEnvelopes();
	writeSineWaveToFile();
	writeSineWaveToAIFF();
	writeSawWaveToAIFF();
	normalizeAudio();
	printMaxSample();
	dumpAudioToText();
}
