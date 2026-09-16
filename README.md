# SystemC Practice

Hands-on SystemC modeling projects, built from scratch (not just wrapping built-in
primitives) to understand what real hardware IP does under the hood.

## Projects

- [`custom_fifo/`](./custom_fifo) — synchronous FIFO with backpressure, built from
  sc_signal storage and explicit pointers rather than sc_fifo<T>.
- [`uart/`](./uart) — clocked UART transmitter and receiver FSMs with 8N1 framing,
  midpoint RX sampling, and a loopback testbench.
