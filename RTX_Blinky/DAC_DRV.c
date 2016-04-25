
/** @file DAC_DRV.c
 *  @brief DAC Driver for the STM32F072RB.
 *  @author Dennis Law
 *  @date April 2016
 */

#include "DAC_DRV.h"

/**	@brief Reads the output register of channel 1.
 *	@returns The value of DAC_DOR1 register.
 */
uint32_t DAC_read(void)
{
	return (DAC->DOR1 & 0x00000FFF);
}

/** @brief Writes an analog value to channel 1.
 *	@param data The data to be written to the data register.
 *
 *	@details The user input value is written to the 12 bit right aligned
 *	register, DAC_DHR12R1. The output voltage is calculated using the formula
 *
 *		DAC(output) = V(DDA) * DOR / 4095
 *
 */
void DAC_write(uint32_t data)
{
	DAC->DHR12R1 = (data & 0x00000FFF);
}

/** @brief Enables channel 1.
 */
void DAC_enable(void)
{
	DAC->CR |= DAC_CR_EN1;
}

/**	@brief Initializes channel 1 in Single Mode with no triggerring.
 */
void DAC_init(void)
{
	/* Enable clock for DAC peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	/* Set corresponding GPIO pin as analog input with no pull
	   resistors */
	GPIOA->MODER |= GPIO_MODER_MODER4;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4);

	/* Disable triggerring */
	DAC->CR &= ~(DAC_CR_TEN1);

	/* Enable output buffer */
	DAC->CR |= DAC_CR_BOFF1;
}
