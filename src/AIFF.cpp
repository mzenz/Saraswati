#include "AIFF.h"
#include "IEEEExtended.h"
#include "Endian.h"
#include "Util.h"
#include <cstdint>
#include <iostream>

/*
AIFF File Format
Byte order: Big-endian

Basic file structure:
 __________________________ 
|   FORM chunk             |
|   chunkId  = 'FORM'      |
|   formType = 'AIFF'      |
|    __________________    |
|   | Common chunk     |   |
|   | chunkId = 'COMM' |   |
|   |__________________|   |
|    __________________    |
|   | Sound data chunk |   |
|   | chunkId = 'SSND' |   |
|   |__________________|   |
|__________________________|

FORM chunk (mandatory, 12 bytes):
  0      4 bytes  "FORM"
  4      4 bytes  <File size - 8>
  8      4 bytes  "AIFF"

COMM chunk (mandatory, 26 bytes)
  0      4 bytes  "COMM"
  4      4 bytes  <COMM chunk size>  // (=18)
  8      2 bytes  <Number of channels(c)>
 10      4 bytes  <Number of frames(f)>
 14      2 bytes  <bits/samples(b)>  // 1..32
 16     10 bytes  <Sample rate (Extended 80-bit floating-point format)>

INST chunk:
  0      4 bytes  "INST"
  4      4 bytes  <INST chunk size(x)>
  8     (x)bytes  <INST chunk>

MARK chunk:
  0      4 bytes  "MARK"
  4      4 bytes  <MARK chunk size(x)>
  8     (x)bytes  <MARK chunk>

SKIP chunk:
  0      4 bytes  "SKIP"
  4      4 bytes  <SKIP chunk size(x)>
  8     (x)bytes  <SKIP chunk>

SSND chunk (mandatory):
  0      4 bytes  "SSND"
  4      4 bytes  <Chunk size(x)>
  8      4 bytes  <Offset(n)>
 12      4 bytes  <block size>       // (=0)
 16     (n)bytes  Comment
 16+(n) (s)bytes  <Sample data>      // (s) := (x) - (n) - 8
*/

namespace {

const char* ID_AIFF = "AIFF";
const char* ID_FORM = "FORM";
const char* ID_COMMON = "COMM";
const char* ID_SSND = "SSND";

constexpr size_t OFFSET_FORM_FILE_SIZE = 4;
constexpr size_t OFFSET_COMM_FRAME_COUNT = 22;
constexpr size_t OFFSET_SSND_CHUNK_SIZE = 42;

void writeFORM(std::ostream& o)
{
	uint32_t fileSize = 0;
	o << ID_FORM;
	o.write(reinterpret_cast<const char*>(&fileSize), sizeof(uint32_t));
	o << ID_AIFF;
}

void writeCOMM(std::ostream& o,
			   uint16_t channels,
			   uint16_t sampleBitDepth,
			   double sampleRate)
{
	const uint32_t chunkSize = mk::swapEndianness32(18);
	const uint32_t frameCount = mk::swapEndianness32(0); // not yet known
	channels = mk::swapEndianness16(channels);
	sampleBitDepth = mk::swapEndianness16(sampleBitDepth);

	o << ID_COMMON;
	o.write(reinterpret_cast<const char*>(&chunkSize), sizeof(uint32_t));
	o.write(reinterpret_cast<const char*>(&channels), sizeof(uint16_t));
	o.write(reinterpret_cast<const char*>(&frameCount), sizeof(uint32_t));
	o.write(reinterpret_cast<const char*>(&sampleBitDepth), sizeof(uint16_t));
	o.write(reinterpret_cast<const char*>(mk::IeeeExtended(sampleRate).raw()), mk::IeeeExtended::size);
}

void writeSSND(std::ostream& o)
{
	const uint32_t chunkSize = mk::swapEndianness32(0);
	const uint32_t offset = 0;
	const uint32_t blockSize = 0;

	o << ID_SSND;
	o.write(reinterpret_cast<const char*>(&chunkSize), sizeof(uint32_t));
	o.write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
	o.write(reinterpret_cast<const char*>(&blockSize), sizeof(uint32_t));
}

} // namespace

namespace mk {

AIFF::AIFF(const std::string& filePath, BitDepth bitDepth, uint16_t channels, double sampleRate)
	: _f(filePath, std::ios::binary)
	, _bitDepth(bitDepth)
	, _channels(channels)
	, _sampleRate(sampleRate)
	, _sampleDepth(0)
	, _samples(0)
{
	if (_f.bad()) {
		std::cerr << "Failed to create AIFF file" << std::endl;
		return;
	}

	switch (_bitDepth) {
		case BitDepth::BitDepth16:
		_sampleDepth = 16;
		break;

		case BitDepth::BitDepth24:
		_sampleDepth = 24;
		break;

		default:
		break;
	}
	
	// write FORM, COMM and SSND chunks beforehand
	writeFORM(_f);
	writeCOMM(_f, _channels, _sampleDepth, _sampleRate);
	writeSSND(_f);
}

AIFF::~AIFF() {
	// fill missing samples (if any) to complete final sample frame
	if (_samples % _channels != 0) {
		const uint8_t remainingSamples = _channels - (_samples % _channels);
		for (size_t i = 0; i < channels() - remainingSamples; ++i) {
			*this << 0.0;
		}
	}

	// rewind stream and write remaining variable-length data

	// since we're done appending samples, the current file position is the file's size
	const uint32_t fileSize = swapEndianness32(static_cast<uint32_t>(_f.tellp()) - 8);

	_f.seekp(OFFSET_FORM_FILE_SIZE);
	_f.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));

	_f.seekp(OFFSET_COMM_FRAME_COUNT);
	const uint32_t frameCount = swapEndianness32(_samples / channels());
	_f.write(reinterpret_cast<const char*>(&frameCount), sizeof(uint32_t));

	_f.seekp(OFFSET_SSND_CHUNK_SIZE);
	const uint32_t ssndChunkSize = swapEndianness32(_samples * _channels * _sampleDepth + 8);
	_f.write(reinterpret_cast<const char*>(&ssndChunkSize), sizeof(uint32_t));
}

void writeSample16(std::ostream& o, double sample) {
	// clamp floating-point sample to [-1;1] and scale
	// it to maximum positive value for a 16-bit sample
	uint16_t s = swapEndianness16(32767 * clamp(sample, -1.0, 1.0));
	o.write((const char*)&s, 2);
}

void writeSample24(std::ostream& o, double sample) {
	// clamp floating-point sample to [-1;1] and scale
	// it to maximum positive value for a 24-bit sample
	uint32_t s = swapEndianness32(8388607 * clamp(sample, -1.0, 1.0)) >> 8;
	o.write((const char*)&s, 3);
}

AIFF& AIFF::operator<<(double sample) {
	if (_f.good()) {
		switch (_bitDepth) {
		case BitDepth::BitDepth16:
			writeSample16(_f, sample);
		break;

		case BitDepth::BitDepth24:
			writeSample24(_f, sample);
		break;

		default:
			break;
		}
		++_samples;
	}
	return *this;
}

} // namespace mk
