#pragma once

#include <cstdint>
#include <string>

namespace mk {

struct IeeeExtended {
	uint8_t _buff[10];

	IeeeExtended(double n = 0);

	void operator=(double n);

	operator double() const;

	std::string toString() const;
};

} // namespace mk
