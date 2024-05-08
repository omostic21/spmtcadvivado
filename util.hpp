#ifndef UTIL_HPP
#define UTIL_HPP

#include "core.hpp"
#include <hls_stream.h>

// Function prototypes remain unchanged to ensure compatibility with other files
void load_matrix_value(id_stream* len_row_batch, ap_uint<128>* from1,
                       data_stream* to1, data_stream* to2, uint32_t total_num) {
    uint32_t counter = 0;
    id_t temp_number = len_row_batch->read();
    for (uint32_t i = 0; i < total_num; ++i) {
#pragma HLS PIPELINE II=1
        ap_uint<128> temp = from1[i];
        val_data<val_t> data1, data2;
        data1.data = temp.range(63, 0).to_uint64();
        data2.data = temp.range(127, 64).to_uint64();

        bool partial_end = ++counter < temp_number;
        data1.partial_end = data2.partial_end = partial_end;
        bool final_end = i + 1 < total_num;
        data1.final_end = data2.final_end = final_end;

        to1->write(data1);
        to2->write(data2);

        if (!partial_end) {
            counter = 0;
            temp_number = len_row_batch->read();
        }
    }
}

void load_row_index(ap_uint<128>* from1, id_stream* to1, id_stream* to2,
                    id_stream* to3, id_stream* to4, uint32_t total_number) {
    for (uint32_t i = 0; i < total_number; ++i) {
#pragma HLS PIPELINE II=1
        ap_uint<128> temp = from1[i];
        to1->write(temp.range(31, 0).to_uint());
        to2->write(temp.range(63, 32).to_uint());
        to3->write(temp.range(95, 64).to_uint());
        to4->write(temp.range(127, 96).to_uint());
    }
}

void load_col_index(ap_uint<128>* from1, id_stream* to1,
                    id_stream* to2, id_stream* to3, id_stream* to4, uint32_t total_num) {
    for (uint32_t i = 0; i < total_num; ++i) {
#pragma HLS PIPELINE II=1
        ap_uint<128> temp = from1[i];
        to1->write(temp.range(31, 0).to_uint());
        to2->write(temp.range(63, 32).to_uint());
        to3->write(temp.range(95, 64).to_uint());
        to4->write(temp.range(127, 96).to_uint());
    }
}

void rowlength_split(id_stream* from1, id_stream* to1, id_stream* to2) {
    while (!from1->empty()) {
#pragma HLS PIPELINE II=1
        id_t temp = from1->read();
        to1->write(temp);
        to2->write(temp);
    }
}

void load_segment_port(ap_uint<128>* port, port_stream* ps, id_t num) {
    for (id_t i = 0; i < num; ++i) {
#pragma HLS PIPELINE II=1
        ps->write(port[i]);
    }
}

void coalesce(data_stream* from0, data_stream* from1, data_stream* from2,
              data_stream* from3, data_stream* from4, data_stream* from5, data_stream* from6,
              data_stream* from7, data_stream* from8, data_stream* from9, data_stream* from10,
              data_stream* from11, data_stream* from12, data_stream* from13, data_stream* from14,
              data_stream* from15, data_stream* from16, data_stream* from17, data_stream* from18,
              data_stream* from19, data_stream* from20, data_stream* from21, data_stream* from22,
              data_stream* from23, data_stream* from24, data_stream* from25, data_stream* from26,
              data_stream* from27, data_stream* from28, data_stream* from29, data_stream* from30,
              data_stream* from31, val_stream* re){

	// This function is used to ensure the order of the final result vector elements
	for (bool enable=true; enable; ){
		val_data<val_t> temp = from0->read();
		val_data<val_t> temp1 = from1->read();
		val_data<val_t> temp2 = from2->read();
		val_data<val_t> temp3 = from3->read();
		val_data<val_t> temp4 = from4->read();
		val_data<val_t> temp5 = from5->read();
		val_data<val_t> temp6 = from6->read();
		val_data<val_t> temp7 = from7->read();
		val_data<val_t> temp8 = from8->read();
		val_data<val_t> temp9 = from9->read();
		val_data<val_t> temp10 = from10->read();
		val_data<val_t> temp11 = from11->read();
		val_data<val_t> temp12 = from12->read();
		val_data<val_t> temp13 = from13->read();
		val_data<val_t> temp14 = from14->read();
		val_data<val_t> temp15 = from15->read();
		val_data<val_t> temp16 = from16->read();
		val_data<val_t> temp17 = from17->read();
		val_data<val_t> temp18 = from18->read();
		val_data<val_t> temp19 = from19->read();
		val_data<val_t> temp20 = from20->read();
		val_data<val_t> temp21 = from21->read();
		val_data<val_t> temp22 = from22->read();
		val_data<val_t> temp23 = from23->read();
		val_data<val_t> temp24 = from24->read();
		val_data<val_t> temp25 = from25->read();
		val_data<val_t> temp26 = from26->read();
		val_data<val_t> temp27 = from27->read();
		val_data<val_t> temp28 = from28->read();
		val_data<val_t> temp29 = from29->read();
		val_data<val_t> temp30 = from30->read();
		val_data<val_t> temp31 = from31->read();

		re->write(temp.data);
		re->write(temp1.data);
		re->write(temp2.data);
		re->write(temp3.data);
		re->write(temp4.data);
		re->write(temp5.data);
		re->write(temp6.data);
		re->write(temp7.data);
		re->write(temp8.data);
		re->write(temp9.data);
		re->write(temp10.data);
		re->write(temp11.data);
		re->write(temp12.data);
		re->write(temp13.data);
		re->write(temp14.data);
		re->write(temp15.data);
		re->write(temp16.data);
		re->write(temp17.data);
		re->write(temp18.data);
		re->write(temp19.data);
		re->write(temp20.data);
		re->write(temp21.data);
		re->write(temp22.data);
		re->write(temp23.data);
		re->write(temp24.data);
		re->write(temp25.data);
		re->write(temp26.data);
		re->write(temp27.data);
		re->write(temp28.data);
		re->write(temp29.data);
		re->write(temp30.data);
		re->write(temp31.data);

		enable = temp1.final_end;
	}
}

