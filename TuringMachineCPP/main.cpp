#include <iostream>
#include <vector>
#include <unordered_map>

// vcpkg packages
//#include "benchmark/benchmark.h"

#include "TuringMachine.h"

static void usage() {
	std::cout << "Invalid usage!\n" << "Valid usage: TuringMachineCPP [path to yaml file]\n\n";
}

int main(int argc, char *argv[]) {

	// Invalid arguments
	if (argc != 2 && !DISABLE_ARGS) {
		usage();
		return 1;
	}

	TuringMachine tm = TuringMachine("test.yaml");
	int y = tm.run();
	std::cout << "Steps: " << y << "\n";
	//tm.print();

	// TODO: benchmark

	return 0;
}