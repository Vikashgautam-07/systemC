#include<systemc.h>

using namespace std;

SC_MODULE(Producer)
{
    sc_fifo_out<bool> out;

    SC_CTOR(Producer)
    {
        SC_THREAD(do_produce);
    }

    void do_produce()
    {
        bool value = false;// initial value to be produced

        while (true)
        {
            if (out.num_free() == 0)
            {
                cout << "At time " << sc_time_stamp() << ": FIFO is full" << endl;
            }
            else
            {
                value = !value;
                out.write(value);
                cout << "At time " << sc_time_stamp() << ": Produced value: " << value << endl;
            }

            wait(5, SC_NS);
        }
    }
};

SC_MODULE(Consumer)
{
    sc_fifo_in<bool> in;

    SC_CTOR(Consumer)
    {
        SC_THREAD(do_consumer);
    }

    void do_consumer()
    {
        while (true)
        {
            if (in.num_available() == 0)
            {
                cout << "At time " << sc_time_stamp() << ": FIFO is empty" << endl;
            }
            else
            {
                bool consumed_value = in.read();
                cout << "At time " << sc_time_stamp() << ": Consumed value: " << consumed_value << endl;
            }

            wait(10, SC_NS);
        }
    }
};

int sc_main(int argc, char * argv[])
{
    sc_fifo<bool> fifo(5);

    Producer pdr("producer");
    pdr.out(fifo);

    Consumer cmr("consumer");
    cmr.in(fifo);

    cout << "Starting simulation" << endl;

    sc_start(50, SC_NS);

    cout << "Simulation finished" << endl;
    return 0;
}