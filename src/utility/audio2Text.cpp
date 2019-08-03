#include "Util.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Missing parameters. Usage: " << argv[0] << " <audio file path>" << std::endl;
		return 1;
	}
	const std::string audioFilePath(argv[1]);
	
	mk::audioToText(audioFilePath, audioFilePath + ".txt");
	
	return 0;
}
