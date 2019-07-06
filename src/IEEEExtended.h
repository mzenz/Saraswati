#pragma once

#include <cstdint>
#include <string>

namespace mk {

struct IeeeExtended {
	uint8_t _buff[10];

	IeeeExtended(double n);

	std::string toString() const;
};

} // namespace mk
