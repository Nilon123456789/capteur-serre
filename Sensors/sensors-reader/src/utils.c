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
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}