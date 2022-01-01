use std::{fs::File, io::Read, collections::HashMap};

use serde::Deserialize;
use serde_yaml::Value;

#[derive(Deserialize)]
struct Base {
    blank: String,
    #[serde(rename = "start state")]
    start_state: String,
    input: Option<String>,
    table: HashMap<String, Value>
}

struct State {
    name: String,
    connections: HashMap<String, Action>
}

struct Action {
    write: String,
    move_left: bool,
    destination: State
}

fn main() {
    // The file must be utf8
    let file_path = "test.yaml";

    let mut s: String = String::new();
    let _ = File::open(file_path).unwrap().read_to_string(&mut s);

    {
        let n: Result<Base, serde_yaml::Error> = serde_yaml::from_str(&s);

        match n {
            Ok(val) => {
                for m in val.table {
                    println!("{}: {} - {}", m.0, m.1.is_mapping(), m.1.is_null());
                }
            },
            Err(e) => {
                println!("There was an error. {}.\nThe file must be UTF-8.", e);
            },
        }
    }

}



