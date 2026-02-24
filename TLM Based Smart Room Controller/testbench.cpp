#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <cstdlib>
#include <ctime>
#include "design.cpp"

using namespace sc_core;
using namespace tlm;
using namespace std;

// ==========================================================
// Testbench (Random Stimulus Generator)
// ==========================================================

struct Testbench : sc_module {

    tlm_utils::simple_initiator_socket<Testbench> socket;

    SC_CTOR(Testbench)
        : socket("socket")
    {
        SC_THREAD(run);
        srand(time(0));
    }

    void run() {

        for (int i = 0; i < 5; i++) {

            wait(20, SC_NS);

            int data[3];

            // Randomized inputs
            data[0] = 20 + rand() % 20;  // Temp: 20–39
            data[1] = 40 + rand() % 50;  // Humidity: 40–89
            data[2] = rand() % 2;        // Motion: 0 or 1

            tlm_generic_payload trans;
            sc_time delay = SC_ZERO_TIME;

            trans.set_command(TLM_WRITE_COMMAND);
            trans.set_data_ptr(
                reinterpret_cast<unsigned char*>(data));
            trans.set_data_length(3 * sizeof(int));

            socket->b_transport(trans, delay);
        }

        cout << "\nSimulation Complete\n";
    }
};


// ==========================================================
// sc_main
// ==========================================================

int sc_main(int argc, char* argv[])
{
    SmartController controller("Controller");
    Testbench tb("TB");

    tb.socket.bind(controller.socket);

    sc_start();

    return 0;
}
