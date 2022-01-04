#include <iostream>
#include <vector>
#include <unordered_map>

#include "TuringMachine.h"

#define is_benchmarking true
#if !is_benchmarking

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

#else

#ifdef _WIN32
#pragma comment ( lib, "Shlwapi.lib" )
#ifdef _DEBUG
#pragma comment ( lib, "benchmarkd.lib" )
#else
#pragma comment ( lib, "benchmark.lib" )
#endif
#endif

#include "benchmark/benchmark.h"

static void BM_TuringMachine(benchmark::State& state) {
	TuringMachine tm = TuringMachine::TuringMachine("test.yaml");
	for (auto _ : state) {
		tm.run();
		tm.reset();
	}
}

BENCHMARK(BM_TuringMachine)->Iterations(100);

BENCHMARK_MAIN();

#endif

