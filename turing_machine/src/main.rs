use std::{fs::File, io::Read, collections::HashMap};

use serde::Deserialize;
use serde_yaml::{Value};

#[derive(Deserialize)]
struct Base {
    blank: String,
    #[serde(rename = "start state")]
    start_state: String,
    input: Option<String>,
    table: HashMap<String, Value>
}

struct Action {
    write: u8,
    move_left: bool,
    destination: u8
}

struct TuringNode {
    cons: [Option<Action>; 256],
}

impl TuringNode {
    fn read(&self, value: u8) -> Option<&Action> {
        return self.cons[value as usize].as_ref();
    }
}

struct Tape {
    blank: u8,
    front_vector: Vec<u8>,
    back_vector: Vec<u8>,
}

impl Tape {
    fn get_at(&mut self, index: i64) -> u8 {
        let front_len = self.front_vector.len();
        let back_len = self.back_vector.len();
        if index >= 0 {
            if index as usize >= back_len {
                self.back_vector.push(self.blank);
            }
            return self.back_vector[index as usize];
        } else {
            let abs_index = index.abs() as usize - 1;
            if abs_index >= front_len {
                self.front_vector.push(self.blank);
            }
            return self.front_vector[abs_index];
        }
    }
    fn write_at(&mut self, write: u8, index: i64) {
        if index >= 0 {
            self.back_vector[index as usize] = write;
        } else {
            self.front_vector[index.abs() as usize - 1] = write;
        }
    }
}

struct TuringMachine {
    id_vector: Vec<TuringNode>,
    id_entry: u8,
    tape: Tape,
}

impl TuringMachine {
    fn run(&mut self) -> i64 {

        let mut steps = 0;
        let mut current_node = &self.id_vector[self.id_entry as usize];
        let mut index = 0;

        loop {
            let cell = self.tape.get_at(index);

            let action_option = current_node.read(cell);
            if action_option.is_none() {
                break;
            }
            let action = action_option.unwrap();
            
            let write = action.write;
            if write != cell {
                self.tape.write_at(write, index);
            }

            let move_direction = action.move_left;
            if move_direction {
                index -= 1;
            } else {
                index += 1;
            }

            steps += 1;
            current_node = &self.id_vector[action.destination as usize];
        }

        return steps;
    }
    fn print(&self) {

    }
}

