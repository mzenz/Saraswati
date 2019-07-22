#pragma once

#include "Note.h"
#include <vector>

namespace mk {

/// Returns a list of notes representing the major scale for this note
std::vector<Note> majorScale(const Note& note);

} // namespace mk
