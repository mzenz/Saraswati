#include "IEEEExtended.h"
#include <cmath>

#ifndef HUGE_VAL
#define HUGE_VAL HUGE
#endif /*HUGE_VAL*/

# define FloatToUnsigned(f) ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)
# define UnsignedToFloat(u)        (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)

namespace {

void doubleToIeeeExtended(double num, uint8_t* bytes);

double ieeeExtendedToDouble(const uint8_t* bytes);

} // namespace

namespace mk {

IeeeExtended::IeeeExtended(double n)
{
	*this = n;
}

void IeeeExtended::operator=(double n)
{
	doubleToIeeeExtended(n, _buff);
}

IeeeExtended::operator double() const
{
	return ieeeExtendedToDouble(_buff);
}

std::string IeeeExtended::toString() const
{
	std::string s;
	for (size_t i = 0; i < 10; ++i) {
		char _s[3];
		snprintf(_s, 3, "%02X", (uint8_t)_buff[i]);
		s += _s;
	}
	return s;
}

} // namespace mk

namespace {

void doubleToIeeeExtended(double num, uint8_t* bytes)
{
    int    sign;
    int expon;
    double fMant, fsMant;
    unsigned long hiMant, loMant;

    if (num < 0) {
        sign = 0x8000;
        num *= -1;
    } else {
        sign = 0;
    }

    if (num == 0) {
        expon = 0; hiMant = 0; loMant = 0;
    }
    else {
        fMant = frexp(num, &expon);
        if ((expon > 16384) || !(fMant < 1)) {    /* Infinity or NaN */
            expon = sign|0x7FFF; hiMant = 0; loMant = 0; /* infinity */
        }
        else {    /* Finite */
            expon += 16382;
            if (expon < 0) {    /* denormalized */
                fMant = ldexp(fMant, expon);
                expon = 0;
            }
            expon |= sign;
            fMant = ldexp(fMant, 32);
            fsMant = floor(fMant);
            hiMant = FloatToUnsigned(fsMant);
            fMant = ldexp(fMant - fsMant, 32);
            fsMant = floor(fMant);
            loMant = FloatToUnsigned(fsMant);
        }
    }

    bytes[0] = expon  >>  8;
    bytes[1] = expon;
    bytes[2] = hiMant >> 24;
    bytes[3] = hiMant >> 16;
    bytes[4] = hiMant >>  8;
    bytes[5] = hiMant;
    bytes[6] = loMant >> 24;
    bytes[7] = loMant >> 16;
    bytes[8] = loMant >>  8;
    bytes[9] = loMant;
}

double ieeeExtendedToDouble(const uint8_t* bytes)
{
    double    f;
    int    expon;
    unsigned long hiMant, loMant;

    expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
    hiMant  = ((unsigned long)(bytes[2] & 0xFF) << 24)
            | ((unsigned long)(bytes[3] & 0xFF) << 16)
            | ((unsigned long)(bytes[4] & 0xFF) << 8)
            | ((unsigned long)(bytes[5] & 0xFF));
    loMant  = ((unsigned long)(bytes[6] & 0xFF) << 24)
            | ((unsigned long)(bytes[7] & 0xFF) << 16)
            | ((unsigned long)(bytes[8] & 0xFF) << 8)
            | ((unsigned long)(bytes[9] & 0xFF));

    if (expon == 0 && hiMant == 0 && loMant == 0) {
        f = 0;
    }
    else {
        if (expon == 0x7FFF) {    /* Infinity or NaN */
            f = HUGE_VAL;
        }
        else {
            expon -= 16383;
            f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
            f += ldexp(UnsignedToFloat(loMant), expon-=32);
        }
    }

    if (bytes[0] & 0x80)
        return -f;
    else
        return f;
}

} // namespace
