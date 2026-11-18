#include <systemc.h>

SC_MODULE(Producer)
{
    sc_fifo_out<int> out;

    SC_CTOR(Producer)
    {
        SC_THREAD(run);
    }

    void run()
    {
        int value = 0;
        int count = 0;

        while (count < 10)
        {
            out.write(value);
            cout << "At time " << sc_time_stamp() << " Produced: " << value << endl;

            value = !value;
            count++;

            wait(10, SC_NS);
        }
    }
};

SC_MODULE(Consumer)
{
    sc_fifo_in<int> in;

    SC_CTOR(Consumer)
    {
        SC_THREAD(run);
    }

    void run()
    {
        int count = 0;

        while (count < 10)
        {
            int item = in.read();
            cout << "At time " << sc_time_stamp() << " Consumed: " << item << endl;

            count++;
            wait(15, SC_NS);
        }
    }
};

int sc_main(int argc, char* argv[])
{
    sc_fifo<int> fifo(5);   

    Producer producer("producer");
    producer.out(fifo);

    Consumer consumer("consumer");
    consumer.in(fifo);

    sc_start(90, SC_NS);
    return 0;
}