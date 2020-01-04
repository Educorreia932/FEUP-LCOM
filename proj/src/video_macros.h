#pragma once

#include <lcom/lcf.h>

/** @file video_macros.h */

/** @addtogroup video_gr
 * @{
*/

/** @brief Physical Address base */
#define BASE_PHYS_ADDRESS 0
/** @brief Memory size */
#define MiB BIT(20)
/** @brief Video Graphics Indexed Color Mode */
#define VG_MODE_INDEXED 4
/** @brief Video Graphics Direct Color Mode */
#define VG_MODE_DIRECT 6

/** @name VBE Functions 
 * @{
*/
/** @brief VBE Function Indicator */
#define VBE_AH_FUNCTION_INDICATOR 0x4F
/** @brief Get Mode Info */
#define VBE_AL_MODE_INFO 0x01
/** @brief Set Mode */
#define VBE_AL_SET_MODE 0x02
/** @brief Set Mode Argument */
#define VBE_BX_SET_MODE_ARGUMENT BIT(14)
/** @brief VBE Interrupt Number */
#define VBE_INTERRUPT_NUMBER 0x10
/** @brief Return Success */
/** @note Returns are on the AX */
#define VBE_FUNCTION_RET_SUCCESS 0x00
/** @} */

/** @} end of Video Macros */
