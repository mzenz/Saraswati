#include "Scale.h"

namespace mk {

std::vector<Note> majorScale(const Note& note) {
	std::vector<Note> scale;
	Note n = note;
	for (auto i = 0; i < 7; ++i) {
		scale.push_back(n);

		// stop iterating if we are at
		// the highest possible note
		if (n == Note::max()) {
			break;
		}

		switch(i) {
			case 2:
			case 6:
			++n;
			break;
		default:
			n += 2;
		}
	}
	return scale;
}

} // namespace mk
