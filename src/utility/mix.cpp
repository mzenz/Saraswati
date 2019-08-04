#include "Util.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cerr << "Missing parameters. Usage: " << argv[0] << " <1st input audio file path> <2nd input audio file path> <output audio file path>" << std::endl;
		return 1;
	}

	const std::string inputFilePath1(argv[1]);
	const std::string inputFilePath2(argv[2]);
	const std::string outputFilePath(argv[3]);

	double gain1 = 0.0;
	double gain2 = 0.0;

	if (argc >= 5) {
		char* end;
		gain1 = strtod(argv[4], &end);
		if (argv[4] == end) {
			std::cerr << "Incorrect gain value: '" << argv[4] << "', pass a value in dB" << std::endl;
			return 1;
		}
	}

	if (argc >= 6) {
		char* end;
		gain2 = strtod(argv[5], &end);
		if (argv[5] == end) {
			std::cerr << "Incorrect gain value: '" << argv[5] << "', pass a value in dB" << std::endl;
			return 1;
		}
	}

	return !mk::mix(inputFilePath1, inputFilePath2, outputFilePath, gain1, gain2);
}
