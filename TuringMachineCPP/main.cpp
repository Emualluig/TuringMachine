#include <iostream>
#include <vector>
#include <unordered_map>

// vcpkg packages
#include "yaml-cpp/yaml.h"
//#include "benchmark/benchmark.h"

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
	std::array<Action*, 256> possibleActions;

	public:
		TuringNode(std::string name) : nodeName{name} {
			std::fill(possibleActions.begin(), possibleActions.end(), nullptr);
		}

		//
		void addConnection(char input, Action *action) {
			possibleActions[input] = action;
		}

		//
		const Action* read(char input) {
			return possibleActions[input];
		}

		//
		std::string getName() {
			return nodeName;
		}		
};

class TuringMachine {

	std::vector<char> frontVector;
	std::vector<char> backVector;

	char blankCellChar = '0';

	TuringNode* entryNode = nullptr;

	void writeAt(int index, char write) {
		if (index >= 0) {
			backVector[index] = write;
		}
		else {
			index = std::abs(index) - 1;

			frontVector[index] = write;
		}
	}
	char getAt(int index) {
		
		/*
		int vIndex = index >= 0 ? index : std::abs(index) - 1;
		std::vector<char>& v = index >= 0 ? backVector : frontVector;
		if (vIndex >= v.size()) {
			v.push_back(blankCellChar);
		}
		return v[vIndex];
		*/

		
		if (index >= 0) {
			if (index >= backVector.size()) {
				backVector.push_back(blankCellChar);
			}
			return backVector[index];

		}
		else {
			index = std::abs(index) - 1; // -1 -> 0, -2 -> 1, -3 -> 2

			if (index >= frontVector.size()) {
				frontVector.push_back(blankCellChar);
			}
			return frontVector[index];

		}
		
		
	}
	void loadTape(std::string tapeInput) {
		for (auto &c : tapeInput) {
			backVector.push_back(c);
		}
	}

	std::vector<TuringNode*> nodes;
	std::vector<Action*> actions;

	bool isInitialized = true;

