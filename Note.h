#include <cstdint>
#include <ostream>
#include <vector>

/// A MIDI note
struct Note {
	/// Returns the lowest note that's possible to encode
	static const Note& min();

	/// Returns the highest note that's possible to encode
	static const Note& max();

	/// Returns a note that is closest to a given frequency
	static Note fromFrequency(float frequency);

	/// Default c'tor. Initializes Note as A4 (a.k.a. "concert A")
	Note(uint8_t key = 81);

	/// Returns MIDI key code
	uint8_t key() const { return _key;}
	
	/// Returns the note's octave
	int octave() const { return _key / 12 - 2; };

	/// Returns the note within the current octave
	int note() const { return _key % 12; };

	/// Returns pure note as a single character
	char pureNote() const;

	/// Returns note's frequency in Hertz
	float frequency() const;

	/// Returns note's period in seconds
	float period() const;

	/// Returns note's wave length in meters,
	/// assuming the speed of sound is 343 m/s
	float waveLength() const;

	/// Returns the interval between this and another note
	int operator-(const Note& other) const;

	/// Returns the interval between this and another note
	int operator==(const Note& other) const { return _key == other._key; }

	/// Returns a list of notes representing the major scale for this note
	std::vector<Note> majorScale() const;

	/// Transposes note by a given interval
	Note& operator+=(int interval);

	/// Transposes note by 1
	Note& operator++();

	/// Transposes note by -1
	Note& operator--();

	/// Sets the MIDI key code
	void set(uint8_t key);

	/// Sets the note by note and octave
	void set(uint8_t note, int octave);

private:
	uint8_t _key;
};

std::ostream& operator<<(std::ostream& s, const Note& note);

// Predefined notes
static const Note C_2(0);
static const Note Db_2(1);
static const Note D_2(2);
static const Note Eb_2(3);
static const Note E_2(4);
static const Note F_2(5);
static const Note Gb_2(6);
static const Note G_2(7);
static const Note Ab_2(8);
static const Note A_2(9);
static const Note Bb_2(10);
static const Note B_2(11);
static const Note C_1(12);
static const Note Db_1(13);
static const Note D_1(14);
static const Note Eb_1(15);
static const Note E_1(16);
static const Note F_1(17);
static const Note Gb_1(18);
static const Note G_1(19);
static const Note Ab_1(20);
static const Note A_1(21);
static const Note Bb_1(22);
static const Note B_1(23);
static const Note C0(24);
static const Note Db0(25);
static const Note D0(26);
static const Note Eb0(27);
static const Note E0(28);
static const Note F0(29);
static const Note Gb0(30);
static const Note G0(31);
static const Note Ab0(32);
static const Note A0(33);
static const Note Bb0(34);
static const Note B0(35);
static const Note C1(36);
static const Note Db1(37);
static const Note D1(38);
static const Note Eb1(39);
static const Note E1(40);
static const Note F1(41);
static const Note Gb1(42);
static const Note G1(43);
static const Note Ab1(44);
static const Note A1(45);
static const Note Bb1(46);
static const Note B1(47);
static const Note C2(48);
static const Note Db2(49);
static const Note D2(50);
static const Note Eb2(51);
static const Note E2(52);
static const Note F2(53);
static const Note Gb2(54);
static const Note G2(55);
static const Note Ab2(56);
static const Note A2(57);
static const Note Bb2(58);
static const Note B2(59);
static const Note C3(60);
static const Note Db3(61);
static const Note D3(62);
static const Note Eb3(63);
static const Note E3(64);
static const Note F3(65);
static const Note Gb3(66);
static const Note G3(67);
static const Note Ab3(68);
static const Note A3(69);
static const Note Bb3(70);
static const Note B3(71);
static const Note C4(72);
static const Note Db4(73);
static const Note D4(74);
static const Note Eb4(75);
static const Note E4(76);
static const Note F4(77);
static const Note Gb4(78);
static const Note G4(79);
static const Note Ab4(80);
static const Note A4(81);
static const Note Bb4(82);
static const Note B4(83);
static const Note C5(84);
static const Note Db5(85);
static const Note D5(86);
static const Note Eb5(87);
static const Note E5(88);
static const Note F5(89);
static const Note Gb5(90);
static const Note G5(91);
static const Note Ab5(92);
static const Note A5(93);
static const Note Bb5(94);
static const Note B5(95);
static const Note C6(96);
static const Note Db6(97);
static const Note D6(98);
static const Note Eb6(99);
static const Note E6(100);
static const Note F6(101);
static const Note Gb6(102);
static const Note G6(103);
static const Note Ab6(104);
static const Note A6(105);
static const Note Bb6(106);
static const Note B6(107);
static const Note C7(108);
static const Note Db7(109);
static const Note D7(110);
static const Note Eb7(111);
static const Note E7(112);
static const Note F7(113);
static const Note Gb7(114);
static const Note G7(115);
static const Note Ab7(116);
static const Note A7(117);
static const Note Bb7(118);
static const Note B7(119);
static const Note C8(120);
static const Note Db8(121);
static const Note D8(122);
static const Note Eb8(123);
static const Note E8(124);
static const Note F8(125);
static const Note Gb8(126);
static const Note G8(127);