void store_result_to_DDR(val_stream* from1, val_t* to1, uint32_t number) {
    for (uint32_t i = 0; i < number; ++i) {
#pragma HLS PIPELINE II=1
        to1[i] = from1->read();
    }
}
void load_vector(hls::stream<ap_uint<128> > * from, ap_uint<128>* vector_pointer, uint32_t number){

    for (uint32_t i=0; i<number; i++){
#pragma HLS PIPELINE II=1
        ap_uint<128> temp = vector_pointer[i];
        from->write(temp);
    }
}

void store_to_vectorbuffer(hls::stream<ap_uint<128> > * from, hls::stream<ap_uint<128> > * from2,
		hls::stream<ap_uint<128> > * from3, hls::stream<ap_uint<128> > * from4, ap_uint<256>* buffer, uint32_t number){
	// merge the four 128bits data into two 256bits data and store it in vector buffer
    for (uint32_t i=0; i<number; i++){
#pragma HLS PIPELINE II=1
    	ap_uint<256> temp;
    	ap_uint<256> temp1;
    	temp(127, 0)   = from->read();
    	temp(255, 128) = from2->read();
    	temp1(127, 0)   = from3->read();
    	temp1(255, 128) = from4->read();
    	buffer[i*2] = temp;
    	buffer[i*2+1] = temp1;
    }
}

void load_row_batch_length(id_stream* row_stream, uint32_t* row_pointer, uint32_t number){

	// help control the whole dataflow engine
    for (uint32_t i=0; i<number; i++){
#pragma HLS PIPELINE II=1
        id_t temp = row_pointer[i];
        row_stream->write(temp);
    }

    // to avoid the pipeline stall, pad one element
    row_stream->write(0);
}

void load_configuration(ap_uint<256>* buffer, id_stream* row_stream,
		ap_uint<128>* vector_pointer, ap_uint<128>* vector_pointer2, ap_uint<128>* vector_pointer3, ap_uint<128>* vector_pointer4,
		uint32_t vector_number, uint32_t* row_pointer, uint32_t number_row_batch){

#pragma HLS DATAFLOW

	hls::stream<ap_uint<128> > vector_stream1("vector_stream1");
#pragma HLS stream variable=vector_stream1 depth=2
	hls::stream<ap_uint<128> > vector_stream2("vector_stream2");
#pragma HLS stream variable=vector_stream2 depth=2
	hls::stream<ap_uint<128> > vector_stream3("vector_stream3");
#pragma HLS stream variable=vector_stream3 depth=2
	hls::stream<ap_uint<128> > vector_stream4("vector_stream4");
#pragma HLS stream variable=vector_stream4 depth=2

	// Concurrently read vectors from the four HP ports
    load_vector(&vector_stream1, vector_pointer, vector_number);
    load_vector(&vector_stream2, vector_pointer2, vector_number);
    load_vector(&vector_stream3, vector_pointer3, vector_number);
    load_vector(&vector_stream4, vector_pointer4, vector_number);
    store_to_vectorbuffer(&vector_stream1, &vector_stream2, &vector_stream3, &vector_stream4, buffer, vector_number);

    load_row_batch_length(row_stream, row_pointer, number_row_batch);

}

#endif