	public:
		TuringMachine(std::string filePath) {
			if (DISABLE_ARGS) {
				filePath = "test.yaml";
			}

			// Get basic data about the machine
			std::string entryNodeString;
			std::string tapeInput;

			// Load in YAML and create nodes
			{
				YAML::Node yaml;

				try {
					yaml = YAML::LoadFile(filePath);
				}
				catch (const YAML::Exception& ex) {
					std::cout << "[YAML] File not found\n";

					throw "Yaml not found";
				}

				// (from state) -> ((transition input), (transition output), (move direction), (destination state))
				std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string, std::string>>> stateNodes;

				// Start parsing the Yaml
				bool validYaml = true;

				// Get the blankCellChar
				if (validYaml && yaml["blank"] && yaml["blank"].Type() == YAML::NodeType::Scalar) {
					blankCellChar = yaml["blank"].as<std::string>()[0];
				}
				else {
					std::cout << "[YAML] \"blank\" was not found or is a list/dictionary\n";
					validYaml = false;
				}

				// Get the entryNodeString (aka "start state")
				if (validYaml && yaml["start state"] && yaml["start state"].Type() == YAML::NodeType::Scalar) {
					entryNodeString = yaml["start state"].as<std::string>();
				}
				else {
					std::cout << "[YAML] \"start state\" was not found or is a list/dictionary\n";
					validYaml = false;
				}

				// Get optional input
				if (validYaml && yaml["input"] && yaml["input"].Type() == YAML::NodeType::Scalar) {
					tapeInput = yaml["input"].as<std::string>();
				}
				else {
					tapeInput = "";
				}

				// Start to read the table
				if (validYaml && yaml["table"] && yaml["table"].Type() == YAML::NodeType::Map) {
					YAML::Node table = yaml["table"];

					// Loop over states
					for (YAML::const_iterator iter = table.begin(); iter != table.end() && validYaml; iter++) {

						// ->first is the key
						// ->second is the value
						YAML::Node key = iter->first;
						YAML::Node value = iter->second;

						// Check if the key is a string
						if (key.Type() == YAML::NodeType::Scalar) {

							std::string stateName = iter->first.as<std::string>();

							// Check if stateName isn't already in  the stateNodes
							if (stateNodes.find(stateName) == stateNodes.end()) {
								stateNodes[stateName]; // Put key with uninitialized value
							}
							else {
								validYaml = false;
								std::cout << "[YAML] The state \"" << stateName << "\" appears twice in table, only once is valid\n";
								break;
							}

							// The value must also be a Map or a Null
							// At this point the stateName is in stateNodes
							if (value.Type() == YAML::NodeType::Map) {

								// Loop over state transitions
								for (YAML::const_iterator transIter = value.begin(); transIter != value.end() && validYaml; transIter++) {

									YAML::Node transKey = transIter->first;
									YAML::Node transValue = transIter->second;

									std::vector<std::string> inputVector;

									// The key is either a scalar or a sequence
									// Then add to inputVector the inputs
									if (transKey.Type() == YAML::NodeType::Scalar) {
										std::string transInput = transKey.as<std::string>();

										inputVector.push_back(transInput);

									}
									else if (transKey.Type() == YAML::NodeType::Sequence) {

										// Get all inputs from sequence
										for (YAML::const_iterator inputIter = transKey.begin(); inputIter != transKey.end() && validYaml; inputIter++) {
											std::string transInput = inputIter->as<std::string>();

											inputVector.push_back(transInput);
										}

									}
									else {
										validYaml = false;
										std::cout << "[YAML] Invalid state read values for \"" << stateName << "\"\n";
										break;
									}

									// The value is either a scalar (a single L or R character which loops back to itself)
									// or a dictionary
									if (transValue.Type() == YAML::NodeType::Scalar) {

										std::string moveDirection = transValue.as<std::string>();

										if (!((moveDirection == "L") || (moveDirection == "R"))) {
											validYaml = false;
											std::cout << "[YAML] Only L or R are the only valid tape movements for\"" << stateName << "\"\n";
											break;
										}

										for (std::string& i : inputVector) {

											std::tuple<std::string, std::string, std::string, std::string> quad =
												std::tuple<std::string, std::string, std::string, std::string>(i, i, transValue.as<std::string>(), stateName);

											stateNodes[stateName].push_back(quad);
										}

									}
									else if (transValue.Type() == YAML::NodeType::Map) {

										// There is either L/R and write or only L/R but not both L and R

										// The write argument is optional
										bool hasWrite = false;
										std::string writeValue;
										if (transValue["write"] && transValue["write"].Type() == YAML::NodeType::Scalar) {
											if (transValue["write"].Type() == YAML::NodeType::Scalar && transValue["write"].as<std::string>().length() == 1) {
												hasWrite = true;
												writeValue = transValue["write"].as<std::string>();
											}
											else {
												std::cout << "[YAML] the write for \"" << stateName << "\" is not a character, skipping...\n";
											}
										}

										// There must be an L or R argument, but not both
										if (transValue["L"] || transValue["R"]) {
											if (transValue["L"] && transValue["R"]) {
												validYaml = false;
												std::cout << "[YAML] You can't have both an L and an R for a tape movement on \"" << stateName << "\"\n";
												break;
											}
											else {
												YAML::Node nodeDirection = transValue["L"] ? transValue["L"] : transValue["R"];
												std::string charDirection = transValue["L"] ? "L" : "R";

												// Check that node is a scalar or a null
												if (!(nodeDirection.Type() == YAML::NodeType::Scalar || nodeDirection.Type() == YAML::NodeType::Null)) {
													validYaml = false;
													std::cout << "[YAML] Invalid value type for tape move on \"" << stateName << "\"\n";
													break;
												}
												else {
													std::string moveNode;

													// Check if is a null
													if (nodeDirection.Type() == YAML::NodeType::Null) {
														moveNode = stateName;
													}
													else {
														// The node is a scalar
														moveNode = nodeDirection.as<std::string>();
													}

													for (std::string& i : inputVector) {

														std::string writeString = i;
														if (hasWrite) {
															writeString = writeValue;
														}

														// Now we add to the stateNodes
														std::tuple<std::string, std::string, std::string, std::string> quad =
															std::tuple<std::string, std::string, std::string, std::string>(i, writeString, charDirection, moveNode);
														stateNodes[stateName].push_back(quad);

													}
												}
											}
										}
									}
									else {
										validYaml = false;
										std::cout << "[YAML] Invalid state transition for \"" << stateName << "\"\n";
										break;
									}

								}

							}
							else if (value.Type() == YAML::NodeType::Null) {
								// This is a final state
							}
							else {
								validYaml = false;
								std::cout << "[YAML] State entry for \"" << stateName << "\" is not a dictionary.\n";
								std::cout << "It is a " << (int)value.Type() << "\n";
								break;
							}

						}
						else {
							validYaml = false;
							std::cout << "[YAML] Invalid state on \"table\", must be a string\n";
							break;
						}

					}

					// Final Yaml validation
					if (validYaml) {

						bool finalValidYaml = true;

						// Determine if there are states referenced by destination that do not exit
						for (const auto& node : stateNodes) {
							auto& stateName = node.first;
							auto& connections = node.second;

							for (const auto& connection : connections) {
								std::string destinationNode = std::get<3>(connection);

								if (stateNodes.find(destinationNode) == stateNodes.end()) {
									// The node referenced in stateNodes does not exist

									std::cout << "[YAML] The state \"" << stateName << "\" references the state \"" << destinationNode << "\" which does not exist.\n";

									finalValidYaml = false;
								}
							}
						}

						// Check if the entryNode defined by entryNodeString exists
						if (finalValidYaml) {
							if (stateNodes.find(entryNodeString) == stateNodes.end()) {
								std::cout << "[YAML] The entry state \"" << entryNodeString << "\" does not exist.";
								finalValidYaml = false;
							}
						}

						// Set validYaml
						if (!finalValidYaml) {
							std::cout << "[YAML] The Turing Machine defined by the YAML is invalid\n";
							validYaml = false;
						}

					}

				}
				else {
					std::cout << "[YAML] \"table\" was not found or was not a dictionary\n";
					validYaml = false;
				}

				// 
				if (validYaml) {

					std::unordered_map<std::string, TuringNode*> nodeMap;

					// Create all the nodes
					for (const auto& node : stateNodes) {
						const auto& stateName = node.first;

						TuringNode* n = new TuringNode(stateName);
						nodeMap.emplace(stateName, n);
						nodes.push_back(n);
					}

					// Set the entry node
					entryNode = nodeMap[entryNodeString];

					// Create connections for each node
					for (const auto& node : stateNodes) {
						const auto& stateName = node.first;
						const auto& connections = node.second;

						TuringNode* currentNode = nodeMap[stateName];

						for (const auto& con : connections) {
							char inputChar = std::get<0>(con)[0];
							char writeChar = std::get<1>(con)[0];
							char direcChar = std::get<2>(con)[0];

							MoveDirection md = (direcChar == 'L') ? MoveDirection::Left : MoveDirection::Right;
							TuringNode* destinationNode = nodeMap[std::get<3>(con)];

							Action* act = new Action{writeChar, md, destinationNode};
							actions.push_back(act);

							currentNode->addConnection(inputChar, act);
						}
					}

					// Load innput into tape
					loadTape(tapeInput);

				}
				else {
					std::cout << "[YAML] Yaml is invalid\n";

					throw "Invalid YAML";
				}
			}
		}
		void print() {
		
			int frontSize = frontVector.size();
			int backSize = backVector.size();

			// Print the tape
			std::cout << "[";
			for (int i = frontSize - 1; i >= 0; i--) {
				if (i == 0) {
					std::cout << frontVector[i];
				}
				else {
					std::cout << frontVector[i] << ", ";
				}
			}
			std::cout << "][";
			for (int i = 0; i < backSize; i++) {
				if (i + 1 == backSize) {
					std::cout << frontVector[i];
				}
				else {
					std::cout << frontVector[i] << ", ";
				}
			}
			std::cout << "]\n";
		
		}
		int run() {
			int steps = 0;
			int index = 0;
			TuringNode* currNode = entryNode;
			while (true) {

				char cellChar = getAt(index);

				const Action* result = currNode->read(cellChar);
				if (result == nullptr) {
					break;
				}

				char write = result->write;
				MoveDirection direction = result->direction;
				TuringNode* next = result->next;

				// Don't write if you don't have too
				if (write != cellChar) {
					writeAt(index, write);
				}

				switch (direction)
				{

					case MoveDirection::Left:
						index--;
						break;
					case MoveDirection::Right:
						index++;
						break;
				}
				
				currNode = next;
				steps++;
				if (steps == 2000000000) {
					break;
				}
			}

			return steps;
		}
		~TuringMachine() {
			for (auto &node : nodes) {
				delete node;
			}
			for (auto &action: actions) {
				delete action;
			}
		}
};

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
	//std::cout << "Steps: " << y << "\n";
	//tm.print();

	return 0;
}