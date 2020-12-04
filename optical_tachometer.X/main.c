/*
 * File:   main.c
 * Author: Sandra Ekholm, Amy Nymalm and Nea Kontturi
 *
 * Created on 02 December 2020, 13:32
 */
#define F_CPU 3333333

#include <xc.h>
#include "testi.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdio.h>

// Function declaration
void RTC_init(void);
void ADC_init(void);
void SEGMENT_init(void);
void update_display(void);

// Configuring RTC
void RTC_init(void)
{
    // Helping variable
    uint8_t temp;
    
    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    // Set temp as the value of register at that moment
    temp = CLKCTRL.XOSC32KCTRLA;
    // Set the final bit of temp into zero (to get access to SEL and CSUT)
    temp &= ~CLKCTRL_ENABLE_bm;
    // Writing to protected register
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    // Waiting until status bit becomes 0
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
    {
        ;
    }
    
    // Set temp as the value of register at the moment
    temp = CLKCTRL.XOSC32KCTRLA;
    // Using external crystal: SEL = 0
    temp &= ~CLKCTRL_SEL_bm;
    // Writing to protected register
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Enable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    // Setting the oscillator enable bit
    temp |= CLKCTRL_ENABLE_bm;
    // Writing to protected register
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Initialize RTC: */
    // Waiting all registers to be synchronized
    while (RTC.STATUS > 0)
    {
        ;
    }
    
    // External Clock Oscillator clock is selected
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    // Allow RTC running in debug mode
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    // Enable periodic interrupt
    RTC.PITINTCTRL = RTC_PI_bm;
    // Selecting number of RTC clock cycles (16384) because we want interrupt
    // twice a second?? and enable periodic interrupt timer
    RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm;
}
void ADC_init(void)
{
    // Set port E pin 0 as input
    PORTE.DIRCLR = PIN0_bm;
    // Disable input buffer
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    // Resolution 10 bits
    ADC0.CTRLA &= ~ADC_RESSEL_bm;
    
    // Voltage reference is 1,5V and prescaler of 16
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc;
    
    // Enable ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
}
void SEGMENT_init(void)
{
    // Tähä 7-segmenttihötskän tunnistus
    
    // portti c configured as an output
    VPORTC.DIR |= PIN0_bm;
    VPORTC.DIR |= PIN1_bm;
    VPORTC.DIR |= PIN2_bm;
    VPORTC.DIR |= PIN3_bm;
    VPORTC.DIR |= PIN4_bm;
    VPORTC.DIR |= PIN5_bm;
    VPORTC.DIR |= PIN6_bm;
    VPORTC.DIR |= PIN7_bm;
}

void update_display(void)
{
    // päivitys
    // testi laitetaa jännä numero segmenttiin :)
    VPORTC.OUT |= PIN0_bm;
    VPORTC.OUT |= PIN1_bm;
    VPORTC.OUT |= PIN2_bm;
    VPORTC.OUT |= PIN3_bm;
    VPORTC.OUT |= PIN6_bm;
    VPORTC.OUT |= PIN7_bm;
}

// RTC interrupt
ISR(RTC_PIT_vect)
{
    // Clearing interrupt flag
    RTC.PITINTFLAGS = RTC_PI_bm;
    // Tähän display päivitys
    update_display();
}

int main(void) 
{
    // Setting internal reference voltage to 1.5V
    VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc;
    // Initialize RTC
    RTC_init();
    // Initialize ADC and its input pin
    ADC_init();
    // Initialize 7-segment display
    SEGMENT_init();
    
    // Enable global interrupts
    sei();
    
    while(1)
    {
        
    }
    test();
    return 0;
}

