# Custom FIFO (SystemC)

A hand-built, cycle-accurate FIFO in SystemC — implemented from `sc_signal` storage
and explicit read/write pointers rather than using the built-in `sc_fifo<T>`, to
understand what a real FIFO IP (AXI-Stream skid buffers, UART TX/RX buffers, DMA
staging buffers) actually has to do under the hood.

## What it models

A synchronous, depth-`N` FIFO with explicit backpressure signaling:

- **Write side**: `wr_en` + `wr_data` in, `full` out — a producer must check `full`
  before asserting `wr_en`.
- **Read side**: `rd_en` in, `rd_data` + `empty` out — a consumer must check `empty`
  before asserting `rd_en`.
- **Depth** is a compile-time template parameter (`CustomFifo<T, DEPTH>`), similar to
  how a synthesizable FIFO would be sized.
- **Simultaneous read + write in the same cycle is supported** — if the FIFO isn't
  full and isn't empty, a push and a pop can both happen in the same clock edge with
  the correct net effect on the occupancy count.

## Interface

```cpp
template <typename T, unsigned int DEPTH>
SC_MODULE(CustomFifo) {
    sc_in<bool>  clk;
    sc_in<bool>  rst;      // active-high, synchronous

    sc_in<bool>  wr_en;
    sc_in<T>     wr_data;
    sc_out<bool> full;

    sc_in<bool>  rd_en;
    sc_out<T>    rd_data;
    sc_out<bool> empty;
};
```

## Design decisions

- **Internal bookkeeping (`wr_ptr`, `rd_ptr`, `count`) is plain member state, not
  `sc_signal`.** They're only ever touched inside a single `SC_METHOD`, so there's no
  need for delta-cycle semantics on them — only the module's external-facing state
  (the ports) needs to be signal-based.
- **A separate `count` variable, rather than comparing `wr_ptr == rd_ptr`, disambiguates
  full vs. empty.** Pointer equality alone is ambiguous (it means either "completely
  empty" or "completely full," and you can't tell which from the pointers alone).
- **Backpressure lives in the producer/consumer's own polling loop, not as a callable
  method on the FIFO.** An earlier version of this design sketched `blocking_write()`
  and `try_write()` as methods a producer would call directly — that's the right shape
  for a TLM/channel-style model, but it doesn't fit a signal/port-based RTL-style
  design. In hardware, backpressure genuinely is "a signal you poll every cycle," not
  "a function call that blocks," so that logic was moved out of the FIFO and into each
  process's own `SC_THREAD` loop instead.

## Bugs found and fixed during development

**1. Read/write priority bug (silently dropped writes).**
The first working version chained read and write handling as `if / else if`, so a
`wr_en` asserted in the same cycle as `rd_en` would be silently ignored whenever the
read branch fired first — even though `full` had told the producer the write would
succeed. Fixed by evaluating `do_read` and `do_write` independently (both computed
against the *pre-mutation* state), then applying the net effect on `count` based on
which combination actually occurred.

**2. Uninitialized-memory read (`INT_MIN` appearing in the data stream).**
During early testbench runs, one read returned `-2147483648` instead of a real value —
a classic signature of reading an uninitialized C++ array slot. This traced back to a
`rd_ptr`/`count` desync: the FIFO believed data was available in a slot that had never
actually been written. Root-caused by adding a per-cycle trace of `wr_ptr`, `rd_ptr`,
and `count`, which made the desync visible directly in the console output rather than
guessing from symptoms alone.

## Build & run

```bash
g++ -std=c++17 -g -I/path/to/systemc/include -L/path/to/systemc/lib-linux64 \
    custom_fifo.cc -lsystemc -lpthread -o custom_fifo
./custom_fifo
```

Requires a local SystemC install (built against `libsystemc`).

## Testbench

`TestBench` runs a producer and consumer as independent `SC_THREAD`s, each polling
`full`/`empty` on every clock edge before asserting `wr_en`/`rd_en`. The producer
pushes values `0`–`9`; the consumer drains them and prints what it received, so the
sequence can be checked by eye against what was sent.

## Possible extensions

- Almost-full / almost-empty threshold flags (common in real FIFO IP, used to start
  throttling before the FIFO is completely full)
- A VCD trace dump of `full`, `empty`, `wr_ptr`, `rd_ptr` for visual waveform inspection
- Parameterizing read/write to happen on different clock domains (async FIFO) — a much
  harder, genuinely interesting follow-up problem once this one is solid