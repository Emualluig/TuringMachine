use std::{fs::File, io::Read, collections::HashMap, rc::{Rc, Weak}, cell::RefCell};

use serde::Deserialize;
use serde_yaml::{Value, Sequence};

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
    id: u8,
    cons: [Action; 256],
}

struct Tape {
    front_vector: Vec<u8>,
    back_vector: Vec<u8>,
}

impl Tape {
    fn getAt(&self, index: i64) -> u8 {
        return 0;
    }
    fn writeAt(&mut self, write: u8, index: i64) {

    }
}

struct TuringMachine {
    id_vector: Vec<u8>,
    id_entry: u8,
    tape: Tape,
}

fn main() {
    // The file must be utf8
    let file_path = "test.yaml";

    let mut s: String = String::new();
    let _ = File::open(file_path).unwrap().read_to_string(&mut s);

    
    let yaml: Result<Base, serde_yaml::Error> = serde_yaml::from_str(&s);

    let tn_map: HashMap<String, Option<u8>> = HashMap::new();

    match yaml {
        Ok(base) => {
            
            // First loop over table and get all state nodes
            let mut all_state_nodes: Vec<String> = Vec::new();
            for state in &base.table {
                println!("{}", state.0);
                all_state_nodes.push(state.0.clone());
            }

            // Check if start_state exist in all state nodes
            let has_start = all_state_nodes.contains(&base.start_state);
            if !has_start {
                println!("[YAML] The state \"{}\" is reference by start state but is not in table.", base.start_state);
                panic!("Error");
            }

            let mut state_table: HashMap<String, Option<(u8, Action)>> = HashMap::new();
            for (state_name, connections) in base.table {
                // The state value must either be a map or a null
                if connections.is_mapping() {

                    let mut v23: [Option<u8>; 256] = [Option::None; 256];

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

                        // Read all input and create Action for each of them
                        for action_input_pre in input_vector {
                            let action_input = action_input_pre as u8;
                            let action_write = if write_char.is_some() { write_char.unwrap() } else { action_input };
                            let action_left = move_left;
                            let action_dest = if destination.is_some() { destination.clone().unwrap() } else { state_name.clone() };

                            if !all_state_nodes.contains(&action_dest) {
                                println!("[YAML] The state {} is used in {} but does not exist.", action_dest, state_name);
                                panic!("Error");
                            }

                            v23[action_input as usize] = Option::None;
                        }

                    }

                } else if connections.is_null() {
                    state_table.insert(state_name, None);
                } else {
                    println!("[YAML] The state {} is invalid. Must be either a map or a null.", state_name);
                    panic!("Error");
                }
            }

        },
        Err(e) => {
            println!("There was an error. {}.\nThe file must be UTF-8.", e);
            panic!("Error");
        },
    }
    

    {
        
    }
}



