#pragma once

#include <cstdint>

namespace mk {

uint32_t swapEndianness32(uint32_t n) {
	return (n & 0xFF000000) >> 24 |
		   (n & 0x00FF0000) >>  8 |
		   (n & 0x0000FF00) <<  8 |
		   (n & 0x000000FF) << 24;
}

uint16_t swapEndianness16(uint16_t n) {
	return (n & 0xFF00) >>  8 |
		   (n & 0x00FF) <<  8;
}

} // namespace mk
