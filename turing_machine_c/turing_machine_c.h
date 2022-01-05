#ifndef _TURING_MACHINE_C_H_
#define _TURING_MACHINE_C_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define NDEBUG
#include <assert.h>


struct Tape {
	int front_size;
	int front_length;
	unsigned char* front_vector;
	int back_size;
	int back_length;
	unsigned char* back_vector;
	unsigned char blank;
	char* input;
};

void tape_reset(struct Tape* tape);

struct Tape tape_create(unsigned char blank, char* input);

unsigned char tape_get(struct Tape* tape, int index);

void tape_write(struct Tape* tape, int index, unsigned char write);

void tape_destroy(struct Tape* tape);

struct TuringNode;

struct Action {
	unsigned char write;
	bool left;
	struct TuringNode* destination;
};

struct TuringNode {
	struct Action* connections[256];
};

struct TuringNode turingnode_create();

void turingnode_add_connection(struct TuringNode* node, unsigned char input, unsigned char write, bool move_left, struct TuringNode* destination);

void turingnode_destroy(struct TuringNode* node);

int turingnode_run(struct TuringNode* entry_node, struct Tape* tape);

#endif // !_TURING_MACHINE_C_H_
