/**************************************************************************//**
 * @main_gg11_tg11.c
 * @brief This project demonstrates pulse width modulation using the TIMER
 * module. The GPIO pin specified in the readme.txt is configured for output and
 * outputs a 1kHz, 30% duty cycle signal. The duty cycle can be adjusted by
 * writing to the CCVB or changing the global dutyCyclePercent variable.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_dbg.h"
// Note: change this to set the desired output frequency in Hz
#define PWM_FREQ 1000/62

// Note: change this to set the desired duty cycle (used to update CCVB value)
static volatile int dutyCyclePercent = 30;

/**************************************************************************//**
 * @brief
 *    Interrupt handler for TIMER1 that changes the duty cycle
 *
 * @note
 *    This handler doesn't actually dynamically change the duty cycle. Instead,
 *    it acts as a template for doing so. Simply change the dutyCyclePercent
 *    global variable here to dynamically change the duty cycle.
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER1);
  TIMER_IntClear(TIMER1, flags);
  GPIO_PinOutToggle(gpioPortC,13);
  // Update CCVB to alter duty cycle starting next period
  //TIMER_CompareBufSet(TIMER1, 0, (TIMER_TopGet(TIMER1) * dutyCyclePercent) / 100);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC13 as output
  GPIO_PinModeSet(gpioPortC, 13, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
}
#if 0
void TIMER1_IRQHanlder()
{
    TIMER_IntClear(TIMER1,TIMER_IF_OF);
    GPIO_PinOutToggle(gpioPortC,13);
}
#endif
/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
	uint32_t temp1;
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
  // Configure TIMER1 Compare/Capture for output compare
  // Use PWM mode, which sets output on overflow and clears on compare events
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);
  TIMER_InitCC(TIMER0, 0, &timerCCInit);
  // Route TIMER1 CC0 to location 0 and enable CC0 route pin
  // TIM1_CC0 #0 is GPIO Pin PC13
  TIMER1->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC0;
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  TIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC2;
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;


  // Initialize the timer
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  //timerInit.oneShot = true;
  //TIMER1->CTRL |= 0x8;
  timerInit.prescale = timerPrescale1024;
  timerInit.enable = false;
  timerInit.sync = true;
  TIMER_Init(TIMER1, &timerInit);
  //TIMER1->CTRL |= 0x8;
  timerInit.enable = true;
  timerInit.sync = false;
  for (temp1 = 0; temp1 <10000; temp1++)
	  ;
  TIMER_Init(TIMER0, &timerInit);
  // Set top value to overflow at the desired PWM_FREQ frequency
  TIMER_TopSet(TIMER1, CMU_ClockFreqGet(cmuClock_TIMER1)*62/1024 / 1000);

  // Set compare value for initial duty cycle
  TIMER_CompareSet(TIMER1, 0, CMU_ClockFreqGet(cmuClock_TIMER1)*15/1024 / 1000);
  // Set top value to overflow at the desired PWM_FREQ frequency
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_TIMER1)*62/1024 / 1000);

  // Set compare value for initial duty cycle
  TIMER_CompareSet(TIMER0, 0, CMU_ClockFreqGet(cmuClock_TIMER1)*31/1024 / 1000);
  temp1 = CMU_ClockFreqGet(cmuClock_TIMER1)*62/1024 / 1000;
  // Enable TIMER1 compare event interrupts to update the duty cycle
  //TIMER_IntEnable(TIMER1, TIMER_IEN_OF);
  //NVIC_EnableIRQ(TIMER1_IRQn);
  //TIMER1->CNT = 0;
  //GPIO_PinOutToggle(gpioPortC,13);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  //DBG_DisableDebugAccess(dbgLockModeAllowErase);
  // Initializations
  initGpio();
  GPIO_PinOutClear(gpioPortC,13);
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

