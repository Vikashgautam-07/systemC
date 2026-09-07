#include <systemc>

using namespace sc_core;
using namespace std;

template <typename T, unsigned int DEPTH>
SC_MODULE(CustomFifo) {
    // ---- Ports ----
    sc_in<bool>  clk;
    sc_in<bool>  rst;          // active-high synchronous reset

    // Write side
    sc_in<bool>  wr_en;        // write-enable request from producer
    sc_in<T>     wr_data;
    sc_out<bool> full;         // producer must respect this (backpressure signal)

    // Read side
    sc_in<bool>  rd_en;        // read request from consumer
    sc_out<T>    rd_data;
    sc_out<bool> empty;        // consumer must respect this

    SC_CTOR(CustomFifo) {
        SC_METHOD(update);
        sensitive << clk.pos();
    }

private:
    T storage[DEPTH] = {};          // the actual backing store
    unsigned int wr_ptr = 0;   // TODO: these wrap around mod DEPTH — think about
    unsigned int rd_ptr = 0;   // whether you need a separate `count` variable to
    unsigned int count  = 0;   // disambiguate "full" vs "empty" when wr_ptr == rd_ptr
                               // (classic FIFO gotcha: ptr equality alone is ambiguous)

    sc_event not_full_event;
    sc_event not_empty_event;

    bool is_full()  const { return count == DEPTH; }
    bool is_empty() const { return count == 0; }

    void update()
    {
        if (rst.read())
        {
            wr_ptr = 0;
            rd_ptr = 0;
            count  = 0;
            full.write(false);
            empty.write(true);
            return;
        }

        bool do_read = rd_en.read() && !is_empty();
        bool do_write = wr_en.read() && !is_full();

        if (do_read)
        {
            rd_data.write(storage[rd_ptr]);
            rd_ptr = (rd_ptr + 1) % DEPTH;
            not_full_event.notify(SC_ZERO_TIME);
            cout << "[Read] data: " << storage[rd_ptr] << " at time " << sc_time_stamp() << endl;
        }
        if (do_write)
        {
            storage[wr_ptr] = wr_data.read();
            wr_ptr = (wr_ptr + 1) % DEPTH;
            not_empty_event.notify(SC_ZERO_TIME);
            cout << "[Wrote] data: " << wr_data.read() << " at time " << sc_time_stamp() << endl;
        }
        
        if (do_read && !do_write)
        {
            count--;
        }
        if (do_write && !do_read)
        {
            count++;
        }

        full.write(is_full());
        empty.write(is_empty());
    }
};

SC_MODULE(TestBench) 
{
    sc_in<bool> clk;
    sc_in<bool> rst;

    sc_out<bool> wr_en;
    sc_out<int> wr_data;
    sc_in<bool> full;
    sc_out<bool> rd_en;
    sc_in<int> rd_data;
    sc_in<bool> empty;

    SC_CTOR(TestBench){
        SC_THREAD(producer);
        sensitive << clk.pos();
        SC_THREAD(consumer);
        sensitive << clk.pos();
    }
    void producer()
    {
        wait(5, SC_NS);
        for (int i = 0; i < 10; ++i) {
            wr_en.write(false);
            wait(clk.posedge_event());
            while (full.read()) wait(clk.posedge_event());   // poll every cycle until room
            wr_data.write(i);
            wr_en.write(true);

            cout << "[PRODUCER Wrote] data: " << i << " at time " << sc_time_stamp() << endl;

            wait(clk.posedge_event());
        }
        wr_en.write(false);
    }
    void consumer()
    {
        wait(5, SC_NS);
        for (int i = 0; i < 10; ++i) {
            rd_en.write(false);
            wait(clk.posedge_event());
            while (empty.read()) wait(clk.posedge_event());

            cout << "[CONSUMER Read] data: " << rd_data.read() << " at time " << sc_time_stamp() << endl;

            rd_en.write(true);
            wait(clk.posedge_event());
        }
        rd_en.write(false);
    }

};

int sc_main(int, char* [])
{
    sc_clock clk_sig("clk_sig", 10, SC_NS);
    sc_signal<bool> rst_sig;
    sc_signal<bool> wr_en_sig;
    sc_signal<int>  wr_data_sig;
    sc_signal<bool> full_sig;
    sc_signal<bool> rd_en_sig;
    sc_signal<int>  rd_data_sig;
    sc_signal<bool> empty_sig;

    CustomFifo<int, 4> fifo("fifo");
    fifo.clk(clk_sig);
    fifo.rst(rst_sig);
    fifo.wr_en(wr_en_sig);
    fifo.wr_data(wr_data_sig);
    fifo.full(full_sig);
    fifo.rd_en(rd_en_sig);
    fifo.rd_data(rd_data_sig);
    fifo.empty(empty_sig);

    TestBench tb("tb");
    tb.clk(clk_sig);
    tb.rst(rst_sig);
    tb.wr_en(wr_en_sig);
    tb.wr_data(wr_data_sig);
    tb.full(full_sig);
    tb.rd_en(rd_en_sig);
    tb.rd_data(rd_data_sig);
    tb.empty(empty_sig);

    sc_start(500, SC_NS);
    return 0;
}