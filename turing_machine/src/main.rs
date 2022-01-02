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
    cons: [Option<Action>; 256],
}

struct Tape {
    front_vector: Vec<u8>,
    back_vector: Vec<u8>,
}

impl Tape {
    fn get_at(&self, index: i64) -> u8 {
        return 0;
    }
    fn write_at(&mut self, write: u8, index: i64) {

    }
}

struct TuringMachine {
    id_vector: Vec<TuringNode>,
    id_entry: u8,
    tape: Tape,
}

fn main() {
    // The file must be utf8
    let file_path = "test.yaml";

    let mut s: String = String::new();
    let _ = File::open(file_path).unwrap().read_to_string(&mut s);

    let yaml: Result<Base, serde_yaml::Error> = serde_yaml::from_str(&s);


    // Store the turing machine struct in this intermediate form
    let mut id_state: Vec<Vec<(u8, Action)>> = Vec::new();
    let mut curr_id: u8 = 0;

    match yaml {
        Ok(base) => {
            
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

            /*
            // Print
            println!("blank: \'{}\'", &base.blank);
            println!("start start: \'{}\' [{}]", &base.start_state, state_ids.get(&base.start_state).unwrap().clone());
            if base.input.is_some() {
                println!("input: {}", &base.input.unwrap());
            }
            println!("table:");
            for (state_string, connections) in &state_table {
                println!("    {} [{}]:", state_string, state_ids.get(state_string).unwrap());
                for (input, act) in connections {
                    println!("        {}:", input);
                    println!("            write: {}", act.write);
                    println!("            move_left: {}", act.move_left);
                    println!("            next: {} [{}]", ids_state.get(&act.destination).unwrap(), act.destination);
                }
            }
            */
            
            

            /*

            // Create TuringNodes
            let mut tn_vec: Vec<TuringNode> = Vec::new();
            for (state_string, connections) in state_table {
                // Work-around from: https://github.com/rust-lang/rust/issues/44796
                const INIT: Option<Action> = Option::None;
                let mut cons: [Option<Action>; 256] = [INIT; 256];
                for (input, act) in connections {
                    cons[input as usize] = Option::Some(act);
                }
                let id = state_ids.get(&state_string).unwrap().clone();
                let tn = TuringNode { id, cons };
                tn_vec.push(tn);
            }

            tn_vec.sort_by(|a, b| {
                a.id.cmp(&b.id)
            });

            let tm = TuringMachine {
                id_vector: tn_vec,
                id_entry: state_ids.get(&base.start_state).unwrap().clone(),
                tape: Tape { 
                    front_vector: Vec::new(), 
                    back_vector: if base.input.is_some() {
                        Vec::new()
                    } else { Vec::new() } },
            };

            */


        },
        Err(e) => {
            println!("There was an error. {}.\nThe file must be UTF-8.", e);
            panic!("Error");
        },
    }
    

    {
        for (id, connections) in id_state.iter().enumerate() {
            println!("{}:", id);
            for (input, action) in connections {
                println!("    {}:", input);
                println!("        write: {}", action.write);
                println!("        left:  {}", action.move_left);
                println!("        desti: {}", action.destination);
            }
        }
    }
}



