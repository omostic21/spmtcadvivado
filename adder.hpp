#ifndef ADDER_HPP
#define ADDER_HPP

#include "quantization.hpp"
#include "core.hpp"
#include <algorithm> // Make sure this is included for std::min

class Adder {
public:
    // Function to add two quantized numbers
    static QuantizedValue add(const QuantizedValue& a, const QuantizedValue& b) {
        // Ensuring both numbers are at the same quantization level
        unsigned int targetQuantLevel = std::min(a.quantLevel, b.quantLevel);
        // In adder.hpp, adjust the calls to match the method signature
        float aValue = Quantizer::dequantize(a);
        float bValue = Quantizer::dequantize(b);

        float result = aValue + bValue;

        // Quantizing result to the target quantization level
        return Quantizer::quantize(result, targetQuantLevel);
    }
};

void final_adder(data_stream* from1, data_stream* to1);
void accumulator(data_stream* from1, data_stream* to1);

#endif // ADDER_HPP
