/*
 * File:   main.c
 * Author: Sandra Ekholm, Amy Nymalm and Nea Kontturi
 *
 * Created on 02 December 2020, 13:32
 */
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
    ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <xc.h>
#include "testi.h"
#include "update_display.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdio.h>

// Function declaration
static void USART0_sendChar(char c);
static int USART0_printChar(char c, FILE *stream);
static void USART0_init(void);
void RTC_init(void);
void ADC_init(void);
void SEGMENT_init(void);

// Global variable to store adc result
volatile uint16_t adcValue;
// Global variable to store the rpm calculated from rotations
uint16_t rpm;
// Variable to count rotations
volatile uint16_t rotations;
// Global variable msd (indicates most significant digit
// value of rotating propeller rpm)
volatile int msd;
//int to check if the propeller is in front of the LDR, and makes sure, the 
//propeller is only counted once
int isPropOn;

// Fuction for sending text to computer terminal/putty
static void USART0_sendChar(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream)
{ 
    USART0_sendChar(c);
    return 0; 
}

static FILE USART_stream =  \
    FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

// Intitialising connection to  computer terminal/putty
static void USART0_init(void)
{
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600); 
    
    USART0.CTRLB |= USART_TXEN_bm;  
    
    stdout = &USART_stream;
}
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
    // twice a second and enable periodic interrupt timer
    RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm;
}
void ADC_init(void)
{
    // Set port E pin 0 as input
    PORTE.DIRCLR = PIN0_bm;
    
    // Disable input buffer, pull-up resistor disabled by default 
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    // Voltage reference is 1,5V (internal reference voltage already defined) 
    // and prescaler of 16
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc;
    
    // Resolution 10 bits
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    
    // Enable ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
    // Selecting AN8 (PE0) to be connected to ADC
    ADC0.MUXPOS =  ADC_MUXPOS_AIN8_gc;
    
    // Freerun mode enabled (next conversion starts automatically)
    ADC0.CTRLA |= ADC_FREERUN_bm;
    
    // Enable interrupts
    ADC0.INTCTRL |= ADC_RESRDY_bm;
    
}
void SEGMENT_init(void)
{
    // 7-segment display configurations
    
    // Port C configured as an output
    VPORTC.DIR |= PIN0_bm;
    VPORTC.DIR |= PIN1_bm;
    VPORTC.DIR |= PIN2_bm;
    VPORTC.DIR |= PIN3_bm;
    VPORTC.DIR |= PIN4_bm;
    VPORTC.DIR |= PIN5_bm;
    VPORTC.DIR |= PIN6_bm;
    VPORTC.DIR |= PIN7_bm;
}

// RTC interrupt
ISR(RTC_PIT_vect)
{
    // Clearing interrupt flag
    RTC.PITINTFLAGS = RTC_PI_bm;
    // rpm calculated from rotations (60 because reading value twice a second
    // *120 but propeller has two wings /2 so *60)
    rpm = rotations*60;
    // testing
    printf("%i rpm\r\n", rpm);
    // Counting first digit from rpm
    while(rpm >=10)
    {
        // Dividing with 10 until only one number is left -> msd
        rpm = rpm / 10;
    }
    msd = rpm;
    // Updating display to msd
    update_display(msd);
    // Resetting rpm and rotations for next round
    rpm = 0;
    rotations=0;
}

// ADC interrupt, ADC conversion is done
ISR(ADC0_RESRDY_vect)
{
    // Clearing interrupt flag
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    // Setting the value adc measured
    adcValue = ADC0.RES;
}

int main(void) 
{
    // In the beginning the value of adc is 0
    adcValue = 0;
    // In the beginning the value of rotations is 0
    rotations = 0;
    // In the beginning the value of rpm is 0
    rpm = 0;
    // In the beginning the value of msd is 0
    msd = 0;
    //In the beginning the value of isPropOn is 0
    isPropOn = 0;
    // Setting internal reference voltage to 1.5V
    VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
    // Initialize output to putty
    USART0_init();
    // Initialize 7-segment display
    SEGMENT_init();
    // Initialize ADC and its input pin
    ADC_init();
    // Initialize RTC
    RTC_init();
    // Start ADC conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    // Enable global interrupts
    sei();
    
    // testing
    printf("Hello, testing testing.\r\n");
    
    while(1)
    {
        //AdcValue while propeller is in front of LDR is 700 originally (will 
        // be replaced later)
        while(adcValue>700)
        {   
            //makes sure the rotations are only updated once per rotation
            isPropOn=1;
        } 
        //when the propeller is not in front of the LDR anymore, the rotations 
        //value is updated.
        if (isPropOn)
        {
            rotations++;
            isPropOn=0;
        }
    }
    // testing
    test();
    return 0;
}

