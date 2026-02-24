#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <iostream>

using namespace sc_core;
using namespace tlm;
using namespace std;

// ==========================================================
// Smart Room Controller Module (Target)
// ==========================================================

struct SmartController : sc_module {

    tlm_utils::simple_target_socket<SmartController> socket;

    // Threshold values
    int temp_threshold;
    int humidity_threshold;

    // Current fan state
    int fan_speed;

    SC_CTOR(SmartController)
        : socket("socket"),
          temp_threshold(30),
          humidity_threshold(70),
          fan_speed(0)
    {
        socket.register_b_transport(this,
                                    &SmartController::b_transport);
    }

    /*
      Transaction Format:
      CPU writes 3 integers in order:
      [temperature, humidity, motion]
    */

    void b_transport(tlm_generic_payload& trans,
                     sc_time& delay)
    {
        int* data =
            reinterpret_cast<int*>(trans.get_data_ptr());

        int temp = data[0];
        int hum  = data[1];
        int motion = data[2];

        cout << "\n--- New Sensor Reading ---\n";
        cout << "Temperature: " << temp << " °C\n";
        cout << "Humidity   : " << hum  << " %\n";
        cout << "Motion     : " << motion << "\n";

        // Decision Logic
        if (motion == 0) {
            fan_speed = 0;
            cout << "No motion detected → Fan OFF\n";
        }
        else {
            if (temp > temp_threshold) {
                fan_speed = 3;
                cout << "Temp > " << temp_threshold
                     << " → High Speed\n";
            }
            else if (hum > humidity_threshold) {
                fan_speed = 2;
                cout << "Humidity > " << humidity_threshold
                     << " → Medium Speed\n";
            }
            else {
                fan_speed = 1;
                cout << "Comfortable → Low Speed\n";
            }
        }

        cout << "Fan Current State = "
             << fan_speed << "\n";

        delay += sc_time(10, SC_NS);
        trans.set_response_status(TLM_OK_RESPONSE);
    }
};
