mod turing_machine;


fn main() {
    // Read in yaml file
    let args: Vec<String> = std::env::args().collect();

    /*
    if args.len() != 1 {
        println!("Invalid usage: turing_machine [path to yaml]");
        panic!("Error");
    }
    
    let file_path = args[0].clone();
    */
    let mut machine = turing_machine::TuringMachine::new("test.yaml".to_string());

    let j = machine.run();
    println!("Steps: {}", j);

}



