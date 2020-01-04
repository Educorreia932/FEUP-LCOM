/** @file math_utils.h */

#pragma once

#include <lcom/lcf.h>
#include <math.h>

/** @defgroup math_utils Math Utilities */

/** @addtogroup	math_utils
 *  
 * @brief A couple extra auxiliary math related functions
 * 
 * @{
 */

/**
 * @brief Returns 1 or -1 according to the value's sign
 * @note The value 0 is never taken into account (0 may lead to +/- 1 because of the floating point standard)
 * 
 * @param f The value to analyse
 * @return float 1 if f >= 0, -1 otherwise
 */
float fsign(float f);

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
 * @brief Retuns the minimum of two numbers
 * 
 * @param a One uint8_t
 * @param b Another uint8_t
 * @return uint8_t The smaller number between a and b
 */
uint8_t min_uint8(uint8_t a, uint8_t b);
/**
 * @brief Retuns the maximum of two numbers
 * 
 * @param a One uint8_t
 * @param b Another uint8_t
 * @return uint8_t The larger number between a and b
 */
uint8_t max_uint8(uint8_t a, uint8_t b);
/**
 * @brief Clamps the value between the established min and max
 * 
 * @param n The value to clamp
 * @param min The min value for n
 * @param max The max value for n
 * @return uint8_t The value n clamped between min and max
 */
uint8_t clamp_uint8(uint8_t n, uint8_t min, uint8_t max);
/**
 * @brief Retuns the minimum of two numbers
 * 
 * @param a One uint16_t
 * @param b Another uint16_t
 * @return uint16_t The smaller number between a and b
 */
uint16_t min_uint16(uint16_t a, uint16_t b);
/**
 * @brief Retuns the maximum of two numbers
 * 
 * @param a One uint16_t
 * @param b Another uint16_t
 * @return uint16_t The larger number between a and b
 */
uint16_t max_uint16(uint16_t a, uint16_t b);
/**
 * @brief Clamps the value between the established min and max
 * 
 * @param n The value to clamp
 * @param min The min value for n
 * @param max The max value for n
 * @return uint16_t The value n clamped between min and max
 */
uint16_t clamp_uint16(uint16_t n, uint16_t min, uint16_t max);

/** 
 * @brief Function to return the digit of n-th position of num. Position starts from 0
 * 
 * @param num The number to the digit of
 * @param n The position of the digit
 * @return uint8_t The digit at the n-th position of num
 */
uint8_t get_digit(uint16_t num, uint8_t n);

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
