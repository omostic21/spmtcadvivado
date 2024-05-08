#ifndef QUANTIZATION_HPP
#define QUANTIZATION_HPP

#include <ap_fixed.h>  // Including fixed-point arithmetic library

// Define fixed-point data types
typedef ap_fixed<16, 8> fixed_t;  // 16 bits total, 8 integer bits

struct QuantizedValue {
    fixed_t value;  // The quantized value itself, using fixed-point representation
    unsigned int quantLevel;  // The quantization level (e.g., bit-width)

    // Constructor
    QuantizedValue(fixed_t v, unsigned int qL) : value(v), quantLevel(qL) {}
};

class Quantizer {
public:
    // Function to quantize a floating-point value to a fixed-point with a given level
    static QuantizedValue quantize(float value, unsigned int level) {
        #pragma HLS INLINE
        const fixed_t scale = (1 << (level - 1)) - 1;  // Compute scale using fixed-point arithmetic

        fixed_t quantizedValue = fixed_t(value);  // Convert float directly to fixed-point
        quantizedValue = (quantizedValue * scale) - scale;  // Apply scale and offset in one operation
        quantizedValue = fixed_t(int(quantizedValue));  // Truncate fractional part to simulate rounding

        return QuantizedValue(quantizedValue, level);
    }

    // Function to dequantize a QuantizedValue back to floating-point
    static float dequantize(const QuantizedValue& qValue) {
        #pragma HLS INLINE
        const fixed_t scale = (1 << (qValue.quantLevel - 1)) - 1;

        float dequantizedValue = float((qValue.value + scale) / scale);  // Apply inverse scale directly
        return dequantizedValue;
    }
};

#endif // QUANTIZATION_HPP
