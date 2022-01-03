use criterion::{criterion_group, criterion_main, Criterion};

use turing_machine::turing_machine;

pub fn criterion_benchmark(c: &mut Criterion) {

    let mut tm = turing_machine::TuringMachine::new("test.yaml".to_string());

    let mut group = c.benchmark_group("turing_benchmark");
    group.sample_size(100);
    group.bench_function("TuringMachine.run", |b| b.iter(|| {
        tm.run();
        tm.reset();
    }));

}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);