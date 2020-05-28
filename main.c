#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

void nvic_setup(void){
	
	/* Nested vector interrupt control */
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
	
}

void tim2_isr(void){
	
	gpio_toggle(GPIOC, GPIO13);   /* LED2 on/off. */
	TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
	
}

void tim2_setup(void){
	
	rcc_periph_clock_enable(RCC_TIM2);
	
	/* Set timer start value. */
	TIM_CNT(TIM2) = 0;

	/* Set timer prescaler. 72MHz/7200 => 10000 counts per second. */
	TIM_PSC(TIM2) = 7200;

	/* End timer value. If this is reached an interrupt is generated. */
	TIM_ARR(TIM2) = 10000; // 1 second

	/* Update interrupt enable. */
	TIM_DIER(TIM2) |= TIM_DIER_UIE;

	/* Start timer. */
	TIM_CR1(TIM2) |= TIM_CR1_CEN;
}

void gpio_setup(void){
	
	rcc_periph_clock_enable(RCC_GPIOC);

	/* GPIO13 of GPIOC bank is the default led in the bluepill board */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set(GPIOC, GPIO13);

}

void general_setup(void){
	
	/* Use external 8MHz clock and generate 72MHz clock using PLL */
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

}

int main(void) {

  general_setup();
  
  gpio_setup();

  nvic_setup();

  tim2_setup();

  while (1); /* Run for ever... */

  return 0;

}
