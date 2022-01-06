# TuringMachine
Rust, C++, C implementations of Turing Machines

Each of the Rust, C++, C implementations are identical in what they do. The code is somewhat identical. The Rust and C++ implemenations can load in a .yaml file while the C implemenation can't. Uses the same notation as https://turingmachine.io/. All implemenations have a max of 255 or 256 nodes.

### TODO
- C++
  - [ ] More comments
- Rust
  - [ ] More comments
  - [ ] Optimize (its roughly 2x slower than C++) (possibly CPU sampling)
- C
  - [ ] More comments

### Performance:
Performance is defined by the average time required to get to 2 billion steps on the 6-state busy beaver then resetting the machine on a sample size of 100. The time required to create the turing machine is not to be measured.

C: 4.2267s (100 samples)

C++: 4.6845s (100 samples)

Rust: 7.4921s (100 samples)

The Rust and C++ use about the same amount of memory, but the C version consumes about a third.
