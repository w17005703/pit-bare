#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"

#define PIN21_MASK           (1u << 21)

void blue_init(void);
void blue_toggle(void);
void PIT_init(void);
void PIT0_IRQHandler(void);

int main(void)
{
    blue_init();
    PIT_init();
    while (true) {
    }
    /* Should not reach this point */
    return 0;
}

void blue_init(void) {
    /* Enable the clock to PORT B */
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

    /* Select the GPIO function (Alternative 1) for pin 21 of PORT B */
    PORTB_PCR21 &= ~PORT_PCR_MUX_MASK;
    PORTB_PCR21 |= (1u << PORT_PCR_MUX_SHIFT);

    /* Set the data direction for pin 21 of PORT B to output */
    GPIOB_PDDR |= PIN21_MASK;

    /* LED off initially */
    GPIOB_PDOR |= PIN21_MASK;
}

void blue_toggle(void) {
    /* Blue LED, ON <- OFF, OFF <- ON */
    GPIOB_PDOR ^= PIN21_MASK; 
}

void PIT_init(void) {
    /* Open the clock gate to the PIT */
    SIM_SCGC6 |= (1u << 23);
    /* Enable the clock for the PIT timers. Continue to run in debug mode */
    PIT_MCR_REG(PIT) = 0u;
    /* Period p = 0.5 s, bus clock f = 60 MHz, v = pf - 1 */ 
    PIT_LDVAL_REG(PIT, 0) = 29999999;
    /* Enable interrupt on timeout */
    PIT_TCTRL_REG(PIT, 0) |= PIT_TCTRL_TIE_MASK;
    /* Enable the interrupt in the NVIC */
    NVIC_EnableIRQ(PIT0_IRQn);
    /* Start the timer running */
    PIT_TCTRL_REG(PIT, 0) |= PIT_TCTRL_TEN_MASK;
}

void PIT0_IRQHandler(void) {
    /* Call the main handler function */
    blue_toggle();
    /* Clear the timer interrupt flag to allow further timer interrupts */
    PIT_TFLG_REG(PIT,0) |= PIT_TFLG_TIF_MASK;
}
