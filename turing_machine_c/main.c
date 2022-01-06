#include "turing_machine_c.h"

#define is_benchmarking true
#if is_benchmarking

#include <time.h>

#endif

int main() {

	char* input = "";
	struct Tape tape = tape_create('B', input);

	struct TuringNode tn_a = turingnode_create();
	struct TuringNode tn_b = turingnode_create();
	struct TuringNode tn_c = turingnode_create();
	struct TuringNode tn_d = turingnode_create();
	struct TuringNode tn_e = turingnode_create();
	struct TuringNode tn_f = turingnode_create();
	struct TuringNode tn_h = turingnode_create();

	turingnode_add_connection(&tn_a, 'B', '1', false, &tn_b);
	turingnode_add_connection(&tn_a, '1', '1', false, &tn_a);
	turingnode_add_connection(&tn_b, 'B', '1', true, &tn_c);
	turingnode_add_connection(&tn_b, '1', '1', true, &tn_b);
	turingnode_add_connection(&tn_c, 'B', 'B', false, &tn_f);
	turingnode_add_connection(&tn_c, '1', '1', true, &tn_d);
	turingnode_add_connection(&tn_d, 'B', '1', false, &tn_a);
	turingnode_add_connection(&tn_d, '1', 'B', true, &tn_e);
	turingnode_add_connection(&tn_e, 'B', '1', true, &tn_h);
	turingnode_add_connection(&tn_e, '1', '1', true, &tn_f);
	turingnode_add_connection(&tn_f, 'B', 'B', true, &tn_a);
	turingnode_add_connection(&tn_f, '1', 'B', true, &tn_c);

#if is_benchmarking
	// Here is the benchmark
	{
		const int cycles = 100;
		float accumulated_time = 0.0f;
		for (int i = 0; i < cycles; i++) {
			// This method of benchmarking comes from: https://stackoverflow.com/questions/2349776/how-can-i-benchmark-c-code-easily
			float start_time = (float)clock() / CLOCKS_PER_SEC;


			{
				int steps = turingnode_run(&tn_a, &tape);
				tape_reset(&tape);
			}

			float end_time = (float)clock() / CLOCKS_PER_SEC;
			accumulated_time += end_time - start_time;
		}
		float time_per_loop = accumulated_time / (float)cycles;
		printf("Each iteration took on average %f seconds.\n", time_per_loop);
	}
#endif

	turingnode_destroy(&tn_a);
	turingnode_destroy(&tn_b);
	turingnode_destroy(&tn_c);
	turingnode_destroy(&tn_d);
	turingnode_destroy(&tn_e);
	turingnode_destroy(&tn_f);
	turingnode_destroy(&tn_h);

	tape_destroy(&tape);

	return 0;
};