#include<systemc.h>
using namespace std;

SC_MODULE(mux)
{
    sc_in<int> i1, i2, i3, i4;
    sc_in<bool> sel1, sel2;
    sc_out<int> out;

    SC_CTOR(mux)
    {
        SC_METHOD(do_mux);
        sensitive << i1 << i2 << i3 << i4 << sel1 << sel2;
    }

    void do_mux()
    {   
        int sel = (sel1.read() << 1 ) | sel2.read();
        
        switch (sel) 
        {
            case 0: out.write(i1.read()); 
                    break;
            case 1: out.write(i2.read()); 
                    break;
            case 2: out.write(i3.read()); 
                    break; 
            case 3: out.write(i4.read()); 
                    break;
        }
    }
};

SC_MODULE(testbench)
{
    sc_out<int> i1, i2, i3, i4;
    sc_out<bool> sel1, sel2;
    sc_in<int> out;
    
    SC_CTOR(testbench)
    {
        SC_THREAD(run);
    }

    void run()
    {
        cout << "Test the simulation of 4 to 1 Multiplexer" << endl;

        struct testcases {
                int i1;
                int i2;
                int i3;
                int i4;
                bool sel1;
                bool sel2;
                int expected;
            };

        // Each test case: { i1, i2, i3, i4, sel1, sel2, expected }
        testcases cases[] = {
            { 1, 2, 3, 4, 0, 0, 1 },
            { 1, 2, 3, 4, 0, 1, 2 },
            { 1, 2, 3, 4, 1, 0, 3 },
            { 1, 2, 3, 4, 1, 1, 4 },
            //fail cases too
            // { 1, 2, 3, 4, 0, 0, 2 },
            // { 1, 2, 3, 4, 0, 1, 3 },
            // { 1, 2, 3, 4, 1, 0, 4 },
            // { 1, 2, 3, 4, 1, 1, 1 }
        };
        

        for (auto& tc: cases)
        {
            i1.write(tc.i1);
            i2.write(tc.i2);
            i3.write(tc.i3);
            i4.write(tc.i4);
            sel1.write(tc.sel1);
            sel2.write(tc.sel2);

            wait(1, SC_NS);

            int accepted = out.read();
            bool ok = (accepted == tc.expected);
            cout << "i1 = " << tc.i1 << " i2 = " << tc.i2 << " i3 = " << tc.i3 << " i4 = " << tc.i4
                 << " sel1 = " << tc.sel1 << " sel2 = " << tc.sel2
                 << " expected = " << tc.expected << " actual = " << accepted
                 << " -> " << (ok ? "PASS" : "FAIL") << endl;
        }
    }
};

int sc_main(int argc, char* argv[])
{
    sc_signal<bool> sel1_sig, sel2_sig;
    sc_signal<int> i1_sig, i2_sig, i3_sig, i4_sig, out_sig;

    mux mx("Mux 4x1");
    mx.i1(i1_sig);
    mx.i2(i2_sig);
    mx.i3(i3_sig);
    mx.i4(i4_sig);
    mx.sel1(sel1_sig);
    mx.sel2(sel2_sig);
    mx.out(out_sig);

    testbench tb("Testbench");
    tb.i1(i1_sig);
    tb.i2(i2_sig);
    tb.i3(i3_sig);
    tb.i4(i4_sig);
    tb.sel1(sel1_sig);
    tb.sel2(sel2_sig);
    tb.out(out_sig);

    cout << "Starting simulation..." << endl;

    sc_start(10, SC_NS);

    cout << "Simulation completed." << endl;
    return 0;
}
