/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/


#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

#include "stm32f0xx.h"                  // Device header
#include "DAC_DRV.h"


/*----------------------------------------------------------------------------
 * SystemCoreClockConfigure: configure SystemCoreClock using HSI
                             (HSE is not populated on Discovery board)
 *----------------------------------------------------------------------------*/
void SystemCoreClockConfigure(void) {

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

  RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock

  FLASH->ACR  = FLASH_ACR_PRFTBE;                          // Enable Prefetch Buffer
  FLASH->ACR |= FLASH_ACR_LATENCY;                         // Flash 1 wait state

  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK
  RCC->CFGR |= RCC_CFGR_PPRE_DIV1;                         // PCLK = HCLK

  RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

  //  PLL configuration:  = HSI/2 * 12 = 48 MHz
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
#if defined(STM32F042x6) || defined(STM32F048xx)  || defined(STM32F070x6) \
 || defined(STM32F078xx) || defined(STM32F071xB)  || defined(STM32F072xB) \
 || defined(STM32F070xB) || defined(STM32F091xC) || defined(STM32F098xx)  || defined(STM32F030xC)
  /* HSI used as PLL clock source : SystemCoreClock = HSI/PREDIV * PLLMUL */
  RCC->CFGR2 =  (RCC_CFGR2_PREDIV_DIV2);
  RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_PREDIV  | RCC_CFGR_PLLMUL12);
#else
  /* HSI used as PLL clock source : SystemCoreClock = HSI/2 * PLLMUL */
  RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12);
#endif


  RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

  RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}



/*----------------------------------------------------------------------------
 * blinkLED: blink LED and check button state
 *----------------------------------------------------------------------------*/
void blinkLED(void const *argument) {
  int32_t max_num = LED_GetCount();
  int32_t num = 0;

  for (;;) {
    LED_On(num);                                           // Turn specified LED on
    osSignalWait(0x0001, osWaitForever);
    LED_Off(num);                                          // Turn specified LED off
    osSignalWait(0x0001, osWaitForever);

    num++;                                                 // Change LED number
    if (num >= max_num) {
      num = 0;                                             // Restart with first LED
    }
  }

}

osThreadId tid_blinkLED;
osThreadDef (blinkLED, osPriorityNormal, 1, 0);
uint16_t DAC_value[91] ={4095,4094,4093,4089,4085,4079,4073,4064,4055,4045,
												4033,4020,4006,3990,3973,3955,3936,3916,3895,3872,
												3848,3823,3797,3769,3741,3711,3681,3649,3616,3582,
												3546,3510,3473,3434,3395,3354,3313,3270,3227,3182,
												3137,3091,3043,2995,2946,2896,2845,2793,2740,2687,
												2632,2577,2521,2464,2407,2349,2290,2230,2170,2109,
												2048,1985,1922,1859,1795,1731,1666,1600,1534,1468,
												1401,1333,1265,1197,1129,1060,991,921,851,781,
												711,641,570,499,428,357,286,214,143,71,0};

uint16_t DAC_value1[181] = {4094,	4094,	4093,	4091,	4089,	4086,	4083,	4079,	
														4074,	4069,	4063,	4056,	4049,	4042,	4033,	4024,	
														4015,	4005,	3994,	3982,	3971,	3958,	3945,	3931,
													  3917,	3902,	3887,	3871,	3854,	3837,	3820,	3802,
														3783,	3764,	3744,	3724,	3703,	3682,	3660,	3638,
														3615,	3592,	3568,	3544,	3519,	3494,	3469,	3443,
														3417,	3390,	3363,	3335,	3307,	3279,	3250,	3221,
														3192,	3162,	3132,	3101,	3071,	3039,	3008,	2976,
														2944,	2912,	2880,	2847,	2814,	2781,	2747,	2713,
														2680,	2645,	2611,	2577,	2542,	2507,	2473,	2438,
														2402,	2367,	2332,	2296,	2261,	2225,	2190,	2154,
														2118,	2083,	2047,	2011,	1976,	1940,	1904,	1869,
														1833,	1798,	1762,	1727,	1692,	1656,	1621,	1587,
														1552,	1517,	1483,	1449,	1414,	1381,	1347,	1313,
														1280,	1247,	1214,	1182,	1150,	1118,	1086,	1055,
														1024,	993,	962,	932,	902,	873,	844,	815,
														787,	759,	731,	704,	677,	651,	625,	600,
														575,	550,	526,	502,	479,	456,	434,	412,
														391,	370,	350,	330,	311,	292,	274,	257,
													  240,	223,	207,	192,	177,	163,	149,	136,
														123,	112,	100,	89,	79,	70,	61,	52,	45,	38,
														31,	25,	20,	15,	11,	8,	5,	3,	1,	0,	0};

uint16_t DAC_value2[181] = {0};												
														
														
uint8_t Counter =0;
uint8_t Direction =0;

/*----------------------------------------------------------------------------
 * main: initialize and start the system
 *----------------------------------------------------------------------------*/
int main (void) {
  uint32_t button_msk = (1U << Buttons_GetCount()) - 1;

  osKernelInitialize ();                                   // initialize CMSIS-RTOS

  // initialize peripherals
  SystemCoreClockConfigure();                              // configure System Clock
  SystemCoreClockUpdate();

  LED_Initialize();                                        // LED Initialization
  Buttons_Initialize();                                    // Buttons Initialization

  // create threads
  tid_blinkLED = osThreadCreate (osThread(blinkLED), NULL);

  osKernelStart ();                                        // start thread execution
	DAC_init();
	DAC_enable();
	
	
	for(Counter=0;Counter<182;Counter++)
	{
		DAC_value2[Counter]=DAC_value1[Counter]/2;
	}
	Counter=0;

  for (;;) { 
		if (Counter<=0)Direction=0;
		else if (Counter>179)Direction=1;
		if (Direction==0)Counter++;
		else Counter--;
		
		DAC_write(DAC_value2[Counter]);
		
		
		
//		DAC_write(4000);
//    osDelay(100);
//		while (Buttons_GetState() & (button_msk));             // Wait while holding USER button
//		DAC_write(0);
//    osSignalSet(tid_blinkLED, 0x0001);
  }
}
