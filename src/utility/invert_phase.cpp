#include "Util.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Missing parameters. Usage: " << argv[0] << " <input audio file path> <output audio file path>" << std::endl;
		return 1;
	}

	const std::string inputFilePath(argv[1]);
	const std::string outputFilePath(argv[2]);

	return !mk::invertPhase(inputFilePath, outputFilePath);
}
