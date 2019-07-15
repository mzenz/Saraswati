#pragma once

#include <cstdint>
#include <string>

namespace mk {

struct IeeeExtended {
	IeeeExtended(double n = 0);

	void operator=(double n);

	operator double() const;

	std::string toString() const;

	const uint8_t* raw() const { return _buff; }

	static constexpr size_t size = 10;

private:
	uint8_t _buff[size];
};

} // namespace mk
