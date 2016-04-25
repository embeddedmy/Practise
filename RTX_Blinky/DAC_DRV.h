
/** @file DAC_DRV.h
 *  @brief DAC Driver include file for the STM32F072RB.
 *  @author Dennis Law
 *  @date April 2016
 */

#ifndef DAC_DRV_H
#define DAC_DRV_H

#include "stm32f0xx.h"
#include <stdint.h>

uint32_t DAC_read(void);
void DAC_write(uint32_t data);
void DAC_load_data_buffer(uint32_t data);
void DAC_enable(void);
void DAC_init(void);

#endif /* DAC_DRV_H */
