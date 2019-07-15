#pragma once

#include <algorithm>

namespace mk {

template<class T>
T clamp(T n, T min, T max) { return std::min(std::max(n, min), max); }

} // namespace mk
