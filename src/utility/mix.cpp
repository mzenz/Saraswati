#include "Util.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Missing parameters. Usage: " << argv[0] << " <1st input audio file path> <2nd input audio file path> <output audio file path>" << std::endl;
		return 1;
	}

	const std::string inputFilePath1(argv[1]);
	const std::string inputFilePath2(argv[2]);
	const std::string outputFilePath(argv[3]);

	return !mk::mix(inputFilePath1, inputFilePath2, outputFilePath);
}
