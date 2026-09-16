# UART in SystemC

This folder contains a small SystemC UART loopback exercise. The transmitter and receiver model a serial UART channel using clock-driven finite state machines and a shared serial line.

## Project goal

The design is meant to:

- serialize a byte into a UART frame
- send it over a single data line
- receive it back in a second module
- model timing using clock cycles rather than native baud-rate units

The UART format used here is the standard 8N1 style:

- idle line = 1
- start bit = 0
- 8 data bits, LSB first
- stop bit = 1

## UART TX module

The `UartTx` module is implemented as a SystemC module with the following interface:

- `clk` : clock input
- `rst` : reset input
- `tx_start` : pulse to begin a transmission
- `tx_data` : byte to send
- `tx_busy` : indicates the transmitter is active
- `tx_line` : serial output line

The TX FSM uses a simple state machine:

- `IDLE`
- `START_BIT`
- `DATA_BITS`
- `STOP_BIT`

### TX behavior

In the idle state:

- the output line is driven high
- the transmitter is not busy
- if `tx_start` is asserted, the current byte is latched and transmission begins

During the start bit:

- the line goes low for one full bit period
- the module marks itself busy

During the data bits:

- the signal drives one bit at a time from the shift register
- bits are sent LSB first
- the FSM advances after each bit period

During the stop bit:

- the output line returns high
- the module stays busy until the stop bit duration is complete
- then it returns to idle

## UART RX module

The `UartRx` module follows the same clock-driven approach as the transmitter. It contains:

- `clk`
- `rst`
- `rx_line`
- `rx_valid`
- `rx_data`
- `rx_error`

The RX design:

- wait for the falling edge of the line (start bit)
- wait for the center of the bit period
- sample the line at the middle of the bit to avoid transition noise
- shift the 8 data bits into a register
- validate the stop bit
- pulse `rx_valid` when a full byte has been received

The receiver validates the start bit at its midpoint, samples each data bit at its midpoint, and checks the stop bit at its midpoint. `rx_valid` is asserted for one clock cycle when a complete frame is valid. `rx_error` is asserted for one clock cycle when the stop bit is invalid.

## Timing model

SystemC does not have a native baud-rate concept, so UART timing is modeled as a number of clock cycles per bit:

- `CYCLES_PER_BIT = clk_freq / baud_rate`

For example, if the clock is 50 MHz and the UART rate is 115200 baud, then one bit period is roughly 434 clock cycles.

In this exercise, `CYCLES_PER_BIT` is set to `8` in `testbench.cc` so that complete frames run quickly in simulation. A frame contains 10 bit periods, so one byte takes 80 clock cycles, excluding the inter-frame gap.

## State-machine style

The design is intentionally written as synchronous logic using `SC_METHOD` and a clocked FSM pattern:

- the method is sensitive to `clk.pos()`
- the state advances on clock edges
- counters track how long the current bit has been active

This is a natural way to model digital communication protocols in SystemC.

## Testbench and verification

`testbench.cc` connects `tx_line` directly to `rx_line` and tests these bytes:

```text
0x00, 0xFF, 0x55, 0xAA, 0x3C
```

For every byte, the testbench:

1. waits until `tx_busy` is low
2. pulses `tx_start` for one clock cycle
3. waits for `rx_valid`
4. compares `rx_data` with the transmitted value

The expected result is:

```text
[TB] summary: 5/5 frames passed
```

The monitor also prints TX start events, RX valid events, and RX errors.

## Build and run

The Makefile uses the SystemC installation at `/usr/local/systemc` and compiles with C++17.

```bash
make
./uart_test
```

To remove generated object files and the executable:

```bash
make clean
```

The source files are:

- `uart.h` : module declarations
- `uart.cc` : TX and RX FSM implementations
- `testbench.cc` : loopback stimulus, monitor, and pass/fail checks
- `Makefile` : build configuration

## Essence of this Project

It is a good foundation for understanding how UART works in hardware and how to model communication protocols with synchronous finite state machines.
This UART example is meant to teach the following concepts:

- FSM design in SystemC
- serial communication timing
- bit-level protocol modeling
- clock-cycle-based baud-rate simulation
- loopback-based verification
