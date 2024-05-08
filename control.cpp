#include "control.hpp"

void control_unit(id_stream* from1, id_stream* from2, id_stream* from3, id_stream* from4, uint32_t number,
		id_stream* to1, id_stream* to2, id_stream* c1, id_stream* c2, id_stream* c3, id_stream* c4){

	// This unit is used to generate the control instructions
	// Instructions: 32 -- REUSE1, 64 -- REUSE2, 2 -- LOAD1 or BUFFER1, 4 -- LOAD2 or BUFFER2
	//               8/16: used to control if no intra-/inter- reuse

	bool keepInput = false;
	uint32_t index1, index2, index3, index4;
	uint32_t last_index1, last_index2;

	for (uint32_t i=0; i<number; ){
#pragma HLS PIPELINE II=1
		if (!keepInput){
			index1 = from1->read();
			index2 = from2->read();
			index3 = from3->read();
			index4 = from4->read();

			bool e1 = index1 == index2;
			bool e2 = index2 == index3;
			bool e3 = index3 == index4;

			// if the four cids can be merged into two column indices
			if (e1 && e2 && e3) {
				to1->write(index1);
				to2->write(index1);
				c1->write(2);
				c2->write(2);
				c3->write(2);
				c4->write(2);

				last_index1 = index1;
				last_index2 = index1;

				i++;
			} else if (e1 && (!e2) && e3) {
				to1->write(index1);
				to2->write(index3);
				c1->write(2);
				c2->write(2);
				c3->write(4);
				c4->write(4);

				last_index1 = index1;
				last_index2 = index3;

				i++;
			} else if (!e1 && e2 && e3) {
				to1->write(index1);
				to2->write(index3);
				c1->write(2);
				c2->write(4);
				c3->write(4);
				c4->write(4);

				last_index1 = index1;
				last_index2 = index3;

				i++;
			} else if (e1 && e2 && !e3) {
				to1->write(index1);
				to2->write(index4);
				c1->write(2);
				c2->write(2);
				c3->write(2);
				c4->write(4);

				last_index1 = index1;
				last_index2 = index4;

				i++;
			} else {
				// if can't be merged into two column indices
				// there are two cases: case 1: reuse; case 2: can't reuse
				// general case:
				// cycle 0: index1 index1 index2 index2
				// cycle 1: index1 index2 newindex1 newindex2
				// the columns we selected have more than 3 elements
				if (i>0) {
					if (index1 == last_index1 && index2 == last_index2) {
						to1->write(index3);
						to2->write(index4);

						c1->write(32);
						c2->write(64);
						c3->write(2);
						c4->write(4);

						i++;
					} else if (e2 && index1 == last_index1) {
						to1->write(index2);
						to2->write(index4);

						c1->write(32);
						c2->write(2);
						c3->write(2);
						c4->write(4);

						last_index2 = index2;

						i++;
					} else if (e2 && index1 == last_index2) {
						to1->write(index2);
						to2->write(index4);

						c1->write(64);
						c2->write(2);
						c3->write(2);
						c4->write(4);

						last_index1 = index1;
						last_index2 = index2;

						i++;
					} else if (index1 == last_index1 && !e2) {
						to1->write(index2);
						to2->write(index3);

						c1->write(32);
						c2->write(2);
						c3->write(4);
						c4->write(4);

						last_index2 = index2;

						i++;
					} else if (index1 == last_index2 && !e2) {
						to1->write(index2);
						to2->write(index3);

						c1->write(64);
						c2->write(2);
						c3->write(4);
						c4->write(4);

						last_index1 = index1;

						i++;
					} else {
						to1->write(index1);
						to2->write(index2);
						c1->write(2);
						c2->write(4);
						c3->write(8);
						c4->write(16);
						keepInput = true;
					}

				} else {
					// need two more cycles
					to1->write(index1);
					to2->write(index2);
					c1->write(2);
					c2->write(4);
					c3->write(8);
					c4->write(16);
					keepInput = true;
				}
			}
		} else {
			to1->write(index3);
			to2->write(index4);

			i++;
			keepInput = false;
		}
	}

	to1->write(TERMINATE);
	to2->write(TERMINATE);
	c1->write(TERMINATE);
	c2->write(TERMINATE);
}

