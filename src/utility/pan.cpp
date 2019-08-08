#include "Util.h"
#include <iostream>

const std::string& commandHelp = "Pans a stereo file into a stereo field position using constant power.";
const std::string& commandParams = "<input stereo file> <output stereo file> <pan position>";

void printHelp(int argc, char** argv) {
	std::string param = "-h";
	if (argc == 2) {
		param = argv[1];
	}

	if (param == "-h"  || param == "--help") {
		std::cout << commandHelp << std::endl;
		std::cout << std::endl;
		std::cout << argv[0] << " [-h | --help] " << commandParams << std::endl;
		std::cout << std::endl;
		std::cout << "Parameters:" << std::endl;
		std::cout << "\t[-h | --help]:\tprints this help message" << std::endl;
		std::cout << "\t:\tprints this help message" << std::endl;
		exit(0);
	}
}

int main(int argc, char* argv[]) {
//	printHelp(argc, argv);

	if (argc != 4) {
		std::cerr << "Missing parameters. Usage: " << argv[0] << " <input audio file path> <output audio file path> <pan position>" << std::endl;
		return 1;
	}

	const std::string inputFilePath(argv[1]);
	const std::string outputFilePath(argv[2]);

	char* end;
	double position = strtod(argv[3], &end);
	if (argv[3] == end || position < -1.0 || position > 1.0) {
		std::cerr << "Incorrect position value: '" << argv[3] << "', pass a value between -1.0 and 1.0 dB" << std::endl;
		return 1;
	}

	return !mk::panStereoFile(inputFilePath, outputFilePath, position);
}
