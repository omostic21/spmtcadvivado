#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "core.hpp"  // Include core.hpp to get the typedefs for stream types
#include "quantization.hpp" // Include this only if quantization logic is used within control logic

// Now that core.hpp is included, there's no need for forward declarations
// of id_stream, val_stream, port_stream, and data_stream as they are already
// defined in core.hpp through typedefs.

void control_unit(id_stream* from1, id_stream* from2, id_stream* from3, id_stream* from4, uint32_t number,
                  id_stream* to1, id_stream* to2, id_stream* c1, id_stream* c2, id_stream* c3, id_stream* c4);

void get_data(id_stream* from1, id_stream* from2, val_stream* to1, val_stream* to2, ap_uint<256>* buffer, id_t num);

void final_data(id_stream* c1, id_stream* c2, id_stream* c3, id_stream* c4, id_stream* c5, id_stream* c6, id_stream* update,
                val_stream* from1, val_stream* from2,
                val_stream* to1, val_stream* to2, val_stream* to3, val_stream* to4, val_stream* to5, val_stream* to6, id_t num);

void dr_controller(port_stream* from1, port_stream* from2, port_stream* from3, port_stream* from4, uint32_t num,
                   id_stream* to1, id_stream* to2, id_stream* c1, id_stream* c2, id_stream* c3, id_stream* c4, id_stream* c5, id_stream* c6,
                   data_stream* data1, data_stream* data2, data_stream* data3, data_stream* data4, data_stream* data5, data_stream* data6,
                   id_stream* rid1, id_stream* rid2, id_stream* rid3, id_stream* rid4, id_stream* rid5, id_stream* rid6,
                   id_stream* update, id_stream* rlen);

#endif // CONTROL_HPP