void get_data(id_stream* from1, id_stream* from2, val_stream* to1, val_stream* to2, ap_uint<256>* buffer, id_t num) {

	val_t _c_0 = (val_t)0;

	for (id_t i=0; i<num; i++){
#pragma HLS PIPELINE II=1
		id_t index1 = from1->read();
		id_t index2 = from2->read();

		ap_uint<64> data1 = buffer[index1/4](((index1%4)+1)*64-1, (index1%4)*64);
		ap_uint<64> data2 = buffer[index2/4](((index2%4)+1)*64-1, (index2%4)*64);
		to1->write(Reinterpret<val_t>(static_cast<ap_uint<64> >(data1)));
		to2->write(Reinterpret<val_t>(static_cast<ap_uint<64> >(data2)));
	}
}

void final_data(id_stream* c1, id_stream* c2, id_stream* c3, id_stream* c4, id_stream* c5, id_stream* c6, id_stream* update,
		val_stream* from1, val_stream* from2,
		val_stream* to1, val_stream* to2, val_stream* to3, val_stream* to4, val_stream* to5, val_stream* to6, id_t num){

	id_t cdata1, cdata2, cdata3, cdata4, cdata5, cdata6;
	val_t final_data1, final_data2, final_data3, final_data4, final_data5, final_data6;
	val_t last_data1, last_data2;
	id_t updated;

	val_t _c_0 = (val_t)0;

	bool enable = false;
	for (id_t i=0; i<num; ){
#pragma HLS PIPELINE II=1

		if (!from1->empty() && !from2->empty()) {
			cdata1 = c1->read();
			cdata2 = c2->read();
			cdata3 = c3->read();
			cdata4 = c4->read();
			cdata5 = c5->read();
			cdata6 = c6->read();
			updated = update->read();

			val_t data1 = from1->read();
			val_t data2 = from2->read();

			final_data1 = (cdata1 == 2) ? last_data1 : ((cdata1 == 0) ? data1 : data2);
			final_data2 = (cdata2 == 2) ? last_data1 : ((cdata2 == 0) ? data1 : data2);
			final_data3 = (cdata3 == 2) ? last_data1 : ((cdata3 == 0) ? data1 : data2);
			final_data4 = (cdata4 == 2) ? last_data1 : ((cdata4 == 0) ? data1 : data2);
			final_data5 = (cdata5 == 2) ? last_data1 : ((cdata5 == 0) ? data1 : data2);
			final_data6 = (cdata6 == 2) ? last_data1 : ((cdata6 == 0) ? data1 : data2);

			to1->write(final_data1);
			to2->write(final_data2);
			to3->write(final_data3);
			to4->write(final_data4);
			to5->write(final_data5);
			to6->write(final_data6);

			last_data1 = (updated == 1) ? data1 : last_data1;

			i++;
		}
	}
}

