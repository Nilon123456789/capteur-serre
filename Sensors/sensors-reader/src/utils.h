/**
 * utils.h
 * 
 * Utility functions
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#ifndef UTILS_H_
#define UTILS_H_

#include <math.h>

#define TO_STRING(x) #x
#define LOCATION __FILE__ ":" TO_STRING(__LINE__)

#define RET_IF_ERR(expr, msg)                                   \
    {                                                           \
        int ret = (expr);                                       \
        if(ret) {                                               \
            LOG_ERR("Error %d: " msg " in " LOCATION, ret);     \
            return ret;                                         \
        }                                                       \
    }

float mapRange(float value, float inMin, float inMax, float outMin, float outMax);

float evaluate_polynomial(float x, const int coefficients[3]);

#endif /* UTILS_H_ */