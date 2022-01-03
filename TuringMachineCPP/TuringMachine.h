#ifndef _TURINGMACHINE_H_
#define _TURINGMACHINE_H_

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

// vcpkg packages
#include "yaml-cpp/yaml.h"

const bool DISABLE_ARGS = true;

class TuringNode;

enum class MoveDirection { Left, Right };

struct Action {
	char write;
	MoveDirection direction;
	TuringNode* next;
};


class TuringNode {
	std::string nodeName;

	// There are 256 possible inputs chars
	Action* possibleActions[256];

	public:
		TuringNode(std::string name);
		void addConnection(char input, Action* action);
		const Action* read(char input);
		std::string getName();
};

class TuringMachine {

	std::vector<char> frontVector;
	std::vector<char> backVector;

	char blankCellChar = '0';

	TuringNode* entryNode = nullptr;

	std::string tpInput = "";

	void writeAt(int index, char write);
	char getAt(int index);
	void loadTape(std::string tapeInput);

	std::vector<TuringNode*> nodes;
	std::vector<Action*> actions;

	bool isInitialized = true;

public:
	TuringMachine(std::string filePath);
	void print();
	int run();
	~TuringMachine();
	void reset() {
		loadTape(tpInput);
	};
};

#endif // !_TURINGMACHINE_H_