void dr_controller(port_stream* from1, port_stream* from2, port_stream* from3, port_stream* from4, uint32_t num,
		id_stream* to1, id_stream* to2, id_stream* c1, id_stream* c2, id_stream* c3, id_stream* c4, id_stream* c5, id_stream* c6,
		data_stream* data1, data_stream* data2, data_stream* data3, data_stream* data4, data_stream* data5, data_stream* data6,
		id_stream* rid1, id_stream* rid2, id_stream* rid3, id_stream* rid4, id_stream* rid5, id_stream* rid6,
		id_stream* update, id_stream* rlen){

	val_t vb[6];
#pragma HLS ARRAY_PARTITION variable=vb complete dim=1
	id_t rb[6];
#pragma HLS ARRAY_PARTITION variable=rb complete dim=1
	id_t cb[2];
#pragma HLS ARRAY_PARTITION variable=cb complete dim=1
	id_t mb[6];
#pragma HLS ARRAY_PARTITION variable=mb complete dim=1

	ap_uint<512> curr_group;

	id_t Nr, Mr, curr_end_idx, updated;

	val_data<val_t> datad1, datad2, datad3, datad4, datad5, datad6;
	id_t ridd1, ridd2, ridd3, ridd4, ridd5, ridd6;
	id_t cidd1, cidd2;

    id_t counter = 0;
    id_t temp_counter = 0;
	id_t temp_number = rlen->read();

	for (id_t i=0; i<num; ) {
#pragma HLS PIPELINE II=1
		curr_group(127, 0) = from1->read();
		curr_group(255, 128) = from2->read();
		curr_group(383, 256) = from3->read();
		curr_group(511, 384) = from4->read();

		Nr = curr_group(2, 0);
		Mr = curr_group(4, 3);

		curr_end_idx = 5;
		for (id_t ii=0; ii<6; ii++){
#pragma HLS UNROLL
			vb[ii] = Reinterpret<val_t>(static_cast<ap_uint<64> >(curr_group(curr_end_idx+ii*64+63, curr_end_idx+ii*64)));
		}

		curr_end_idx += (Nr+Mr) * 64;

		for (id_t ii=0; ii<6; ii++){
#pragma HLS UNROLL
			rb[ii] = curr_group(curr_end_idx + 4 + ii*5, curr_end_idx + ii*5);
		}

		curr_end_idx += (Nr+Mr) * 5;

		for (id_t ii=0; ii<2; ii++) {
#pragma HLS UNROLL
			cb[ii] = curr_group(curr_end_idx + 31 + ii*32, curr_end_idx + ii*32);
		}

		curr_end_idx += Mr*32;

		for (id_t ii=0; ii<6; ii++) {
#pragma HLS UNROLL
			mb[ii] = curr_group(curr_end_idx + 1 + ii*2, curr_end_idx + ii*2);
		}

		curr_end_idx += (Nr+Mr)*2;

		updated = curr_group[curr_end_idx];

		for (id_t ii=0; ii<6; ii++) {
#pragma HLS UNROLL
			if (mb[ii] == 3)
				rb[ii] = TERMINATE;
		}

		for (id_t ii=0; ii<2; ii++) {
#pragma HLS UNROLL
			if (cb[ii] == TERMINATE)
				cb[ii] = 0;

		}

		c1->write(mb[0]);
		c2->write(mb[1]);
		c3->write(mb[2]);
		c4->write(mb[3]);
		c5->write(mb[4]);
		c6->write(mb[5]);

        id_t next = i+1;
        temp_counter = counter+1;

		datad1.data = vb[0];
        datad1.partial_end = (temp_counter < temp_number);
        datad1.final_end = (next < num);
        data1->write(datad1);

		datad2.data = vb[1];
        datad2.partial_end = (temp_counter < temp_number);
        datad2.final_end = (next < num);
        data2->write(datad2);

		datad3.data = vb[2];
        datad3.partial_end = (temp_counter < temp_number);
        datad3.final_end = (next < num);
        data3->write(datad3);

		datad4.data = vb[3];
        datad4.partial_end = (temp_counter < temp_number);
        datad4.final_end = (next < num);
        data4->write(datad4);

		datad5.data = vb[4];
        datad5.partial_end = (temp_counter < temp_number);
        datad5.final_end = (next < num);
        data5->write(datad5);

		datad6.data = vb[5];
        datad6.partial_end = (temp_counter < temp_number);
        datad6.final_end = (next < num);
        data6->write(datad6);

        rid1->write(rb[0]);
        rid2->write(rb[1]);
        rid3->write(rb[2]);
        rid4->write(rb[3]);
        rid5->write(rb[4]);
        rid6->write(rb[5]);

        to1->write(cb[0]);
        to2->write(cb[1]);

        update->write(updated);

        counter = (temp_counter < temp_number)? temp_counter:0;
        temp_number = (temp_counter < temp_number)? temp_number:rlen->read();
        i = next;
	}
}
