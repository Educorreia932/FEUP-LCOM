/** @file math_utils.h */

#pragma once

#include <lcom/lcf.h>
#include <math.h>

/** @defgroup math_utils */

/** @addtogroup	math_utils
 *  
 * @brief A couple extra auxiliary math related functions
 * 
 * @{
 */

/**
 * @brief Clamps the value between the established min and max
 * 
 * @param f The value to clamp
 * @param min The min value for f
 * @param max The max value for f
 * @return float The value f clamped between min and max
 */
float fclampf(float f, float min, float max);

/**
 * @brief Returns 1 or -1 according to the value's sign
 * @note The value 0 is never taken into account (0 may lead to +/- 1 because of the floating point standard)
 * 
 * @param f The value to analyse
 * @return float 1 if f >= 0, -1 otherwise
 */
float fsign(float f);

// thx Quake Arena 3
/**
 * @brief Returns The inverse of the square root of number
 * @note This function was not made by us, it is a copy from the code developed for Quake Arena 3, where the fast calculation of inverse square roots were important for 3d rendering
 * 
 * @param number The number to operate over
 * @return float The inverse of the square root of number
 */
float Q_rsqrt( float number );

/** @} */
