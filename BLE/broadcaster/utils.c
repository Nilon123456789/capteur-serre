/**
 * utils.c
 * 
 * Utility functions
 * 
 * Author: Nils Lahaye 2023
 * 
*/
#include "utils.h"

/**
 * @brief Map a value from a range to another
 * 
 * @param value Value to map
 * @param inMin Minimum value of the input range
 * @param inMax Maximum value of the input range
 * @param outMin Minimum value of the output range
 * @param outMax Maximum value of the output range
 * 
 * @return The mapped value
*/

float mapRange(float value, float inMin, float inMax, float outMin, float outMax) {
    return fmaxf(fminf((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin, outMax), outMin);
}

/**
 * @brief Evaluate a polynomial
 * 
 * @param x Value to evaluate the polynomial at
 * @param coefficients[3] Array of coefficients of the polynomial
 * 
 * @return The value of the polynomial at x
*/
float evaluate_polynomial(float x, const int coefficients[3]) {
    return coefficients[0] + coefficients[1] * x + coefficients[2] * x * x;
}