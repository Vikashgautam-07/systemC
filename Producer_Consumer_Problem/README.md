# SystemC Producer–Consumer Examples

This folder contains two SystemC examples of a producer and a consumer
communicating through a FIFO.

## What is `sc_fifo`?

`sc_fifo<T>` is a SystemC channel that buffers values of type `T` between
concurrent processes or modules. Its capacity is fixed when the FIFO is
created. The producer connects to it through an `sc_fifo_out<T>` port, and
the consumer connects through an `sc_fifo_in<T>` port.

The FIFO decouples the two processes: they can produce and consume at different
times, with values waiting in the FIFO until they are read. A write to a full
FIFO or a read from an empty FIFO blocks until space or data is available.
The FIFO also provides status methods such as `num_free()` and
`num_available()` so a process can check its current state before attempting
an operation.

## Examples

### `bool_fifo_producer_consumer.cc`

This example uses `sc_fifo<bool>` with a capacity of five values.

- The producer attempts to produce every 5 ns. It flips a Boolean value on
  each successful write, producing a sequence such as `1, 0, 1, 0`.
- The consumer checks for a queued value every 10 ns and reads it when one is
  available.
- `num_free()` is used to detect a full FIFO; if it is full, that production
  attempt is skipped. `num_available()` is used to detect an empty FIFO; if
  empty, the consumer skips that read attempt.
- The simulation runs for 50 ns. Because the producer attempts writes twice as
  often as the consumer reads, the FIFO can fill.

### `int_fifo_producer_consumer.cc`

This example uses `sc_fifo<int>` with a capacity of five values.

- The producer writes ten integer values, alternating between `0` and `1`,
  with a 10 ns wait between writes.
- The consumer reads ten values, with a 15 ns wait between reads.
- The producer and consumer use the FIFO's blocking `write()` and `read()`
  operations directly rather than polling the FIFO status.
- The simulation is configured to run for 90 ns. Since the consumer runs
  more slowly than the producer, it may not have read all ten values before
  the simulation stops.

## Build and run

From this directory, compile either example with SystemC installed and
available to the compiler and linker:

```sh
g++ -std=c++17 bool_fifo_producer_consumer.cc -o bool_fifo -lsystemc
./bool_fifo

g++ -std=c++17 int_fifo_producer_consumer.cc -o int_fifo -lsystemc
./int_fifo
```