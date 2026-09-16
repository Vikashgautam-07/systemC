# UART in SystemC

This folder contains a small SystemC UART exercise based on a transmitter/receiver skeleton. The goal is to model a serial UART channel using clock-driven state machines and to practice timing-sensitive digital design in SystemC.

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

The `UartRx` module is defined as a skeleton and follows the same clock-driven approach as the transmitter. It contains:

- `clk`
- `rst`
- `rx_line`
- `rx_valid`
- `rx_data`
- `rx_error`

The RX design is expected to:

- wait for the falling edge of the line (start bit)
- wait for the center of the bit period
- sample the line at the middle of the bit to avoid transition noise
- shift the 8 data bits into a register
- validate the stop bit
- pulse `rx_valid` when a full byte has been received

This is a key UART design point: sampling at the edge of a bit can be wrong because the signal is still transitioning. Sampling in the middle of the bit period gives the most stable reading.

## Timing model

SystemC does not have a native baud-rate concept, so UART timing is modeled as a number of clock cycles per bit:

- `CYCLES_PER_BIT = clk_freq / baud_rate`

For example, if the clock is 50 MHz and the UART rate is 115200 baud, then one bit period is roughly 434 clock cycles.

In this exercise, `CYCLES_PER_BIT` is intentionally kept small for testing and learning, and is set to a simple placeholder value for ease of simulation.

## State-machine style

The design is intentionally written as synchronous logic using `SC_METHOD` and a clocked FSM pattern:

- the method is sensitive to `clk.pos()`
- the state advances on clock edges
- counters track how long the current bit has been active

This is a natural way to model digital communication protocols in SystemC.

## Suggested verification strategy

A simple loopback test is the standard way to validate the design:

1. Connect `UartTx::tx_line` directly to `UartRx::rx_line`
2. Drive `tx_start` with a byte such as `0x00`, `0xFF`, `0x55`, or `0xAA`
3. Advance the clock for enough cycles to complete a full frame
4. Check that the RX data matches the transmitted byte
5. Confirm that `rx_valid` pulses when the byte is complete

This gives confidence that the serial protocol is working as expected.

## Summary

This UART example is meant to teach the following concepts:

- FSM design in SystemC
- serial communication timing
- bit-level protocol modeling
- clock-cycle-based baud-rate simulation
- loopback-based verification

It is a good foundation for understanding how UART works in hardware and how to model communication protocols with synchronous finite state machines.
