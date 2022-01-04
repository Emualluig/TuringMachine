# TuringMachine
Rust and C++ implementations of Turing Machines

Both the Rust and C++ implementations are identical in what they do.

Uses the same notation as https://turingmachine.io/

TODO:
- C++:
  - [ ] Benchmarking
  - [ ] More comments
- Rust:
  - [ ] Benchmarking
  - [ ] More comments
  - [ ] Optimize (its roughly 2x slower than C++)
  - [ ] Remove limit of 254 nodes (node id 255 is reserved)

### Performance:
Performance is defined by the average time required to get to 2 billion steps on the 6-state busy beaver then resetting the machine on a sample size of 100.

The time required to create the turing machine is not to be measured.

C++: 4.6845s (100 samples)

Rust: 7.4921s (100 samples)

They both use about the same amount of memory.
