#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

void nvic_setup(void)
{
	/* Nested vector interrupt control */
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
}

void tim2_isr(void)
{
	gpio_toggle(GPIOC, GPIO13);   /* LED2 on/off. */
	TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
}


int main(void) {

  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOC);

  nvic_setup();

  rcc_periph_clock_enable(RCC_TIM2);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
  gpio_set(GPIOC, GPIO13);

  /* Set timer start value. */
  TIM_CNT(TIM2) = 0;

  /* Set timer prescaler. 72MHz/7200 => 100000 counts per second. */
  TIM_PSC(TIM2) = 7200;

  /* End timer value. If this is reached an interrupt is generated. */
  TIM_ARR(TIM2) = 10;

  /* Update interrupt enable. */
  TIM_DIER(TIM2) |= TIM_DIER_UIE;

  /* Start timer. */
  TIM_CR1(TIM2) |= TIM_CR1_CEN;

  while (1); /* Halt. */

  return 0;

}
