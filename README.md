# TuringMachine
Rust and C++ implementations of Turing Machines

Both the Rust and C++ implementations are identical in what they do.

Uses the same notation as https://turingmachine.io/

TODO:
- C++:
  - [ ] Benchmarking
  - [ ] More comments
  - [ ] Look into more optimizations
- Rust:
  - [ ] Benchmarking
  - [ ] More comments
  - [ ] Optimize (its roughly 2x slower than C++)
  - [ ] Remove limit of 256 nodes

### Performance:
Performance is defined by the average time required to get to 2 billion steps on the 6-state busy beaver on a sample size of 100.

The time required to create the turing machine is not to be measured.

C++: 6 seconds (roughtly, and including initialization)

Rust: 9.56 seconds (including reset, excluding initialization)
