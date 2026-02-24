#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <vector>
#include <iostream>

using namespace sc_core;
using namespace tlm;
using namespace std;

#define MEM_SIZE 256
#define SAMPLE_COUNT 32

// ======================================================
// Memory Module
// ======================================================
struct Memory : sc_module {

    tlm_utils::simple_target_socket<Memory> socket;
    vector<unsigned char> mem;
    sc_time latency;

    SC_CTOR(Memory)
        : socket("socket"),
          mem(MEM_SIZE, 0),
          latency(sc_time(5, SC_NS))
    {
        socket.register_b_transport(this, &Memory::b_transport);
    }

    void b_transport(tlm_generic_payload& trans, sc_time& delay) {

        tlm_command cmd = trans.get_command();
        unsigned long long addr = trans.get_address();
        unsigned char* ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();

        if (addr + len > MEM_SIZE) {
            trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }

        if (cmd == TLM_READ_COMMAND)
            memcpy(ptr, &mem[addr], len);
        else if (cmd == TLM_WRITE_COMMAND)
            memcpy(&mem[addr], ptr, len);

        delay += latency;
        trans.set_response_status(TLM_OK_RESPONSE);
    }
};

// ======================================================
// DMA Controller
// ======================================================
struct DMA : sc_module {

    tlm_utils::simple_target_socket<DMA> cpu_socket;
    tlm_utils::simple_initiator_socket<DMA> mem_socket;

    unsigned int src;
    unsigned int dst;
    unsigned int size;

    SC_CTOR(DMA)
        : cpu_socket("cpu_socket"),
          mem_socket("mem_socket"),
          src(0), dst(0), size(0)
    {
        cpu_socket.register_b_transport(this, &DMA::b_transport);
    }

    void b_transport(tlm_generic_payload& trans, sc_time& delay) {

        unsigned long long addr = trans.get_address();
        unsigned int* data =
            reinterpret_cast<unsigned int*>(trans.get_data_ptr());

        if (addr == 0x00) src = *data;
        else if (addr == 0x04) dst = *data;
        else if (addr == 0x08) size = *data;
        else if (addr == 0x0C) start_transfer();

        trans.set_response_status(TLM_OK_RESPONSE);
    }

    void start_transfer() {

        sc_time delay = SC_ZERO_TIME;
        unsigned char buffer;

        for (unsigned int i = 0; i < size; i++) {

            tlm_generic_payload trans;

            // READ
            trans.set_command(TLM_READ_COMMAND);
            trans.set_address(src + i);
            trans.set_data_ptr(&buffer);
            trans.set_data_length(1);
            mem_socket->b_transport(trans, delay);

            // WRITE
            trans.set_command(TLM_WRITE_COMMAND);
            trans.set_address(dst + i);
            mem_socket->b_transport(trans, delay);
        }

        cout << "\nDMA Logging Complete\n";
        cout << "Total Transfer Time: " << delay << endl;
        cout << "Effective Bandwidth: "
             << (double)size / delay.to_seconds()
             << " Bytes/sec\n\n";
    }
};

// ======================================================
// CPU Module
// ======================================================
struct CPU : sc_module {

    tlm_utils::simple_initiator_socket<CPU> socket;

    SC_CTOR(CPU) : socket("socket") {
        SC_THREAD(run);
    }

    void write_reg(unsigned long long addr, unsigned int data) {

        tlm_generic_payload trans;
        sc_time delay = SC_ZERO_TIME;

        trans.set_command(TLM_WRITE_COMMAND);
        trans.set_address(addr);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));
        trans.set_data_length(4);

        socket->b_transport(trans, delay);
    }

    void run() {

        wait(10, SC_NS);

        write_reg(0x00, 0);
        write_reg(0x04, 128);
        write_reg(0x08, SAMPLE_COUNT);
        write_reg(0x0C, 1);
    }
};

// ======================================================
// sc_main
// ======================================================
int sc_main(int argc, char* argv[]) {

    Memory mem("Memory");
    DMA dma("DMA");
    CPU cpu("CPU");

    cpu.socket.bind(dma.cpu_socket);
    dma.mem_socket.bind(mem.socket);

    for (int i = 0; i < SAMPLE_COUNT; i++)
        mem.mem[i] = i + 1;

    sc_start();
    return 0;
}
