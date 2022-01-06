#include "turing_machine_c.h"

void tape_reset(struct Tape* tape) {
	assert(tape);
	assert(tape->input);

	// Destroy back_vector if exists
	if (tape->back_vector) {
		free(tape->back_vector);
		tape->back_length = 0;
		tape->back_size = 1;
	}

	// Reset the front_vector
	if (tape->front_vector) {
		free(tape->front_vector);
		tape->front_length = 0;
		tape->front_size = 1;
		tape->front_vector = malloc(tape->front_size * sizeof(unsigned char));
	}
	else {
		tape->front_length = 0;
		tape->front_size = 1;
		tape->front_vector = malloc(tape->front_size * sizeof(unsigned char));
	}

	// Fill in the back_vector
	const unsigned char* input = tape->input;

	int length = 0;
	while (input[length] != '\0') {
		length++;
	}

	tape->back_length = length;
	tape->back_size = length + 1;
	tape->back_vector = malloc(tape->back_size * sizeof(unsigned char));
	int index = 0;
	while (input[index] != '\0') {
		tape->back_vector[index] = input[index];
		index++;
	}
}

struct Tape tape_create(unsigned char blank, char* input) {
	assert(input);

	struct Tape tp = {
		.front_size = 1,
		.front_length = 0,
		.front_vector = NULL,
		.back_size = 1,
		.back_length = 0,
		.back_vector = NULL,
		.blank = blank,
		.input = input,
	};

	tape_reset(&tp);

	return tp;
}

unsigned char tape_get(struct Tape* tape, int index) {
	assert(tape);
	// The index can be [front_length - 1, back_length]
	assert(-1 * (tape->front_length) - 1 <= index);
	assert(index <= (tape->back_length));

	bool positive_index = index >= 0;
	unsigned char** use_vector = positive_index ? &tape->back_vector : &tape->front_vector;
	int* use_size   = positive_index ? &tape->back_size : &tape->front_size;
	int* use_length = positive_index ? &tape->back_length : &tape->front_length;
	int  use_index  = positive_index ? index : abs(index) - 1;
	if (use_index >= *use_size) {
		*use_size *= 2;
		*use_vector = realloc(*use_vector, *use_size * sizeof(unsigned char));
	}
	if (use_index >= *use_length) {
		(*use_vector)[use_index] = tape->blank;
		*use_length += 1;
	}

	return (*use_vector)[use_index];
}

void tape_write(struct Tape* tape, int index, unsigned char write) {
	assert(tape);
	// The index can be (front_length - 1, back_length)
	assert(-1 * (tape->front_length) - 1 <= index);
	assert(index <= (tape->back_length));

	unsigned char* use_vector = (index >= 0) ? tape->back_vector : tape->front_vector;
	unsigned int   use_index = (index >= 0) ? index : abs(index) - 1;

	use_vector[use_index] = write;
}

void tape_destroy(struct Tape* tape) {
	assert(tape);

	if (tape->front_vector) {
		free(tape->front_vector);
	}

	if (tape->back_vector) {
		free(tape->back_vector);
	}
}

//

struct TuringNode turingnode_create() {

	struct TuringNode tn;

	for (int i = 0; i < 256; i++) {
		tn.connections[i] = NULL;
	}

	return tn;
}

void turingnode_add_connection(struct TuringNode* node, unsigned char input, unsigned char write, bool move_left, struct TuringNode* destination) {
	struct Action* act = malloc(sizeof(struct Action));
	act->write = write;
	act->left = move_left;
	act->destination = destination;

	node->connections[input] = act;
}

void turingnode_destroy(struct TuringNode* node) {
	for (int i = 0; i < 256; i++) {
		struct Action* action = node->connections[i];
		if (action) {
			free(action);
		}
	}
}

int turingnode_run(struct TuringNode* entry_node, struct Tape* tape) {
	assert(entry_node);
	assert(tape);

	int steps = 0;
	int index = 0;
	struct TuringNode* current_node = entry_node;

	while (true) {
		unsigned char cell = tape_get(tape, index);

		struct Action* act = current_node->connections[cell];
		if (act == NULL) {
			break;
		}
		unsigned char write = act->write;
		if (cell != write) {
			tape_write(tape, index, write);
		}
		current_node = act->destination;
		switch (act->left) {
		case true:
			index--;
			break;
		case false:
			index++;
			break;
		}

		if (steps == 2'000'000'000) {
			break;
		}
		steps++;
	}

	return steps;
}
