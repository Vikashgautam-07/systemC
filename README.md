# SystemC Practice

Hands-on SystemC modeling projects, built from scratch (not just wrapping built-in
primitives) to understand what real hardware IP does under the hood.

## Projects

- [`Logic_gate/`](./Logic_gate) — combinational two-input AND and OR gate
  simulations.
- [`clock/`](./clock) — periodic `sc_clock` generation and monitoring of its
  high and low transitions.
- [`multiplexer/`](./multiplexer) — a 4-to-1 multiplexer that routes one of four
  inputs according to two selector bits.
- [`custom_fifo/`](./custom_fifo) — synchronous FIFO with backpressure, built
  from `sc_signal` storage and explicit pointers rather than `sc_fifo<T>`.
- [`uart/`](./uart) — clocked UART transmitter and receiver FSMs with 8N1
  framing, midpoint RX sampling, and a loopback testbench.

## Getting started

These examples require a C++ compiler and a local SystemC installation. Projects
are independent; see each project's README or build file for its specific
compile and run instructions.

## License

Vikashgautam-07/systemC is licensed under the [MIT License](./LICENSE), a
permissive license that allows commercial and private use, modification, and
distribution. Copies or substantial portions must retain the copyright and
license notice. The software is provided without warranty, and its authors
are not liable for claims or damages.
