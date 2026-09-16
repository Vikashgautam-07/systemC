#include "uart.h"

UartTx::UartTx(sc_module_name name, unsigned int cycles_per_bit)
    : sc_module(name), CYCLES_PER_BIT(cycles_per_bit)
{
    SC_METHOD(fsm);
    sensitive << clk.pos();
}

void UartTx::fsm()
{
    if (rst.read())
    {
        state = State::IDLE;
        tx_busy.write(false);
        tx_line.write(true);
        bit_index = 0;
        cycle_count = 0;
        return;
    }

    switch (state)
    {
        case State::IDLE:
            tx_line.write(true);
            tx_busy.write(false);
            if (tx_start.read())
            {
                shift_reg = tx_data.read();
                bit_index = 0;
                cycle_count = 0;
                state = State::START_BIT;
            }
            break;

        case State::START_BIT:
            tx_line.write(false);
            tx_busy.write(true);
            if (++cycle_count >= CYCLES_PER_BIT)
            {
                cycle_count = 0;
                state = State::DATA_BITS;
            }
            break;

        case State::DATA_BITS:
            tx_line.write(shift_reg[bit_index]);
            tx_busy.write(true);
            if (++cycle_count >= CYCLES_PER_BIT)
            {
                cycle_count = 0;
                if (++bit_index >= 8)
                {
                    bit_index = 0;
                    state = State::STOP_BIT;
                }
            }
            break;

        case State::STOP_BIT:
            tx_line.write(true);
            tx_busy.write(true);
            if (++cycle_count >= CYCLES_PER_BIT)
            {
                cycle_count = 0;
                state = State::IDLE;
            }
            break;
    }
}

UartRx::UartRx(sc_module_name name, unsigned int cycles_per_bit)
    : sc_module(name), CYCLES_PER_BIT(cycles_per_bit)
{
    SC_METHOD(fsm);
    sensitive << clk.pos();
}

void UartRx::fsm()
{
    if (rst.read())
    {
        state = State::IDLE;
        rx_valid.write(false);
        rx_error.write(false);
        rx_data.write(0);
        bit_index = 0;
        cycle_count = 0;
        shift_reg = 0;
        return;
    }

    rx_valid.write(false);
    rx_error.write(false);

    switch (state)
    {
        case State::IDLE:
            if (rx_line.read() == false)
            {
                bit_index = 0;
                shift_reg = 0;
                cycle_count = 0;
                state = State::START_BIT;
            }
            break;

        case State::START_BIT:
            if (++cycle_count == CYCLES_PER_BIT / 2)
            {
                if (rx_line.read() == false)
                {
                    cycle_count = 0;
                    state = State::DATA_BITS;
                }
                else
                {
                    cycle_count = 0;
                    state = State::IDLE;
                }
            }
            else if (cycle_count >= CYCLES_PER_BIT)
            {
                cycle_count = 0;
                state = State::DATA_BITS;
            }
            break;

        case State::DATA_BITS:
            if (++cycle_count == CYCLES_PER_BIT / 2)
            {
                shift_reg[bit_index] = rx_line.read();
            }
            else if (cycle_count >= CYCLES_PER_BIT)
            {
                cycle_count = 0;
                if (++bit_index >= 8)
                {
                    bit_index = 0;
                    state = State::STOP_BIT;
                }
            }
            break;

        case State::STOP_BIT:
            if (++cycle_count == CYCLES_PER_BIT / 2)
            {
                if (rx_line.read() == true)
                {
                    rx_data.write(shift_reg);
                    rx_valid.write(true);
                }
                else
                {
                    rx_error.write(true);
                }

                cycle_count = 0;
                state = State::IDLE;
            }
            break;
    }
}