fn main() {
    // Read in yaml file
    let args: Vec<String> = std::env::args().collect();

    // The file must be utf8
    let file_path = "test.yaml";

    let mut s: String = String::new();
    let _ = File::open(file_path).unwrap().read_to_string(&mut s);

    let yaml: Result<Base, serde_yaml::Error> = serde_yaml::from_str(&s);


    // Store the turing machine struct in this intermediate form
    let mut id_state: Vec<Vec<(u8, Action)>> = Vec::new();
    let mut curr_id: u8 = 0;

    // These value are going to be initialized before creating the turing machine
    // The program panics if they are not
    let tm_entry: u8;
    let tm_blank: u8;
    let tm_input: Option<String>;

    {
        // Read the yaml


    }

    match yaml {
        Ok(base) => {
            
            tm_blank = base.blank.to_string().as_bytes()[0];
            tm_input = base.input;

            // First loop over table and get all state nodes
            // Create HashMap to link the string name to a numerical id
            let mut state_ids: HashMap<String, u8> = HashMap::new();
            base.table.iter().enumerate().for_each(|(id, (name, _))| {
                state_ids.insert(name.to_string(), id as u8);
            });

            // Check if start_state exist in all state nodes
            let has_start = state_ids.contains_key(&base.start_state);
            if !has_start {
                println!("[YAML] The state \"{}\" is reference by start state but is not in table.", base.start_state);
                panic!("Error");
            } else {
                tm_entry = state_ids.get(&base.start_state).unwrap().clone();
            }

            for (state_name, connections) in base.table {

                // Add new entry to id_state vector
                id_state.push(Vec::new());

                // The state value must either be a map or a null
                if connections.is_mapping() {

                    for (input, action) in connections.as_mapping().unwrap() {

                        let mut input_vector: Vec<u64> = Vec::new();
                        // The input is either a sequence of u8 or a single u8
                        if input.is_sequence() {
                            let sequence = input.as_sequence().unwrap();
                            
                            for value in sequence {
                                if value.is_u64() {
                                    let u_value = value.as_u64().unwrap();
                                    input_vector.push(u_value);
                                } else {
                                    println!("[YAML] Value in sequence for state {} is not a non-negative integer.", state_name);
                                    panic!("Error");
                                }
                            }

                        } else if input.is_u64() {
                            input_vector.push(input.as_u64().unwrap());
                        } else if input.is_string() {
                            input_vector.push(input.as_str().unwrap().as_bytes()[0] as u64);
                        } else {
                            println!("[YAML] Invalid input for state {}. Must be a non-negative integer of a sequence of non-negative integers.", state_name);
                            panic!("Error");
                        }

                        let move_left: bool;
                        let mut destination: Option<String> = Option::None;
                        let mut write_char: Option<u8> = Option::None;
                        // A connection is either L/R or {[L/R]: [state], write[optional]: u8}
                        if action.is_string() {
                            let direction = action.as_str().unwrap().to_string();
                            move_left = if direction == "L" { true } else { false };
                        } else if action.is_mapping() {

                            let write_yaml = action.get("write");
                            let left_yaml = action.get("L");
                            let right_yaml = action.get("R");

                            // write is optional
                            if write_yaml.is_some() {
                                if write_yaml.unwrap().is_u64() {
                                    write_char = Some(write_yaml.unwrap().as_u64().unwrap() as u8);
                                } else if write_yaml.unwrap().is_string() {
                                    write_char = Some(write_yaml.unwrap().as_str().unwrap().as_bytes()[0]);
                                } else {
                                    println!("[YAML] State transition on {} must be a string.", state_name);
                                }
                            }

                            // There is either left_yaml or right_yaml but not both
                            if left_yaml.is_some() ^ right_yaml.is_some() {
                                move_left = if left_yaml.is_some() { true } else { false };
                                let side_yaml = if left_yaml.is_some() { left_yaml.unwrap() } else { right_yaml.unwrap() };
                                if side_yaml.is_string() {
                                    destination = Some(side_yaml.as_str().unwrap().to_string());
                                } else {
                                    println!("[YAML] A state transition destination must be a string. On {}.", state_name);
                                    panic!("Error");
                                }

                            } else {
                                println!("[YAML] A state transition must have either an L or an R but not both. On {}.", state_name);
                                panic!("Error");
                            }

                        } else {
                            println!("[YAML] Transition for a node must either be a dictionary or a L or R for {}.", state_name);
                            panic!("Error");
                        }

                        let mut input_action_vec: Vec<(u8, Action)> = Vec::new();
                        // Read all input and create Action for each of them
                        for action_input_pre in input_vector {
                            let action_input = action_input_pre as u8;
                            let action_write = if write_char.is_some() { write_char.unwrap() } else { action_input };
                            let action_left = move_left;
                            let action_dest = if destination.is_some() { destination.clone().unwrap() } else { state_name.clone() };

                            if !state_ids.contains_key(&action_dest) {
                                println!("[YAML] The state {} is used in {} but does not exist.", action_dest, state_name);
                                panic!("Error");
                            }

                            let act: Action = Action {write: action_write, move_left: action_left, destination: state_ids[&action_dest] };

                            input_action_vec.push((action_input, act));
                        }

                        // Add current connections to list of all connections for this state
                        id_state[curr_id as usize].append(&mut input_action_vec);

                    }

                } else if connections.is_null() {
                    // do nothing
                } else {
                    println!("[YAML] The state {} is invalid. Must be either a map or a null.", state_name);
                    panic!("Error");
                }

                curr_id += 1;
            }
        },
        Err(e) => {
            println!("There was an error. {}.\nThe file must be UTF-8.", e);
            panic!("Error");
        },
    }
    

    {
        // Create turing nodes
        let mut tn_vec: Vec<TuringNode> = Vec::new();
        const INIT: Option<Action> = Option::None;
        for connection in id_state {
            let mut cons: [Option<Action>; 256] = [INIT; 256];
            for (input, action) in connection {
                cons[input as usize] = Some(action);
            }
            let tn = TuringNode { cons };
            tn_vec.push(tn);
        }

        // Create the turing machine
        let mut tm = TuringMachine {
            id_vector: tn_vec,
            id_entry: tm_entry,
            tape: {
                Tape {
                    blank: tm_blank,
                    front_vector: Vec::new(), 
                    back_vector: if tm_input.is_some() {
                        tm_input.unwrap().as_bytes().to_vec()
                    } else {
                        Vec::new()
                    }, 
                }
            },
        };

        // Run the machine
        let j = tm.run();
        println!("steps: {}", j);

    }
}



