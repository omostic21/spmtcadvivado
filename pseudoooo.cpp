#include <ap_int.h> // For arbitrary precision integers
#include <hls_stream.h> // For HLS stream interface

// Define constants for quantization and packing
#define MAX_BIT_WIDTH 16
#define PACKED_WORD_SIZE 32
#define NUM_ELEMENTS 1024 // Example size, adjust as necessary

// Define the data structure for the packed data
struct PackedData {
    ap_uint<PACKED_WORD_SIZE> data;
    ap_uint<8> bit_width; // Assuming 8 bits are enough to hold the bit-width value
};

// Pseudo code function for dynamic quantization and packing
void quantizepack(hls::stream<float>& input_stream, 
                               hls::stream<PackedData>& output_stream) {
    // Specify the interface for the streams
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream

    // Ensure the function is fully pipelined, achieving II=1
    #pragma HLS PIPELINE II=1

    // Local variables
    ap_uint<PACKED_WORD_SIZE> packed_word = 0;
    ap_uint<8> current_bit_position = 0;
    
    PackedData output;

    // Main loop for processing all elements
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_min max=c_max // Provide min and max trip count values for better estimation
        #pragma HLS UNROLL factor=1 // Disables loop unrolling

        // Read a value from the input stream
        float value = input_stream.read();
        
        // Dynamic quantization logic (placeholder, replace with actual quantization logic)
        ap_uint<MAX_BIT_WIDTH> QuantizedValuevalue = quantization(value); // Function to dynamically quantize the value
        
        // Determine the bit width to use for this value 
        
   
        // Check if there is enough space in the current packed word
        if (current_bit_position + bit_width <= PACKED_WORD_SIZE) {
            // Pack the quantized value into the packed_word
            packed_word |= (quantized_value << current_bit_position);
            current_bit_position += bit_width;
        } else {
            // Output the current packed_word and reset for the next word
            output.data = packed_word;
            output.bit_width = current_bit_position; // Record the used bit width
            output_stream.write(output); // Write to the output stream
            
            // Reset the packed_word and current_bit_position
            packed_word = quantized_value;
            current_bit_position = bit_width;
        }
    }
    
    // Output the final packed_word
    if (current_bit_position > 0) {
        output.data = packed_word;
        output.bit_width = current_bit_position; // Record the used bit width
        output_stream.write(output); // Write to the output stream
    }
}

// Function to quantize 
ap_uint<MAX_BIT_WIDTH> quantization(float value) {
    #pragma HLS INLINE
const fixed_t scale = (1 << (level - 1)) - 1;  // Compute scale using fixed-point arithmetic

        fixed_t quantizedValue = fixed_t(value);  // Convert float directly to fixed-point
        quantizedValue = (quantizedValue * scale) - scale;  // Apply scale and offset in one operation
        quantizedValue = fixed_t(int(quantizedValue));  // Truncate fractional part to simulate rounding

        return QuantizedValue(quantizedValue, level);
}

// Function to determine the bit width for a quantized value (placeholder)
ap_uint<8> determine_bit_width(float value) {
    #pragma HLS INLINE
    // Implement logic to determine the bit width based on
    // the value's magnitude and other criteria.
}
