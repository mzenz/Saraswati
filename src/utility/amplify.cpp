#include "Util.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Missing parameters. Usage: " << argv[0] << " <input audio file path>  <output audio file path> <gain factor in dB>" << std::endl;
		return 1;
	}

	const std::string inputFilePath(argv[1]);
	const std::string outputFilePath(argv[2]);
	char* end;
	const double gain = strtod(argv[3], &end);
	if (argv[3] == end) {
		std::cerr << "Incorrect gain value: '" << argv[3] << "', pass a value in dB" << std::endl;
		return 1;
	}

	return !mk::amplify(inputFilePath, outputFilePath, gain);
}
